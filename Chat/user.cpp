// --------------------------------------------------------------------------
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "user.h"

// --------------------------------------------------------------------------
// Definicion de las constantes necesarias en el módulo
// --------------------------------------------------------------------------

// --------------------------------------------------------------------------
// Definicion de los tipos necesarios en el módulo
// --------------------------------------------------------------------------
typedef struct{
	char         NickName[MAX_CARS_NICKNAME];		// El nick name
	unsigned int UserIdx;							// El usuario que lo posee
}TNick;

typedef struct{
	TNick		 Nick[MAX_USERNICKS];
	unsigned int NumNicks; 
}TNickList;

TUser		 Users[MAX_USERS];

// --------------------------------------------------------------------------
// Definición de las variables necesarias en el módulo
// --------------------------------------------------------------------------
TNickList	 NickList;
TUser		 UserList[MAX_USERS];
unsigned int WhoIsList[MAX_USERNICKS+1];
// --------------------------------------------------------------------------
int User_CreateUser(TUser *User,unsigned int IPAddress,char *HostName,char *Nick,char *Actor)
{
	if ((! HostName) || (! Nick)) return(0);
	if (! strlen(HostName) || ! strlen(Nick)) return(0);

	memset((void *)User  ,sizeof(TUser),0);

	User->IPAddress = IPAddress;
	strcpy(User->HostName,HostName);
	strcpy(User->Nick    ,Nick);

	if (Actor)
    	strcpy(User->Actor,Actor);

	User->Valid = true;
	return(1);
}
// --------------------------------------------------------------------------
void User_DestroyUser(TUser *User)
{
	if (! User) return;

	User->Valid = false;
}
// --------------------------------------------------------------------------
unsigned int User_FindNick(char *Nick)
{
	unsigned int cI;
	for (cI=0;cI<NickList.NumNicks;cI++)
	{
		if (! strcmp(NickList.Nick[cI].NickName,Nick)) 
			return(NickList.Nick[cI].UserIdx);
	}

	return(-1);
}
// --------------------------------------------------------------------------
unsigned int User_AddNickToList(char *Nick,unsigned int UserIdx)
{	
	unsigned int NickIdx;
	
	if (! Nick) return(0);
	if (! Nick[0]) return(0);

	NickIdx = User_FindNick(Nick);
	
	if (NickIdx==-1)
	{	
		if (NickList.NumNicks==MAX_USERNICKS)	
			return(0);
		else
		{
			NickList.Nick[NickList.NumNicks].UserIdx = UserIdx;
			strcpy(NickList.Nick[NickList.NumNicks].NickName,Nick);
			NickList.NumNicks++;
		}
	}

	return(1);	
}
// --------------------------------------------------------------------------
unsigned int User_GetUserIdx(unsigned int IPAddress)
{
	unsigned int cI;

	for (cI=0;cI<MAX_USERS;cI++)
		if ((UserList[cI].IPAddress == IPAddress) && (UserList[cI].Valid))
			return(cI);

	return(-1);
}
// --------------------------------------------------------------------------
unsigned int User_GetFreeUserIdx()
{
	unsigned int cI;

	for (cI=0;cI<MAX_USERS;cI++)
		if (! UserList[cI].Valid) 
			return(cI);

	return(-1);
}
// --------------------------------------------------------------------------
bool User_ExistsUser (unsigned int IPAddress)
{
	return( User_GetUserIdx(IPAddress) !=-1);
}
// --------------------------------------------------------------------------
unsigned int User_AddUser(unsigned int IPAddress,char *HostName,char *Nick,char *Actor)
{
	unsigned int UserIdx;
	UserIdx = User_GetFreeUserIdx();

	if (UserIdx == -1) return(0);

	if (! User_CreateUser(&UserList[UserIdx],IPAddress,HostName,Nick,Actor))
		return(0);

	User_AddNickToList(Nick,UserIdx);
	return(1);
}
// --------------------------------------------------------------------------
void User_RemoveUser(unsigned int IPAddress)
{
	unsigned int UserIdx;
	UserIdx = User_GetUserIdx(IPAddress);
	if (UserIdx==-1) return;

	User_DestroyUser(&UserList[UserIdx]);
}
// --------------------------------------------------------------------------
unsigned int User_ChangeNick(unsigned int IPAddress,char *Nick)
{
	unsigned int UserIdx;
	UserIdx = User_GetUserIdx(IPAddress);
	if (UserIdx==-1) return(0);

	if (! Nick) return(0);
	strcpy(UserList[UserIdx].Nick,Nick);

	User_AddNickToList(Nick,UserIdx);
	return(1);
}
// --------------------------------------------------------------------------
unsigned int User_ChangeActor(unsigned int IPAddress,char *Actor)
{
	unsigned int UserIdx;

	UserIdx = User_GetUserIdx(IPAddress);
	if (UserIdx==-1) return(0);

	if (! Actor) return(0);
	strcpy(UserList[UserIdx].Actor,Actor);

	if (!Actor[0])
		return(1);
	
	return(0);
}
// --------------------------------------------------------------------------
// Retrieving information
// --------------------------------------------------------------------------
unsigned int *User_WhoIs(char *Nick)
{	
	unsigned int cI;
	unsigned int cJ;

	WhoIsList[0] = -1;

	if (! Nick) return(WhoIsList);
	if (! Nick[0]) return(WhoIsList);
	
	cJ = 0;
	for (cI=0;cI<NickList.NumNicks;cI++)
	{
		if (! strcmp(Nick,NickList.Nick[cI].NickName))
		{
			WhoIsList[cJ++] = UserList[NickList.Nick[cI].UserIdx].IPAddress;
			WhoIsList[cJ] = -1;
		}
	}
	
	return(WhoIsList);
}
// --------------------------------------------------------------------------
TUser *User_GetInfo(unsigned int IPAddress)
{
	unsigned int UserIdx;
	UserIdx = User_GetUserIdx(IPAddress);
	if (UserIdx==-1) return(NULL);

	return(&UserList[UserIdx]);
}
// --------------------------------------------------------------------------
char *User_GetHostName(unsigned int IPAddress)
{
	unsigned int UserIdx;
	UserIdx = User_GetUserIdx(IPAddress);
	if (UserIdx==-1) return(NULL);

	return(UserList[UserIdx].HostName);
}
// --------------------------------------------------------------------------
char *User_GetNick(unsigned int IPAddress)
{
	unsigned int UserIdx;
	UserIdx = User_GetUserIdx(IPAddress);
	if (UserIdx==-1) return(NULL);

	return(UserList[UserIdx].Nick);
}
// --------------------------------------------------------------------------
char *User_GetActor(unsigned int IPAddress)
{
	unsigned int UserIdx;
	UserIdx = User_GetUserIdx(IPAddress);
	if (UserIdx==-1) return(NULL);

	return(UserList[UserIdx].Actor);
}
// --------------------------------------------------------------------------
