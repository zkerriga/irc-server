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
#include "Configuration.hpp"

#include "mbedtls/net.h"
#include "mbedtls/ssl.h"
#include "mbedtls/entropy.h"
#include "mbedtls/ctr_drbg.h"
#include "mbedtls/certs.h"
#include "mbedtls/pk.h"
#include <map>

class SSLConnection {

	class SSLInfo {
	public:
		~SSLInfo();
		SSLInfo();

		mbedtls_net_context netContext;
		mbedtls_ssl_context sslContext;
	private:
		SSLInfo(const SSLInfo & other);
		SSLInfo & operator=(const SSLInfo & other);
	};

public:
	explicit SSLConnection(const Configuration & conf);
	~SSLConnection();

	void		init(const char * crtPath,
					 const char * keyPath,
					 const char * pass);
	socket_type	getListener() const;
	bool		isSSLSocket(socket_type sock);
	socket_type accept();
	ssize_t		recv(socket_type fd, unsigned char * buff, size_t maxLen);
	ssize_t		send(socket_type fd, const std::string & buff, size_t maxLen);
	void		erase(socket_type fd);

private:
	SSLConnection();
	SSLConnection(SSLConnection const & other);
	SSLConnection & operator=(SSLConnection const & other);

	void	_initEnvironment();
	void	_initRng();
	void	_initCertsAndPkey(const char * crtPath,
							  const char * keyPath,
							  const char * pass);
	void	_initAsServer();
	void	_initListening();

	bool	_sslContextInit(SSLInfo * sslInfo);
	bool	_performHandshake(SSLInfo * sslInfo);

	const std::string			_port;
	std::map<socket_type, SSLInfo *>	_connections;

	mbedtls_net_context			_listener;

	mbedtls_entropy_context		_entropy;
	mbedtls_ctr_drbg_context	_ctrDrbg;
	mbedtls_ssl_config			_conf;
	mbedtls_x509_crt			_serverCert;
	mbedtls_pk_context			_pkey;
};
