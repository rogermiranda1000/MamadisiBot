#include "MamadisiBot.h"

static const char PREPARED_STMT_RESPONSE[] = "SELECT Responses.response, Responses.image, Reactions.emoji FROM Messages LEFT JOIN Responses ON Messages.id = Responses.id LEFT JOIN Reactions ON Messages.id = Reactions.id WHERE ? REGEXP Messages.message AND (Messages.sended_by IS NULL OR Messages.sended_by = ?) AND (Messages.server IS NULL OR Messages.server = ?)";
#define PREPARED_STMT_RESPONSE_LEN (sizeof(PREPARED_STMT_RESPONSE)/sizeof(char))
static const char PREPARED_STMT_ADMINS[] = "SELECT id FROM Admins";
#define PREPARED_STMT_ADMINS_LEN (sizeof(PREPARED_STMT_ADMINS)/sizeof(char))
static const char PREPARED_STMT_WRITERS[] = "SELECT id FROM Writers";
#define PREPARED_STMT_WRITERS_LEN (sizeof(PREPARED_STMT_WRITERS)/sizeof(char))


MamadisiBot::~MamadisiBot() {
	mysql_close(this->_conn);
}

void MamadisiBot::rebootServer() {
    // TODO why not work? :(
    // TODO delay
	sync();
	reboot(RB_AUTOBOOT);
}

/**
  * This will check on the database if there's a response to this message.
  * If it finds one, it will call react(), sendMessage(), or sendImage()
  */
void MamadisiBot::onMessage(SleepyDiscord::Message message) {
	if (message.author.bot) return;
	uint64_t authorID = message.author.ID.number();
	uint64_t serverID = message.serverID.number();
	std::string msg = message.content;

	std::cout << "New message by " << authorID << " on server " << serverID << std::endl;

	if (msg.rfind(CMD " ", 0) == 0) {
        size_t match = msg.find(' ', 4);

		const char *response;
		switch (this->command(msg.substr(4, match-4) /* msg without 'uwu' prefix */, (match != std::string::npos) ? msg.substr(match+1) : std::string() /* arguments */, authorID)) {
            case EXECUTED:
                response = "Comando ejecutado! uwu";
                break;
            case NO_PERMISSIONS:
                response = "Ño >:(";
                break;
            case UNKNOWN:
                response = "Comando desconocido";
                break;
		    case ERROR:
		        response = "Sintaxis incorrecta";
                break;
            default:
                response = "Unknown response code";
		}
        this->sendMsg(message.channelID,  (char*)response);
	}
	else this->searchResponse(authorID, serverID, msg, message);
}

// TODO
CMD_RESPONSE MamadisiBot::command(std::string cmd, std::string args, uint64_t user) {
	std::cout << "Command '" << cmd << "'" << std::endl;
	if (cmd == std::string(CMD_HELP)) {
	    return EXECUTED;
	}
	else if (cmd == std::string(CMD_REBOOT)) {
	    if (this->_admins.find(user) == this->_admins.end()) return NO_PERMISSIONS;

        MamadisiBot::rebootServer();
        return EXECUTED;
	}
	else if (cmd == std::string(CMD_ADD)) {
        if (this->_admins.find(user) == this->_admins.end() && this->_writers.find(user) == this->_admins.end()) return NO_PERMISSIONS;
        //std::cout << "Debug: " << args << std::endl;

        // RegEx parse
        std::regex rgx(CMD_ADD_SYNTAX);
        std::smatch match;
        if (!std::regex_search(args, match, rgx)) return ERROR;

        std::string regexUser = match.str(1), regexMsg = match.str(2), regexAnswer = match.str(3), regexReaction = match.str(4);
        uint64_t desired_user = atoll(regexUser.c_str());
        if (!addResponse(regexUser.length() > 0 ? &desired_user : nullptr, regexMsg.length() > 0 ? regexMsg.c_str() : nullptr,
                         regexAnswer.length() > 0 ? regexAnswer.c_str() : nullptr, regexReaction.length() > 0 ? regexReaction.c_str() : nullptr)) return ERROR;
        return EXECUTED;
	}
	return UNKNOWN;
}

std::set<uint64_t> MamadisiBot::getAdmins() {
    return this->getSuperuser(true);
}

std::set<uint64_t> MamadisiBot::getWriters() {
    return this->getSuperuser(false);
}

std::set<uint64_t> MamadisiBot::getSuperuser(bool isAdmin) {
    std::set<uint64_t> users;
    MYSQL_STMT *stmt = nullptr;

    stmt = mysql_stmt_init(this->_conn);
    if (stmt == nullptr) {
        std::cout << "Init prepared statement error" << std::endl;
        return users;
    }

    if (mysql_stmt_prepare(stmt, isAdmin ? PREPARED_STMT_ADMINS : PREPARED_STMT_WRITERS, isAdmin ? PREPARED_STMT_ADMINS_LEN : PREPARED_STMT_WRITERS_LEN) != 0) {
        std::cout << "Init prepared statement error" << mysql_stmt_error(stmt) << std::endl;
        return users;
    }

    /*if (mysql_stmt_bind_param(stmt, bind)) {
        std::cout << "Prepared statement bind error" << std::endl;
        mysql_stmt_close(stmt);
        return; // it makes no sense to continue
    }*/

    if (mysql_stmt_execute(stmt)) {
        std::cout << "Prepared statement error" << mysql_stmt_error(stmt) << std::endl;
        mysql_stmt_close(stmt);
        return users;
    }

    MYSQL_RES *result = mysql_stmt_result_metadata(stmt);
    if (!result) {
        std::cout << "Prepared statement error" << mysql_stmt_error(stmt) << std::endl;
        mysql_stmt_close(stmt);
        return users;
    }

    int column_count= mysql_num_fields(result);
    if (column_count != 1) {
        std::cout << "Invalid column count returned" << std::endl;
        mysql_stmt_close(stmt);
        return users;
    }

    // get the result
    MYSQL_BIND result_bind[1];
    memset(result_bind, 0, sizeof(result_bind));

    uint64_t userId;

    result_bind[0].buffer_type = MYSQL_TYPE_LONGLONG;
    result_bind[0].buffer = &userId;
    result_bind[0].buffer_length = sizeof(userId);

    if (mysql_stmt_bind_result(stmt, result_bind)) {
        std::cout << "Prepared statement return error" << mysql_stmt_error(stmt) << std::endl;
        mysql_stmt_close(stmt);
        return users;
    }

    while (!mysql_stmt_fetch(stmt)) {
        users.insert(userId);

        mysql_stmt_free_result(stmt);
    }

    // free the memory
    mysql_stmt_close(stmt);

    if (isAdmin) std::cout << "Found admins: ";
    else std::cout << "Found writers: ";
    std::set<uint64_t>::iterator it;
    for(it = users.begin(); it != users.end(); ++it) {
        std::cout << *it;
        std::cout << " ";
    }
    std::cout << std::endl;

    return users;
}

// TODO images
bool MamadisiBot::addResponse(uint64_t *posted_by, const char *post, const char *answer, const char *reaction) {
    if ((posted_by == nullptr && post == nullptr) || (answer == nullptr && reaction == nullptr)) return false;

    // debug
    /*if (posted_by != nullptr) std::cout << posted_by << std::endl;
    if (post != nullptr) std::cout << post << std::endl;
    if (answer != nullptr) std::cout << answer << std::endl;
    if (reaction != nullptr) std::cout << reaction << std::endl;*/
    return true;
}

void MamadisiBot::searchResponse(uint64_t author, uint64_t server, std::string msg, SleepyDiscord::Message message) {
	MYSQL_STMT *stmt = nullptr;

	stmt = mysql_stmt_init(this->_conn);
	if (stmt == nullptr) {
		std::cout << "Init prepared statement error" << std::endl;
		return; // it makes no sense to continue
	}

	if (mysql_stmt_prepare(stmt, PREPARED_STMT_RESPONSE, PREPARED_STMT_RESPONSE_LEN) != 0) {
		std::cout << "Init prepared statement error" << mysql_stmt_error(stmt) << std::endl;
		return; // it makes no sense to continue
	}

	MYSQL_BIND bind[3];
	memset(bind, 0, sizeof(MYSQL_BIND) * 3);

	bind[0].buffer_type = MYSQL_TYPE_STRING;//MYSQL_TYPE_VARCHAR;
	bind[0].buffer = (char*)msg.c_str();
	bind[0].buffer_length = msg.length();

	bind[1].buffer_type = MYSQL_TYPE_LONGLONG;
	bind[1].buffer = &author;
	bind[1].buffer_length = sizeof(author);

	bind[2].buffer_type = MYSQL_TYPE_LONGLONG;
	bind[2].buffer = &server;
	bind[2].buffer_length = sizeof(server);

	//unsigned int lenght = 1;
	//mysql_stmt_attr_set(stmt, STMT_ATTR_ARRAY_SIZE, &lenght);

	if (mysql_stmt_bind_param(stmt, bind)) {
		std::cout << "Prepared statement bind error" << std::endl;
		mysql_stmt_close(stmt);
		return; // it makes no sense to continue
	}

	if (mysql_stmt_execute(stmt)) {
		std::cout << "Prepared statement error" << mysql_stmt_error(stmt) << std::endl;
		mysql_stmt_close(stmt);
		return; // it makes no sense to continue
	}

	MYSQL_RES *result = mysql_stmt_result_metadata(stmt);
	if (!result) {
		std::cout << "Prepared statement error" << mysql_stmt_error(stmt) << std::endl;
		mysql_stmt_close(stmt);
		return; // it makes no sense to continue
	}

	int column_count= mysql_num_fields(result);
	if (column_count != 3) {
		std::cout << "Invalid column count returned" << std::endl;
		mysql_stmt_close(stmt);
		return;
	}

	// get the result
	MYSQL_BIND result_bind[3];
	memset(result_bind, 0, sizeof(result_bind));

	char strResult[2000][3];
	unsigned long lenght[3] = {0};
	my_bool isNull[3];

	for (int n = 0; n < 3; n++) {
		result_bind[n].buffer_type = MYSQL_TYPE_STRING;
		result_bind[n].buffer = strResult[n];
		result_bind[n].buffer_length = sizeof(strResult)/3;
		result_bind[n].length = &lenght[n];
		result_bind[n].is_null = &isNull[n];
	}

	if (mysql_stmt_bind_result(stmt, result_bind)) {
		std::cout << "Prepared statement return error" << mysql_stmt_error(stmt) << std::endl;
		mysql_stmt_close(stmt);
		return;
	}

	while (!mysql_stmt_fetch(stmt)) {
		std::cout << "Response found" << std::endl;
		if (!isNull[2]) this->react(message, strResult[2]);

		if (!isNull[1]) this->sendImage(message.channelID, (isNull[0] ? nullptr : strResult[0]), strResult[1]);
		else if (!isNull[0]) this->sendMsg(message.channelID, strResult[0]);

		mysql_stmt_free_result(stmt);
	}

	// free the memory
	mysql_stmt_close(stmt);




	// other tests
	/*if (message.startsWith("uwu 1")) sendMessage(message.channelID, "Hello " + message.author.username);
	else if (message.startsWith("uwu 3")) uploadFile(message.channelID, "/home/rogermiranda1000/MamadisiBotC/img/pufferfish.gif", "");
	else if (message.startsWith("uwu 4")) addReaction(message.channelID, message, "<:mt:808810079536939078");  mt emoji */
}

void MamadisiBot::connect(const char *ip, unsigned int port, const char *user, const char *password, const char *database) {
	// Establish Connection
	this->_conn = mysql_init(NULL);

	if(!mysql_real_connect(this->_conn, ip, user,password, database, port, NULL, 0)) {
		std::cout << "Connection Error: " << mysql_error(this->_conn) << std::endl;
		exit(EXIT_FAILURE);
	}

    this->_admins = this->getAdmins();
    this->_writers = this->getWriters();
}

void MamadisiBot::react(SleepyDiscord::Message message, char *emoji) {
    this->addReaction(message.channelID, message, std::string(emoji));
}

void MamadisiBot::sendMsg(SleepyDiscord::Snowflake<SleepyDiscord::Channel> channel, char *msg) {
    this->sendMessage(channel, std::string(msg));
}

void MamadisiBot::sendImage(SleepyDiscord::Snowflake<SleepyDiscord::Channel> channel, char *msg, char *img) {
	std::string msgStr("");
	if (msg != nullptr) msgStr = std::string(msg);
    this->uploadFile(channel, std::string(img), msgStr);
}
