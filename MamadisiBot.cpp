#include "MamadisiBot.h"

void MamadisiBot::onMessage(SleepyDiscord::Message message) {
	if (message.startsWith("uwu 1")) sendMessage(message.channelID, "Hello " + message.author.username);
	else if (message.startsWith("uwu 3")) uploadFile(message.channelID, "/home/rogermiranda1000/MamadisiBotC/img/pufferfish.gif", "");
	else if (message.startsWith("uwu 4")) addReaction(message.channelID, message, "<:mt:808810079536939078"); // mt emoji
}
