/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SSLConnection.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: matrus <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/02/05 10:14:18 by matrus            #+#    #+#             */
/*   Updated: 2021/02/05 10:14:22 by matrus           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "SSLConnection.hpp"
#include <string>
#include <stdexcept>
#include <sys/socket.h>
#include "mbedtls/programs/ssl/ssl_test_lib.h"
#include "mbedtls/debug.h"
#include "BigLogger.hpp"
#include "tools.hpp"
#include "debug.hpp"

SSLConnection::SSLConnection() : _port() {}

SSLConnection::~SSLConnection() {
	mbedtls_net_close(&_listener);
	mbedtls_net_free(&_listener);
	mbedtls_ssl_config_free(&_conf);
	mbedtls_x509_crt_free(&_serverCert);
	mbedtls_pk_free(&_pkey);
	mbedtls_ctr_drbg_free(&_ctrDrbg);
	mbedtls_entropy_free(&_entropy);

	_connections.clear();
}

void SSLConnection::init(const char * const crtPath,
						 const char * const keyPath,
						 const char * const pass) {
	_initEnvironment();
	_initRng();
	_initCertsAndPkey(crtPath, keyPath, pass);
	_initAsServer();
	_initListening();
}

void SSLConnection::_initEnvironment() {
	mbedtls_entropy_init( &_entropy );
	mbedtls_ctr_drbg_init( &_ctrDrbg );
	mbedtls_pk_init( &_pkey );
	mbedtls_x509_crt_init( &_serverCert );
	mbedtls_ssl_config_init( &_conf );
	mbedtls_net_init( &_listener );
}

void SSLConnection::_initRng() {
	const std::string seed = "JUST another random seed%^&TYU";
	if (mbedtls_ctr_drbg_seed(&_ctrDrbg,
							   mbedtls_entropy_func,
							   &_entropy,
							   reinterpret_cast<const unsigned char *>(seed.c_str()),
							   seed.size())
		!= 0 )
	{
		throw std::runtime_error("SSL failed to generate drbg seed");
	}
}

void SSLConnection::_initCertsAndPkey(const char * const crtPath,
									  const char * const keyPath,
									  const char * const pass) {
	int ret;
	ret = mbedtls_x509_crt_parse_file(&_serverCert, crtPath);
	if (ret != 0) {
		throw std::runtime_error("mbedtls_x509_crt_parse failed");
	}
	ret = mbedtls_pk_parse_keyfile(&_pkey, keyPath, pass);
	if (ret != 0) {
		throw std::runtime_error("mbedtls_pk_parse_key failed");
	}
}

#ifdef DEBUG_LVL

void my_debug( void *ctx, int level,
					  const char *file, int line,
					  const char *str )
{
	((void) level);

	mbedtls_fprintf( (FILE *) ctx, "%s:%04d: %s", file, line, str );
	fflush(  (FILE *) ctx  );
}

#endif

void SSLConnection::_initAsServer() {
	if (mbedtls_ssl_config_defaults(&_conf,
									MBEDTLS_SSL_IS_SERVER,
									MBEDTLS_SSL_TRANSPORT_STREAM,
									MBEDTLS_SSL_PRESET_DEFAULT) != 0)
	{
		throw std::runtime_error("SSL setting default configs failed");
	}

	mbedtls_ssl_conf_rng(&_conf, mbedtls_ctr_drbg_random, &_ctrDrbg);
	//	mbedtls_ssl_conf_dbg( &_conf, my_debug, stdout );
	/* here can be debug function, see mbedtls_ssl_conf_dbg() */
	mbedtls_ssl_conf_ca_chain( &_conf, &_serverCert, nullptr );
	if(mbedtls_ssl_conf_own_cert( &_conf, &_serverCert, &_pkey ) != 0 ) {
		throw std::runtime_error("mbedtls_ssl_conf_own_cert failed");
	}
#ifdef DEBUG_LVL
	mbedtls_ssl_conf_dbg(&_conf, my_debug, stdout);
	mbedtls_debug_set_threshold(1);
#endif
}

void SSLConnection::_initListening() {
	int ret = 0;
	if ((ret = mbedtls_net_bind(&_listener, nullptr, _port.c_str(), MBEDTLS_NET_PROTO_TCP)) != 0) {
		if (ret == MBEDTLS_ERR_NET_SOCKET_FAILED)
			throw std::runtime_error("SSL socket() failed");
		else if (ret == MBEDTLS_ERR_NET_BIND_FAILED)
			throw std::runtime_error("SSL bind() failed");
		else if (ret == MBEDTLS_ERR_NET_LISTEN_FAILED)
			throw std::runtime_error("SSL listen() failed");
		else
			throw std::runtime_error("SSL _initListening() undefined error");
	}
	if (mbedtls_net_set_nonblock(&_listener) != 0) {
		throw std::runtime_error("Unnable to set SSL socket noblock");
	}
}

socket_type SSLConnection::getListener() const {
	return _listener.fd;
}

ssize_t SSLConnection::send(socket_type fd, const std::string & buff, size_t maxLen) {
	if (_connections.find(fd) == _connections.end()) {
		BigLogger::cout("Socket trying to send via SSL does not exist", BigLogger::RED);
		return -1;
	}

	int nBytes = mbedtls_ssl_write(&_connections[fd]->sslContext,
							reinterpret_cast<const unsigned char *>(buff.c_str()),
							std::min(buff.size(), maxLen));
	if (nBytes < 0) {
		if (nBytes == MBEDTLS_ERR_SSL_WANT_WRITE) {
			BigLogger::cout("SSL_WANT_WRITE event happen in _ssl.send()", BigLogger::YELLOW);
		}
		else if (nBytes == MBEDTLS_ERR_SSL_WANT_READ) {
			BigLogger::cout("SSL_WANT_READ event happen in _ssl.send()", BigLogger::YELLOW);
		}
		else {
			BigLogger::cout("Undefined error happen in ssl_write()", BigLogger::RED);
			BigLogger::cout("Consider to close this connection", BigLogger::RED);
			/* todo: reload ssl (how?) */
		}
		return nBytes;
	}
	return nBytes;
}

ssize_t SSLConnection::recv(socket_type fd, unsigned char * buff, size_t maxLen) {
	if (_connections.find(fd) == _connections.end()) {
		BigLogger::cout("Socket trying to recv via SSL does not exist", BigLogger::RED);
		return -1;
	}

	int nBytes = mbedtls_ssl_read(&_connections[fd]->sslContext, buff, maxLen);
	if (nBytes < 0) {
		if (nBytes == MBEDTLS_ERR_SSL_WANT_READ) {
			BigLogger::cout("SSL_WANT_READ event happen in _ssl.recv()", BigLogger::YELLOW);
		}
		else if (nBytes == MBEDTLS_ERR_SSL_WANT_WRITE) {
			BigLogger::cout("SSL_WANT_WRITE event happen in _ssl.recv()", BigLogger::YELLOW);
		}
		else if (nBytes == EOF) {
			BigLogger::cout("EOF event happen in _ssl.recv()", BigLogger::RED);
			/* todo: ssl_reconnect() ??*/
		}
		else if (nBytes == MBEDTLS_ERR_SSL_CLIENT_RECONNECT) {
			BigLogger::cout("MBEDTLS_ERR_SSL_CLIENT_RECONNECT event happen in _ssl.recv()", BigLogger::YELLOW);
		}
		else {
			BigLogger::cout("Undefined error happen in ssl_read()", BigLogger::RED);
			BigLogger::cout("Consider to close this connection", BigLogger::RED);
			/* todo: ssl_reconnect() ??*/
		}
		return nBytes;
	}
	return nBytes;
}

bool SSLConnection::isSSLSocket(socket_type sock) {
	return sock == _listener.fd || (_connections.find(sock) != _connections.end());
}

socket_type SSLConnection::accept() {
	struct sockaddr_storage		remoteAddr = {};
	socklen_t					addrLen = sizeof(remoteAddr);
	size_t						addrLenWritten = 0;

	SSLInfo * newSSLInfo = new SSLInfo();
	int ret = mbedtls_net_accept(&_listener, &newSSLInfo->netContext, &remoteAddr, addrLen, &addrLenWritten);
	if (ret != 0) {
		if (ret == MBEDTLS_ERR_NET_ACCEPT_FAILED) {
			BigLogger::cout("SSL accept() failed", BigLogger::YELLOW);
		}
		else if (ret == MBEDTLS_ERR_NET_BUFFER_TOO_SMALL) {
			BigLogger::cout("SSL accept() failed: buffer to small o_O", BigLogger::YELLOW);
		}
		else if (ret == MBEDTLS_ERR_SSL_WANT_READ) {
			BigLogger::cout("SSL accept() failed: SSL_WANT_READ event happen", BigLogger::YELLOW);
		}
		delete newSSLInfo;
		return -1;
	}
	if ( !  (_sslContextInit(newSSLInfo)
	     || _performHandshake(newSSLInfo)) ) {
		delete newSSLInfo;
		return -1;
	}
	_connections[newSSLInfo->netContext.fd] = newSSLInfo;
	return newSSLInfo->netContext.fd;
}

bool SSLConnection::_sslContextInit(SSLConnection::SSLInfo * sslInfo) {
	if (mbedtls_ssl_setup(&sslInfo->sslContext, &_conf) != 0) {
		BigLogger::cout("SSL setup error", BigLogger::YELLOW);
		return false;
	}

	mbedtls_ssl_set_bio(&sslInfo->sslContext, &sslInfo->netContext, mbedtls_net_send, mbedtls_net_recv, nullptr);

	return true;
}

bool SSLConnection::_performHandshake(SSLConnection::SSLInfo * sslInfo) {
	int ret;
	while (	(ret = mbedtls_ssl_handshake(&sslInfo->sslContext)) != 0) {
		if (ret == MBEDTLS_ERR_SSL_HELLO_VERIFY_REQUIRED) {
			BigLogger::cout("Handshake hello required!", BigLogger::RED);
		}
		if( ret != MBEDTLS_ERR_SSL_WANT_READ && ret != MBEDTLS_ERR_SSL_WANT_WRITE ) {
			BigLogger::cout("Handshake failed!", BigLogger::RED);
			return false;
		}
	}
	return true;
}

void SSLConnection::erase(socket_type fd) {
	_connections.erase(fd);
}

SSLConnection::SSLConnection(const Configuration & conf)
	: _port(conf.getTslPort()) {}

SSLConnection::SSLInfo::~SSLInfo() {
	mbedtls_net_free(&netContext);
	mbedtls_ssl_free(&sslContext);
}

SSLConnection::SSLInfo::SSLInfo() {
	mbedtls_ssl_init(&sslContext);
}
