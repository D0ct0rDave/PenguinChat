//---------------------------------------------------------------------------
#ifndef ChatHistoryH
#define ChatHistoryH
//---------------------------------------------------------------------------

#include "chat.h"
#include "user.h"

typedef struct{
    TChatMessage Msg;
    char         User [MAX_CARS_NICKNAME];
    char         Actor[MAX_CARS_ACTORNAME];
}TChatHistoryEntry;

void ChatHistory_AddChatMessage(TChatMessage* CMess,char *User,char *Actor);
void ChatHistory_ClearHistory();
TChatHistoryEntry *ChatHistory_GetChatMessage(unsigned int NumMsg);
unsigned int ChatHistory_GetNumMessages();
//---------------------------------------------------------------------------
#endif
 