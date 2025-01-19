// --------------------------------------------------------------------------
#ifndef ChatH
#define ChatH
// --------------------------------------------------------------------------
#define	        MAX_LEN_CHAT_MESSAGE		500
#define	        MAX_LEN_FILE_MESSAGE		(65536-12)

#define	        CHAT_MESSAGE_CONNECT		1
#define	        CHAT_MESSAGE_DISCONNECT		2
#define	        CHAT_MESSAGE_CONNECTED		3
#define	        CHAT_MESSAGE_COMMON		    10
#define	        CHAT_MESSAGE_BROADCAST		11
#define	        CHAT_MESSAGE_SEND_FILE		20
#define	        CHAT_MESSAGE_RECV_FILE		21
#define	        CHAT_MESSAGE_PRIVATE		30

#define         CHAT_MESSAGE_HEADERSIZE     (sizeof(unsigned long) + sizeof(unsigned long) + sizeof(unsigned long))

typedef struct{
	unsigned long  SrcID;
	unsigned long  MsgID;
    unsigned long  MsgLen;

	// Siempre el ultimo de los campos
	char	 Message[MAX_LEN_FILE_MESSAGE];

}TChatMessage;
// --------------------------------------------------------------------------
void Chat_InitChat	           (unsigned int IPAddress);
void Chat_SendChatMessage      (char *Message);
void Chat_SendConnectMessage   (char *MessageStr);
void Chat_SendDisconnectMessage(char *MessageStr);
void Chat_SendConnectedMessage (char *MessageStr);
void Chat_SendBroadCastMessage (char *Message);
void Chat_SendPrivateMessage   (unsigned int DstIPAddress,char *Message);

void Chat_SendPrivateFile      (unsigned int DstIPAddress,char *Filename);
void Chat_RecvPrivateFile      (unsigned int DstIPAddress,char *Filename);

bool Chat_RecvChatMessage       (TChatMessage *CMess);
bool Chat_RecvPrivateChatMessage(TChatMessage *CMess);

void Chat_FinishChat	       ();
// --------------------------------------------------------------------------
#endif
// --------------------------------------------------------------------------