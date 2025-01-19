
#include "cript.h"

// --------------------------------------------------------------------------
void Cript_EncodeData(TCriptData *CD,unsigned int Key)
{
	char* KeyComponents = (char *)&Key;
	char* LenData       = (char *)&CD->Len;
	unsigned int cI,cJ,Len;

	Len = CD->Len;
/*
	// Encript len
	for (cJ=0;cJ<4;cJ++)
    	for (cI=0;cI<4;cI++)
			LenData[cJ] ^= KeyComponents[cI];
*/
	// Encript data
	for (cJ=0;cJ<Len;cJ++)
		for (cI=0;cI<4;cI++)
			CD->Data[cJ] ^= KeyComponents[cI];
}
// --------------------------------------------------------------------------
void Cript_DecodeData(TCriptData *CD,unsigned int Key)
{
	char* KeyComponents = (char *)&Key;
	char* LenData       = (char *)&CD->Len;
	unsigned int cI,cJ,Len;

/*
	// Decript len
	for (cJ=0;cJ<4;cJ++)
		for (cI=0;cI<4;cI++)
			LenData[cJ] ^= KeyComponents[3-cI];
*/

	Len = CD->Len;

	// Decript data
	for (cJ=0;cJ<Len;cJ++)
		for (cI=0;cI<4;cI++)
			CD->Data[cJ] ^= KeyComponents[3-cI];
}
// --------------------------------------------------------------------------