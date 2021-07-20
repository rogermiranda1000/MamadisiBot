#include "secrets.h"
#include "MamadisiBot.h"

int main() {
	MyClientClass client(TOKEN, SleepyDiscord::USER_CONTROLED_THREADS);
	client.run();

	return 0;
}
