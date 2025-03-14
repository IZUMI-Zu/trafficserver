.. Licensed to the Apache Software Foundation (ASF) under one
   or more contributor license agreements.  See the NOTICE file
   distributed with this work for additional information
   regarding copyright ownership.  The ASF licenses this file
   to you under the Apache License, Version 2.0 (the
   "License"); you may not use this file except in compliance
   with the License.  You may obtain a copy of the License at

   http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing,
   software distributed under the License is distributed on an
   "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
   KIND, either express or implied.  See the License for the
   specific language governing permissions and limitations
   under the License.

.. include:: ../../../../common.defs

.. _admin-stats-core-ssl:

SSL/TLS
*******

.. ts:stat:: global proxy.process.ssl.origin_server_bad_cert integer
   :type: counter

   Indicates the number of certificates presented by origin servers which
   contained invalid information, since statistics collection began.

.. ts:stat:: global proxy.process.ssl.origin_server_cert_verify_failed integer
   :type: counter

   The number of origin server SSL certificates presented which failed
   verification, since statistics collection began.

.. ts:stat:: global proxy.process.ssl.origin_server_decryption_failed integer
   :type: counter

   The number of SSL connections to origin servers which returned data that
   could not be properly decrypted, since statistics collection began.

.. ts:stat:: global proxy.process.ssl.origin_server_expired_cert integer
   :type: counter

   The number of SSL connections to origin servers for which expired origin
   certificates were presented, since statistics collection began.

.. ts:stat:: global proxy.process.ssl.origin_server_other_errors integer
   :type: counter

   The number of SSL connections to origin servers which encountered otherwise
   uncategorized errors, since statistics collection began.

.. ts:stat:: global proxy.process.ssl.origin_server_revoked_cert integer
   :type: counter

   The number of SSL connections to origin servers during which a revoked
   certificate was presented by the origin, since statistics collection began.

.. ts:stat:: global proxy.process.ssl.origin_server_unknown_ca integer
   :type: counter

   The number of SSL connections to origin servers during which the origin
   presented a certificate signed by an unrecognized Certificate Authority,
   since statistics collection began.

.. ts:stat:: global proxy.process.ssl.origin_server_unknown_cert integer
   :type: counter

.. ts:stat:: global proxy.process.ssl.origin_server_wrong_version integer
   :type: counter

   The number of SSL connections to origin servers which were terminated due to
   unsupported SSL/TLS protocol versions, since statistics collection began.

.. ts:stat:: global proxy.process.ssl.ssl_error_ssl integer
   :type: counter

.. ts:stat:: global proxy.process.ssl.ssl_error_syscall integer
   :type: counter

.. ts:stat:: global proxy.process.ssl.ssl_error_async integer
   :type: counter

   Track the number of times OpenSSL async jobs paused.

.. ts:stat:: global proxy.process.ssl.ssl_session_cache_eviction integer
   :type: counter

.. ts:stat:: global proxy.process.ssl.ssl_session_cache_hit integer
   :type: counter

.. ts:stat:: global proxy.process.ssl.ssl_origin_session_cache_hit integer
   :type: counter

.. ts:stat:: global proxy.process.ssl.ssl_session_cache_lock_contention integer
   :type: counter

.. ts:stat:: global proxy.process.ssl.ssl_session_cache_miss integer
   :type: counter

.. ts:stat:: global proxy.process.ssl.ssl_origin_session_cache_miss integer
   :type: counter

.. ts:stat:: global proxy.process.ssl.ssl_session_cache_new_session integer
   :type: counter

.. ts:stat:: global proxy.process.ssl.ssl_sni_name_set_failure integer
   :type: counter

.. ts:stat:: global proxy.process.ssl.total_handshake_time integer
   :type: counter
   :units: milliseconds

   The total amount of time spent performing SSL/TLS handshakes for new sessions
   since statistics collection began.

.. ts:stat:: global proxy.process.ssl.total_attempts_handshake_count_in integer
   :type: counter

   The total number of inbound SSL/TLS handshake attempts received since
   statistics collection began.

.. ts:stat:: global proxy.process.ssl.total_success_handshake_count_in integer
   :type: counter

   The total number of inbound SSL/TLS handshakes successfully performed since
   statistics collection began.

.. ts:stat:: global proxy.process.ssl.total_attempts_handshake_count_out integer
   :type: counter

   The total number of outbound SSL/TLS handshake attempts made since
   statistics collection began.

.. ts:stat:: global proxy.process.ssl.total_success_handshake_count_out integer
   :type: counter

   The total number of outbound SSL/TLS handshakes successfully performed since
   statistics collection began.

.. ts:stat:: global proxy.process.ssl.total_ticket_keys_renewed integer
   :type: counter

.. ts:stat:: global proxy.process.ssl.total_tickets_created integer
   :type: counter

.. ts:stat:: global proxy.process.ssl.total_tickets_not_found integer
   :type: counter

.. ts:stat:: global proxy.process.ssl.total_tickets_renewed integer
   :type: counter

.. ts:stat:: global proxy.process.ssl.total_tickets_verified integer
   :type: counter

.. ts:stat:: global proxy.process.ssl.total_tickets_verified_old_key integer
   :type: counter

.. ts:stat:: global proxy.process.ssl.user_agent_bad_cert integer
   :type: counter

   Incoming client SSL connections which have presented invalid data in lieu of
   a client certificate, since statistics collection began.

.. ts:stat:: global proxy.process.ssl.user_agent_cert_verify_failed integer
   :type: counter

   Incoming client SSL connections which presented a client certificate that did
   not pass verification, since statistics collection began.

.. ts:stat:: global proxy.process.ssl.user_agent_decryption_failed integer
   :type: counter

   Incoming client SSL connections which failed to be properly decrypted, since
   statistics collection began.

.. ts:stat:: global proxy.process.ssl.user_agent_expired_cert integer
   :type: counter

   Incoming client SSL connections which presented a client certificate that had
   already expired, since statistics collection began.

.. ts:stat:: global proxy.process.ssl.user_agent_other_errors integer
   :type: counter

   Incoming client SSL connections which experienced otherwise uncategorized
   errors, since statistics collection began.

.. ts:stat:: global proxy.process.ssl.user_agent_revoked_cert integer
   :type: counter

   Incoming client SSL connections which presented a client certificate that had
   been revoked, since statistics collection began.

.. ts:stat:: global proxy.process.ssl.user_agent_decryption_failed_or_bad_record_mac integer
   :type: counter

   Incoming client SSL connections which failed decryption or had a mismatched
   MAC, since statistics collection began.

.. ts:stat:: global proxy.process.ssl.user_agent_http_request integer
   :type: counter

   Incoming client SSL connections which attempted to use plaintext HTTP without
   SSL encryption, since statistics collection began.

.. ts:stat:: global proxy.process.ssl.user_agent_inappropriate_fallback integer
   :type: counter

   Incoming client SSL connections which used a fallback to an older TLS version
   that |TS| doesn't support, since statistics collection began.

.. ts:stat:: global proxy.process.ssl.user_agent_no_shared_cipher integer
   :type: counter

   Incoming client SSL connections which failed due to no match in supported
   ciphers between the client and |TS|, since statistics collection began.

.. ts:stat:: global proxy.process.ssl.user_agent_version_too_high integer
   :type: counter

   Incoming client SSL connections which failed due to the client only
   supporting TLS versions that are too high for |TS| to support, since
   statistics collection began.

   This stat is only incremented when |TS| is built against an SSL library, such
   as OpenSSL, that supports the ``SSL_R_VERSION_TOO_HIGH`` error.

.. ts:stat:: global proxy.process.ssl.user_agent_version_too_low integer
   :type: counter

   Incoming client SSL connections which failed due to the client only
   supporting TLS versions that are too low for |TS| to accept, since statistics
   collection began.

   This stat is only incremented when |TS| is built against an SSL library, such
   as OpenSSL, that supports the ``SSL_R_VERSION_TOO_LOW`` error.

.. ts:stat:: global proxy.process.ssl.user_agent_session_hit integer
   :type: counter

   Incoming client SSL connections which successfully used a previously
   negotiated session, since statistics collection began.

.. ts:stat:: global proxy.process.ssl.user_agent_session_miss integer
   :type: counter

   Incoming client SSL connections which unsuccessfully attempted to use a
   previously negotiated session, since statistics collection began.

.. ts:stat:: global proxy.process.ssl.user_agent_sessions integer
   :type: counter

   A counter indicating the number of SSL sessions negotiated for incoming
   client connections, since statistics collection began.

.. ts:stat:: global proxy.process.ssl.user_agent_session_timeout integer
   :type: counter

   Incoming client SSL connections which terminated with an expired session,
   since statistics collection began.

.. ts:stat:: global proxy.process.ssl.user_agent_unknown_ca integer
   :type: counter

   Incoming client SSL connections which presented a client certificate signed
   by an unrecognized Certificate Authority, since statistics collection began.

.. ts:stat:: global proxy.process.ssl.user_agent_unknown_cert integer
   :type: counter

.. ts:stat:: global proxy.process.ssl.user_agent_wrong_version integer
   :type: counter

   Incoming client SSL connections terminated due to an unsupported or disabled
   version of SSL/TLS, since statistics collection began.

.. ts:stat:: global proxy.process.tunnel.current_active_connections integer
   :type: gauge

   A gauge of current active SNI Routing Tunnels.

.. ts:stat:: global proxy.process.tunnel.total_client_connections_tls_tunnel integer
   :type: counter

   Total number of TCP connections for TLS tunnels where the far end is the client
   created based on a ``tunnel_route`` key in a table in the :file:`sni.yaml` file.

.. ts:stat:: global proxy.process.tunnel.current_client_connections_tls_tunnel integer
   :type: counter

   Current number of TCP connections for TLS tunnels where the far end is the client
   created based on a ``tunnel_route`` key in a table in the :file:`sni.yaml` file.

.. ts:stat:: global proxy.process.tunnel.total_client_connections_tls_forward integer
   :type: counter

   Total number of TCP connections for TLS tunnels where the far end is the client
   created based on a ``forward_route`` key in a table in the :file:`sni.yaml` file.

.. ts:stat:: global proxy.process.tunnel.current_client_connections_tls_forward integer
   :type: counter

   Current number of TCP connections for TLS tunnels where the far end is the client
   created based on a ``forward_route`` key in a table in the :file:`sni.yaml` file.

.. ts:stat:: global proxy.process.tunnel.total_client_connections_tls_partial_blind integer
   :type: counter

   Total number of TCP connections for TLS tunnels where the far end is the client
   created based on a ``partial_blind_route`` key in a table in the :file:`sni.yaml` file.

.. ts:stat:: global proxy.process.tunnel.current_client_connections_tls_partial_blind integer
   :type: counter

   Current number of TCP connections for TLS tunnels where the far end is the client
   created based on a ``partial_blind_route`` key in a table in the :file:`sni.yaml` file.

.. ts:stat:: global proxy.process.tunnel.total_client_connections_tls_http integer
   :type: counter

   Total number of TLS connections for tunnels where the far end is the client
   initiated with an HTTP request.

.. ts:stat:: global proxy.process.tunnel.current_client_connections_tls_http integer
   :type: counter

   Current number of TLS connections for tunnels where the far end is the client
   initiated with an HTTP request.

.. ts:stat:: global proxy.process.tunnel.total_server_connections_tls integer
   :type: counter

   Total number of TCP connections for TLS tunnels where the far end is the server
   created based on a ``partial_blind_route`` key in a table in the :file:`sni.yaml` file.

.. ts:stat:: global proxy.process.tunnel.current_server_connections_tls integer
   :type: counter

   Current number of TCP connections for TLS tunnels where the far end is the server
   created based on a ``partial_blind_route`` key in a table in the :file:`sni.yaml` file.

.. _pre-warming-tls-tunnel-stats:

Pre-warming TLS Tunnel
----------------------

Stats for Pre-warming TLS Tunnel is registered dynamically. The ``POOL`` in below represents combination of ``<Hostname of destination>.<Type of Tunnel>.<ALPN Name (if there)>``.

.. ts:stat:: global proxy.process.tunnel.prewarm.POOL.current_init integer
   :type: gauge

   Represents the current number of initializing connections in the pool.

.. ts:stat:: global proxy.process.tunnel.prewarm.POOL.current_open integer
   :type: gauge

   Represents the current number of established connections in the pool.

.. ts:stat:: global proxy.process.tunnel.prewarm.POOL.total_hit integer
   :type: counter

   Represents the total number of pre-warmed connection is used.

.. ts:stat:: global proxy.process.tunnel.prewarm.POOL.total_miss integer
   :type: counter

   Represents the total number of pre-warmed connection is not used.

.. ts:stat:: global proxy.process.tunnel.prewarm.POOL.total_handshake_time integer
   :type: counter

   Represents the total number of handshake duration of pre-warming.

.. ts:stat:: global proxy.process.tunnel.prewarm.POOL.total_handshake_count integer
   :type: counter

   Represents the total number of handshake time of pre-warming.

.. ts:stat:: global proxy.process.tunnel.prewarm.POOL.total_retry integer
   :type: counter

   Represents the total number of pre-warming retry.
