#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "comm.h"

#define MAX_MAILSLOT_NAME_CARS	256

HANDLE CommonServHandleR;
HANDLE CommonServHandleW;
HANDLE CommonClntHandleR;
HANDLE CommonClntHandleW;
HANDLE OwnServHandle;
HANDLE OwnClntHandle;

// --------------------------------------------------------------------------
bool Comm_CreateCommonServerMailSlot(char *MailSlotName)
{
	char StrBuff[MAX_MAILSLOT_NAME_CARS];

	// Crear mailslot servidor
    sprintf(StrBuff,"\\\\.\\mailslot\\%s_READ",MailSlotName);
	CommonServHandleR = CreateMailslot(StrBuff,0,0,NULL);

	// Crear mailslot servidor
    sprintf(StrBuff,"\\\\.\\mailslot\\%s_WRITE",MailSlotName);
	CommonServHandleW = CreateMailslot(StrBuff,0,0,NULL);

    return(true); // CommonServHandle != INVALID_HANDLE_VALUE);
}
// --------------------------------------------------------------------------
bool Comm_CreateCommonClientMailSlot(char *MailSlotName)
{
	char StrBuff[MAX_MAILSLOT_NAME_CARS];

	// Crear mailslot cliente de lectura
    sprintf(StrBuff,"\\\\*\\mailslot\\%s_READ",MailSlotName);

    CommonClntHandleR = CreateFile( StrBuff,
							GENERIC_WRITE,
							FILE_SHARE_READ,
							(LPSECURITY_ATTRIBUTES) NULL,
							OPEN_EXISTING,
							FILE_ATTRIBUTE_NORMAL,
							(HANDLE) NULL);

	// Crear mailslot cliente de escritura
    sprintf(StrBuff,"\\\\*\\mailslot\\%s_WRITE",MailSlotName);

    CommonClntHandleW = CreateFile( StrBuff,
         GENERIC_READ |  // read and write access
         GENERIC_WRITE,
         0,              // no sharing
//                          FILE_SHARE_READ,
							(LPSECURITY_ATTRIBUTES) NULL,
							OPEN_EXISTING,
							FILE_ATTRIBUTE_NORMAL,
							(HANDLE) NULL);

	return ((CommonClntHandleR != INVALID_HANDLE_VALUE) && (CommonClntHandleW != INVALID_HANDLE_VALUE));
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
	CloseHandle(CommonServHandleR);
    CloseHandle(CommonServHandleW);
}
// --------------------------------------------------------------------------
void Comm_DestroyCommonClientMailSlot()
{
	CloseHandle(CommonClntHandleR);
    CloseHandle(CommonClntHandleW);
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
		case MSG_RECIPIENT_COMMON_PIPE:	Res = WriteFile(CommonClntHandleR,Msg->MsgData,Msg->MsgLen,&BytesEscritos,(LPOVERLAPPED) NULL);
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
		case MSG_RECIPIENT_COMMON_PIPE:	Res = ReadFile(CommonClntHandleW,Msg->MsgData,MAX_MSG_LEN,&BytesLeidos,(LPOVERLAPPED) NULL);
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