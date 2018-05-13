#ifndef DISCORD_HELPER_H
#define DISCORD_HELPER_H

#include <time.h>
#include <stdio.h>
#include <string.h>
#include <discord_rpc.h>
#include <raylib.h>

static const char* discordhelper_id = "444698989015597058";
static DiscordRichPresence discordhelper_presence;
static DiscordEventHandlers discordhelper_handlers;

void discordhelper_initialize();
void discordhelper_setarcade(const char*, int);
void discordhelper_shutdown();

#endif
