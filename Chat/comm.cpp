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
// --------------------------------------------------------------------------
bool Comm_CreateCommonServerMailSlot(char *MailSlotName)
{
	char StrBuff[MAX_MAILSLOT_NAME_CARS];

	sprintf(StrBuff,"\\\\.\\mailslot\\%s",MailSlotName);

	// Crear mailslot servidor
	CommonServHandle = CreateMailslot(StrBuff,60000,0,NULL);

    return(CommonServHandle != INVALID_HANDLE_VALUE);
}
// --------------------------------------------------------------------------
bool Comm_CreatePrivateServerMailSlot(char *MailSlotName)
{
    strcpy(PrivatePipeName,MailSlotName);

	char StrBuff[MAX_MAILSLOT_NAME_CARS];
	sprintf(StrBuff,"\\\\.\\mailslot\\%s",PrivatePipeName);

	// Crear mailslot servidor
	PrivateServHandle = CreateMailslot(StrBuff,60000,0,NULL);

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
unsigned long Comm_SendMessage(unsigned int Recipient,void* Data,unsigned long Len)
{
	DWORD BytesEscritos;
	int  Res;

    if (Recipient == 0)
    {
        Res = WriteFile(CommonClntHandle,Data,Len,&BytesEscritos,(LPOVERLAPPED) NULL);
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
            char* substr = strstr(he->h_name,".");
            int iHostnameLen = (substr)?substr - he->h_name:strlen(he->h_name);
            strncpy(szHost,he->h_name,iHostnameLen);
            szHost[iHostnameLen] = 0;

            HANDLE ClientHandle = Comm_CreatePrivateClientMailSlot(szHost,PrivatePipeName);

            if (ClientHandle != INVALID_HANDLE_VALUE)
            {
                Res = WriteFile(ClientHandle,Data,Len,&BytesEscritos,(LPOVERLAPPED) NULL);
                DWORD err = GetLastError();

                Comm_DestroyPrivateClientMailSlot(ClientHandle);
            }
        }
    }

    return (Res != 0);
}
// --------------------------------------------------------------------------
unsigned long Comm_PendMessage(unsigned int Recipient)
{
	unsigned long   DataSize = 0;
	BOOL            Res = false;

    switch(Recipient)
    {
        case MSG_RECIPIENT_COMMON_PIPE:
        Res = GetMailslotInfo(CommonServHandle, 0, &DataSize, 0, 0);
        break;

        case MSG_RECIPIENT_PRIVATE_PIPE:
        Res = GetMailslotInfo(PrivateServHandle, 0, &DataSize, 0, 0);
        break;
    }

    if (Res)
    {
        if (DataSize == MAILSLOT_NO_MESSAGE)
            return(0);
        else
    	    return(DataSize);
    }
    else
        return(0);
}
// --------------------------------------------------------------------------
unsigned long Comm_RecvMessage(unsigned int Recipient,void* Data,unsigned long Len)
{
	DWORD           BytesLeidos;
	BOOL            Res=0;

    switch(Recipient)
    {
        case MSG_RECIPIENT_COMMON_PIPE:
        Res = ReadFile(CommonServHandle,Data,Len,&BytesLeidos,(LPOVERLAPPED) NULL);
        break;

        case MSG_RECIPIENT_PRIVATE_PIPE:
        Res = ReadFile(PrivateServHandle,Data,Len,&BytesLeidos,(LPOVERLAPPED) NULL);
        break;
    }

    if (Res)
    {
        if (BytesLeidos == MAILSLOT_NO_MESSAGE)
            return(0);
        else
    	    return(BytesLeidos);
    }
    else
        return(0);
}
// --------------------------------------------------------------------------
void Comm_InitModule(char *CommonPipe,char *PrivatePipe)
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

    if (! Comm_CreatePrivateServerMailSlot(PrivatePipe))
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