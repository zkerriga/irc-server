/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestForConnect.hpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: matrus <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/22 08:55:19 by matrus            #+#    #+#             */
/*   Updated: 2021/01/22 08:55:21 by matrus           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "types.hpp"
#include "ACommand.hpp"
#include <list>
#include "ISocketKeeper.hpp"
#include "BigLogger.hpp"
#include "Configuration.hpp"

class RequestForConnect : public ISocketKeeper {
public:
	enum e_request_types {
		REQUEST,
		SERVER,
		CLIENT
	};

	RequestForConnect(socket_type socket, const ACommand::command_prefix_t & prefix,
					  const std::string & password, const std::string & version,
					  const std::string & flags, const std::string & options,
					  const Configuration & conf);
	explicit RequestForConnect(socket_type socket, const Configuration & conf);
	virtual ~RequestForConnect();

	virtual socket_type	getSocket() const;
	time_t				getLastReceivedMsgTime() const;
	size_t				getHopCount() const;
	time_t				getTimeout() const;
	const std::string &	getPassword() const;

	bool				wasPassReceived() const;
	void				setPassReceived();

	void				registerAsClient(const ACommand::command_prefix_t & prefix,
										 const std::string & password);
	void				registerAsServer(const ACommand::command_prefix_t & prefix,
										 const std::string & password,
										 const std::string & version,
										 const std::string & flag,
										 const std::string & options);

private:
	friend class ServerInfo;

	RequestForConnect();
	RequestForConnect(RequestForConnect const & other);
	RequestForConnect & operator=(RequestForConnect const & other);

	socket_type					_socket;
	ACommand::command_prefix_t	_prefix;
	std::string 				_password;
	std::string 				_version;
	std::string 				_flags;
	std::string 				_options;

	time_t						_lastReceivedMsgTime;
	size_t						_hopCount;
	time_t						_timeout;

	bool						_wasPassCmdReceived;
	enum e_request_types		_type;
};
