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

#include "mbedtls/net.h"
#include "mbedtls/ssl.h"
#include "mbedtls/entropy.h"
#include "mbedtls/ctr_drbg.h"

class SSLConnection {

public:

	SSLConnection();

	~SSLConnection();

	void	init();
/* todo: add send/recv functions */

private:

	SSLConnection(SSLConnection const & sslconnection);
	SSLConnection & operator=(SSLConnection const & sslconnection);

	void	_netInit();
	void	_rngInit();
	void	_listen();
	void	_sslInit();

	mbedtls_net_context		_listenerSSL;

	mbedtls_entropy_context		_entropy;
	mbedtls_ctr_drbg_context	_ctrDrbg;
	mbedtls_ssl_context			_ssl;
	mbedtls_ssl_config			_sslConf;

};
