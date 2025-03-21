/** @file

  HTTP configuration support.

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

#include "swoc/MemArena.h"
#include "swoc/swoc_ip.h"

#include "tscore/ink_inet.h"
#include "tsutil/ts_ip.h"
#include "tscore/ink_resolver.h"
#include "ts/apidefs.h"
#include "ts/apidefs.h"
#include "tscore/ink_assert.h"
#include <algorithm>
#include <array>

/** Load IP addresses from a configuration value.
 *
 * @param name Configuration variable name.
 * @return The results of parsing the value.
 */
ts::IPAddrPair RecHttpLoadIp(char const *name);

/// Load up an IpMap with IP addresses from the configuration file.

/** Load a set of IP address from a configuration variable.
 *
 * @param name Variable name
 * @param addrs Destination address set.
 */
void RecHttpLoadIpAddrsFromConfVar(const char *name, swoc::IPRangeSet &addrs);

/** A set of session protocols.
    This depends on using @c SessionProtocolNameRegistry to get the indices.
*/
class SessionProtocolSet
{
  using self_type = SessionProtocolSet; ///< Self reference type.
  /// Storage for the set - a bit vector.
  uint32_t m_bits = 0;

public:
  static constexpr int MAX = sizeof(m_bits) * CHAR_BIT;

  /// Default constructor.
  /// Constructs and empty set.
  SessionProtocolSet() = default;

  uint32_t
  indexToMask(int idx) const
  {
    return 0 <= idx && idx < static_cast<int>(MAX) ? static_cast<uint32_t>(1) << idx : 0;
  }

  /// Mark the protocol at @a idx as present.
  void
  markIn(int idx)
  {
    m_bits |= this->indexToMask(idx);
  }

  /// Mark all the protocols in @a that as present in @a this.
  void
  markIn(self_type const &that)
  {
    m_bits |= that.m_bits;
  }
  /// Mark the protocol at a idx as not present.
  void
  markOut(int idx)
  {
    m_bits &= ~this->indexToMask(idx);
  }
  /// Mark the protocols in @a that as not in @a this.
  void
  markOut(self_type const &that)
  {
    m_bits &= ~(that.m_bits);
  }
  /// Test if a protocol is in the set.
  bool
  contains(int idx) const
  {
    return 0 != (m_bits & this->indexToMask(idx));
  }
  /// Test if all the protocols in @a that are in @a this protocol set.
  bool
  contains(self_type const &that) const
  {
    return that.m_bits == (that.m_bits & m_bits);
  }
  /// Mark all possible protocols.
  void
  markAllIn()
  {
    m_bits = ~static_cast<uint32_t>(0);
  }
  /// Clear all protocols.
  void
  markAllOut()
  {
    m_bits = 0;
  }

  /// Check for intersection.
  bool
  intersects(self_type const &that)
  {
    return 0 != (m_bits & that.m_bits);
  }

  /// Check for empty set.
  bool
  isEmpty() const
  {
    return m_bits == 0;
  }

  /// Equality (identical sets).
  bool
  operator==(self_type const &that) const
  {
    return m_bits == that.m_bits;
  }
};

// Predefined sets of protocols, useful for configuration.
extern SessionProtocolSet HTTP_PROTOCOL_SET;
extern SessionProtocolSet HTTP2_PROTOCOL_SET;
extern SessionProtocolSet DEFAULT_NON_TLS_SESSION_PROTOCOL_SET;
extern SessionProtocolSet DEFAULT_TLS_SESSION_PROTOCOL_SET;
extern SessionProtocolSet DEFAULT_QUIC_SESSION_PROTOCOL_SET;

const char *RecNormalizeProtoTag(const char *tag);

/** Registered session protocol names.

    We do this to avoid lots of string compares. By normalizing the string names we can just compare
    their indices in this table.

    @internal To simplify the implementation we limit the maximum number of strings to 32. That will
    be sufficient for the foreseeable future. We can come back to this if it ever becomes a problem.

    @internal Because we have so few strings we just use a linear search. If the size gets much
    larger we should consider doing something more clever.

    @internal This supports providing constant strings because those strings are exported to the
    C API and this logic @b must return exactly those pointers.
*/
class SessionProtocolNameRegistry
{
public:
  static int constexpr MAX     = SessionProtocolSet::MAX; ///< Maximum # of registered names.
  static int constexpr INVALID = -1;                      ///< Normalized invalid index value.

  static std::string_view convert_openssl_alpn_wire_format(int index);

  using TextView = swoc::TextView;

  /// Default constructor.
  /// Creates empty registry with no names.
  SessionProtocolNameRegistry() = default;

  /** Get the index for @a name, registering it if needed.
      The name is copied internally.
      @return The index for the registered @a name.
  */
  int toIndex(TextView name);

  /** Get the index for @a name, registering it if needed.
      The caller @b guarantees @a name is persistent and immutable.
      @return The index for the registered @a name.
  */
  int toIndexConst(TextView name);

  /** Convert a @a name to an index.
      @return The index for @a name or @c INVALID if it is not registered.
  */
  int indexFor(TextView name) const;

  /** Convert an @a index to the corresponding name.
      @return A pointer to the name or @c nullptr if the index isn't registered.
  */
  TextView nameFor(int index) const;

  /** Convert an @a index to the corresponding name in OpenSSL ALPN wire format.

      OpenSSL ALPN wire format (vector of non-empty, 8-bit length-prefixed, byte strings)
      https://www.openssl.org/docs/man1.1.1/man3/SSL_CTX_set_alpn_protos.html

      @return A pointer to the name or @c nullptr if the index isn't registered.
  */
  static TextView wireNameFor(int index);

  /// Mark protocols as present in @a sp_set based on the names in @a value.
  /// The names can be separated by ;/|,: and space.
  /// @internal This is separated out to make it easy to access from the plugin API
  /// implementation.
  void markIn(const char *value, SessionProtocolSet &sp_set);

protected:
  int                       m_n = 0; ///< Index of first unused slot.
  std::array<TextView, MAX> m_names;
  swoc::MemArena            m_arena; ///< Storage for non-constant strings.
};

extern SessionProtocolNameRegistry globalSessionProtocolNameRegistry;

/** Description of an proxy port.

    This consolidates the options needed for proxy ports, both data
    and parsing. It provides a static global set of ports for
    convenience although it can be used with an externally provided
    set.

    Options are described by a colon separated list of keywords
    without spaces. The options are applied in left to right order. If
    options do not conflict the order is irrelevant.

    IPv6 addresses must be enclosed by brackets. Unfortunate but colon is
    so overloaded there's no other option.
 */
struct HttpProxyPort {
private:
  using self = HttpProxyPort; ///< Self reference type.
public:
  /// Explicitly supported collection of proxy ports.
  using Group = std::vector<self>;

  /// Type of transport on the connection.
  enum TransportType {
    TRANSPORT_NONE = 0,     ///< Unspecified / uninitialized
    TRANSPORT_DEFAULT,      ///< Default (normal HTTP).
    TRANSPORT_COMPRESSED,   ///< Compressed HTTP.
    TRANSPORT_BLIND_TUNNEL, ///< Blind tunnel (no processing).
    TRANSPORT_SSL,          ///< SSL connection.
    TRANSPORT_PLUGIN,       /// < Protocol plugin connection
    TRANSPORT_QUIC,         ///< SSL connection.
  };

  int           m_fd;                         ///< Pre-opened file descriptor if present.
  TransportType m_type   = TRANSPORT_DEFAULT; ///< Type of connection.
  in_port_t     m_port   = 0;                 ///< Port on which to listen.
  uint8_t       m_family = AF_INET;           ///< IP address family.
  /// True if proxy protocol is required on incoming requests.
  bool m_proxy_protocol = false;
  /// True if inbound connects (from client) are transparent.
  bool m_inbound_transparent_p = false;
  /// True if outbound connections (to origin servers) are transparent.
  bool m_outbound_transparent_p = false;
  // True if transparent pass-through is enabled on this port.
  bool m_transparent_passthrough = false;
  // True if allow-plain is enabled on this port.
  bool m_allow_plain = false;
  /// True if MPTCP is enabled on this port.
  bool m_mptcp = false;
  /// Local address for inbound connections (listen address).
  IpAddr m_inbound_ip;
  /// Path for listening on unix domain socket
  UnAddr m_unix_path;
  /// Local address for outbound connections (to origin server).
  ts::IPAddrPair m_outbound;
  /// Ordered preference for DNS resolution family ( @c FamilyPrefence )
  /// A value of @c PREFER_NONE indicates that entry and subsequent ones
  /// are invalid.
  HostResPreferenceOrder m_host_res_preference;
  /// Static preference list that is the default value.
  static HostResPreferenceOrder const DEFAULT_HOST_RES_PREFERENCE;
  /// Enabled session transports for this port.
  SessionProtocolSet m_session_protocol_preference;

  /// Default constructor.
  HttpProxyPort();

  /// Check for SSL port.
  bool isSSL() const;

  /// Check for QUIC port.
  bool isQUIC() const;

  /// Check for SSL port.
  bool isPlugin() const;

  /// Process options text.
  /// @a opts should not contain any whitespace, only the option string.
  /// This object's internal state is updated as specified by @a opts.
  /// @return @c true if a port option was successfully processed, @c false otherwise.
  bool processOptions(const char *opts ///< String containing the options.
  );

  /** Global instance.

      This is provided because most of the work with this data is used as a singleton
      and it's handy to encapsulate it here.
  */
  static std::vector<self> &global();

  /// Check for SSL ports.
  /// @return @c true if any port in @a ports is an SSL port.
  static bool hasSSL(Group const &ports ///< Ports to check.
  );

  /// Check for SSL ports.
  /// @return @c true if any global port is an SSL port.
  static bool hasSSL();

  /// Check for QUIC ports.
  /// @return @c true if any port in @a ports is an QUIC port.
  static bool hasQUIC(Group const &ports ///< Ports to check.
  );

  /// Check for QUIC ports.
  /// @return @c true if any global port is an QUIC port.
  static bool hasQUIC();

  /** Load all relevant configuration data.

      This is hardwired to look up the appropriate values in the
      configuration files. It clears @a ports and then loads all found
      values in to it.

      @return @c true if at least one valid port description was
      found, @c false if none.
  */
  static bool loadConfig(std::vector<self> &ports ///< Destination for found port data.
  );

  /** Load all relevant configuration data into the global ports.

      @return @c true if at least one valid port description was
      found, @c false if none.
  */
  static bool loadConfig();

  /** Load ports from a value string.

      Load ports from single string with port descriptors. Ports
      found are added to @a ports. @a value may safely be @c nullptr or empty.

      @note This is used primarily internally but is available if needed.
      @return @c true if a valid port was found, @c false if none.
  */
  static bool loadValue(std::vector<self> &ports, ///< Destination for found port data.
                        const char        *value  ///< Source port data.
  );

  /** Load ports from a value string into the global ports.

      Load ports from single string of port descriptors into the
      global set of ports. @a value may safely be @c nullptr or empty.

      @return @c true if a valid port was found, @c false if none.
  */
  static bool loadValue(const char *value ///< Source port data.
  );

  /// Load default value if @a ports is empty.
  /// @return @c true if the default was needed / loaded.
  static bool loadDefaultIfEmpty(std::vector<self> &ports ///< Load target.
  );

  /// Load default value into the global set if it is empty.
  /// @return @c true if the default was needed / loaded.
  static bool loadDefaultIfEmpty();

  /** Find an HTTP port in @a ports.
      If @a family is specified then only ports for that family
      are checked.
      @return The port if found, @c nullptr if not.
  */
  static const self *findHttp(Group const &ports,             ///< Group to search.
                              uint16_t     family = AF_UNSPEC ///< Desired address family.
  );

  /** Find an HTTP port in the global ports.
      If @a family is specified then only ports for that family
      are checked.
      @return The port if found, @c nullptr if not.
  */
  static const self *findHttp(uint16_t family = AF_UNSPEC);

  /** Create text description to be used for inter-process access.
      Prints the file descriptor and then any options.

      @return The number of characters used for the description.
  */
  int print(char  *out, ///< Output string.
            size_t n    ///< Maximum output length.
  );

  static const char *const PORTS_CONFIG_NAME; ///< New unified port descriptor.

  /// Default value if no other values can be found.
  static const char *const DEFAULT_VALUE;

  // Keywords (lower case versions, but compares should be case insensitive)
  static const char *const OPT_FD_PREFIX;               ///< Prefix for file descriptor value.
  static const char *const OPT_OUTBOUND_IP_PREFIX;      ///< Prefix for inbound IP address.
  static const char *const OPT_INBOUND_IP_PREFIX;       ///< Prefix for outbound IP address.
  static const char *const OPT_IPV6;                    ///< IPv6.
  static const char *const OPT_IPV4;                    ///< IPv4
  static const char *const OPT_TRANSPARENT_INBOUND;     ///< Inbound transparent.
  static const char *const OPT_TRANSPARENT_OUTBOUND;    ///< Outbound transparent.
  static const char *const OPT_TRANSPARENT_FULL;        ///< Full transparency.
  static const char *const OPT_TRANSPARENT_PASSTHROUGH; ///< Pass-through non-HTTP.
  static const char *const OPT_ALLOW_PLAIN;             ///< Backup to plain HTTP.
  static const char *const OPT_SSL;                     ///< SSL (experimental)
  static const char *const OPT_QUIC;                    ///< QUIC (experimental)
  static const char *const OPT_PROXY_PROTO;             ///< Proxy Protocol
  static const char *const OPT_PLUGIN;                  ///< Protocol Plugin handle (experimental)
  static const char *const OPT_BLIND_TUNNEL;            ///< Blind tunnel.
  static const char *const OPT_COMPRESSED;              ///< Compressed.
  static const char *const OPT_HOST_RES_PREFIX;         ///< Set DNS family preference.
  static const char *const OPT_PROTO_PREFIX;            ///< Transport layer protocols.
  static const char *const OPT_MPTCP;                   ///< MPTCP.

  static std::vector<self> &m_global; ///< Global ("default") data.

protected:
  /// Process @a value for DNS resolution family preferences.
  void processFamilyPreference(const char *value);
  /// Process @a value for session protocol preferences.
  void processSessionProtocolPreference(const char *value);

  /** Check a prefix option and find the value.
      @return The address of the start of the value, or @c nullptr if the prefix doesn't match.
  */

  const char *checkPrefix(char const *src ///< Input text
                          ,
                          const char *prefix ///< Keyword prefix
                          ,
                          size_t prefix_len ///< Length of keyword prefix.
  );
};

inline bool
HttpProxyPort::isSSL() const
{
  return TRANSPORT_SSL == m_type;
}
inline bool
HttpProxyPort::isQUIC() const
{
  return TRANSPORT_QUIC == m_type;
}
inline bool
HttpProxyPort::isPlugin() const
{
  return TRANSPORT_PLUGIN == m_type;
}

inline bool
HttpProxyPort::loadValue(const char *value)
{
  return self::loadValue(m_global, value);
}
inline bool
HttpProxyPort::loadConfig()
{
  return self::loadConfig(m_global);
}
inline bool
HttpProxyPort::loadDefaultIfEmpty()
{
  return self::loadDefaultIfEmpty(m_global);
}
inline std::vector<HttpProxyPort> &
HttpProxyPort::global()
{
  return m_global;
}
inline bool
HttpProxyPort::hasSSL()
{
  return self::hasSSL(m_global);
}
inline bool
HttpProxyPort::hasQUIC()
{
  return self::hasQUIC(m_global);
}
inline const HttpProxyPort *
HttpProxyPort::findHttp(uint16_t family)
{
  return self::findHttp(m_global, family);
}

/** Session Protocol initialization.
    This must be called before any proxy port parsing is done.
*/
extern void ts_session_protocol_well_known_name_indices_init();

/** Convert the comma separated ALPN protocol list to wire format.
 *
 * For the definition of wire format, see the NOTES section in the OpenSSL
 * description of SSL_CTX_set_alpn_select_cb:
 *
 * https://www.openssl.org/docs/man1.1.1/man3/SSL_CTX_set_alpn_select_cb.html
 *
 * @param[in] protocols The comma separated list of protocols to convert to
 *   wire format.
 *
 * @param[out] wire_format_buffer The output ALPN wire format string converted
 *   from @a protocols. This is zero'd out if the conversion fails.
 *
 * @param[in,out] wire_format_buffer_len As an input, this is the size
 *   allocated for @a wire_format_buffer. As an output, this is set to the final
 *   size of @a wire_format_buffer after conversion. This is set to zero if the
 *   conversion fails.
 *
 * @return True if the conversion was successful, false otherwise. Note that
 * the wire format does not support an empty protocol list, therefore this
 * function returns false if @a protocols is an empty string.
 *
 * TODO: ideally this would take a swoc::TextView for @a protocols, but currently
 * swoc::TextView does not have a char* constructor while std::string_view does.
 * Once that is added, this can be seamlessly switched to a swoc::TextView.
 */
bool convert_alpn_to_wire_format(std::string_view protocols, unsigned char *wire_format_buffer, int &wire_format_buffer_len);
