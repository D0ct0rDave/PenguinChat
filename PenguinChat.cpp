//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop
USERES("PenguinChat.res");
USEFORM("GUIChat.cpp", GUI);
USEUNIT("ChatHistory.cpp");
USEUNIT("SkinCache.cpp");
USEUNIT("chat\chat.cpp");
USEUNIT("chat\comm.cpp");
USEUNIT("chat\cript.cpp");
USEUNIT("chat\files.cpp");
USEUNIT("chat\misc.cpp");
USEUNIT("chat\user.cpp");
USEUNIT("GUIUtils.cpp");
USELIB("..\libs\TexLib\Lib\Borland\TexLib.lib");
//---------------------------------------------------------------------------
WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
    try
    {
        Application->Initialize();
        Application->CreateForm(__classid(TGUI), &GUI);
         Application->Run();
    }
    catch (Exception &exception)
    {
        Application->ShowException(&exception);
    }
    return 0;
}
//---------------------------------------------------------------------------
