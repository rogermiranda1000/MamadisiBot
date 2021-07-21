#pragma once

#include <iostream> // print the error messages
#include <mariadb/mysql.h>
#include "sleepy_discord/sleepy_discord.h"

class MamadisiBot : public SleepyDiscord::DiscordClient {
public:
	~MamadisiBot();

	using SleepyDiscord::DiscordClient::DiscordClient;
	void onMessage(SleepyDiscord::Message message) override;
	void connect(const char *ip, unsigned int port, const char *user, const char *password, const char *database);

private:
	MYSQL *_conn;

	void react(SleepyDiscord::Message message, const char *emoji);
	void sendMessage(SleepyDiscord::Snowflake<SleepyDiscord::Channel> channel, const char *msg);
	void sendImage(SleepyDiscord::Snowflake<SleepyDiscord::Channel> channel, const char *msg, const char *img);
};
