#ifndef COMMANDS_H
#define COMMANDS_H
static bool killingalot = false;
static uint32 killalotmuch = 1;

BOOL CCommand_TestCommand(char const* cmd, char const* args);
BOOL CCommand_Beastmaster(char const* cmd, char const* args);
BOOL CCommand_DBLookup(char const* cmd, char const* args);

BOOL CCommand_Invis(char const* cmd, char const* args);

BOOL CCommand_KillServer(char const* cmd, char const* args);
BOOL CCommand_SirKillAlot(char const* cmd, char const* args);
BOOL CCommand_ToggleKillAlot(char const* cmd, char const* args);

BOOL CCommand_TrollSomeone(char const* cmd, char const* args);
BOOL CCommand_TrollAll(char const* cmd, char const* args);

BOOL CCommand_ReportBug(char const* cmd, char const* args);
#endif