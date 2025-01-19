// --------------------------------------------------------------------------
#ifndef CRIPT_HEADERFILE
#define CRIPT_HEADERFILE
// --------------------------------------------------------------------------

#define	MAX_CRIPT_DATA_LEN	1024

typedef struct{
	unsigned int  Len;
	char*         Data;
}TCriptData;

void Cript_EncodeData(TCriptData *CD,unsigned int Key);
void Cript_DecodeData(TCriptData *CD,unsigned int Key);

// --------------------------------------------------------------------------
#endif
// --------------------------------------------------------------------------