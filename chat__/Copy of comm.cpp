#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "comm.h"

#include <winsock2.h>

// --------------------------------------------------------------------------

#define MAX_MAILSLOT_NAME_CARS	256

HANDLE CommonServHandle;
HANDLE CommonClntHandle;
HANDLE PrivateServHandle;
char   PrivatePipeName[MAX_MAILSLOT_NAME_CARS];

char szCommonRead  [MAX_MAILSLOT_NAME_CARS];
char szCommonWrite [MAX_MAILSLOT_NAME_CARS];
char szPrivateRead [MAX_MAILSLOT_NAME_CARS];
char szPrivateWrite[MAX_MAILSLOT_NAME_CARS];
// --------------------------------------------------------------------------
bool Comm_CreateCommonServerMailSlot(char *MailSlotName)
{
	char StrBuff[MAX_MAILSLOT_NAME_CARS];

	sprintf(StrBuff,"\\\\.\\mailslot\\%s",MailSlotName);

	// Crear mailslot servidor
	CommonServHandle = CreateMailslot(StrBuff,0,0,NULL);

    return(CommonServHandle != INVALID_HANDLE_VALUE);
}
// --------------------------------------------------------------------------
bool Comm_CreatePrivateServerMailSlot(char *MailSlotName)
{
    strcpy(PrivatePipeName,MailSlotName);

	char StrBuff[MAX_MAILSLOT_NAME_CARS];
	sprintf(StrBuff,"\\\\.\\mailslot\\%s",PrivatePipeName);

	// Crear mailslot servidor
	PrivateServHandle = CreateMailslot(StrBuff,0,0,NULL);

    return(PrivateServHandle != INVALID_HANDLE_VALUE);
}
// --------------------------------------------------------------------------
HANDLE Comm_CreateClientMailSlot(char* Host,char* MailSlotName)
{
	char StrBuff[MAX_MAILSLOT_NAME_CARS];

    sprintf(StrBuff,"\\\\%s\\mailslot\\%s",Host,MailSlotName);

	// Crear mailslot cliente
    HANDLE ClientHandle = CreateFile( StrBuff,
							GENERIC_WRITE,
							FILE_SHARE_READ,
							(LPSECURITY_ATTRIBUTES) NULL,
							OPEN_EXISTING,
							FILE_ATTRIBUTE_NORMAL,
							(HANDLE) NULL);

	return(ClientHandle);
}
// --------------------------------------------------------------------------
bool Comm_CreateCommonClientMailSlot(char *MailSlotName)
{
    #pragma message ("Cambiar el . por * para el mailslot del cliente!")

	// Crear mailslot cliente
    CommonClntHandle = Comm_CreateClientMailSlot("*",MailSlotName);

	return(CommonClntHandle != INVALID_HANDLE_VALUE);
}
// --------------------------------------------------------------------------
HANDLE Comm_CreatePrivateClientMailSlot(char* Host,char* MailSlotName)
{
	// Crear mailslot cliente
    return ( Comm_CreateClientMailSlot(Host,MailSlotName) );
}
// --------------------------------------------------------------------------
void Comm_DestroyCommonServerMailSlot()
{
	CloseHandle(CommonServHandle);
}
// --------------------------------------------------------------------------
void Comm_DestroyPrivateServerMailSlot()
{
	CloseHandle(PrivateServHandle);
}
// --------------------------------------------------------------------------
void Comm_DestroyCommonClientMailSlot()
{
    CloseHandle(CommonClntHandle);
}
// --------------------------------------------------------------------------
void Comm_DestroyPrivateClientMailSlot(HANDLE handle)
{
    CloseHandle(handle);
}
// --------------------------------------------------------------------------
BOOL Comm_SendMessage(unsigned int Recipient,TMessage *Msg)
{
	DWORD BytesEscritos;
	int  Res;

    if (Recipient == 0)
    {
        Res = WriteFile(CommonClntHandle,Msg->MsgData,Msg->MsgLen,&BytesEscritos,(LPOVERLAPPED) NULL);
    }
    else
    {
        char szHost[1024];
        /*
        sprintf(szHost,"%d.%d.%d.%d",
                        Recipient& 0xff,
                        (Recipient>> 8) & 0xff,
                        (Recipient>>16) & 0xff,
                        (Recipient>>24) & 0xff);
        */
        hostent* he = gethostbyaddr((char*)&Recipient,4,AF_INET);

        if (he)
        {
            int iEnd = strstr(he->h_name,".") - he->h_name;
            strncpy(szHost,he->h_name,iEnd);
            szHost[iEnd] = 0;

            HANDLE ClientHandle = Comm_CreatePrivateClientMailSlot(szHost,szPrivateWrite);

            if (ClientHandle != INVALID_HANDLE_VALUE)
            {
                Res = WriteFile(ClientHandle,Msg->MsgData,Msg->MsgLen,&BytesEscritos,(LPOVERLAPPED) NULL);
                Comm_DestroyPrivateClientMailSlot(ClientHandle);
            }
        }
    }

    return (Res != 0);
}
// --------------------------------------------------------------------------
BOOL Comm_RecvMessage(unsigned int Recipient,TMessage *Msg)
{
	BOOL  Res=0;

    switch(Recipient)
    {
        case MSG_RECIPIENT_COMMON_PIPE:
        Res = ReadFile(CommonServHandle,Msg->MsgData,MAX_MSG_LEN,&Msg->MsgLen,(LPOVERLAPPED) NULL);
        break;

        case MSG_RECIPIENT_PRIVATE_PIPE:
        Res = ReadFile(PrivateServHandle,Msg->MsgData,MAX_MSG_LEN,&Msg->MsgLen,(LPOVERLAPPED) NULL);
        break;
    }

	return(Res && (Msg->MsgLen>0));
}
// --------------------------------------------------------------------------
void Comm_InitModule(char *CommonPipe,char *PrivatePipe)
{
	GetPrivateProfileString("COMM","CommonRead"  ,"Read1" ,szCommonRead  ,MAX_MAILSLOT_NAME_CARS,"./PChat.ini");
	GetPrivateProfileString("COMM","CommonWrite" ,"Write1",szCommonWrite ,MAX_MAILSLOT_NAME_CARS,"./PChat.ini");
	GetPrivateProfileString("COMM","PrivateRead" ,"Read2" ,szPrivateRead ,MAX_MAILSLOT_NAME_CARS,"./PChat.ini");
	GetPrivateProfileString("COMM","PrivateWrite","Write2",szPrivateWrite,MAX_MAILSLOT_NAME_CARS,"./PChat.ini");

	if (! Comm_CreateCommonServerMailSlot(szCommonRead))
	{
#ifdef _BORLANDC_
		ShowMessage("Unable to stablish connection. Code error 235 \n");
#endif
		exit(1);
	}

	if (! Comm_CreateCommonClientMailSlot(szCommonWrite))
	{
		Comm_DestroyCommonServerMailSlot();
#ifdef _BORLANDC_
		ShowMessage("Unable to stablish connection. Code error 237 \n");
#endif
		exit(1);
	}

    if (! Comm_CreatePrivateServerMailSlot(szPrivateRead))
	{
        Comm_DestroyCommonServerMailSlot();
        Comm_DestroyCommonClientMailSlot();

#ifdef _BORLANDC_
		ShowMessage("Unable to stablish connection. Code error 235 \n");
#endif
		exit(1);
	}
}
// --------------------------------------------------------------------------
void Comm_FinishModule()
{
	Comm_DestroyCommonServerMailSlot();
	Comm_DestroyCommonClientMailSlot();
    Comm_DestroyPrivateServerMailSlot();
}
// --------------------------------------------------------------------------