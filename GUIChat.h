//---------------------------------------------------------------------------
#ifndef GUIChatH
#define GUIChatH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>

#include "tex.h"
#include "chat.h"
#include <Buttons.hpp>
//---------------------------------------------------------------------------

class TGUI : public TForm
{
__published:	// IDE-managed Components
    TTimer *Timer1;
    TPaintBox *pb_Display;
    TScrollBar *sb_Chat;
    TPanel *pn_Group;
    TLabel *lb_SysMess;
    TLabel *lb_Nick;
    TEdit *ed_Message;
    TMemo *mSysMess;
    TPanel *pn_Actor;
    TPaintBox *pb_Actor;
    TSpeedButton *bt_FlipActor;
    TTimer *Timer2;
    void __fastcall FormCreate(TObject *Sender);
    void __fastcall FormDestroy(TObject *Sender);
    void __fastcall ed_MessageKeyDown(TObject *Sender, WORD &Key,
          TShiftState Shift);
    void __fastcall Timer1Timer(TObject *Sender);

    void __fastcall FormShow(TObject *Sender);
    void __fastcall pb_ActorPaint(TObject *Sender);
    void __fastcall pb_DisplayPaint(TObject *Sender);
    void __fastcall bt_FlipActorClick(TObject *Sender);
    void __fastcall FormResize(TObject *Sender);
	void __fastcall sb_ChatChange(TObject *Sender);
    void __fastcall Timer2Timer(TObject *Sender);

private:	// User declarations
    void    StablishChatConetion();
    void    ParseChatMessage(TChatMessage *CMess);

    void 	DrawText(TPaintBox *Dsp,TRect &Rect,unsigned int X,unsigned int Y,AnsiString String);
    void    DrawActor(Texture *tex,TCanvas *C,unsigned int cx,unsigned int cy);
    void    DisplayChatMessage(TChatMessage *CMess,char *NickName,Texture *Actor);
    void    DisplayChatWindow();
    void 	AddMessageToChatWindow(TChatMessage *CMess,char *NickName,Texture *Actor);

    void    WriteCaption();
    void    WriteAlternateCaption();

    void __fastcall Deactivate(TObject *Sender);
    void __fastcall Activate(TObject *Sender);



    void    HandleControls();       // Controlar las scrollbars etc...

public:		// User declarations
    __fastcall TGUI(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TGUI *GUI;
//---------------------------------------------------------------------------
#endif
