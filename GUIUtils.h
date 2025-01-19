//---------------------------------------------------------------------------
#ifndef GUIUtilsH
#define GUIUtilsH
//---------------------------------------------------------------------------
#include "tex.h"
#include <vcl.h>

void GUIUtils_DrawTextureInCanvas      (Texture *Tex,TCanvas *Canvas);
void GUIUtils_DrawTextureInCanvasAtPos (Texture *Tex,TCanvas *Canvas,int cX,int cY);
Texture *GUIUtils_CopyCanvasRect2Texture(TCanvas *Canvas,int iX,int iY,int fX,int fY,PixelFormat pixelFormat);
void GUIUtils_CopyTextureRect2Canvas   (Texture *tex,int iX,int iY,int fX,int fY,TCanvas *Canvas);

void GUIUtils_DrawFloatMatrix   (float *Matrix,unsigned int uiROWS,unsigned int uiCOLS,TCanvas *Canvas);
void GUIUtils_DrawFloatMatrixFit(float *Matrix,unsigned int uiROWS,unsigned int uiCOLS,TCanvas *Canvas);
//---------------------------------------------------------------------------
#endif
