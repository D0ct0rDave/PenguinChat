#include "Misc.h"

#define UXDATE_SECS_PER_MIN             60
#define UXDATE_SECS_PER_HOUR            3600
#define UXDATE_SECS_PER_DAY             86400
#define UXDATE_SECS_PER_MONTH           (unsigned long)((float)UXDATE_SECS_PER_DAY * ((float)(4*30+7*31+28)/12))
#define UXDATE_SECS_PER_YEAR            (UXDATE_SECS_PER_DAY*365)
// --------------------------------------------------------------------------
bool Misc_Primo(unsigned int Number)
{								
	//                          0     1     2   3     4    5     6    7     8     9
	bool NumerosPrimos[10] = {false,false,true,true,false,true,false,true,false,false };
	return(NumerosPrimos[Number]);
}
// --------------------------------------------------------------------------
void Misc_EncodeNumberStr(unsigned int Number,char *String)
{
	unsigned int  Counter = 0;
	unsigned char Digit;

	while (Number/10>0)
	{
		Digit = Number%10;

		if (Misc_Primo(Digit))
			String[Counter] = 'a' + Digit + Counter;
		else	
			String[Counter] = 'A' + Digit + Counter;

		Number /= 10;
		Counter++;
	}

	if (Misc_Primo(Number))
		String[Counter] = 'a' + Number + Counter;
	else	
		String[Counter] = 'A' + Number + Counter;

	String[Counter+1] = 0;
}
// --------------------------------------------------------------------------
// Esta funcion lee la fecha actual de la bios, y la convierte en el formato
// utilizado en UNIX que son los time-stamps
// --------------------------------------------------------------------------
unsigned long Misc_GetUnixTimeStamp(unsigned short Anyo,unsigned short Mes,unsigned short Dia,unsigned short Hora,unsigned short Min,unsigned short Sec)
{
    unsigned long res;

    Anyo -= 1970;
    res = (Anyo*UXDATE_SECS_PER_YEAR    ) +
          ((Mes-1)*UXDATE_SECS_PER_MONTH) +
          ((Dia-1)*UXDATE_SECS_PER_DAY  ) +
          (Hora*UXDATE_SECS_PER_HOUR    ) +
          ( Min*UXDATE_SECS_PER_MIN     ) +
           Sec;

    return(res);
}
// --------------------------------------------------------------------------