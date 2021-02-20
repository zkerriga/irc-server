#include "gtest/gtest.h"

#include "ACommand.hpp"
#include "Modes.hpp"
#include "StandardChannel.hpp"
#include "User.hpp"

TEST(channels, basic) {
	const char *	av[] = {"./ircserv", "6668", "pass"};
	Configuration conf(3, av);
	ServerInfo *	info = new ServerInfo(1, "matrus.com", 1, conf);
	IClient *		creator = new User(1, "zkerriga", 0, "username", "localhost", 1, "", "real", info, conf);
	IChannel *		channel = new StandardChannel("#name", creator, conf);

	IClient *		user1 = new User(2, "cgarth", 0, "username2", "localhost", 1, "", "real2", info, conf);

	ASSERT_TRUE(channel->isOnChannel(creator));
	ASSERT_FALSE(channel->isOnChannel(user1));

	channel->join(user1);
	ASSERT_TRUE(channel->isOnChannel(user1));

	channel->part(user1);
	ASSERT_FALSE(channel->isOnChannel(user1));

	ASSERT_EQ(creator, channel->findClient("zkerriga"));
	ASSERT_EQ(nullptr, channel->findClient("abcdef"));

	delete info;
	delete creator;
	delete channel;
}
