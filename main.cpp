#include "secrets.h" // place here the #defines for TOKEN, SQL_IP, SQL_PORT, SQL_USER, SQL_PASSWORD, & SQL_DATABASE
#include "MamadisiBot.h"

int main(int argc, char const *argv[]) {
	MamadisiBot client(TOKEN, SleepyDiscord::USER_CONTROLED_THREADS);
	client.connect(SQL_IP, SQL_PORT, SQL_USER, SQL_PASSWORD, SQL_DATABASE);
	client.run();

	return 0;
}
