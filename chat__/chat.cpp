// --------------------------------------------------------------------------
#include <windows.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "Misc.h"
#include "cript.h"
#include "comm.h"
#include "Chat.h"

#define MAX_CAR_NUM 100

unsigned int DateUnion;
unsigned int CommonKey;
unsigned int OwnKey;
unsigned int OwnID;

// --------------------------------------------------------------------------
unsigned int Chat_EncodeNumber(unsigned int Number,unsigned int Key)
{
	TCriptData CD;

	CD.Len  = sizeof(unsigned int);
	memcpy((void *)CD.Data,(void *)&Number,CD.Len);

	Cript_EncodeData(&CD,Key);

    memcpy(&Number,CD.Data,sizeof(unsigned int));

	return(Number);
}
// --------------------------------------------------------------------------
unsigned int Chat_EncodeNumberWithItSelf(unsigned int Number)
{
    return( Chat_EncodeNumber(Number,Number) );
}
// --------------------------------------------------------------------------
void Chat_EncodeData(char *Data,unsigned long Len,unsigned int Key)
{
	TCriptData CD;

	CD.Len  = Len;
	memcpy((void *)CD.Data,(void *)Data,CD.Len);

	Cript_EncodeData(&CD,Key);
	memcpy(Data,CD.Data,Len);
}
// --------------------------------------------------------------------------
void Chat_DecodeData(char *Data,unsigned long *Len,unsigned int Key)
{
	Cript_DecodeData((TCriptData *)Data,Key);
	*Len = *(unsigned int *)Data;
}
// --------------------------------------------------------------------------
void Chat_GetOwnerName(unsigned int OwnKey,char *OwnerName)
{
	// Encriptar ip con TimeStamp
	Chat_EncodeData((char *)&OwnKey,sizeof(unsigned int),DateUnion);

	// Obtener nombre del mailslot comun
	Misc_EncodeNumberStr(OwnKey,OwnerName);
}
// --------------------------------------------------------------------------
void Chat_InitChat(unsigned int IPAddress)
{
	char  CommonName[MAX_CAR_NUM];
	char  OwnName   [MAX_CAR_NUM];

	// Establecer el OwnID del usuario del programa
	OwnID = IPAddress;

	// Tomar fecha del dia
	SYSTEMTIME SysTime;
	GetSystemTime((LPSYSTEMTIME)&SysTime);

	// Obtener Unix Time Stamp
	DateUnion = Misc_GetUnixTimeStamp(SysTime.wYear,SysTime.wMonth,SysTime.wDay,0,0,0);

	// Encriptar timestamp con el propio valor obtenido
	CommonKey  = Chat_EncodeNumberWithItSelf(DateUnion);

	// Obtener nombre del mailslot comun
	Misc_EncodeNumberStr(CommonKey,(char *)CommonName);

	// Obtener clave del propietario
	OwnKey = Chat_EncodeNumber(IPAddress,DateUnion);

	// Obtener nombre del mailslot del propietario
	Chat_GetOwnerName(OwnKey,(char *)OwnName);

	// Inicializar comunicacion
// 	Comm_InitModule ((char *)CommonName,(char *)OwnName);
    Comm_InitModule ((char *)CommonName,(char *)"PrivateSlot");
}
// --------------------------------------------------------------------------
void Chat_SendGeneralChatMessage(TChatMessage *CMess)
{
	TCriptData		CD;
	TMessage		Message;
	unsigned int	PacketLen;

	// Rellenar la estructura de encriptacion
	PacketLen   = CHAT_MESSAGE_HEADERSIZE + CMess->MsgLen;
	CD.Len      = PacketLen;
	memcpy(CD.Data,CMess,PacketLen);

	// Encriptar los datos con la clave del propietario destino
	Cript_EncodeData(&CD,DateUnion);

	// Establecer estructura mensaje
	Message.MsgLen  = PacketLen + sizeof(unsigned int);
	Message.MsgData = (void *)&CD;

	// Enviar mensaje
	Comm_SendMessage(MSG_RECIPIENT_COMMON_PIPE,&Message);
}
// --------------------------------------------------------------------------
void Chat_SendP2PChatMessage(unsigned int DstIPAddress,TChatMessage* CMess)
{
	TCriptData		CD;
	TMessage		Message;
	unsigned int	PacketLen;
	unsigned int    DstOwnKey;

	// Encriptar con la clave del usuario fuente
	Chat_EncodeData((char *)CMess->Message,CMess->MsgLen,OwnKey);
	PacketLen = CHAT_MESSAGE_HEADERSIZE + CMess->MsgLen;

	// Rellenar la estructura de encriptacion
	CD.Len      = PacketLen;
	memcpy((void *)CD.Data,(void *)CMess,PacketLen);

	// Obtener la clave propietaria del usuario destino
	DstOwnKey = Chat_EncodeNumber(DstIPAddress,DateUnion);

	// Encriptar los datos con la clave del propietario destino
	Cript_EncodeData(&CD,DstOwnKey);

	// Establecer estructura mensaje
	Message.MsgLen  = PacketLen + sizeof(unsigned int);
	Message.MsgData = (void *)&CD;

	// Obtener nombre del mailslot propietario del usuario destino
/*
	char  DstName[MAX_CAR_NUM];
	unsigned int   DstOwnKey;
	MSG_RECIPIENT_OWN_PIPE
    Chat_GetOwnerName(DstIPAddress,DstName);
*/
	// Enviar mensaje
	Comm_SendMessage(DstIPAddress ,&Message);
}
// --------------------------------------------------------------------------
void Chat_SendChatMessage(char *MessageStr)
{
	TChatMessage CMess;

	CMess.MsgID  = CHAT_MESSAGE_COMMON;
	CMess.SrcID  = OwnID;

    if(strlen(MessageStr) + 1 > MAX_LEN_CHAT_MESSAGE)
        CMess.MsgLen = MAX_LEN_CHAT_MESSAGE;
    else
        CMess.MsgLen = strlen(MessageStr) + 1;

    strncpy(CMess.Message,MessageStr,CMess.MsgLen - 1);
    CMess.Message[CMess.MsgLen - 1] = 0;

    Chat_SendGeneralChatMessage(&CMess);
}
// --------------------------------------------------------------------------
void Chat_SendConnectMessage(char *MessageStr)
{
	TChatMessage CMess;

	CMess.MsgID  = CHAT_MESSAGE_CONNECT;
	CMess.SrcID  = OwnID;
	CMess.MsgLen = strlen(MessageStr) + 1;
	strcpy(CMess.Message,MessageStr);

	Chat_SendGeneralChatMessage(&CMess);
}
// --------------------------------------------------------------------------
void Chat_SendDisconnectMessage(char *MessageStr)
{
	TChatMessage CMess;

	CMess.MsgID  = CHAT_MESSAGE_DISCONNECT;
	CMess.SrcID  = OwnID;
	CMess.MsgLen = strlen(MessageStr) + 1;
	strcpy(CMess.Message,MessageStr);

	Chat_SendGeneralChatMessage(&CMess);
}
// --------------------------------------------------------------------------
void Chat_SendConnectedMessage(char *MessageStr)
{
	TChatMessage CMess;

	CMess.MsgID  = CHAT_MESSAGE_CONNECTED;
	CMess.SrcID  = OwnID;
	CMess.MsgLen = strlen(MessageStr) + 1;
	strcpy(CMess.Message,MessageStr);

	Chat_SendGeneralChatMessage(&CMess);
}
// --------------------------------------------------------------------------
void Chat_SendBroadCastMessage(char *MessageStr)
{
	TChatMessage CMess;

	CMess.MsgID  = CHAT_MESSAGE_BROADCAST;
	CMess.SrcID  = OwnID;
    if(strlen(MessageStr) + 1 > MAX_LEN_CHAT_MESSAGE)
        CMess.MsgLen = MAX_LEN_CHAT_MESSAGE;
    else
        CMess.MsgLen = strlen(MessageStr) + 1;

    strncpy(CMess.Message,MessageStr,CMess.MsgLen - 1);
    CMess.Message[CMess.MsgLen - 1] = 0;

	Chat_SendGeneralChatMessage(&CMess);
}
// --------------------------------------------------------------------------
void Chat_SendPrivateMessage(unsigned int DstIPAddress,char *MessageStr)
{
	TChatMessage CMess;

	CMess.MsgID  = CHAT_MESSAGE_PRIVATE;
	CMess.SrcID  = OwnID;

    if(strlen(MessageStr) + 1 > MAX_LEN_CHAT_MESSAGE)
        CMess.MsgLen = MAX_LEN_CHAT_MESSAGE;
    else
        CMess.MsgLen = strlen(MessageStr) + 1;

    strncpy(CMess.Message,MessageStr,CMess.MsgLen - 1);
    CMess.Message[CMess.MsgLen - 1] = 0;

    Chat_SendP2PChatMessage(DstIPAddress,&CMess);
}
// --------------------------------------------------------------------------
bool Chat_RecvChatMessage(TChatMessage *CMess)
{
	TCriptData		CD;
	TMessage		CommMess;

    CommMess.MsgData = (char*)&CD;

	if (Comm_RecvMessage(MSG_RECIPIENT_COMMON_PIPE,&CommMess))
	{
		// Mensaje comun
		Cript_DecodeData(&CD,DateUnion);
		memcpy(CMess,CD.Data,CD.Len);
		return(true);
	}

	return(false);
}
// --------------------------------------------------------------------------
bool Chat_RecvPrivateChatMessage(TChatMessage *CMess)
{
	TCriptData		CD;
	TMessage		CommMess;
	unsigned int    SrcOwnKey;

    CommMess.MsgData = (char*)&CD;

	if (Comm_RecvMessage(MSG_RECIPIENT_PRIVATE_PIPE,&CommMess))
	{
		// Desencriptar con nuestra clave
		Cript_DecodeData(&CD,OwnKey);
        if (CD.Len > MAX_LEN_FILE_MESSAGE) return(false);

        // Copiar datos desencriptados en mensaje
		memcpy(CMess,CD.Data,CD.Len);

        // Obtener la clave propietaria del usuario fuente
        SrcOwnKey = Chat_EncodeNumber(CMess->SrcID,DateUnion);

    	// Desencriptar con la clave del usuario fuente
	    Chat_DecodeData((char *)CMess->Message,&CMess->MsgLen,SrcOwnKey);

		return(true);
	}

/*
	if (Comm_RecvMessage(MSG_RECIPIENT_OWN_PIPE,&CommMess))
	{

		return(true);
	}
*/

	return(false);
}
// --------------------------------------------------------------------------
void Chat_SendPrivateFile(unsigned int DstIPAddress,char *Filename)
{
	TChatMessage CMess;

    FILE *fd = fopen(Filename,"rb");
    if (fd == NULL) return;

    fseek(fd,0,SEEK_END);
    CMess.MsgLen = ftell(fd);
    fseek(fd,0,SEEK_SET);

    if (CMess.MsgLen > MAX_LEN_FILE_MESSAGE)
        CMess.MsgLen = MAX_LEN_FILE_MESSAGE;

    fread(CMess.Message,CMess.MsgLen,1,fd);    fclose(fd);

	CMess.MsgID  = CHAT_MESSAGE_RECV_FILE;
	CMess.SrcID  = OwnID;

    Chat_SendP2PChatMessage(DstIPAddress,&CMess);
}
// --------------------------------------------------------------------------
void Chat_RecvPrivateFile(unsigned int DstIPAddress,char *Filename)
{
	TChatMessage CMess;

	CMess.MsgID  = CHAT_MESSAGE_RECV_FILE;
	CMess.SrcID  = OwnID;
	CMess.MsgLen = strlen(Filename) + 1;
	strcpy(CMess.Message,Filename);

    Chat_SendP2PChatMessage(DstIPAddress,&CMess);
}
// --------------------------------------------------------------------------
void Chat_FinishChat()
{
	Comm_FinishModule();
}
// --------------------------------------------------------------------------