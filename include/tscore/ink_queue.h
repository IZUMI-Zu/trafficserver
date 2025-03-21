/** @file

  A brief file description

  @section license License

  Licensed to the Apache Software Foundation (ASF) under one
  or more contributor license agreements.  See the NOTICE file
  distributed with this work for additional information
  regarding copyright ownership.  The ASF licenses this file
  to you under the Apache License, Version 2.0 (the
  "License"); you may not use this file except in compliance
  with the License.  You may obtain a copy of the License at

      http://www.apache.org/licenses/LICENSE-2.0

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.
 */

#pragma once

/***********************************************************************

    Generic Queue Implementation (for pointer data types only)

    Uses atomic memory operations to avoid blocking.
    Intended as a replacement for llqueue.


***********************************************************************/

#include "tscore/ink_platform.h"
#include "tscore/ink_defs.h"
#include "tscore/ink_apidefs.h"

/*
  For information on the structure of the x86_64 memory map:

  http://en.wikipedia.org/wiki/X86-64#Linux

  Essentially, in the current 48-bit implementations, the
  top bit as well as the  lower 47 bits are used, leaving
  the upper-but one 16 bits free to be used for the version.
  We will use the top-but-one 15 and sign extend when generating
  the pointer was required by the standard.
*/

void ink_queue_load_64(void *dst, void *src);

#ifdef __x86_64__
#define INK_QUEUE_LD64(dst, src) *((uint64_t *)&(dst)) = *((uint64_t *)&(src))
#else
#define INK_QUEUE_LD64(dst, src) (ink_queue_load_64((void *)&(dst), (void *)&(src)))
#endif

// passing a const volatile value of 0 works around a gcc bug
#if TS_HAS_128BIT_CAS
#define INK_QUEUE_LD(dst, src)                                                                     \
  do {                                                                                             \
    const volatile __int128_t iqld0 = 0;                                                           \
    *(__int128_t *)&(dst)           = __sync_val_compare_and_swap((__int128_t *)&(src), 0, iqld0); \
  } while (0)
#else
#define INK_QUEUE_LD(dst, src) INK_QUEUE_LD64(dst, src)
#endif

/*
 * Generic Free List Manager
 */
// Warning: head_p is read and written in multiple threads without a
// lock, use INK_QUEUE_LD to read safely.
union head_p {
  head_p() : data(){};

#if (defined(__i386__) || defined(__arm__) || defined(__mips__)) && (SIZEOF_VOIDP == 4)
  typedef int32_t version_type;
  typedef int64_t data_type;
#elif TS_HAS_128BIT_CAS
  typedef int64_t    version_type;
  typedef __int128_t data_type;
#else
  using version_type = int64_t;
  using data_type    = int64_t;
#endif

  struct {
    void        *pointer;
    version_type version;
  } s;

  data_type data;
};

/*
 * Why is version required? One scenario is described below
 * Think of a list like this -> A -> C -> D
 * and you are popping from the list
 * Between the time you take the ptr(A) and swap the head pointer
 * the list could start looking like this
 * -> A -> B -> C -> D
 * If the version check is not there, the list will look like
 * -> C -> D after the pop, which will result in the loss of "B"
 *
 * For more information, see:  https://en.wikipedia.org/wiki/ABA_problem .
 * (Versioning is a case of the "tagged state reference" workaround.)
 */
#define ZERO_HEAD_P(_x)

#ifdef DEBUG
#define FROM_PTR(_x) (void *)(((uintptr_t)_x) + 1)
#define TO_PTR(_x)   (void *)(((uintptr_t)_x) - 1)
#else
#define FROM_PTR(_x) ((void *)(_x))
#define TO_PTR(_x)   ((void *)(_x))
#endif

#if (defined(__i386__) || defined(__arm__) || defined(__mips__)) && (SIZEOF_VOIDP == 4)
#define FREELIST_POINTER(_x) (_x).s.pointer
#define FREELIST_VERSION(_x) (_x).s.version
#define SET_FREELIST_POINTER_VERSION(_x, _p, _v) \
  (_x).s.pointer = _p;                           \
  (_x).s.version = _v
#elif TS_HAS_128BIT_CAS
#define FREELIST_POINTER(_x) (_x).s.pointer
#define FREELIST_VERSION(_x) (_x).s.version
#define SET_FREELIST_POINTER_VERSION(_x, _p, _v) \
  (_x).s.pointer = _p;                           \
  (_x).s.version = _v
#elif defined(__x86_64__) || defined(__ia64__) || defined(__powerpc64__) || defined(__mips64) || defined(__riscv)
/* Layout of FREELIST_POINTER
 *
 *  0 ~ 47 bits : 48 bits, Virtual Address
 * 48 ~ 62 bits : 15 bits, Freelist Version
 *      63 bits :  1 bits, The type of Virtual Address (0 = user space, 1 = kernel space)
 */
/* Detect which shift is implemented by the simple expression ((~0 >> 1) < 0):
 *
 * If the shift is 'logical' the highest order bit of the left side of the comparison is 0 so the result is positive.
 * If the shift is 'arithmetic' the highest order bit of the left side is 1 so the result is negative.
 */
#if ((~0 >> 1) < 0)
/* the shift is `arithmetic' */
#define FREELIST_POINTER(_x) \
  ((void *)((((intptr_t)(_x).data) & 0x0000FFFFFFFFFFFFLL) | ((((intptr_t)(_x).data) >> 63) << 48))) // sign extend
#else
/* the shift is `logical' */
#define FREELIST_POINTER(_x) \
  ((void *)((((intptr_t)(_x).data) & 0x0000FFFFFFFFFFFFLL) | ((~((((intptr_t)(_x).data) >> 63) - 1)) << 48)))
#endif

#define FREELIST_VERSION(_x)                     ((((intptr_t)(_x).data) & 0x7FFF000000000000LL) >> 48)
#define SET_FREELIST_POINTER_VERSION(_x, _p, _v) (_x).data = ((((intptr_t)(_p)) & 0x8000FFFFFFFFFFFFLL) | (((_v) & 0x7FFFLL) << 48))
#elif defined(__aarch64__)
/* Layout of FREELIST_POINTER
 *
 *  0 ~ 51 bits : 52 bits, Virtual Address
 * 52 ~ 62 bits : 11 bits, Freelist Version
 *      63 bits :  1 bits, The type of Virtual Address (0 = user space, 1 = kernel space)
 */
#if ((~0 >> 1) < 0)
/* the shift is `arithmetic' */
#define FREELIST_POINTER(_x) \
  ((void *)((((intptr_t)(_x).data) & 0x000FFFFFFFFFFFFFLL) | ((((intptr_t)(_x).data) >> 63) << 52))) // sign extend
#else
/* the shift is `logical' */
#define FREELIST_POINTER(_x) \
  ((void *)((((intptr_t)(_x).data) & 0x000FFFFFFFFFFFFFLL) | ((~((((intptr_t)(_x).data) >> 63) - 1)) << 52)))
#endif

#define FREELIST_VERSION(_x)                     ((((intptr_t)(_x).data) & 0x7FF0000000000000LL) >> 52)
#define SET_FREELIST_POINTER_VERSION(_x, _p, _v) (_x).data = ((((intptr_t)(_p)) & 0x800FFFFFFFFFFFFFLL) | (((_v) & 0x7FFLL) << 52))
#else
#error "unsupported processor"
#endif

struct _InkFreeList {
  head_p      head;
  const char *name;
  uint32_t    type_size, chunk_size, used, allocated, alignment;
  uint32_t    allocated_base, used_base;
  uint32_t    hugepages_failure;
  bool        use_hugepages;
  int         advice;
};

using InkFreeListOps = struct ink_freelist_ops;
using InkFreeList    = struct _InkFreeList;

const InkFreeListOps *ink_freelist_malloc_ops();
const InkFreeListOps *ink_freelist_freelist_ops();
void                  ink_freelist_init_ops(int nofl_class, int nofl_proxy);

/*
 * alignment must be a power of 2
 */
InkFreeList *ink_freelist_create(const char *name, uint32_t type_size, uint32_t chunk_size, uint32_t alignment,
                                 bool use_hugepages = false);

void  ink_freelist_init(InkFreeList **fl, const char *name, uint32_t type_size, uint32_t chunk_size, uint32_t alignment,
                        bool use_hugepages);
void  ink_freelist_madvise_init(InkFreeList **fl, const char *name, uint32_t type_size, uint32_t chunk_size, uint32_t alignment,
                                bool use_hugepages, int advice);
void *ink_freelist_new(InkFreeList *f);
void  ink_freelist_free(InkFreeList *f, void *item);
void  ink_freelist_free_bulk(InkFreeList *f, void *head, void *tail, size_t num_item);
void  ink_freelists_dump(FILE *f);
void  ink_freelists_dump_baselinerel(FILE *f);
void  ink_freelists_snap_baseline();

struct InkAtomicList {
  InkAtomicList() {}
  head_p      head{};
  const char *name   = nullptr;
  uint32_t    offset = 0;
};

#if !defined(INK_QUEUE_NT)
#define INK_ATOMICLIST_EMPTY(_x) (!(TO_PTR(FREELIST_POINTER((_x.head)))))
#else
/* ink_queue_nt.c doesn't do the FROM/TO pointer swizzling */
#define INK_ATOMICLIST_EMPTY(_x) (!((FREELIST_POINTER((_x.head)))))
#endif

// WARNING: the "name" string is not copied, it has to be a statically-stored constant string.
//
void ink_atomiclist_init(InkAtomicList *l, const char *name, uint32_t offset_to_next);

void *ink_atomiclist_push(InkAtomicList *l, void *item);
void *ink_atomiclist_pop(InkAtomicList *l);
void *ink_atomiclist_popall(InkAtomicList *l);
/*
 * WARNING WARNING WARNING WARNING WARNING WARNING WARNING
 * only if only one thread is doing pops it is possible to have a "remove"
 * which only that thread can use as well.
 * WARNING WARNING WARNING WARNING WARNING WARNING WARNING
 */
void *ink_atomiclist_remove(InkAtomicList *l, void *item);
