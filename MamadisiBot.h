#pragma once

#include <iostream> // print the error messages
#include <set>
#include <regex>
#include <mariadb/mysql.h>
#include "sleepy_discord/sleepy_discord.h"
#include <functional> // function parameters
#include "ImageDownloader.h"
#include "EquationSolver.h"
#include <mutex> // std::mutex
#include <cassert> // assert

// reboot includes
#include <unistd.h>
#include <sys/reboot.h>

#define CMD             "uwu"
#define CMD_HELP        "help"
#define CMD_ADD         "add"
#define CMD_ADD_LITERAL "add_literal"
#define CMD_ADD_DELIMITER "\\s\\|\\s"
/**
 * [@user <id> | ]@text <str> | @response <str>[ | @file <url>] // TODO
 * [@user <id> | ]@text <str> | @response <str>
 * [@user <id> | ]@text <str> | @reaction <emoji>
 * [@user <id> | ]@text <str> | @file <url>
 */
#define CMD_ADD_SYNTAX "^(?:@user (?:<@!)?(\\d+)>?" CMD_ADD_DELIMITER ")?@text (.+)" CMD_ADD_DELIMITER "(?:(?:@response (.+))|(?:@file (.+))|(?:@reaction (.+)))$"
#define CMD_REBOOT  "reboot"
#define CMD_MATH	"math"

#define DOWNLOAD_PATH "/home/rogermiranda1000/MamadisiBotC/img/"

typedef enum {
    EXECUTED,			// al ok
	SILENT,				// executed, but say nothing
    NO_PERMISSIONS,		// the user doesn't have the required permissions to use that command
    UNKNOWN,			// unknown command
	NOT_AVALIABLE,		// disabled by the admin
    ERROR,				// an error has occurred while executing the command
	SYNTAX_ERROR		// malformated command
} CMD_RESPONSE;

class MamadisiBot : public SleepyDiscord::DiscordClient {
public:
	MamadisiBot(const char *token, SleepyDiscord::Mode mode, EquationSolver *solver) : SleepyDiscord::DiscordClient(token, mode) {
		this->_solver = solver;
	}
	~MamadisiBot();

	using SleepyDiscord::DiscordClient::DiscordClient;
	void onMessage(SleepyDiscord::Message message) override;
	void connect(const char *ip, unsigned int port, const char *user, const char *password, const char *database);

private:
	std::mutex mtx;
	MYSQL *_conn;
    std::set<uint64_t> _admins;
    std::set<uint64_t> _writers;
	EquationSolver *_solver;

    static void rebootServer();
    static std::string parseRegex(std::string str);

    bool runSentence(const char *sql, MYSQL_BIND *bind = nullptr, MYSQL_BIND *result_bind = nullptr, std::function<void (void)> onResponse = nullptr);
    CMD_RESPONSE command(SleepyDiscord::Message message, std::string cmd, std::string args, uint64_t user);
    bool addResponse(uint64_t server, uint64_t *posted_by, const char *post, const char *answer, std::string *img, std::string *reaction);
	void searchResponse(uint64_t author, uint64_t server, std::string msg, SleepyDiscord::Message message);
    std::set<uint64_t> getAdmins();
    std::set<uint64_t> getWriters();
    std::set<uint64_t> getSuperuser(bool isAdmin);
    void react(SleepyDiscord::Message message, char *emoji);
    void sendMsg(SleepyDiscord::Snowflake<SleepyDiscord::Channel> channel, char *msg);
	void sendLargeMessage(SleepyDiscord::Snowflake<SleepyDiscord::Channel> channel, std::vector<std::string> strings);
    void sendImage(SleepyDiscord::Snowflake<SleepyDiscord::Channel> channel, char *msg, char *img);
};
