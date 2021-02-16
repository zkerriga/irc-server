/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Modes.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zkerriga <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/19 09:56:59 by zkerriga          #+#    #+#             */
/*   Updated: 2021/01/19 09:57:03 by zkerriga         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <string>

class Modes {
public:
	Modes(const Modes & other);
	Modes & operator=(const Modes & other);
	~Modes();

	Modes(const std::string & available);

	virtual bool	set(char mode);
	virtual void	reset();
	virtual void	unset(char mode);
	virtual bool	check(char mode) const;
	virtual bool	checkAll(const std::string & modes) const;
	virtual bool	parse(const std::string & modesActions);
	std::string 	toString() const;

private:
	Modes();

	bool			_isAvailable(char mode) const;
	void			_set(char mode);

	const std::string	_available;
	std::string			_modes;
};

struct UserChannelPrivileges {
	static const char	mCreator	= 'O'; // give "channel creator" status;
	static const char	mOperator	= 'o'; // give/take channel operator privilege;
	static const char	mVoice		= 'v'; // give/take the voice privilege;

	static Modes *				create();
	static const std::string	createAsString();
};

struct UserMods {
	static const char	mAway		= 'a'; // user is flagged as away;
	static const char	mInvisible	= 'i'; // marks a users as invisible;
	static const char	mWallops	= 'w'; // user receives wallops;
	static const char	mRestricted	= 'r'; // restricted user connection;
	static const char	mOperator	= 'o'; // operator flag;
	static const char	mLocalOper	= 'O'; // local operator flag;
	static const char	mReceipt	= 's'; // marks a user for receipt of server notices

	static Modes *				create();
	static const std::string	createAsString();
};

struct ChannelMods {
	static const char	mAnonymous	= 'a'; // toggle the anonymous channel flag
	static const char	mInviteOnly	= 'i'; // toggle the invite-only channel flag
	static const char	mModerated	= 'm'; // toggle the moderated channel
	static const char	mNoOutside	= 'n'; // toggle the no messages to channel from clients on the outside
	static const char	mQuiet		= 'q'; // toggle the quiet channel flag
	static const char	mPrivate	= 'p'; // toggle the private channel flag
	static const char	mSecret		= 's'; // toggle the secret channel flag
	static const char	mReop		= 'r'; // toggle the server reop channel flag
	static const char	mTopicOper	= 't'; // toggle the topic settable by channel operator only flag

	static const char	mKey		= 'k'; // set/remove the channel key (password)
	static const char	mLimit		= 'l'; // set/remove the user limit to channel
	static const char	mBanMask	= 'b'; // set/remove ban mask to keep users out
	static const char	mExceptMask	= 'e'; // set/remove an exception mask to override a ban mask
	static const char	mInviteMask	= 'I'; // set/remove an invitation mask to automatically override the invite-only flag

	static Modes *				create();
	static const std::string	createAsString();
};
