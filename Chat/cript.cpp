#include "cript.h"

// --------------------------------------------------------------------------
void Cript_EncodeData(void* Data,unsigned long Len,unsigned int Key)
{
	char *KeyComponents = (char *)&Key;
	unsigned int cI,cJ;

	// Encript data
	for (cJ=0;cJ<Len;cJ++)
		for (cI=0;cI<4;cI++)
			((char*)Data)[cJ] ^= KeyComponents[cI];
}
// --------------------------------------------------------------------------
void Cript_DecodeData(void* Data,unsigned long Len,unsigned int Key)
{
	char *KeyComponents = (char *)&Key;
	unsigned int cI,cJ;

	// Decript data
	for (cJ=0;cJ<Len;cJ++)
		for (cI=0;cI<4;cI++)
			((char*)Data)[cJ] ^= KeyComponents[3-cI];
}
// --------------------------------------------------------------------------