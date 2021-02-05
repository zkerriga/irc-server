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
#include "BigLogger.hpp"

SSLConnection::SSLConnection()
{}

SSLConnection::~SSLConnection()
{
	/* todo: delete map */
}

void SSLConnection::init()
{
	_netInit();
	_rngInit();
	_listen();
	_sslInitAsServer();
}

void SSLConnection::_netInit() {
	mbedtls_net_init(&_listenerSSL);
}

void SSLConnection::_rngInit()
{
	mbedtls_ctr_drbg_init(&_ctrDrbg);
	mbedtls_entropy_init(&_entropy);
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

void SSLConnection::_listen() {
	const std::string sslPort = "6697"; /* todo: default port by RFC 7194, but by checklist port should be PORT + 1 ??! */
	int ret = 0;

	if ((ret = mbedtls_net_bind(&_listenerSSL, nullptr, sslPort.c_str(), MBEDTLS_NET_PROTO_TCP)) != 0) {
		if (ret == MBEDTLS_ERR_NET_SOCKET_FAILED)
			throw std::runtime_error("SSL socket() failed");
		else if (ret == MBEDTLS_ERR_NET_BIND_FAILED)
			throw std::runtime_error("SSL bind() failed");
		else if (ret == MBEDTLS_ERR_NET_LISTEN_FAILED)
			throw std::runtime_error("SSL listen() failed");
		else
			throw std::runtime_error("SSL _listen() undefined error");
	}
	if ( (mbedtls_net_set_nonblock(&_listenerSSL)) != 0) {
		throw std::runtime_error("Unnable to set SSL socket noblock");
	}
}

void SSLConnection::_sslInitAsServer() {
	mbedtls_ssl_init(&_ssl);
	mbedtls_ssl_config_init(&_sslConf);

	if (mbedtls_ssl_config_defaults(&_sslConf,
										   MBEDTLS_SSL_IS_SERVER,
										   MBEDTLS_SSL_TRANSPORT_STREAM,
										   MBEDTLS_SSL_PRESET_DEFAULT) != 0)
	{
		throw std::runtime_error("SSL setting default configs failed");
	}

	mbedtls_ssl_conf_authmode(&_sslConf, MBEDTLS_SSL_VERIFY_NONE); /* todo: should we use some cert here? */
	mbedtls_ssl_conf_rng(&_sslConf, mbedtls_ctr_drbg_random, &_ctrDrbg);
	/* here can be debug function, see mbedtls_ssl_conf_dbg() */
	mbedtls_ssl_set_bio(&_ssl, &_listenerSSL, mbedtls_net_send, mbedtls_net_recv, nullptr);
	if (mbedtls_ssl_setup(&_ssl, &_sslConf) != 0) {
		throw std::runtime_error("SSL setup failed");
	}
}

socket_type SSLConnection::getListener() const {
	return _listenerSSL.fd;
}

ssize_t SSLConnection::send(socket_type sock, const std::string & buff, size_t maxLen)
{
	if (_connections.find(sock) == _connections.end()) {
		BigLogger::cout("Socket trying to send via SSL does not exist", BigLogger::RED);
		return -1;
	}
	int nBytes = mbedtls_ssl_write(&_connections[sock]->sslContext,
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
			/* todo: reload ssl (how?) */
		}
		return nBytes;
	}
	return nBytes;
}

ssize_t SSLConnection::recv(unsigned char * buff, size_t maxLen)
{
	int nBytes = mbedtls_ssl_read(&_ssl, buff, maxLen); /* recv only get from already established connection (listener) */
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
		return nBytes;
	}
	return nBytes;
}

bool SSLConnection::isSSLSocket(socket_type sock) {
	return sock == _listenerSSL.fd || (_connections.find(sock) != _connections.end());
}

socket_type SSLConnection::accept() {
	mbedtls_net_context newContext;
	struct sockaddr_storage		remoteAddr = {};
	socklen_t					addrLen = sizeof(remoteAddr);
	size_t						addrLenWritten = 0;

	int ret = mbedtls_net_accept(&_listenerSSL, &newContext, &remoteAddr, addrLen, &addrLenWritten);
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
		return -1;
	}
	try {
		_connections[newContext.fd] = new sslInfo(newContext, _ctrDrbg);
	} catch (SSLConnection::sslInfo::SetupError &) {
		BigLogger::cout("SSL setup error", BigLogger::YELLOW);
		return -1;
	} catch (SSLConnection::sslInfo::ConfigError &) {
		BigLogger::cout("SSL config error", BigLogger::YELLOW);
		return -1;
	}

	/* todo: make new ssl context by accepted context */
	return newContext.fd;
}

SSLConnection::sslInfo::sslInfo(const mbedtls_net_context & context, mbedtls_ctr_drbg_context & drbg)
	: netContext(context)
{
	mbedtls_ssl_init(&sslContext);
	mbedtls_ssl_config_init(&sslConfig);

	if (mbedtls_ssl_config_defaults(&sslConfig,
									 MBEDTLS_SSL_IS_CLIENT,
									 MBEDTLS_SSL_TRANSPORT_STREAM,
									 MBEDTLS_SSL_PRESET_DEFAULT) != 0)
	{
		throw ConfigError();
	}

	mbedtls_ssl_conf_authmode(&sslConfig, MBEDTLS_SSL_VERIFY_NONE); /* todo: should we use some cert here? */
	mbedtls_ssl_conf_rng(&sslConfig, mbedtls_ctr_drbg_random, &drbg);
	/* here can be debug function, see mbedtls_ssl_conf_dbg() */
	mbedtls_ssl_set_bio(&sslContext, &netContext, mbedtls_net_send, mbedtls_net_recv, nullptr);
	if (mbedtls_ssl_setup(&sslContext, &sslConfig) != 0) {
		throw SetupError();
	}
}

SSLConnection::sslInfo::~sslInfo()
{
	/* todo: destroy struct */
}
