// --------------------------------------------------------------------------
#ifndef COMM_HEADERFILE
#define COMM_HEADERFILE
// --------------------------------------------------------------------------

#define MAX_MSG_LEN						    400

#define MSG_RECIPIENT_COMMON_PIPE           0
#define MSG_RECIPIENT_PRIVATE_PIPE          1

typedef struct{

	unsigned long  MsgLen;
	void*          MsgData;

}TMessage;

// --------------------------------------------------------------------------

void Comm_InitModule (char* CommonPipe,char* PrivatePipe);
BOOL Comm_SendMessage(unsigned int Recipient,TMessage *Msg);
BOOL Comm_RecvMessage(unsigned int Recipient,TMessage *Msg);
void Comm_FinishModule();

// --------------------------------------------------------------------------
#endif
// --------------------------------------------------------------------------
