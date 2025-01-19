// --------------------------------------------------------------------------
#ifndef UserH
#define UserH
// --------------------------------------------------------------------------
#define			MAX_USERS			50
#define			MAX_USERNICKS		1024
#define			MAX_CARS_NICKNAME	20
#define			MAX_CARS_HOSTNAME	256
#define			MAX_CARS_ACTORNAME	32		// Actorname ActorDir/ActorFile

#define			MAX_WIDTH_ACTOR		64
#define			MAX_HEIGHT_ACTOR	64

typedef struct{
	bool			Valid;
	unsigned long	IPAddress;
	char			HostName[MAX_CARS_HOSTNAME +1];
	char			Nick	[MAX_CARS_NICKNAME +1];
	char			Actor	[MAX_CARS_ACTORNAME+1];
}TUser;

// --------------------------------------------------------------------------
bool		  User_ExistsUser (unsigned int IPAddress);
unsigned int  User_AddUser    (unsigned int IPAddress,char *HostName,char *Nick,char *Actor);
void		  User_RemoveUser (unsigned int IPAddress);
unsigned int  User_ChangeNick (unsigned int IPAddress,char *Nick);
unsigned int  User_ChangeActor(unsigned int IPAddress,char *Actor);

// Retrieving information

unsigned int *User_WhoIs      (char *Nick);
TUser        *User_GetInfo    (unsigned int IPAddress);
char		 *User_GetHostName(unsigned int IPAddress);
char	     *User_GetNick    (unsigned int IPAddress);
char         *User_GetActor   (unsigned int IPAddress);

// --------------------------------------------------------------------------
int			 User_CreateUser (TUser *User,unsigned int IPAddress,char *HostName,char *Nick,char *Actor);
void		 User_DestroyUser(TUser *User);
// --------------------------------------------------------------------------
#endif
// --------------------------------------------------------------------------