// --------------------------------------------------------------------------
#ifndef ChatH
#define ChatH
// --------------------------------------------------------------------------
#define	        CHAT_MESSAGE_CONNECT		1
#define	        CHAT_MESSAGE_DISCONNECT		2
#define	        CHAT_MESSAGE_CONNECTED		3
#define	        CHAT_MESSAGE_COMMON		    10
#define	        CHAT_MESSAGE_BROADCAST		11
#define	        CHAT_MESSAGE_PRIVATE		30
// --------------------------------------------------------------------------

#define         CHAT_MESSAGE_HEADERSIZE     (sizeof(unsigned long) + sizeof(unsigned long) + sizeof(unsigned long))

typedef struct{
	unsigned long  SrcID;
	unsigned long  MsgID;
    unsigned long  MsgLen;

	// Siempre el ultimo de los campos
	char	       Message[0];

}TChatMessage;

// --------------------------------------------------------------------------

TChatMessage* Chat_CreateMessage(unsigned long _iMsgSize);
void Chat_DestroyMessage(TChatMessage* CMess);

// --------------------------------------------------------------------------
void Chat_InitChat	           (unsigned int IPAddress);
void Chat_FinishChat	       ();

// --------------------------------------------------------------------------
void Chat_SendChatMessage      (char *Message);
void Chat_SendConnectMessage   (char *MessageStr);
void Chat_SendDisconnectMessage(char *MessageStr);
void Chat_SendConnectedMessage (char *MessageStr);
void Chat_SendBroadCastMessage (char *Message);
void Chat_SendPrivateMessage   (unsigned int DstIPAddress,TChatMessage* Message);

// --------------------------------------------------------------------------
/// Se espera del usuario que llame después a DestroyMessage
// --------------------------------------------------------------------------
TChatMessage* Chat_RecvMessage   ();
TChatMessage* Chat_RecvPrivateMessage();

// --------------------------------------------------------------------------
#endif
// --------------------------------------------------------------------------