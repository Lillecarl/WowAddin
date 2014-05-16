#include "stdafx.h"
#include "Commands.h"

#include <sstream>
#include <ctime>
#include <vector>

extern ObjectMgr s_objMgr;
void SetMessageHandlers()
{
    ClientServices::SetMessageHandler(SMSG_DBLOOKUP, LookupResultsHandler, (void*)0xDEADBABE);
    s_client.SetMessageHandler(SMSG_GAMEOBJECT_QUERY_RESPONSE, RunAgainHandler, (void*)0xDEAD1337);
}

BOOL LookupResultsHandler(void *param, NETMESSAGE msgId, uint32 time, CDataStore *msg)
{
    char received[256];

    msg->GetString(received, sizeof(received));

    Console::Write("Hello from LookupResultsHandler: param %08X, time %u, ticks %u, received '%s'", ECHO_COLOR, param, time, GetTickCount(), received);

    return TRUE;
}

BOOL RunAgainHandler(void *param, NETMESSAGE msgId, uint32 time, CDataStore *msg)
{
    CCommand_SirKillAlot("killalot", "1000");

    return FALSE;
}