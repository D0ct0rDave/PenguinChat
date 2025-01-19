//---------------------------------------------------------------------------
#ifdef USE_PRECOMPILED_HEADERS
    #include "Misc_PrecompiledHeaders.h"
    #pragma hdrstop
#endif

#include <vcl.h>
//---------------------------------------------------------------------------

#include "GUIUtils.h"
//---------------------------------------------------------------------------
// Use this fucntion every time you need to draw a texture into a Canvas efficiently
//---------------------------------------------------------------------------
void GUIUtils_DrawTextureInCanvas(Texture *Tex,TCanvas *Canvas)
{
    unsigned int cI,cJ,Width,Height,OffsetX,OffsetY,TamX,TamY;
    float StepX,StepY;
    Graphics::TBitmap *b;
    unsigned int *ScanPixel;
    unsigned int Color;

    Width    = Canvas->ClipRect.Right  - Canvas->ClipRect.Left;
    Height   = Canvas->ClipRect.Bottom - Canvas->ClipRect.Top;
    if ((! Width) || (! Height)) return;    // generally while debugging

    // ------------------------------------
    // Control image dimensions
    // ------------------------------------
    if (Tex->width > Width)
    {
        StepX = Tex->width/Width;
        TamX  = Width;
    }
    else
    {
        StepX = 1;
        TamX  = Tex->width;
    }
    // ------------------------------------
    if (Tex->height > Height)
    {
        StepY = Tex->height/Height;
        TamY  = Height;
    }
    else
    {
        StepY = 1;
        TamY  = Tex->height;
    }
    // ------------------------------------
    // Create the bitmap object that will contain the image data
    // ------------------------------------
    b              = new Graphics::TBitmap;
    b->Width       = TamX;
    b->Height      = TamY;
    b->PixelFormat = pf32bit;

    // ------------------------------------
    // Setup bitmap contents (The main inner loop)
    // ------------------------------------
    for (cJ=0;cJ<TamY;cJ++)
    {
        ScanPixel = (unsigned int *)b->ScanLine[cJ];

	    for (cI=0;cI<TamX;cI++)
        {
            // Canvas pixels for builder are in RGBA mode
            // (R-7..0,G-15..8,B-23..16,A-31..24). Look for TColor in help files
            // Scanline pixels are BGRA mode !!!!!
			
            unsigned int Color = GetARGBColor(Tex, cI*StepX, cJ*StepY);
            *ScanPixel = _lrotl(Color & 0x00ff00ff,16) | (Color & 0x00ff00);
            *ScanPixel++;
        }
    }

    // ------------------------------------
    // Dump information into the destination canvas
    // ------------------------------------
    OffsetX = (Width  - TamX)/2;
    OffsetY = (Height - TamY)/2;

    BitBlt(Canvas->Handle,OffsetX,OffsetY,TamX,TamY,b->Canvas->Handle,0,0,SRCCOPY);
    delete b;
}
//---------------------------------------------------------------------------
// Use this fucntion every time you need to draw a texture into a Canvas efficiently
//---------------------------------------------------------------------------
void GUIUtils_DrawTextureInCanvasAtPos(Texture *Tex,TCanvas *Canvas,int cX,int cY)
{
    unsigned int cI,cJ;
    Graphics::TBitmap *b;
    unsigned int *ScanPixel;
    unsigned int Color;

    // ------------------------------------
    // Create the bitmap object that will contain the image data
    // ------------------------------------
    b              = new Graphics::TBitmap;
    b->Width       = Tex->TX;
    b->Height      = Tex->TY;
    b->PixelFormat = pf32bit;

    // ------------------------------------
    // Setup bitmap contents (The main inner loop)
    // ------------------------------------
    for (cJ=0;cJ<Tex->TY;cJ++)
    {
        ScanPixel = (unsigned int *)b->ScanLine[cJ];

	    for (cI=0;cI<Tex->TX;cI++)
        {
            // Canvas pixels for builder are in RGBA mode
            // (R-7..0,G-15..8,B-23..16,A-31..24). Look for TColor in help files
            // Scanline pixels are BGRA mode !!!!!
			unsigned int Color = GetARGBColor(Tex, cI*StepX, cJ*StepY);
            *ScanPixel = _lrotl(Color & 0x00ff00ff,16) | (Color & 0x00ff00);
            *ScanPixel++;
        }
    }

    // ------------------------------------
    // Dump information into the destination canvas
    // ------------------------------------
    BitBlt(Canvas->Handle,cX,cY,Tex->TX,Tex->TY,b->Canvas->Handle,0,0,SRCCOPY);
    delete b;
}
//---------------------------------------------------------------------------
Texture *GUIUtils_CopyCanvasRect2Texture(TCanvas *Canvas,int iX,int iY,int fX,int fY,PixelFormat pixelFormat)
{
    if ((fX < iX) || (fY < iY)) return(NULL);
    if ((iX<0) ||(iY<0))		return(NULL);

	Texture *Tex;
    unsigned int TX,TY,cI,cJ;
    Graphics::TBitmap *b;
    unsigned int *ScanPixel,Color;

    TX = fX-iX+1;
    TY = fY-iY+1;

    // Dump canvas info into Bitmap
    b              = new Graphics::TBitmap;
    b->Width       = TX;
    b->Height      = TY;
    b->PixelFormat = pf32bit;
    BitBlt(b->Canvas->Handle,0,0,TX,TY,Canvas->Handle,iX,iY,SRCCOPY);

    Tex = poCreateTexture(TX, TY, pixelFormat);

    for (cJ=0;cJ<TY;cJ++)
    {
    	ScanPixel = (unsigned int *)b->ScanLine[cJ];

	    for (cI=0;cI<TX;cI++)
        {
	        Color = *ScanPixel;
            Color = _lrotl(Color & 0x00ff00ff,16) | (Color & 0x00ff00);

            SetARGBColor(Tex, cI, cJ, Color);

            *ScanPixel++;
        }
    }

    delete b;
    return(Tex);
}
//---------------------------------------------------------------------------
void GUIUtils_CopyTextureRect2Canvas(Texture *tex,int iX,int iY,int fX,int fY,TCanvas *Canvas)
{
    if ((fX < iX) || (fY < iY)) return;
    if ((iX<0) ||(iY<0))		return;

    unsigned int         TX,TY,cI,cJ;
    Graphics::TBitmap   *b;
    unsigned int        *ScanPixel;
    unsigned int		Color;
    unsigned int        *pColor = (unsigned int *)&Color;

    TX = fX-iX+1;
    TY = fY-iY+1;

    // Dump canvas info into Bitmap
    b              = new Graphics::TBitmap;
    b->Width       = TX;
    b->Height      = TY;
    b->PixelFormat = pf32bit;

    for (cJ=0;cJ<TY;cJ++)
    {
    	ScanPixel = (unsigned int *)b->ScanLine[cJ];

	    for (cI=0;cI<TX;cI++)
        {
            Color   = GetARGBColor(tex, cI, cJ);
            *pColor = _lrotl(*pColor & 0x00ff00ff,16) | (*pColor & 0x00ff00);

            *ScanPixel = *(unsigned int *)&Color;
            *ScanPixel++;
        }
    }

    BitBlt(Canvas->Handle,0,0,TX,TY,b->Canvas->Handle,0,0,SRCCOPY);
    delete b;
}
//---------------------------------------------------------------------------
void GUIUtils_DrawFloatMatrixFit(float *Matrix,unsigned int uiROWS,unsigned int uiCOLS,TCanvas *Canvas)
{
	unsigned int  cI,cJ,cX,cY,TX,TY;
    float 		  HeightValue;
    float         XScale,YScale;

    unsigned int   uiColor;
    unsigned char *pucColor = (unsigned char *)&uiColor;
    unsigned int  *pPixel;

    if (! Matrix) return;

    TX = (Canvas->ClipRect.Right  - Canvas->ClipRect.Left);
    TY = (Canvas->ClipRect.Bottom - Canvas->ClipRect.Top);

    if ((! TX) || (! TY)) return;

    Graphics::TBitmap   *b = new Graphics::TBitmap;
    b->Width       = TX;
    b->Height      = TY;
    b->PixelFormat = pf32bit;

    XScale = (float)uiCOLS/(float)TX;
    YScale = (float)uiROWS/(float)TY;

    pucColor[3] = 0;    // Set alpha value to 0

    cY     = 0;
    for (cJ=0;cJ<TY;cJ++)
    {
        pPixel = (unsigned int *)b->ScanLine[cJ];
        cX     = cY*uiCOLS;

	    for (cI=0;cI<TY;cI++)
        {
            HeightValue = Matrix[cX];

            pucColor[0] = HeightValue*255.0f;
            pucColor[1] = pucColor[0];
            pucColor[2] = pucColor[0];

            *pPixel = uiColor;

            pPixel++;
            cX+=XScale;
        }
        cY+=YScale;
    }

    BitBlt(Canvas->Handle,0,0,TX,TY,b->Canvas->Handle,0,0,SRCCOPY);
    delete b;
}
//---------------------------------------------------------------------------
void GUIUtils_DrawFloatMatrix(float *Matrix,unsigned int uiROWS,unsigned int uiCOLS,TCanvas *Canvas)
{
	unsigned int  cI,cJ,Offset;
    float 		  HeightValue;
    float         XScale,YScale;

    unsigned int   uiColor;
    unsigned char *pucColor = (unsigned char *)&uiColor;
    unsigned int  *pPixel;

    if (! Matrix) return;

    Graphics::TBitmap   *b = new Graphics::TBitmap;
    b->Width       = uiCOLS;
    b->Height      = uiROWS;
    b->PixelFormat = pf32bit;

    pucColor[3] = 0;    // Set alpha value to 0

    Offset = 0;
    for (cJ=0;cJ<uiROWS;cJ++)
    {
        pPixel = (unsigned int *)b->ScanLine[cJ];
	    for (cI=0;cI<uiCOLS;cI++)
        {
            HeightValue = Matrix[Offset];

            pucColor[0] = HeightValue*255.0f;
            pucColor[1] = pucColor[0];
            pucColor[2] = pucColor[0];

            *pPixel = uiColor;

            pPixel++;
			Offset++;
        }
    }

    BitBlt(Canvas->Handle,0,0,uiCOLS,uiROWS,b->Canvas->Handle,0,0,SRCCOPY);
    delete b;
}
//---------------------------------------------------------------------------
#pragma package(smart_init)
