//---------------------------------------------------------------------------
#include <vcl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <dir.h>
#include <errno.h>

#include <winsock2.h>

#pragma hdrstop

#include "skincache.h"
#include "GUIChat.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
//---------------------------------------------------------------------------
// Includes necesarios
//---------------------------------------------------------------------------
#include "chat.h"
#include "user.h"
#include "ChatHistory.h"
#include "palette.h"
#include "tex.h"
//---------------------------------------------------------------------------
// Defines necesarios
//---------------------------------------------------------------------------
#define     LEFT_MARGIN             2
#define     UPPER_MARGIN            2
#define     CHAT_ACTOR_SIZE         48
#define     CHAT_MESSAGE_HEIGHT     (CHAT_ACTOR_SIZE + UPPER_MARGIN)
#define     CHAT_MESSAGE_WIDTH      (CHAT_ACTOR_SIZE + LEFT_MARGIN)

#define     CHAT_ACTORS_DIRECTORY   ".\\Actors"
#define     CHAT_FILES_DIRECTORY    ".\\Files"

unsigned int MY_IP_ADDRESS = 0x7f000001;

TSkinCache ActorCache(CHAT_ACTORS_DIRECTORY);
bool Act = false;
TGUI *GUI;

unsigned int    FirstVMsg;
unsigned int    MaxMsgsWnd;
unsigned int    CurrentMsg;

bool            CallAttention = false;
TChatMessage    ChatMessage;
//---------------------------------------------------------------------------
// Cosas de la Interfaz gráfica
//---------------------------------------------------------------------------
unsigned int WhereActor = 0;    // Indica si la foto debe salir a la derecha
                                // o a la izquierda
unsigned int CurrentHeigth;
// --------------------------------------------------------------------------
#define CHAT_MESSAGE_PRIVATEMSG     0
#define CHAT_MESSAGE_SENDFILEREQ    1
#define CHAT_MESSAGE_RECVFILEREQ    2
#define CHAT_MESSAGE_SENDFILE       3
#define CHAT_MESSAGE_SENDACTOR      4
#define CHAT_MESSAGE_RECVACTOR      5
// --------------------------------------------------------------------------
TChatMessage* GUIChat_CreatePrivateMessage(unsigned long Len)
{
    return( Chat_CreateMessage(Len+4) );
}

void GUIChat_SetPrivateMessageID(TChatMessage* CMess,unsigned int PrivMsgType)
{
    *(unsigned int *)CMess->Message = PrivMsgType;
}

unsigned int GUIChat_GetPrivateMessageID(TChatMessage* CMess)
{
    return ( *(unsigned int *)CMess->Message );
}

void* GUIChat_GetPrivateMessageData(TChatMessage* CMess)
{
    return ( CMess->Message + sizeof(unsigned int) );
}
// --------------------------------------------------------------------------
void GUIChat_SendPrivateMessage(unsigned int DstIPAddress,unsigned int PrivMsgType,unsigned char *Message,unsigned int Len)
{
    // Create message struct
    TChatMessage* CMess = GUIChat_CreatePrivateMessage(Len);
    GUIChat_SetPrivateMessageID(CMess,PrivMsgType);

        // Copy message data
        memcpy(GUIChat_GetPrivateMessageData(CMess),Message,Len);

        // Send data
        Chat_SendPrivateMessage(DstIPAddress,CMess);

    // Destroy message struct
    Chat_DestroyMessage(CMess);
}
// --------------------------------------------------------------------------
void GUIChat_SendFileGeneric(unsigned int DstIPAddress,unsigned int PrivMsgType,char* InDir,char *Filename)
{
    chdir(InDir);

        int filenamelen = strlen(Filename) + 1;
        int Len;

        FILE *fd = fopen(Filename,"rb");
        if (fd == NULL) return;

        fseek(fd,0,SEEK_END);
        Len = ftell(fd);
        fseek(fd,0,SEEK_SET);

            // Create message capable of storing filename & file contents
            TChatMessage* CMess = GUIChat_CreatePrivateMessage(filenamelen + Len);
            GUIChat_SetPrivateMessageID(CMess,PrivMsgType);

                // Get pointer to data
                char* pData = (char*)GUIChat_GetPrivateMessageData(CMess);

                // Copy the filename at the beginning of the buffer
                memcpy(pData,Filename,filenamelen);

                // Read file contents
                fread(pData + filenamelen,Len,1,fd);

                // Send data
                Chat_SendPrivateMessage(DstIPAddress,CMess);

            // Destroy message struct
            Chat_DestroyMessage(CMess);

        fclose(fd);

    chdir("..");
}
//---------------------------------------------------------------------------
void GUIChat_SendFile(unsigned int DstIPAddress,char *Filename)
{
    GUIChat_SendFileGeneric(DstIPAddress,CHAT_MESSAGE_SENDFILE,CHAT_FILES_DIRECTORY,Filename);
}
//---------------------------------------------------------------------------
void GUIChat_SendActor(unsigned int DstIPAddress,char *Filename)
{
    GUIChat_SendFileGeneric(DstIPAddress,CHAT_MESSAGE_RECVACTOR,CHAT_ACTORS_DIRECTORY,Filename);
}
// --------------------------------------------------------------------------
void GUIChat_RecvFileGeneric(char *OutDir,TChatMessage* CMess)
{
    chdir(OutDir);

        // Get filename
        char* Filename    = CMess->Message + 4;
        int   FilenameLen = strlen(Filename)+1;

        FILE *fd = fopen(Filename,"wb");
        if (fd)
        {
            fwrite(CMess->Message + 4 + FilenameLen,CMess->MsgLen - 4 - FilenameLen,1,fd);
            fclose(fd);
        }

    chdir("..");
}
// --------------------------------------------------------------------------
void GUIChat_RecvFile(TChatMessage* CMess)
{
    GUIChat_RecvFileGeneric(CHAT_FILES_DIRECTORY,CMess);
}
// --------------------------------------------------------------------------
void GUIChat_RecvActor(TChatMessage* CMess)
{
    GUIChat_RecvFileGeneric(CHAT_ACTORS_DIRECTORY,CMess);
}
//---------------------------------------------------------------------------
__fastcall TGUI::TGUI(TComponent* Owner)    : TForm(Owner)
{
}
//---------------------------------------------------------------------------
void Control_InitialState()
{
	// Comprobar que existe el directorio Actors
    if (chdir(CHAT_ACTORS_DIRECTORY) == -1)
    {
    	if (mkdir(CHAT_ACTORS_DIRECTORY)==-1)
    	{
    		ShowMessage("Imposible crear directorio Actors. Adios!");
            exit(1);
     	}
    }
    else
    	chdir("..");

	// Comprobar que existe el directorio Files
    if (chdir(CHAT_FILES_DIRECTORY) == -1)
    {
    	if (mkdir(CHAT_FILES_DIRECTORY)==-1)
    	{
    		ShowMessage("Imposible crear directorio Files. Adios!");
            exit(1);
     	}
    }
    else
    	chdir("..");
}
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
void __fastcall TGUI::Deactivate(TObject *Sender)
{
    Timer2->Enabled = true;
}
//---------------------------------------------------------------------------
void __fastcall TGUI::Activate(TObject *Sender)
{
    Timer2->Enabled = false;
    WriteCaption();
}
//---------------------------------------------------------------------------
void __fastcall TGUI::FormCreate(TObject *Sender)
{
    // Install application event handlers
    Application->OnActivate = Activate;
    // Application->OnDeactivate = Deactivate;

    AnsiString asNickname = InputBox("Penguin Chat:","Nickname:","Pengin");
	Control_InitialState();

    char szHostName[MAXC_SNAME];
    MY_IP_ADDRESS = uiGetNetPars(szHostName);

    // To debug 2 apps in the same computer
    // MY_IP_ADDRESS = (unsigned int)Application->Handle;

    Chat_InitChat(MY_IP_ADDRESS);
    User_AddUser(MY_IP_ADDRESS,szHostName,asNickname.c_str(),"prueba.tga");

    ChatHistory_ClearHistory();

//    MaxMsgsWnd = pb_Display->Height / 64;
    CurrentMsg = 0;

    CurrentHeigth = Height;
}
//---------------------------------------------------------------------------
void __fastcall TGUI::FormDestroy(TObject *Sender)
{
    Chat_FinishChat();
}
//---------------------------------------------------------------------------
void __fastcall TGUI::ed_MessageKeyDown(TObject *Sender, WORD &Key,TShiftState Shift)
{
    char StrBuff[1024];

    if(Key == VK_RETURN)
    {
        // Control sobre que lado se debe visualizar la photo
        if (WhereActor) StrBuff[0] = 'R'; else StrBuff[0] = 'L';
        StrBuff[1] = ' ';

        // Escribir la string del mensaje
        strncpy(&StrBuff[2],ed_Message->Text.c_str(),1022);
        StrBuff[1023] = 0;

        Chat_SendChatMessage(StrBuff);
        ed_Message->Text = "";
    }
}
//---------------------------------------------------------------------------
void TGUI::StablishChatConetion()
{
    char StrBuff[100];

    // Establecer conexion al chat
    sprintf(StrBuff,"%s %s %s",
            User_GetHostName(MY_IP_ADDRESS),
            User_GetNick(MY_IP_ADDRESS),
            User_GetActor(MY_IP_ADDRESS));

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
void ParseFile(TChatMessage *CMess)
{

}
//---------------------------------------------------------------------------
void TGUI::DrawActor(Texture *tex,TCanvas *C,unsigned int cx,unsigned int cy)
{
	unsigned int r,g,b;
    unsigned int cI;
    unsigned int cJ;
    unsigned int TX,TY;
    TFColor Back;

    if (! tex) return;

    TX = CHAT_ACTOR_SIZE; // C->ClipRect.Width();
    TY = CHAT_ACTOR_SIZE; // +C->ClipRect.Height();

    if ((TX == 0) || (TY == 0)) return;

    Back.r = ((C->Brush->Color >>  0) & 0x000000ff) / 255.0f;
    Back.g = ((C->Brush->Color >>  8) & 0x000000ff) / 255.0f;
    Back.b = ((C->Brush->Color >> 16) & 0x000000ff) / 255.0f;
    Back.a = ((C->Brush->Color >> 24) & 0x000000ff) / 255.0f;



    float fXStep = (float)tex->TX / (float)TX;
    float fYStep = (float)tex->TY / (float)TY;

    float fY = 0.0f;
    for (cJ=0;cJ<TY;cJ++)
    {
        float fX = 0.0f;
        for (cI=0;cI<TX;cI++)
        {
            TFColor FColor = GetFColorf(tex,fX,fY);

            if (FColor.a != 0.0f)
            	FColor = Interpolate(Back,FColor,(float)FColor.a);

            C->Pixels[cx + cI][cy + cJ] = ((int)(FColor.b*255.0f) << 16) | ((int)(FColor.g*255.0f) << 8) | ((int)(FColor.r*255.0f));

            fX += fXStep;
        }

        fY += fYStep;
    }
}
//---------------------------------------------------------------------------
void __fastcall TGUI::FormShow(TObject *Sender)
{
    // Application Caption
    WriteCaption();

    // Label caption
    char StrBuff [100];

    StablishChatConetion();
    sprintf(StrBuff,"User: %s",User_GetNick(MY_IP_ADDRESS));

    // Display User Properties
    lb_Nick->Caption = StrBuff;

    // Enable chat timer
    Timer1->Enabled = true;
}
//---------------------------------------------------------------------------
void TGUI::HandleControls()
{
}
//---------------------------------------------------------------------------
void TGUI::DrawText(TPaintBox *Dsp,TRect &Rect,unsigned int X,unsigned int Y,AnsiString String)
{
	unsigned int TextHeight = Dsp->Canvas->TextHeight("W");
    float MediumCharWidth   = (float)Dsp->Canvas->TextWidth(String)/String.Length();
    int MaxChars            = ((Rect.Right - Rect.Left)/MediumCharWidth)-2;

    while (String.Length() > MaxChars)
    {
        Dsp->Canvas->TextOut(X,Y,String.SubString(1,MaxChars));
        String = String.SubString(MaxChars,String.Length()-MaxChars);

        Y               += TextHeight;
        
        if (Y + TextHeight >= Rect.Bottom) return;
    }

    Dsp->Canvas->TextOut(X,Y,String);
}
//---------------------------------------------------------------------------
void TGUI::DisplayChatMessage(TChatMessage *CMess,char *NickName,Texture *Actor)
{
	 AnsiString NickNameString,MessageString,String;
     Windows::TRect Rect;
     unsigned int cXA,cYA,cXM,cYM,cXN,cYN;

     // Controlar las coordenadas de display
     if (CMess->Message[0] == 'L')
     {
        cXA = LEFT_MARGIN;
        cYA = UPPER_MARGIN + CurrentMsg * CHAT_MESSAGE_HEIGHT;

        cXM = LEFT_MARGIN + CHAT_MESSAGE_WIDTH;
        cYM = cYA;

        cXN = cXA;
        cYN = (cYA + CHAT_ACTOR_SIZE) - pb_Display->Canvas->Font->Height;
     }
else if (CMess->Message[0] == 'R')
     {
        cXA = pb_Display->Width - (CHAT_MESSAGE_WIDTH);
        cYA = UPPER_MARGIN + CurrentMsg * CHAT_MESSAGE_HEIGHT;

        cXM = LEFT_MARGIN;
        cYM = cYA;

        cXN = cXA;
        cYN = (cYA + CHAT_ACTOR_SIZE) - pb_Display->Canvas->Font->Height;
     }

     Rect.Left  = cXM;
     Rect.Right = cXM + pb_Display->Width -  (LEFT_MARGIN + CHAT_MESSAGE_HEIGHT) - 1;
     Rect.Top   = cYM;
     Rect.Bottom= cYM + CHAT_MESSAGE_HEIGHT - 1;

     // Display Message
     NickNameString = "<" + AnsiString(NickName) + ">";

     MessageString  = (char *)(&CMess->Message[2]);
     String         = NickNameString + MessageString;
     pb_Display->Canvas->Font->Color = clBlack;
     DrawText(pb_Display,Rect,cXM,cYM,String);

     // Display Nickname
     if (CMess->SrcID == MY_IP_ADDRESS)
        pb_Display->Canvas->Font->Color = clBlue;
     else
         pb_Display->Canvas->Font->Color = clNavy;

     DrawText(pb_Display,Rect,cXM,cYM,NickNameString);

/*
     // Display message
     MessageString  = (char *)(&CMess->Message[2]);
     String         = MessageString;
     pb_Display->Canvas->Font->Color = clBlack;
     DrawText(pb_Display,Rect,cXM + pb_Display->Canvas->TextWidth(NickNameString) - 1,cYM,String);
*/


     // Draw actor
     DrawActor(Actor,pb_Display->Canvas,cXA,cYA);
     /*
      // Draw nickname
     Rect.Left  = cXA;
     Rect.Right = cXA+pb_Display->Canvas->TextWidth(NickName);
     if (Rect.Right > cXA + CHAT_ACTOR_SIZE) Rect.Right = cXA + CHAT_ACTOR_SIZE;
     Rect.Top   = cYA;
     Rect.Bottom= cYA+pb_Display->Canvas->TextHeight(NickName);

     pb_Display->Canvas->Font->Color = clRed;
     pb_Display->Canvas->TextRect(Rect,cXA,cYA,NickName);
     */
}
//---------------------------------------------------------------------------
void TGUI::AddMessageToChatWindow(TChatMessage *CMess,char *NickName,Texture *Actor)
{
	if (MaxMsgsWnd >= ChatHistory_GetNumMessages())
    {
    	// Si el numero de mensajes que caben es mayor del que hay en la historia,
        // dibujar utilizando el puntero normal
		DisplayChatMessage(CMess,
        				   User_GetNick(CMess->SrcID),
                           Actor);
        CurrentMsg++;
    }
	else
    {
       	// Si el último mensaje visible coincide con el ultimo de la historia
        if (FirstVMsg + MaxMsgsWnd+1 == ChatHistory_GetNumMessages())
        {
            FirstVMsg++;
            // Scroll bar control
            sb_Chat->Enabled  = true;
            sb_Chat->Max      = FirstVMsg;
            sb_Chat->Position = sb_Chat->Max;

            DisplayChatWindow();
        }
        else
        {
        	sb_Chat->Max++;
        }
	}
}
//---------------------------------------------------------------------------
void TGUI::DisplayChatWindow()
{
    pb_Display->Canvas->Rectangle(0,0,pb_Display->Width,pb_Display->Height);

    unsigned int cMsg,NumMsgs;
    char *UserNick;
    char *UserMsg;

    Texture *UserActor;
	TChatHistoryEntry *CHEntry;

    if (sb_Chat->Enabled) NumMsgs = MaxMsgsWnd; else NumMsgs = ChatHistory_GetNumMessages();

    CurrentMsg = 0;
    for (cMsg=0;cMsg<NumMsgs;cMsg++)
    {
    	CHEntry   = ChatHistory_GetChatMessage(FirstVMsg + cMsg);
    	UserActor = ActorCache.GetSkin(CHEntry->Actor);

		DisplayChatMessage(&CHEntry->Msg,CHEntry->User,UserActor);
    	CurrentMsg++;
    }
}
//---------------------------------------------------------------------------
void __fastcall TGUI::pb_ActorPaint(TObject *Sender)
{
	Texture *SkinActor = ActorCache.GetSkin(User_GetActor(MY_IP_ADDRESS));
    DrawActor(SkinActor,pb_Actor->Canvas,0,0);
}
//---------------------------------------------------------------------------
void __fastcall TGUI::pb_DisplayPaint(TObject *Sender)
{
    DisplayChatWindow();
}
//---------------------------------------------------------------------------
void __fastcall TGUI::bt_FlipActorClick(TObject *Sender)
{
    WhereActor = ! WhereActor;

    if (WhereActor)
    {
        // Foto a la derecha
        ed_Message->Left   = pn_Actor->Left;
        pn_Actor->Left     = ed_Message->Left + ed_Message->Width + 4;

    }
    else
    {
        // Foto a la izquierda
        pn_Actor->Left   = ed_Message->Left;
        ed_Message->Left = pn_Actor->Left + pn_Actor->Width + 4;
    }

    lb_Nick->Left = ed_Message->Left;
    ed_Message->SetFocus();
}
//---------------------------------------------------------------------------
void __fastcall TGUI::FormResize(TObject *Sender)
{
    int NewMaxMsgsWnd;
    int NewChatSpace;

    // altura controles fijos + Margen superior del display
    NewChatSpace  = Height - (8 + 8 + 246 + 8);
    NewMaxMsgsWnd = NewChatSpace / CHAT_MESSAGE_HEIGHT;

    if (NewMaxMsgsWnd <= 0)
        NewMaxMsgsWnd = 1;

    Height        = (NewMaxMsgsWnd * CHAT_MESSAGE_HEIGHT) + (8 + 8 + 246 + 8);

    if (NewMaxMsgsWnd != MaxMsgsWnd)
    {
        // Setup de alto del display
        pb_Display->Height = NewMaxMsgsWnd * CHAT_MESSAGE_HEIGHT + 2;
        sb_Chat ->Height   = pb_Display->Height;
        pn_Group->Top      = 8 + pb_Display->Height + 8;

        // Si hay menos mesajes en la historia de los que caben en la ventana
        MaxMsgsWnd = NewMaxMsgsWnd;
        if (MaxMsgsWnd > ChatHistory_GetNumMessages())
        {
        	sb_Chat->Enabled = false;
            FirstVMsg  = 0;
            CurrentMsg = ChatHistory_GetNumMessages()-1;
        }
        else
        {
            FirstVMsg         = ChatHistory_GetNumMessages() - MaxMsgsWnd;
            sb_Chat->Enabled  = true;
        	sb_Chat->Max      = FirstVMsg;
            sb_Chat->Position = FirstVMsg;
        }
    }

    if (Width != 306) Width = 306;
}
//---------------------------------------------------------------------------
void __fastcall TGUI::sb_ChatChange(TObject *Sender)
{
	FirstVMsg = sb_Chat->Position;
    DisplayChatWindow();
}
//---------------------------------------------------------------------------
void TGUI::WriteCaption()
{
    Application->Title = "PenguinChat";
}
//---------------------------------------------------------------------------
void TGUI::WriteAlternateCaption()
{
    Application->Title = User_GetNick(ChatMessage.SrcID);
}
//---------------------------------------------------------------------------
bool bAlternate = false;

void __fastcall TGUI::Timer2Timer(TObject *Sender)
{
    if (bAlternate)
    {
        WriteAlternateCaption();
    }
    else
    {
        WriteCaption();
    }

    bAlternate = ! bAlternate;
}
//---------------------------------------------------------------------------
void __fastcall TGUI::Timer1Timer(TObject *Sender)
{
    TChatMessage* CMess  = NULL;
    TChatMessage* PCMess = NULL;
    bool          bMsgRecv;

    do {
        bMsgRecv = false;

        // Look for common chat messages
        CMess = Chat_RecvMessage();
        if (CMess)
        {
            bMsgRecv = true;
            ParseChatMessage(CMess);
            Chat_DestroyMessage(CMess);
        }

        // Look for private chat messages
        PCMess = Chat_RecvPrivateMessage();
        if (PCMess)
        {
            bMsgRecv = true;
            ParseChatMessage(PCMess);
            Chat_DestroyMessage(PCMess);
        }

        // Si recibimos un mensaje y no tenemos la aplicación activa, hacer parpadear el
        // botón en la barra de tareas.
        if (bMsgRecv && (!Application->Active))
            Timer2->Enabled = true;

    }while(bMsgRecv);
}
//---------------------------------------------------------------------------
void TGUI::ParseChatMessage(TChatMessage *CMess)
{
	Texture *SkinActor;
    char StrBuff [100];
    char NickName[100],*pNickName;
    char HostName[100],*pHostName;
    char Actor   [100],*pActor;

    pNickName = User_GetNick (CMess->SrcID);
    pHostName = User_GetHostName (CMess->SrcID);
    pActor    = User_GetActor(CMess->SrcID);

    // Process Message
    switch (CMess->MsgID)
    {
        case CHAT_MESSAGE_CONNECT	:   // Introducir el user en nuestra BD
                                        if (! User_ExistsUser(CMess->SrcID))
                                        {
                                            ParseChatUser(CMess->Message,HostName,NickName,Actor);
                                            User_AddUser(CMess->SrcID,HostName,NickName,Actor);

                                            SkinActor = ActorCache.GetSkin(User_GetActor(CMess->SrcID));
                                            if (! SkinActor)
                                            {
                                                // Pedir fichero al otro
                                                char* szActor = User_GetActor(CMess->SrcID);
                                                GUIChat_SendPrivateMessage(CMess->SrcID,CHAT_MESSAGE_SENDACTOR,szActor,strlen(szActor)+1);
                                            }

                                            // Retrieve parameters
                                            pNickName = User_GetNick (CMess->SrcID);
                                            pHostName = User_GetHostName (CMess->SrcID);
                                            pActor    = User_GetActor(CMess->SrcID);
                                        }

                                        sprintf(StrBuff,"%s is connected from %s using %s as actor.",pNickName,pHostName,pActor);

                                        mSysMess->Lines->Add(StrBuff);

                                        sprintf(StrBuff,"%s %s %s",
                                                User_GetHostName(MY_IP_ADDRESS),
                                                User_GetNick(MY_IP_ADDRESS),
                                                User_GetActor(MY_IP_ADDRESS));

                                        Chat_SendConnectedMessage(StrBuff);
                                        break;

        case CHAT_MESSAGE_DISCONNECT:   Chat_SendDisconnectMessage(NULL);
                                        break;

        case CHAT_MESSAGE_CONNECTED	:   // Se llega aqui cuando enviamos informacion
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
                                            ParseChatUser(CMess->Message,HostName,NickName,Actor);
                                            User_AddUser(CMess->SrcID,HostName,NickName,Actor);

                                            sprintf(StrBuff,"%s is connected from %s using %s as actor.",NickName,HostName,Actor);
                                            mSysMess->Lines->Add(StrBuff);

                                            SkinActor = ActorCache.GetSkin(User_GetActor(CMess->SrcID));
                                            if (! SkinActor)
                                            {
                                                // Pedir fichero al otro
                                                char* szActor = User_GetActor(CMess->SrcID);
                                                GUIChat_SendPrivateMessage(CMess->SrcID,CHAT_MESSAGE_SENDACTOR,szActor,strlen(szActor)+1);
                                            }
                                        }
                                        break;

        case CHAT_MESSAGE_COMMON	:   // Need skin actor in order to show proper avatar
                                        SkinActor = ActorCache.GetSkin(User_GetActor(CMess->SrcID));

                                        // It should be possible that we don't have the skin...
                                        if (! SkinActor)
                                        {
                                            // Pedir fichero al otro
                                            char* szActor = User_GetActor(CMess->SrcID);

                                            if (szActor)
                                                GUIChat_SendPrivateMessage(CMess->SrcID,CHAT_MESSAGE_SENDACTOR,szActor,strlen(szActor)+1);
                                        }

                                        ChatHistory_AddChatMessage(CMess,pNickName,pActor);
                                        AddMessageToChatWindow(CMess,pNickName,SkinActor);
                                        break;

        case CHAT_MESSAGE_BROADCAST	:
                                        break;

        case CHAT_MESSAGE_PRIVATE	:   // Use private messages to send files / actors / private messages
                                        unsigned int PrivMsgID = GUIChat_GetPrivateMessageID(CMess);
                                        void*  PrivMsgData     = GUIChat_GetPrivateMessageData(CMess);

                                        switch (PrivMsgID)
                                        {
                                            case CHAT_MESSAGE_PRIVATEMSG:
                                            {
                                                // Need skin actor in order to show proper avatar
                                                SkinActor = ActorCache.GetSkin(pActor);

                                                // It should be possible that we don't have the skin...
                                                if (! SkinActor)
                                                {
                                                    // Pedir fichero al otro
                                                    char* szActor = pActor;

                                                    if (szActor)
                                                        GUIChat_SendPrivateMessage(CMess->SrcID,CHAT_MESSAGE_SENDACTOR,szActor,strlen(szActor)+1);
                                                }

                                                ChatHistory_AddChatMessage(CMess,pNickName,pActor);
                                                AddMessageToChatWindow(CMess,pNickName,SkinActor);
                                            }
                                            break;

                                            // Someone request to send a file
                                            case CHAT_MESSAGE_SENDFILEREQ:
                                            {
                                                sprintf(StrBuff,"%s requests sending file %s.",pNickName);
                                                mSysMess->Lines->Add(StrBuff);
                                            }
                                            break;

                                            // Someone accepted a request for sending a file
                                            case CHAT_MESSAGE_RECVFILEREQ:
                                            {
                                                sprintf(StrBuff,"%s accepted the request to receive a file",pNickName);
                                                mSysMess->Lines->Add(StrBuff);
                                            }
                                            break;

                                            // Someone sent a file
                                            case CHAT_MESSAGE_SENDFILE:
                                            {
                                                sprintf(StrBuff,"%s sent a file %s.",pNickName);
                                                mSysMess->Lines->Add(StrBuff);
                                            }
                                            break;

                                            // Someone request to receive an actor
                                            case CHAT_MESSAGE_SENDACTOR:
                                            {
                                                GUIChat_SendActor(CMess->SrcID,(char*)PrivMsgData);

                                                sprintf(StrBuff,"%s requests to receive an actor.",pNickName);
                                                mSysMess->Lines->Add(StrBuff);
                                            }
                                            break;

                                            // Someone request to send an actor
                                            case CHAT_MESSAGE_RECVACTOR:
                                           {
                                                GUIChat_RecvActor(CMess);

                                                sprintf(StrBuff,"%s sent an actor.",pNickName);
                                                mSysMess->Lines->Add(StrBuff);
                                            }
                                            break;
                                        }
                                        break;
    }
}
//---------------------------------------------------------------------------
