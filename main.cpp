#include "secrets.h" // place here the #defines for DISCORD_TOKEN, SQL_IP, SQL_PORT, SQL_USER, SQL_PASSWORD, SQL_DATABASE, and WOLFRAM_ID (if you want to use the math function, if not don't place the define)
#include "MamadisiBot.h"
#include "EquationSolver.h"

// detect control-C
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

MamadisiBot *client = nullptr;
EquationSolver *solver = nullptr;

void forceExit(int s) {
    std::cout << std::endl << "Caught signal " << s << std::endl;

    // stop bot
    client->quit();
    delete client;
	
    delete solver;

    exit(EXIT_FAILURE);
}

int main(int argc, char const *argv[]) {
    struct sigaction sigIntHandler;

    sigIntHandler.sa_handler = forceExit;
    sigemptyset(&sigIntHandler.sa_mask);
    sigIntHandler.sa_flags = 0;

    sigaction(SIGINT, &sigIntHandler, NULL);

#ifdef WOLFRAM_ID
	solver = new EquationSolver(WOLFRAM_ID);
#endif

	client = new MamadisiBot(DISCORD_TOKEN, SleepyDiscord::USER_CONTROLED_THREADS, solver);
	client->connect(SQL_IP, SQL_PORT, SQL_USER, SQL_PASSWORD, SQL_DATABASE);
	client->run();

	return 0;
}
