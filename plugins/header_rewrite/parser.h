/*
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
//////////////////////////////////////////////////////////////////////////////////////////////
//
// Interface for the config line parser
//
#pragma once

#include <string>
#include <vector>
#include <algorithm>

#include "ts/ts.h"
#include "lulu.h"

///////////////////////////////////////////////////////////////////////////////
//
class Parser
{
public:
  Parser() = default; // No from/to URLs for this parser
  Parser(char *from_url, char *to_url) : _from_url(from_url), _to_url(to_url) {}

  // noncopyable
  Parser(const Parser &)         = delete;
  void operator=(const Parser &) = delete;

  // These are not const char *, because, you know, everything else with argv is a char *
  char *
  from_url() const
  {
    return _from_url;
  }

  char *
  to_url() const
  {
    return _to_url;
  }

  bool
  empty() const
  {
    return _empty;
  }

  bool
  is_cond() const
  {
    return _cond;
  }

  bool
  is_else() const
  {
    return _else;
  }

  const std::string &
  get_op() const
  {
    return _op;
  }

  std::string &
  get_arg()
  {
    return _arg;
  }

  const std::string &
  get_value() const
  {
    return _val;
  }

  bool
  mod_exist(const std::string &m) const
  {
    return std::find(_mods.begin(), _mods.end(), m) != _mods.end();
  }

  bool cond_is_hook(TSHttpHookID &hook) const;

  const std::vector<std::string> &
  get_tokens() const
  {
    return _tokens;
  }

  bool parse_line(const std::string &original_line);

private:
  bool preprocess(std::vector<std::string> tokens);

  bool                     _cond     = false;
  bool                     _else     = false;
  bool                     _empty    = false;
  char                    *_from_url = nullptr;
  char                    *_to_url   = nullptr;
  std::vector<std::string> _mods;
  std::string              _op;
  std::string              _arg;
  std::string              _val;

protected:
  std::vector<std::string> _tokens;
};

class HRWSimpleTokenizer
{
public:
  explicit HRWSimpleTokenizer(const std::string &line);

  // noncopyable
  HRWSimpleTokenizer(const HRWSimpleTokenizer &) = delete;
  void operator=(const HRWSimpleTokenizer &)     = delete;

  const std::vector<std::string> &
  get_tokens() const
  {
    return _tokens;
  }

protected:
  std::vector<std::string> _tokens;
};
