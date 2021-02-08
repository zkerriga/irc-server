/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SSLConnection.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: matrus <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/02/05 10:14:20 by matrus            #+#    #+#             */
/*   Updated: 2021/02/05 10:14:22 by matrus           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "types.hpp"
#include "mbedtls/net.h"
#include "mbedtls/ssl.h"
#include "mbedtls/entropy.h"
#include "mbedtls/ctr_drbg.h"
#include "mbedtls/certs.h"
#include "mbedtls/pk.h"
#include <map>

class SSLConnection {

	class sslInfo {
	public:
		sslInfo(const mbedtls_net_context & context,
				mbedtls_ctr_drbg_context & drbg,
				mbedtls_ssl_context & ssl,
				mbedtls_ssl_config & conf);
		~sslInfo();
		mbedtls_net_context netContext;
		mbedtls_ssl_context sslContext;
		mbedtls_ssl_config sslConfig;

		class SetupError : public std::exception {};
		class ConfigError : public std::exception {};
	private:
		sslInfo();
		sslInfo(const sslInfo & other);
		sslInfo & operator=(const sslInfo & other);
	};

public:

	SSLConnection();

	~SSLConnection();

	void		init();
	socket_type	getListener() const;
	socket_type accept();
	ssize_t		recv(unsigned char * buff, size_t maxLen);
	bool		isSSLSocket(socket_type sock);
	ssize_t		send(socket_type sock, const std::string & buff, size_t maxLen);

/* todo: add send/recv functions */

private:

	SSLConnection(SSLConnection const & sslconnection);
	SSLConnection & operator=(SSLConnection const & sslconnection);

	void	_initEnvironment();
	void	_initRng();
	void	_initCertsAndPkey();

	void	_netInit();
	void	_listen();
	void	_initAsServer();
	void	_sslInitAsClient(sslInfo * sslInfo);

	std::map<socket_type, sslInfo *>  _connections;

	mbedtls_net_context			_listener;

	mbedtls_entropy_context		_entropy;
	mbedtls_ctr_drbg_context	_ctrDrbg;
	mbedtls_ssl_context			_ssl;
	mbedtls_ssl_config			_conf;
	mbedtls_x509_crt			_serverCert;
	mbedtls_pk_context			_pkey;

};
