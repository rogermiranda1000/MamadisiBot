#pragma once

#include <iostream> // print the error messages
#include <mariadb/mysql.h>
#include <regex>
#include "sleepy_discord/sleepy_discord.h"

class MamadisiBot : public SleepyDiscord::DiscordClient {
public:
	~MamadisiBot();

	using SleepyDiscord::DiscordClient::DiscordClient;
	void onMessage(SleepyDiscord::Message message) override;
	void connect(const char *ip, unsigned int port, const char *user, const char *password, const char *database);

private:
	MYSQL *_conn;

	MYSQL_RES *sqlPerformQuery(const char *sql_query);
};
