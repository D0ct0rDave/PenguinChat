#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "comm.h"

#define MAX_MAILSLOT_NAME_CARS	256

HANDLE CommonServHandle;
HANDLE CommonClntHandle;
HANDLE OwnServHandle;
HANDLE OwnClntHandle;

#define BUFSIZE 4096
// --------------------------------------------------------------------------
bool Comm_CreateCommonServerMailSlot(char *MailSlotName)
{
	char StrBuff[MAX_MAILSLOT_NAME_CARS];

	sprintf(StrBuff,"\\\\.\\pipe\\%s",MailSlotName);

/*
	// Crear mailslot servidor
	CommonServHandle = CreateMailslot(StrBuff,0,0,NULL);

	return(CommonServHandle != INVALID_HANDLE_VALUE);
*/
      CommonServHandle = CreateNamedPipe(
          StrBuff,             // pipe name
          PIPE_ACCESS_DUPLEX,       // read/write access
          PIPE_TYPE_MESSAGE |       // message type pipe
          PIPE_READMODE_MESSAGE |   // message-read mode
          PIPE_WAIT,                // blocking mode
          PIPE_UNLIMITED_INSTANCES, // max. instances
          BUFSIZE,                  // output buffer size
          BUFSIZE,                  // input buffer size
          0,                        // client time-out
          NULL);                    // default security attribute

      if (CommonServHandle == INVALID_HANDLE_VALUE)
      {
          printf("CreatePipe failed");
          return 0;
      }

      /*
      // Wait for the client to connect; if it succeeds,
      // the function returns a nonzero value. If the function
      // returns zero, GetLastError returns ERROR_PIPE_CONNECTED.
      BOOL fConnected = ConnectNamedPipe(CommonServHandle, NULL) ?
         TRUE : (GetLastError() == ERROR_PIPE_CONNECTED);
         */
}
// --------------------------------------------------------------------------
bool Comm_CreateCommonClientMailSlot(char *MailSlotName)
{
	char StrBuff[MAX_MAILSLOT_NAME_CARS];


    sprintf(StrBuff,"\\\\*\\pipe\\%s",MailSlotName);


    // Try to open a named pipe; wait for it, if necessary. 

   while (1) 
   {
      CommonClntHandle = CreateFile(
         StrBuff,   // pipe name
         GENERIC_READ |  // read and write access
         GENERIC_WRITE,
         0,              // no sharing
         NULL,           // default security attributes
         OPEN_EXISTING,  // opens existing pipe
         0,              // default attributes
         NULL);          // no template file

   // Break if the pipe handle is valid.

      if (CommonClntHandle != INVALID_HANDLE_VALUE)
         break;
 
      // Exit if an error other than ERROR_PIPE_BUSY occurs. 
 
      if (GetLastError() != ERROR_PIPE_BUSY)
      {
         printf("Could not open pipe"); 
         return 0;
      }
 
      // All pipe instances are busy, so wait for 20 seconds. 

      if (!WaitNamedPipe(StrBuff, 20000))
      { 
         printf("Could not open pipe"); 
         return 0;
      } 
   } 
 
// The pipe connected; change to message-read mode. 

   DWORD dwMode = PIPE_READMODE_MESSAGE;
   BOOL fSuccess = SetNamedPipeHandleState(
      CommonClntHandle,    // pipe handle
      &dwMode,  // new pipe mode 
      NULL,     // don't set maximum bytes
      NULL);    // don't set maximum time
   if (!fSuccess) 
   {
      printf("SetNamedPipeHandleState failed");
      return 0;
   }



    /*
	// Crear mailslot cliente
    CommonClntHandle = CreateFile( StrBuff,
							GENERIC_WRITE,
							FILE_SHARE_READ,
							(LPSECURITY_ATTRIBUTES) NULL,
							OPEN_EXISTING,
							FILE_ATTRIBUTE_NORMAL,
							(HANDLE) NULL);

    */

	return(CommonClntHandle != INVALID_HANDLE_VALUE);
}
// --------------------------------------------------------------------------
bool Comm_CreateOwnServerMailSlot(char *MailSlotName)
{	
	char StrBuff[MAX_MAILSLOT_NAME_CARS];
	
	sprintf(StrBuff,"\\\\.\\mailslot\\%s",MailSlotName);

	// Crear mailslot servidor
	OwnServHandle = CreateMailslot(StrBuff,0,0,NULL);

	return(OwnServHandle != INVALID_HANDLE_VALUE);
}
// --------------------------------------------------------------------------
bool Comm_CreateOwnClientMailSlot(char *MailSlotName)
{	
	char StrBuff[MAX_MAILSLOT_NAME_CARS];

    sprintf(StrBuff,"\\\\*\\mailslot\\%s",MailSlotName);

	// Crear mailslot cliente 
    OwnClntHandle = CreateFile( StrBuff,
							GENERIC_WRITE,
							FILE_SHARE_READ,
							(LPSECURITY_ATTRIBUTES) NULL,
							OPEN_EXISTING,
							FILE_ATTRIBUTE_NORMAL,
							(HANDLE) NULL);

    
	return(OwnClntHandle != INVALID_HANDLE_VALUE);
}
// --------------------------------------------------------------------------
void Comm_DestroyCommonServerMailSlot()
{
	CloseHandle(CommonServHandle);
}
// --------------------------------------------------------------------------
void Comm_DestroyCommonClientMailSlot()
{
	CloseHandle(CommonClntHandle);
}
// --------------------------------------------------------------------------
void Comm_DestroyOwnServerMailSlot()
{
	CloseHandle(OwnServHandle);
}
// --------------------------------------------------------------------------
void Comm_DestroyOwnClientMailSlot()
{
	CloseHandle(OwnClntHandle);
}
// --------------------------------------------------------------------------
void Comm_InitModule(char *CommonPipe,char *OwnPipe)
{
	if (! Comm_CreateCommonServerMailSlot(CommonPipe))
	{
#ifdef _BORLANDC_
		ShowMessage("Unable to stablish connection. Code error 235 \n");
#endif
		exit(1);

	}

	if (! Comm_CreateCommonClientMailSlot(CommonPipe))
	{
		Comm_DestroyCommonServerMailSlot();
#ifdef _BORLANDC_
		ShowMessage("Unable to stablish connection. Code error 237 \n");
#endif
		exit(1);
	}

	if (!  (OwnPipe))
	{		
		Comm_DestroyCommonServerMailSlot();
		Comm_DestroyCommonClientMailSlot();
#ifdef _BORLANDC_
		ShowMessage("Unable to stablish connection. Code error 236 \n");
#endif
		exit(1);
	}
}
// --------------------------------------------------------------------------
void Comm_SendMessage(unsigned int Recipient,TMessage *Msg)
{
	DWORD BytesEscritos;
	int  Res;

	switch (Recipient)
	{
		case MSG_RECIPIENT_COMMON_PIPE:	Res = WriteFile(CommonClntHandle,Msg->MsgData,Msg->MsgLen,&BytesEscritos,(LPOVERLAPPED) NULL);
										break;
		case MSG_RECIPIENT_OWN_PIPE:	Res = WriteFile(OwnClntHandle   ,Msg->MsgData,Msg->MsgLen,&BytesEscritos,(LPOVERLAPPED) NULL);
										break;
		default:
										break;
	}
}
// --------------------------------------------------------------------------
BOOL Comm_RecvMessage(unsigned int Recipient,TMessage *Msg)
{
	DWORD BytesLeidos;
	BOOL  Res=0;

	switch (Recipient)
	{
		case MSG_RECIPIENT_COMMON_PIPE:	Res = ReadFile(CommonServHandle,Msg->MsgData,MAX_MSG_LEN,&BytesLeidos,(LPOVERLAPPED) NULL);
										break;
		case MSG_RECIPIENT_OWN_PIPE:	Res = ReadFile(OwnServHandle   ,Msg->MsgData,MAX_MSG_LEN,&BytesLeidos,(LPOVERLAPPED) NULL);
										break;
		default:
										break;
	}

	return(Res && (BytesLeidos>0));
}
// --------------------------------------------------------------------------
void Comm_FinitModule()
{
	Comm_DestroyCommonServerMailSlot();
	Comm_DestroyCommonClientMailSlot();

	Comm_DestroyOwnServerMailSlot();
}
// --------------------------------------------------------------------------