#include "MamadisiBot.h"

static const char PREPARED_STMT_RESPONSE[] = "SELECT Responses.response, Responses.image, Reactions.emoji FROM Messages LEFT JOIN Responses ON Messages.id = Responses.id LEFT JOIN Reactions ON Messages.id = Reactions.id WHERE ? REGEXP Messages.message AND (Messages.sended_by IS NULL OR Messages.sended_by = ?) AND (Messages.server IS NULL OR Messages.server = ?)";
#define PREPARED_STMT_RESPONSE_LEN (sizeof(PREPARED_STMT_RESPONSE)/sizeof(char))

MamadisiBot::~MamadisiBot() {
	mysql_close(this->_conn);
}

/**
  * This will check on the database if there's a response to this message.
  * If it finds one, it will call react(), sendMessage(), or sendImage()
  */
void MamadisiBot::onMessage(SleepyDiscord::Message message) {
	if (message.author.bot) return;
	uint64_t authorID = message.author.ID.number();
	uint64_t serverID = message.serverID.number();

	std::cout << "New message by " << authorID << " on server " << serverID << std::endl;

	this->searchResponse(authorID, serverID, message);
}

void MamadisiBot::searchResponse(uint64_t author, uint64_t server, SleepyDiscord::Message message) {
	MYSQL_STMT *stmt = nullptr;

	stmt = mysql_stmt_init(this->_conn);
	if (stmt == nullptr) {
		std::cout << "Init prepared statement error" << std::endl;
		return; // it makes no sense to continue
	}

	if (mysql_stmt_prepare(stmt, PREPARED_STMT_RESPONSE, strlen(PREPARED_STMT_RESPONSE) /*PREPARED_STMT_RESPONSE_LEN*/) != 0) {
		std::cout << "Init prepared statement error" << mysql_stmt_error(stmt) << std::endl;
		return; // it makes no sense to continue
	}

	MYSQL_BIND bind[3];
	memset(bind, 0, sizeof(MYSQL_BIND) * 3);

	std::string msg = message.content;
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
}

void MamadisiBot::react(SleepyDiscord::Message message, char *emoji) {
	addReaction(message.channelID, message, std::string(emoji));
}

void MamadisiBot::sendMsg(SleepyDiscord::Snowflake<SleepyDiscord::Channel> channel, char *msg) {
	sendMessage(channel, std::string(msg));
}

void MamadisiBot::sendImage(SleepyDiscord::Snowflake<SleepyDiscord::Channel> channel, char *msg, char *img) {
	std::string msgStr("");
	if (msg != nullptr) msgStr = std::string(msg);
	uploadFile(channel, std::string(img), msgStr);
}
