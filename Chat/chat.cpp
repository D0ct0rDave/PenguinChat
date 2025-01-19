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
	Cript_EncodeData(&Number,4,Key);
	return(Number);
}
// --------------------------------------------------------------------------
unsigned int Chat_EncodeNumberWithItSelf(unsigned int Number)
{
    return( Chat_EncodeNumber(Number,Number) );
}
// --------------------------------------------------------------------------
void Chat_GetOwnerName(unsigned int OwnKey,char *OwnerName)
{
	// Encriptar ip con TimeStamp
    Cript_EncodeData(&OwnKey,sizeof(unsigned int),DateUnion);

	// Obtener nombre del mailslot comun
	Misc_EncodeNumberStr(OwnKey,OwnerName);
}
// --------------------------------------------------------------------------
TChatMessage* Chat_CreateMessage(unsigned long MsgSize)
{
    // La idea es crear un paquete de tamaño definido por el usuario,
    TChatMessage* CMess = (TChatMessage*) malloc(CHAT_MESSAGE_HEADERSIZE + MsgSize);

    CMess->SrcID = OwnID;
    CMess->MsgLen= MsgSize;

    return (CMess);
    // de forma que podamos enviar ficheros de cualquier tamaño
}
// --------------------------------------------------------------------------
void Chat_DestroyMessage(TChatMessage* CMess)
{
    free((void*)CMess);
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
void Chat_SendGeneralMessage(void *Data,unsigned long Len,unsigned int MsgID)
{
    // Create message struct
    TChatMessage* CMess = Chat_CreateMessage(Len);

    // Fill out fields
    CMess->MsgID  = MsgID;
    memcpy(CMess->Message,Data,Len);

	// Calcular tamaño total del paquete
	unsigned long PacketLen = CHAT_MESSAGE_HEADERSIZE + CMess->MsgLen;

	// Encriptar toda la estructura con la clave del propietario destino
	Cript_EncodeData(CMess,PacketLen,DateUnion);

	// Enviar mensaje
	Comm_SendMessage(MSG_RECIPIENT_COMMON_PIPE,CMess,PacketLen);

    // Destroy structure
    Chat_DestroyMessage(CMess);
}
// --------------------------------------------------------------------------
void Chat_SendP2PChatMessage(unsigned int DstIPAddress,TChatMessage* CMess)
{
    // Fill out fields
    CMess->MsgID  = CHAT_MESSAGE_PRIVATE;

	// Calcular tamaño total del paquete
	unsigned long PacketLen = CHAT_MESSAGE_HEADERSIZE + CMess->MsgLen;

	// Obtener la clave propietaria del usuario destino
	unsigned int DstOwnKey = Chat_EncodeNumber(DstIPAddress,DateUnion);

	// Encriptar toda la estructura con la clave del propietario destino
	Cript_EncodeData(CMess,PacketLen,DstOwnKey);

	// Encriptar los datos con la clave del propietario fuente
	Cript_EncodeData(CMess->Message,PacketLen - CHAT_MESSAGE_HEADERSIZE,OwnKey);

	// Enviar mensaje al destinatario
	Comm_SendMessage(DstIPAddress,CMess,PacketLen);

    // Destroy structure
    Chat_DestroyMessage(CMess);
}
// --------------------------------------------------------------------------
void Chat_SendChatMessage(char *MessageStr)
{
    Chat_SendGeneralMessage(MessageStr,strlen(MessageStr)+1,CHAT_MESSAGE_COMMON); // include EOS
}
// --------------------------------------------------------------------------
void Chat_SendConnectMessage(char *MessageStr)
{
    Chat_SendGeneralMessage(MessageStr,strlen(MessageStr)+1,CHAT_MESSAGE_CONNECT); // include EOS
}
// --------------------------------------------------------------------------
void Chat_SendDisconnectMessage(char *MessageStr)
{
    Chat_SendGeneralMessage(MessageStr,strlen(MessageStr)+1,CHAT_MESSAGE_DISCONNECT); // include EOS
}
// --------------------------------------------------------------------------
void Chat_SendConnectedMessage(char *MessageStr)
{
    Chat_SendGeneralMessage(MessageStr,strlen(MessageStr)+1,CHAT_MESSAGE_CONNECTED); // include EOS
}
// --------------------------------------------------------------------------
void Chat_SendBroadCastMessage(char *MessageStr)
{
    Chat_SendGeneralMessage(MessageStr,strlen(MessageStr)+1,CHAT_MESSAGE_BROADCAST); // include EOS
}
// --------------------------------------------------------------------------
void Chat_SendPrivateMessage(unsigned int DstIPAddress,TChatMessage* CMess)
{
    Chat_SendP2PChatMessage(DstIPAddress,CMess);
}
// --------------------------------------------------------------------------
TChatMessage* Chat_RecvMessage()
{
	TChatMessage*   CMess = NULL;

    unsigned long Len = Comm_PendMessage(MSG_RECIPIENT_COMMON_PIPE);
    if (Len > 0)
    {
        // Len includes the headersize
        CMess = Chat_CreateMessage(Len - CHAT_MESSAGE_HEADERSIZE);

        // Receive data
	    if (Comm_RecvMessage(MSG_RECIPIENT_COMMON_PIPE,(void*)CMess,Len))
	    {
            // Mensaje comun
    		Cript_DecodeData((void*)CMess,Len,DateUnion);
    	}
    }

	return(CMess);
}
// --------------------------------------------------------------------------
TChatMessage* Chat_RecvPrivateMessage()
{
	TChatMessage*   CMess = NULL;

    unsigned long Len = Comm_PendMessage(MSG_RECIPIENT_PRIVATE_PIPE);
    if (Len > 0)
    {
        // Len includes the headersize
        CMess = Chat_CreateMessage(Len - CHAT_MESSAGE_HEADERSIZE);

        // Receive data
	    if (Comm_RecvMessage(MSG_RECIPIENT_PRIVATE_PIPE,(void*)CMess,Len))
	    {
            // Mensaje comun
    		Cript_DecodeData((void*)CMess,Len,OwnKey);

            // Obtener la clave propietaria del usuario fuente
            unsigned int SrcOwnKey = Chat_EncodeNumber(CMess->SrcID,DateUnion);

            Cript_DecodeData(CMess->Message,Len - CHAT_MESSAGE_HEADERSIZE,SrcOwnKey);
    	}
    }

	return(CMess);
}
// --------------------------------------------------------------------------
void Chat_FinishChat()
{
	Comm_FinishModule();
}
// --------------------------------------------------------------------------