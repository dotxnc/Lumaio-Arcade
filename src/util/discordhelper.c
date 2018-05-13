#include "discordhelper.h"


static void _dcd_dc(int e, const char* t) {
    
}

static void _dcd_er(int e, const char* t){
    
}

static void _dcd_jg(const char* name) {
    
}

static void _dcd_jr(const DiscordUser* u) {
    
}

static void _dcd_rd(const DiscordUser* u) {
    
}

static void _dcd_sg(const char* t) {
    
}

void discordhelper_initialize()
{
    
    memset(&discordhelper_handlers, 0, sizeof(DiscordEventHandlers));
    discordhelper_handlers.disconnected = _dcd_dc;
    discordhelper_handlers.errored = _dcd_er;
    discordhelper_handlers.joinGame = _dcd_jg;
    discordhelper_handlers.joinRequest = _dcd_jr;
    discordhelper_handlers.ready = _dcd_rd;
    discordhelper_handlers.spectateGame = _dcd_sg;
    Discord_Initialize("444698989015597058", &discordhelper_handlers, 1, 0);
    
    memset(&discordhelper_presence, 0, sizeof(DiscordRichPresence));
    discordhelper_presence.details = "Browsing the arcade";
    discordhelper_presence.state = "";
    discordhelper_presence.startTimestamp = time(NULL);
    discordhelper_presence.endTimestamp = 0;
    discordhelper_presence.largeImageKey = "";
    discordhelper_presence.smallImageKey = "";
    discordhelper_presence.partyMax = 0;
    discordhelper_presence.partySize = 0;
    discordhelper_presence.matchSecret = "";
    discordhelper_presence.joinSecret = "";
    discordhelper_presence.spectateSecret = "";
    discordhelper_presence.instance = 1;
    Discord_UpdatePresence(&discordhelper_presence);
}

void discordhelper_setarcade(const char* name, int score)
{
    if (name == NULL) {
        discordhelper_presence.details = "Browsing the arcade";
        discordhelper_presence.state = "";
    } else {
        char details[128];
        char state[128];
        sprintf(details, "Playing: %s", name);
        sprintf(state, "Highscore: %d", score);
        discordhelper_presence.details = details;
        discordhelper_presence.state = state;
    }
    discordhelper_presence.startTimestamp = time(NULL);
    Discord_UpdatePresence(&discordhelper_presence);
}

void discordhelper_shutdown()
{
    Discord_Shutdown();
}

