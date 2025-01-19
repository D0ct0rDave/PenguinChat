//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "chat.h"
#include "GUIChat.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TGUI *GUI;
//---------------------------------------------------------------------------
__fastcall TGUI::TGUI(TComponent* Owner)
    : TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TGUI::Edit1KeyDown(TObject *Sender, WORD &Key,TShiftState Shift)
{
    char StrBuff[1024];
    if (Key == VK_RETURN)
    {
        strncpy(StrBuff,Edit1->Text.c_str(),1023);
        Chat_SendChatMessage(StrBuff);
    }
}
//---------------------------------------------------------------------------
void __fastcall TGUI::Timer1Timer(TObject *Sender)
{
    TChatMessage CMess;

    if (Chat_RecvChatMessage(&CMess))
    {
        Memo1->Lines->Add(CMess.Message);
    }
}
//---------------------------------------------------------------------------

void __fastcall TGUI::FormCreate(TObject *Sender)
{
    Chat_InitChat(0x7f00001);    
}
//---------------------------------------------------------------------------

void __fastcall TGUI::FormDestroy(TObject *Sender)
{
    Chat_FinishChat();    
}
//---------------------------------------------------------------------------

