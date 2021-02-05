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
#include <map>

class SSLConnection {

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

	void	_netInit();
	void	_rngInit();
	void	_listen();
	void	_sslInit();


	std::map<socket_type, mbedtls_ssl_context>  _connections;

	mbedtls_net_context		_listenerSSL;

	mbedtls_entropy_context		_entropy;
	mbedtls_ctr_drbg_context	_ctrDrbg;
	mbedtls_ssl_context			_ssl;
	mbedtls_ssl_config			_sslConf;

};
