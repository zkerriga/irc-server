/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zkerriga <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/21 10:02:29 by zkerriga          #+#    #+#             */
/*   Updated: 2021/01/21 10:02:30 by zkerriga         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <map>
#include <string>
#include <queue>

#include "ACommand.hpp"
#include "ReplyList.hpp"

class Parser {
public:
	typedef std::map<socket_type, std::string>	receive_container;
	typedef std::queue<ACommand *>				commands_container;
	typedef std::vector<std::string>			arguments_array;
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
	 * @info
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
	static bool	argumentsParser(const IServerForCmd & server,
								const arguments_array & arguments,
								const parsing_unit_type<CommandClass> * parsers,
								CommandClass * commandObjectPointer,
								std::string & senderReplies) {
		arguments_array::const_iterator		it		= arguments.begin();
		arguments_array::const_iterator		ite		= arguments.end();
		bool								status	= true;
		e_argument_parsing_result			ret;

		for (; parsers->parser; ++parsers) {
			if (it == ite) {
				if (_checkRequired(parsers)) {
					senderReplies.append(server.getServerPrefix() + " " + errNeedMoreParams(CommandClass::commandName));
					return false;
				}
				break;
			}
			ret = (commandObjectPointer->*(parsers->parser))(server, *it);
			if (ret == SUCCESS) {
				++it;
			}
			else if (ret == ERROR) {
				status = false;
				++it;
			}
			else if (ret == CRITICAL_ERROR
					 || (ret == SKIP_ARGUMENT && parsers->required)) {
				return false;
			}
		}
		return status;
	}

private:
	template <class CommandClass>
	static bool	_checkRequired(const parsing_unit_type<CommandClass> * const parsers) {
		for (const parsing_unit_type<CommandClass> * i = parsers; i->parser; ++i) {
			if (i->required) {
				return true;
			}
		}
		return false;
	}

public:
	struct pair_name_construct {
		const char *	commandName;
		ACommand *		(*create)(const std::string &, const socket_type);
	};

	static const pair_name_construct	all[];
	static const char *					crlf;
	static const char					space;

	Parser();
	Parser(const Parser & other);
	~Parser();
	Parser & operator= (const Parser & other);

	commands_container		getCommandsContainerFromReceiveMap(receive_container & receiveBuffers);
	static std::string		toUpperCase(const std::string & str);

	static arguments_array	splitArgs(const std::string & strIn);
	static void				fillPrefix(ACommand::command_prefix_t & prefix, std::string const & cmd);
	static bool				isPrefix(const std::string & line);
	static bool				safetyStringToUl(size_t & dest, const std::string & str);
	static std::string		copyStrFromCharToChar(const std::string & str, char from, char to);
	static bool				isNumericString(const std::string & str);
	static bool				isNameValid(const std::string & name, const Configuration & conf);

private:
	static inline bool		_messageIsFull(const std::string & message);
	static char				_charToUpper(char c);
	static std::string		_getCommandNameByMessage(std::string message);
	static ACommand *		_getCommandObjectByName(const std::string & commandName, const std::string & cmdMessage, socket_type fd);
	static std::string		_extractMessage(receive_container::iterator & it);
};

