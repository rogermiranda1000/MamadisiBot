#include "MamadisiBot.h"

MamadisiBot::~MamadisiBot() {
	mysql_close(this->_conn);
}

void MamadisiBot::onMessage(SleepyDiscord::Message message) {
	if (message.startsWith("uwu 1")) sendMessage(message.channelID, "Hello " + message.author.username);
	else if (message.startsWith("uwu 3")) uploadFile(message.channelID, "/home/rogermiranda1000/MamadisiBotC/img/pufferfish.gif", "");
	else if (message.startsWith("uwu 4")) addReaction(message.channelID, message, "<:mt:808810079536939078"); // mt emoji

	// sql test
	/*MYSQL_ROW row;
	MYSQL_RES *res = this->sqlPerformQuery("SELECT ID, Nombre FROM Nombres;");
	while ((row = mysql_fetch_row(res)) != NULL){
		sendMessage(message.channelID, row[0]);
	}
	// clean up the database result
	mysql_free_result(res);*/

	std::regex reg("^test\\..");
	if (regex_match(message.content, reg)) sendMessage(message.channelID, "match");
}

void MamadisiBot::connect(const char *ip, unsigned int port, const char *user, const char *password, const char *database) {
	// Establish Connection
	this->_conn = mysql_init(NULL);

	if(!mysql_real_connect(this->_conn, ip, user,password, database, port, NULL, 0)) {
		std::cout << "Connection Error: " << mysql_error(this->_conn) << std::endl;
		exit(EXIT_FAILURE);
	}
}

MYSQL_RES *MamadisiBot::sqlPerformQuery(const char *sql_query) {
    //send query to db
    if(mysql_query(this->_conn, sql_query)){
        std::cout << "MySQL Query Error: " << mysql_error(this->_conn) << std::endl;
        return nullptr;
    }

    return mysql_use_result(this->_conn);
}
