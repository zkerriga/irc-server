/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CommandLogger.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zkerriga <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/19 09:58:35 by zkerriga          #+#    #+#             */
/*   Updated: 2021/01/19 09:58:43 by zkerriga         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <map>

class CommandLogger {
public:
	CommandLogger();
	CommandLogger(const CommandLogger & other);
	~CommandLogger();
	CommandLogger & operator= (const CommandLogger & other);

	void		incExecLocal(const std::string & cmdName);
	void		incExecRemote(const std::string & cmdName);
	void		incBytesGenerated(const std::string & cmdName, size_t bytes);

private:
	struct log_command_t {
		size_t execsLocal;
		size_t bytesGenerated;
		size_t execsRemote;
	};

	std::map<std::string, log_command_t> _db;
};

