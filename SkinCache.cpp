//---------------------------------------------------------------------------
#include <string.h>
#include "SkinCache.h"

TSkinCache::TSkinCache(char* SkinDir)
{
    // Copy skin directory
    strcpy(SkinDirectory,SkinDir);

	unsigned int cS;
    for (cS=0;cS<MAX_SKINS;cS++)
    {
    	SkinWH[cS].TimeStamp = -1;
    	SkinWH[cS].Skin = NULL;
        SkinWH[cS].SkinName[0] = 0;
    }
}
//---------------------------------------------------------------------------
TSkinCache::~TSkinCache()
{
	unsigned int cS;
    for (cS=0;cS<MAX_SKINS;cS++)
    {
    	if (SkinWH[cS].TimeStamp != -1)
 	   		DestroyTexture(&SkinWH[cS].Skin);
    }
}
//---------------------------------------------------------------------------
Texture *TSkinCache::GetSkin(char *SkinName)
{
	unsigned int cS;

    for (cS=0;cS<MAX_SKINS;cS++)
    {
    	if ( (! strcmp(SkinName,SkinWH[cS].SkinName)) && (SkinWH[cS].TimeStamp!=-1))
        {
        	UpdateCounters(cS);
        	return(&SkinWH[cS].Skin);
        }
    }

    char Filename[1024];
    sprintf(Filename,"%s\\%s",SkinDirectory,SkinName);

    cS = GetFreeSlot();
	SkinWH[cS].Skin = poLoadTexture(Filename);
	if (SkinWH[cS].Skin != NULL)
    {
    	strcpy(SkinWH[cS].SkinName,SkinName);
        UpdateCounters(cS);

        return(&SkinWH[cS].Skin);
	}
}
//---------------------------------------------------------------------------
// Implementa la politica de eliminación LRU
//---------------------------------------------------------------------------
unsigned int TSkinCache::GetFreeSlot()
{
	unsigned int cS;
    unsigned int MaxTimeStamp,MaxSkinEntry;

    MaxTimeStamp = 0;
    MaxSkinEntry = 0;

    for (cS=0;cS<MAX_SKINS;cS++)
    {
    	if (SkinWH[cS].TimeStamp==-1)
        	return(cS);
		else
        {
        	if (SkinWH[cS].TimeStamp > MaxTimeStamp)
            {
            	MaxTimeStamp = SkinWH[cS].TimeStamp;
                MaxSkinEntry = cS;
            }
        }
    }

    return(MaxSkinEntry);
}
//---------------------------------------------------------------------------
// Incrementa todos los contadores en 1 unidad excepto SkinSlot que lo establece
// a 0
//---------------------------------------------------------------------------
void TSkinCache::UpdateCounters(unsigned int SkinSlot)
{
	unsigned int cS;
    unsigned int MaxTimeStamp,MaxSkinEntry;

    MaxTimeStamp = 0;
    MaxSkinEntry = 0;

    for (cS=0;cS<MAX_SKINS;cS++)
    {
    	if (cS == SkinSlot)
        	SkinWH[cS].TimeStamp = 0;
        else
        	SkinWH[cS].TimeStamp++;
    }
}
//---------------------------------------------------------------------------
