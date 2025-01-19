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

    unsigned int NumberSize = sizeof(unsigned int);

	CD.Len   = NumberSize;
    CD.Data = (char*)&Number;

	Cript_EncodeData(&CD,Key);

	return(Number);
}
// --------------------------------------------------------------------------
unsigned int Chat_EncodeNumberWithItSelf(unsigned int Number)
{
    return( Chat_EncodeNumber(Number,Number) );
}
// --------------------------------------------------------------------------
void Chat_EncodeData(char *Data,unsigned int Len,unsigned int Key)
{
	TCriptData CD;

	CD.Len  = Len;
    CD.Data = Data;

	Cript_EncodeData(&CD,Key);
}
// --------------------------------------------------------------------------
void Chat_DecodeData(char *Data,unsigned int Len,unsigned int Key)
{
	TCriptData CD;

	CD.Len  = Len;
    CD.Data = Data;

    Cript_DecodeData(&CD,Key);
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
	unsigned int	PacketLen = CHAT_MESSAGE_HEADERSIZE + CMess->MsgLen;

	// Rellenar la estructura de encriptación
	CD.Len      =  PacketLen;
	CD.Data     = (char*)&CMess;

	// Encriptar los datos con la clave del propietario destino
 	Cript_EncodeData(&CD,DateUnion);

	// Establecer estructura mensaje
	Message.MsgLen  = PacketLen;
	Message.MsgData = (void *)&CMess;

	// Enviar mensaje
	Comm_SendMessage(MSG_RECIPIENT_COMMON_PIPE,&Message);
}
// --------------------------------------------------------------------------
void Chat_SendP2PChatMessage(unsigned int DstIPAddress,TChatMessage* CMess)
{
	TCriptData		CD;
	TMessage		Message;
	unsigned int    DstOwnKey;
    unsigned int	PacketLen = CHAT_MESSAGE_HEADERSIZE + CMess->MsgLen;

    // Encriptar con la clave del usuario fuente
	Chat_EncodeData((char *)CMess->Message,CMess->MsgLen,OwnKey);

	// Rellenar la estructura de encriptacion
	CD.Len      = PacketLen;
	CD.Data     = (char*)&CMess;

	// Obtener la clave propietaria del usuario destino
	DstOwnKey = Chat_EncodeNumber(DstIPAddress,DateUnion);

	// Encriptar los datos con la clave del propietario destino
	Cript_EncodeData(&CD,DstOwnKey);

	// Establecer estructura mensaje
	Message.MsgLen  = PacketLen;
	Message.MsgData = (void *)&CMess;

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

    // Prepare Comm Message struct
    CommMess.MsgData = (void*)CMess;

    // Receive messages
	if (Comm_RecvMessage(MSG_RECIPIENT_COMMON_PIPE,&CommMess))
	{
        // Prepare CriptData struct
        CD.Data = (char*)CMess;
        CD.Len  = CommMess.MsgLen;

    	// Mensaje común
		//Cript_DecodeData(&CD,DateUnion);
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

    // Prepare CriptData struct
    CD.Data = (char*)CMess;

    // Prepare Comm Message struct
    CommMess.MsgData = (char*)CMess;

	if (Comm_RecvMessage(MSG_RECIPIENT_PRIVATE_PIPE,&CommMess))
	{
        // Retrieve length
        unsigned int Len = CommMess.MsgLen;

		// Desencriptar con nuestra clave
		Cript_DecodeData(&CD,OwnKey);
        if (CD.Len > MAX_LEN_FILE_MESSAGE) return(false);

        // Obtener la clave propietaria del usuario fuente
        SrcOwnKey = Chat_EncodeNumber(CMess->SrcID,DateUnion);

    	// Desencriptar con la clave del usuario fuente
	    Chat_DecodeData((char *)CMess->Message,CMess->MsgLen,SrcOwnKey);

		return(true);
	}

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