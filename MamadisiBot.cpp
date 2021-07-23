#include "MamadisiBot.h"

static const char PREPARED_STMT_RESPONSE[] = "SELECT Responses.response, Responses.image, Reactions.emoji FROM Messages LEFT JOIN Responses ON Messages.id = Responses.id LEFT JOIN Reactions ON Messages.id = Reactions.id WHERE ? REGEXP Messages.message AND (Messages.sended_by IS NULL OR Messages.sended_by = ?) AND (Messages.server IS NULL OR Messages.server = ?)";
static const char PREPARED_STMT_ADMINS[] = "SELECT id FROM Admins";
static const char PREPARED_STMT_WRITERS[] = "SELECT id FROM Writers";
static const char PREPARED_STMT_INSERT_MESSAGE[] = "INSERT INTO Messages(server, sended_by, message) VALUE (?,?,?)";
static const char PREPARED_STMT_INSERT_RESPONSE[] = "INSERT INTO Responses(id, response, image) VALUE (LAST_INSERT_ID(),?,NULL)"; // TODO img


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
		switch (this->command(serverID, msg.substr(4, match-4) /* msg without 'uwu' prefix */, (match != std::string::npos) ? msg.substr(match+1) : std::string() /* arguments */, authorID)) {
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
CMD_RESPONSE MamadisiBot::command(uint64_t server, std::string cmd, std::string args, uint64_t user) {
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
        if (!addResponse(server, regexUser.length() > 0 ? &desired_user : nullptr, regexMsg.length() > 0 ? regexMsg.c_str() : nullptr,
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

    // get the result
    MYSQL_BIND result_bind[1];
    memset(result_bind, 0, sizeof(result_bind));

    uint64_t userId;

    result_bind[0].buffer_type = MYSQL_TYPE_LONGLONG;
    result_bind[0].buffer = &userId;
    result_bind[0].buffer_length = sizeof(userId);

    auto onResponse = [&]() {
        users.insert(userId);
    };

    if (!this->runSentence(isAdmin ? PREPARED_STMT_ADMINS : PREPARED_STMT_WRITERS, nullptr, result_bind, onResponse)) return users; // error

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
bool MamadisiBot::addResponse(uint64_t server, uint64_t *posted_by, const char *post, const char *answer, const char *reaction) {
    if ((posted_by == nullptr && post == nullptr) || (answer == nullptr && reaction == nullptr)) return false;


    MYSQL_BIND bind[3];
    memset(bind, 0, sizeof(MYSQL_BIND) * 3);

    bind[0].buffer_type = MYSQL_TYPE_LONGLONG;
    bind[0].buffer = &server;
    bind[0].buffer_length = sizeof(uint64_t);

    char null = STMT_INDICATOR_NULL;
    bind[1].buffer_type = MYSQL_TYPE_LONGLONG;
    if (posted_by != nullptr) bind[1].buffer = posted_by;
    else bind[1].u.indicator = &null;
    bind[1].buffer_length = sizeof(uint64_t);

    bind[2].buffer_type = MYSQL_TYPE_STRING;
    if (post != nullptr) bind[2].buffer = (char*)post;
    else bind[2].u.indicator = &null;
    bind[2].buffer_length = strlen(post);

    /*if (posted_by != nullptr) std::cout << posted_by << std::endl;
    if (post != nullptr) std::cout << post << std::endl;
    if (answer != nullptr) std::cout << answer << std::endl;
    if (reaction != nullptr) std::cout << reaction << std::endl;*/
    if (!this->runSentence(PREPARED_STMT_INSERT_MESSAGE, bind, nullptr, nullptr)) return false;
    return true;//this->runSentence(PREPARED_STMT_INSERT_RESPONSE, bind, result_bind, onResponse);
}

bool MamadisiBot::runSentence(const char *sql, MYSQL_BIND *bind, MYSQL_BIND *result_bind, std::function<void (void)> onResponse) {
    MYSQL_STMT *stmt = nullptr;

    stmt = mysql_stmt_init(this->_conn);
    if (stmt == nullptr) {
        std::cout << "Init prepared statement error" << std::endl;
        return false; // it makes no sense to continue
    }

    if (mysql_stmt_prepare(stmt, sql, strlen(sql)) != 0) {
        std::cout << "Init prepared statement error" << mysql_stmt_error(stmt) << std::endl;
        return false; // it makes no sense to continue
    }

    if (bind != nullptr) {
        if (mysql_stmt_bind_param(stmt, bind)) {
            std::cout << "Prepared statement bind error" << std::endl;
            mysql_stmt_close(stmt);
            return false; // it makes no sense to continue
        }
    }

    if (mysql_stmt_execute(stmt)) {
        std::cout << "Prepared statement error" << mysql_stmt_error(stmt) << std::endl;
        mysql_stmt_close(stmt);
        return false; // it makes no sense to continue
    }

    if (result_bind != nullptr) {
        MYSQL_RES *result = mysql_stmt_result_metadata(stmt);
        if (!result) {
            std::cout << "Prepared statement error" << mysql_stmt_error(stmt) << std::endl;
            mysql_stmt_close(stmt);
            return false; // it makes no sense to continue
        }

        if (mysql_stmt_bind_result(stmt, result_bind)) {
            std::cout << "Prepared statement return error" << mysql_stmt_error(stmt) << std::endl;
            mysql_stmt_close(stmt);
            return false;
        }

        while (!mysql_stmt_fetch(stmt)) {
            onResponse();

            mysql_stmt_free_result(stmt);
        }
    }

    // free the memory
    mysql_stmt_close(stmt);

    return true;
}

void MamadisiBot::searchResponse(uint64_t author, uint64_t server, std::string msg, SleepyDiscord::Message message) {
	MYSQL_BIND bind[3];
	memset(bind, 0, sizeof(MYSQL_BIND) * 3);

	bind[0].buffer_type = MYSQL_TYPE_STRING;
	bind[0].buffer = (char*)msg.c_str();
	bind[0].buffer_length = msg.length();

	bind[1].buffer_type = MYSQL_TYPE_LONGLONG;
	bind[1].buffer = &author;
	bind[1].buffer_length = sizeof(author);

	bind[2].buffer_type = MYSQL_TYPE_LONGLONG;
	bind[2].buffer = &server;
	bind[2].buffer_length = sizeof(server);

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

    auto onResponse = [&]() {
		std::cout << "Response found" << std::endl;
		if (!isNull[2]) this->react(message, strResult[2]);

		if (!isNull[1]) this->sendImage(message.channelID, (isNull[0] ? nullptr : strResult[0]), strResult[1]);
		else if (!isNull[0]) this->sendMsg(message.channelID, strResult[0]);
	};

    this->runSentence(PREPARED_STMT_RESPONSE, bind, result_bind, onResponse);




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
