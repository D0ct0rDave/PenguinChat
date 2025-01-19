//---------------------------------------------------------------------------
#ifndef SkinCacheH
#define SkinCacheH

#include "tex.h"

#ifndef MAX_CHARS_ACTORNAME
#define MAX_CHARS_ACTORNAME 32
#endif

#define 	MAX_SKINS		64

//---------------------------------------------------------------------------
typedef struct{
	char SkinName[MAX_CHARS_ACTORNAME+1];
	Texture*	Skin;
    int			TimeStamp;
}TSkinEntry;

class TSkinCache
{
	public:	TSkinCache(char* SkinDir = ".\\");
            ~TSkinCache();

            Texture *GetSkin(char *SkinName);
    private:
    		unsigned int GetFreeSlot();
    		void UpdateCounters(unsigned int SkinSlot);

    		TSkinEntry SkinWH[MAX_SKINS];
            char SkinDirectory[1024];
};
//---------------------------------------------------------------------------
#endif
