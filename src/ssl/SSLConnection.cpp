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

SSLConnection::SSLConnection()
{}

SSLConnection::~SSLConnection()
{

}

void SSLConnection::init()
{
	_netInit();
	_rngInit();
	_listen();
	_sslInit();
}

void SSLConnection::_netInit() {
	mbedtls_net_init(&_listenerSSL);
}

void SSLConnection::_rngInit()
{
	mbedtls_ctr_drbg_init(&_ctrDrbg);
	mbedtls_entropy_init(&_entropy);
	const std::string seed = "JUST another random seed%^&TYU";
	if ((mbedtls_ctr_drbg_seed(&_ctrDrbg,
							   mbedtls_entropy_func,
							   &_entropy,
							   reinterpret_cast<const unsigned char *>(seed.c_str()),
							   seed.size()) ) != 0 )
	{
		/* todo: catch throw */
		throw std::runtime_error("SSL failed to generate drbg seed");
	}
}

void SSLConnection::_listen() {
	const std::string sslPort = "6697"; /* todo: default port by RFC 7194, but by checklist port should be PORT + 1 ??! */
	int ret = 0;

	if ((ret = mbedtls_net_bind(&_listenerSSL, nullptr, sslPort.c_str(), MBEDTLS_NET_PROTO_TCP)) != 0) {
		/* todo: catch throw */
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

void SSLConnection::_sslInit() {
	mbedtls_ssl_init(&_ssl);
	mbedtls_ssl_config_init(&_sslConf);

	if ((mbedtls_ssl_config_defaults(&_sslConf,
										   MBEDTLS_SSL_IS_SERVER,
										   MBEDTLS_SSL_TRANSPORT_STREAM,
										   MBEDTLS_SSL_PRESET_DEFAULT) ) != 0)
	{
		/* todo: catch throw */
		throw std::runtime_error("SSL setting default configs failed");
	}

	mbedtls_ssl_conf_authmode(&_sslConf, MBEDTLS_SSL_VERIFY_NONE); /* todo: should we use some cert here? */
	mbedtls_ssl_conf_rng(&_sslConf, mbedtls_ctr_drbg_random, &_ctrDrbg);
	/* here can be debug function, see mbedtls_ssl_conf_dbg() */
	mbedtls_ssl_set_bio(&_ssl, &_listenerSSL, mbedtls_net_send, mbedtls_net_recv, nullptr);
	if ((mbedtls_ssl_setup(&_ssl, &_sslConf) != 0)) {
		throw std::runtime_error("SSL setup failed");
	}
}

socket_type SSLConnection::getListener() const {
	return _listenerSSL.fd;
}

ssize_t SSLConnection::send(socket_type sock, const std::string & buff, size_t maxLen)
{
	if (_connections.find(sock) == _connections.end()) {
		/* todo: BigLogger::cout("Socket trying to send via SSL does not exist", BigLogger::RED); */
		return -1;
	}
	mbedtls_ssl_context sslContext = _connections[sock];

	int nBytes = mbedtls_ssl_write(&sslContext,
							reinterpret_cast<const unsigned char *>(buff.c_str()),
							std::min(buff.size(), maxLen));
	if (nBytes < 0) {
		if (nBytes == MBEDTLS_ERR_SSL_WANT_WRITE) {
			/* todo: BigLogger::cout("SSL_WANT_WRITE event happen in _ssl.send()", BigLogger::YELLOW); */
		}
		else if (nBytes == MBEDTLS_ERR_SSL_WANT_READ) {
			/* todo: BigLogger::cout("SSL_WANT_READ event happen in _ssl.send()", BigLogger::YELLOW); */
		}
		else {
			/* todo: BigLogger::cout("Undefined error happen in ssl_write()", BigLogger::RED); */
			/* todo: reload ssl (how?) */
		}
		return nBytes;
	}
	return nBytes;
}

ssize_t SSLConnection::recv(socket_type sock, unsigned char * buff, size_t maxLen)
{
	if (_connections.find(sock) == _connections.end()) {
		/* todo: BigLogger::cout("Socket trying to recv via SSL does not exist", BigLogger::RED); */
		return -1;
	}
	mbedtls_ssl_context sslContext = _connections[sock];
	int nBytes = mbedtls_ssl_read(&sslContext, buff, maxLen);
	if (nBytes < 0) {
		if (nBytes == MBEDTLS_ERR_SSL_WANT_READ) {
			/* todo: BigLogger::cout("SSL_WANT_READ event happen in _ssl.recv()", BigLogger::YELLOW); */
		}
		else if (nBytes == MBEDTLS_ERR_SSL_WANT_WRITE) {
			/* todo: BigLogger::cout("SSL_WANT_WRITE event happen in _ssl.recv()", BigLogger::YELLOW); */
		}
		else if (nBytes == EOF) {
			/* todo: BigLogger::cout("EOF event happen in _ssl.recv()", BigLogger::RED); */
			/* todo: ssl_reconnect() ??*/
		}
		return nBytes;
	}
	return nBytes;
}

