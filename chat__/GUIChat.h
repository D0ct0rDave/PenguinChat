//---------------------------------------------------------------------------
#ifndef GUIChatH
#define GUIChatH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
//---------------------------------------------------------------------------
class TGUI : public TForm
{
__published:	// IDE-managed Components
    TEdit *Edit1;
    TMemo *Memo1;
    TTimer *Timer1;
    void __fastcall Edit1KeyDown(TObject *Sender, WORD &Key,
          TShiftState Shift);
    void __fastcall Timer1Timer(TObject *Sender);
    void __fastcall FormCreate(TObject *Sender);
    void __fastcall FormDestroy(TObject *Sender);
private:	// User declarations
public:		// User declarations
    __fastcall TGUI(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TGUI *GUI;
//---------------------------------------------------------------------------
#endif
