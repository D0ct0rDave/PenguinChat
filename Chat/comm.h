// --------------------------------------------------------------------------
#ifndef COMM_HEADERFILE
#define COMM_HEADERFILE
// --------------------------------------------------------------------------

#define MSG_RECIPIENT_COMMON_PIPE           0
#define MSG_RECIPIENT_PRIVATE_PIPE          1

// --------------------------------------------------------------------------

/// Initializes the current communications module
void Comm_InitModule (char* CommonPipe,char* PrivatePipe);

/// Sends a message to a given recipient
unsigned long Comm_SendMessage(unsigned int Recipient,void* Data,unsigned long Len);

/// Receives the data pending in a given recipient
unsigned long Comm_RecvMessage(unsigned int Recipient,void* Data,unsigned long Len);

/// Retrieves the size of a pending message in a given recipient
unsigned long Comm_PendMessage(unsigned int Recipient);

/// Finishes the current communications module
void Comm_FinishModule();

// --------------------------------------------------------------------------
#endif
// --------------------------------------------------------------------------
