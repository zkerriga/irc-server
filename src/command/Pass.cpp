/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Pass.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zkerriga <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/20 12:07:17 by zkerriga          #+#    #+#             */
/*   Updated: 2021/01/20 12:07:24 by zkerriga         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Pass.hpp"
#include "Parser.hpp"

Pass::Pass() : ACommand("nouse", 0) {
	/* todo: default constructor */
}

Pass::Pass(const Pass & other) : ACommand("nouse", 0)  {
	/* todo: copy constructor */
	*this = other;
}

Pass::~Pass() {
	/* todo: destructor */
}

Pass & Pass::operator=(const Pass & other) {
	if (this != &other) {
		/* todo: operator= */
	}
	return *this;
}

std::string	eraseStrTillNextSpace(std::string & str) {
	std::string cuttedSubstr;
	cuttedSubstr = str.substr(0, str.find(' '));
	str.erase(0, str.find(' ') + 1);
	return cuttedSubstr;
}

ACommand::replies_container Pass::execute(Server & server) {
	_commandName = "pass";
	Parser::fillPrefix(_prefix, _rawCmd);
	/* todo: finish realization */
	(void)server;
	return ACommand::replies_container();
}

Pass::Pass(const std::string & rawCmd, const int senderFd)
	: ACommand(rawCmd, senderFd) {}

ACommand *Pass::create(const std::string & commandLine, const int senderFd) {
	return new Pass(commandLine, senderFd);
}

void Pass::_execute(Server & server) {

	// Check if sender exists
		// YES: Reply already exist
	// Check if udefined object exists
		// YES: discard command (2813 4.1.1)
	// Registrate undefined object

	(void)server;
}

