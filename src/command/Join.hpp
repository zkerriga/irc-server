/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Join.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zkerriga <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/20 12:07:17 by zkerriga          #+#    #+#             */
/*   Updated: 2021/01/20 12:07:18 by zkerriga         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <string>
#include <vector>

#include "ACommand.hpp"
#include "Parser.hpp"
#include "ReplyList.hpp"

namespace Pars {

	typedef enum e_argument_parsing_result {
		SUCCESS,
		ERROR,
		SKIP_ARGUMENT,
		CRITICAL_ERROR
	} parsing_result_type;

	template <class CommandClass>
	struct parsing_unit_type {
		typedef parsing_result_type (CommandClass::*parsing_method_type)(
				const IServerForCmd & server,
				const std::string &
		);
		parsing_method_type		parser;
		bool					required;
	};

	/**
	 * @Info
	 * Данная функция предназначена для чёткой работы с аргументами
	 * команд. Она использует конкретные обработчики параметров и следит
	 * за их количеством, создавая корректные ошибки.
	 * Функция не генерирует исключения, но передаваемые обработчики могут
	 * это сделать.
	 * @tparam CommandClass
	 * Любой класс, реализующий абстрактный класс ACommand
	 * @param arguments
	 * Разделённые аргументы команды
	 * @param parsers
	 * Обработчики, которые занимаются валидацией и
	 * инициализацией полей в объекте команды. Обработчики также могут
	 * заполнять ответы.
	 * Обработчики должны в возращаемом значении должны указывать на ошибки и
	 * их критичность.
	 * @param commandObjectPointer
	 * Объект команды, которая вызывает функцию.
	 * @param senderReplies
	 * Референс на строку, которая будет отправлена
	 * источнику команды.
	 * @return Функция возвращает true, если аргументы представлены в
	 * правильном количестве и являются валидными. Иначе возвращает false.
	 */
	template <class CommandClass>
	bool	argumentsParser(const IServerForCmd & server,
							const Parser::arguments_array & arguments,
							const parsing_unit_type<CommandClass> * parsers,
							CommandClass * commandObjectPointer,
							std::string & senderReplies) {
		Parser::arguments_array::const_iterator		it	= arguments.begin();
		Parser::arguments_array::const_iterator		ite	= arguments.end();
		e_argument_parsing_result					ret;
		bool										status = true;

		for (size_t i = 0; parsers[i].parser; ++i) {
			if (it == ite) {
				if (parsers[i].required) { /* todo: обязательные дальше */
					senderReplies.append(server.getServerPrefix() + " " + errNeedMoreParams(CommandClass::commandName));
					return false;
				}
				break;
			}
			ret = (commandObjectPointer->*(parsers[i].parser))(server, *it);
			if (ret == SUCCESS) {
				++it;
			}
			else if (ret == ERROR) {
				status = false;
				++it;
			}
			else if (ret == CRITICAL_ERROR) {
				return false;
			}
			else if (ret == SKIP_ARGUMENT && parsers[i].required) {
				return false;
			}
		}
		return status;
	}

} //namespase Pars


class Join : public ACommand {
public:
	static const char * const		commandName;

	~Join();
	Join(const std::string & commandLine, socket_type senderFd);

	static
	ACommand *	create(const std::string & commandLine, socket_type senderFd);
	virtual replies_container	execute(IServerForCmd & server);

private:
	Join();
	Join(const Join & other);
	Join & operator= (const Join & other);

	bool		_isParamsValid(const IServerForCmd & server);
	void		_execute(IServerForCmd & server);

	static const Pars::parsing_unit_type<Join>	_parsers[];
	Pars::parsing_result_type	_prefixParser(const IServerForCmd & server, const std::string & prefixArgument);
	Pars::parsing_result_type	_commandNameParser(const IServerForCmd & server, const std::string & commandArgument);
	Pars::parsing_result_type	_channelsParser(const IServerForCmd & server, const std::string & channelsArgument);
	Pars::parsing_result_type	_passwordsParser(const IServerForCmd & server, const std::string & passwordsArgument);

	std::vector<std::pair<std::string, std::string> >	_channels;
};
