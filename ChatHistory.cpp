//---------------------------------------------------------------------------
#include <stdlib.h>
#include <string.h>

#include "ChatHistory.h"
#include "user.h"

#define     MAX_CHAT_MESSAGES       32

typedef struct{
    TChatHistoryEntry Msgs[MAX_CHAT_MESSAGES];
    unsigned int NumMsgs;
}TChatHistory;

TChatHistory CHist;
//---------------------------------------------------------------------------
TChatHistoryEntry Msgs[MAX_CHAT_MESSAGES-1];
//---------------------------------------------------------------------------
void ChatHistory_AddChatMessage(TChatMessage* CMess,char* User,char* Actor)
{

    if (CHist.NumMsgs == MAX_CHAT_MESSAGES)
    {
        // Copiar desde el segundo hasta el ultimo mensaje
        memcpy((void *)Msgs      ,(void *)&CHist.Msgs[1],(MAX_CHAT_MESSAGES-1)*sizeof(TChatHistoryEntry));
        memcpy((void *)CHist.Msgs,(void *)Msgs          ,(MAX_CHAT_MESSAGES-1)*sizeof(TChatHistoryEntry));
        CHist.NumMsgs--;
    }

    // copiar mensaje chat
    memcpy((void *)&CHist.Msgs[CHist.NumMsgs],CMess,sizeof(TChatHistoryEntry));
    strcpy(CHist.Msgs[CHist.NumMsgs].User ,User);
    strcpy(CHist.Msgs[CHist.NumMsgs].Actor,Actor);
    CHist.NumMsgs++;
}
//---------------------------------------------------------------------------
void ChatHistory_ClearHistory()
{
    CHist.NumMsgs=0;
}
//---------------------------------------------------------------------------
TChatHistoryEntry *ChatHistory_GetChatMessage(unsigned int NumMsg)
{
    return(& CHist.Msgs[NumMsg] );
}
//---------------------------------------------------------------------------
unsigned int ChatHistory_GetNumMessages()
{
    return(CHist.NumMsgs);
}
//---------------------------------------------------------------------------

