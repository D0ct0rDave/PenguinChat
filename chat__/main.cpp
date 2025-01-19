#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "chat.h"
#include <winsock2.h>
#include "user.h"

unsigned int MY_IP_ADDRESS = 0x7f000001;
// ----------------------------------------------------------------------------
#define MAXC_SNAME 256
void GetHostName(char *_szHostName)
{
	gethostname(_szHostName,MAXC_SNAME);
}
// ----------------------------------------------------------------------------
unsigned int uiGetHostByName(char *_szHostName)
{
	struct hostent		*st;

	if ( (st=gethostbyname(_szHostName)) == 0) return(-1);
	return ( *(unsigned int*)(st->h_addr_list[0]) );
}
//---------------------------------------------------------------------------
unsigned int uiGetNetPars(char* _szHostName)
{
	WSADATA		WSAData;
	LPWSADATA	lpWSAData = &WSAData;

	WSAStartup(2,lpWSAData);
	GetHostName(_szHostName);

	return( uiGetHostByName(_szHostName) );
}
//---------------------------------------------------------------------------
void Init()
{
	char szHostName[MAXC_SNAME];
	MY_IP_ADDRESS = uiGetNetPars(szHostName);

	Chat_InitChat(MY_IP_ADDRESS);
	User_AddUser(MY_IP_ADDRESS,szHostName,"Lolo","actors/pp/prueba.tga");	
}
void Finish()
{
	Chat_FinishChat();
}
//---------------------------------------------------------------------------
void StablishChatConetion()
{
	char StrBuff[100];

	// Establecer conexion al chat
	sprintf(StrBuff,"%s %s %s",
		GetHostName(MY_IP_ADDRESS),
		GetUserNick(MY_IP_ADDRESS),
		GetUserActor(MY_IP_ADDRESS));

	Chat_SendConnectMessage(StrBuff);
}
//---------------------------------------------------------------------------
void ParseChatUser(char *Str,char *NickName,char *hostname,char *Actor)
{
	// Get NickName
	while (*Str!=' ')
	{
		*NickName = *Str;
		NickName++;
		Str++;
	}
	*NickName=0;
	Str++;

	// Get hosname
	while (*Str!=' ')
	{
		*hostname = *Str;
		hostname++;
		Str++;
	}
	*hostname=0;
	Str++;

	// Get Actor
	while ((*Str!=' ') && (*Str))
	{
		*Actor = *Str;
		Actor++;
		Str++;
	}
	*Actor=0;
	Str++;
}
//---------------------------------------------------------------------------
void ParseChatMessage(TChatMessage *CMess)
{
	char StrBuff [100];
	char NickName[100],*pNickName = NickName;
	char HostName[100],*pHostName = HostName;
	char Actor   [100],*pActor    = Actor;

	switch (CMess->MsgID)
	{
		case CHAT_MESSAGE_CONNECT	:   // Introducir el user en nuestra BD
										if (! User_ExistsUser(CMess->SrcID))
										{
											ParseChatUser(CMess->Message,NickName,HostName,Actor);
											User_AddUser(CMess->SrcID,HostName,NickName,Actor);
										}
										else
										{
											pNickName = GetUserNick (CMess->SrcID);
											pHostName = GetHostName (CMess->SrcID);
											pActor    = GetUserActor(CMess->SrcID);
										}

										sprintf(StrBuff,"%s is connected from %s using %s as actor.",pNickName,pHostName,pActor);

										printf("%s\n",StrBuff);

										sprintf(StrBuff,"%s %s %s",
											GetHostName(MY_IP_ADDRESS),
											GetUserNick(MY_IP_ADDRESS),
											GetUserActor(MY_IP_ADDRESS));

										Chat_SendConnectedMessage(StrBuff);
		break;

		case CHAT_MESSAGE_DISCONNECT:   Chat_SendDisconnectMessage(NULL);
		break;

		case CHAT_MESSAGE_CONNECTED	:	    // Se llega aqui cuando enviamos informacion
											// de conexion y el resto de users nos
											// informan de que ellos estan conectados.
											// En ese caso, podemos recibir mensajes
											// nuestros (propios), o del resto de usuarios
											// en cuyo caso, deberemos ir añadiendolos
											// a la lista de usuarios conectados.
											// Si algun otro usuario pide conexion,
											// habremos enviado un mensaje de confirmacion
											// de conexion, por lo que no será necesario
											// añadir nuevos usuarios (se supone que
											// ya los conociamos de antes)
											if (! User_ExistsUser(CMess->SrcID))
											{
												ParseChatUser(CMess->Message,NickName,HostName,Actor);
												User_AddUser(CMess->SrcID,HostName,NickName,Actor);

												sprintf(StrBuff,"%s is connected from %s using %s as actor.",pNickName,pHostName,pActor);
												printf("%s\n",StrBuff);
											}
											break;

		case CHAT_MESSAGE_COMMON	:		printf("%s:%s\n",GetUserNick(CMess->SrcID),CMess->Message);
											break;
		case CHAT_MESSAGE_BROADCAST	:
											break;
		case CHAT_MESSAGE_SEND_FILE	:
											break;
		case CHAT_MESSAGE_RECV_FILE	:
											break;
		case CHAT_MESSAGE_PRIVATE	:
											break;
	}
}
//---------------------------------------------------------------------------
void main()
{
	Init();

		StablishChatConetion();

		do{
			
			TChatMessage CMess;
			while (Chat_RecvChatMessage(&CMess))
				ParseChatMessage(&CMess);
				
		}while(1);

	Finish();
	
}
//---------------------------------------------------------------------------