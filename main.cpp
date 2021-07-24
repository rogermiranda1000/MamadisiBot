#include "secrets.h" // place here the #defines for TOKEN, SQL_IP, SQL_PORT, SQL_USER, SQL_PASSWORD, & SQL_DATABASE
#include "MamadisiBot.h"

// detect control-C
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

MamadisiBot *client;

void forceExit(int s) {
    std::cout << std::endl << "Caught signal " << s << std::endl;

    // stop bot
    client->quit();
    delete client;

    exit(EXIT_FAILURE);
}

int main(int argc, char const *argv[]) {
    struct sigaction sigIntHandler;

    sigIntHandler.sa_handler = forceExit;
    sigemptyset(&sigIntHandler.sa_mask);
    sigIntHandler.sa_flags = 0;

    sigaction(SIGINT, &sigIntHandler, NULL);

	client = new MamadisiBot(TOKEN, SleepyDiscord::USER_CONTROLED_THREADS);
	client->connect(SQL_IP, SQL_PORT, SQL_USER, SQL_PASSWORD, SQL_DATABASE);
	client->run();

	return 0;
}
