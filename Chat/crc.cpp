// ------------------------------------------------
#include "crc.h"
#include <string.h>

// ----------------------------------------------------------------------------
// CRC variables
// ----------------------------------------------------------------------------
static bool			bFirstInit = true;
static unsigned int crc32_table[256]; // Lookup table array

// ----------------------------------------------------------------------------
// CRC functions
// ----------------------------------------------------------------------------

// Reflection is a requirement for the official CRC-32 standard.
// You can create CRCs without it, but they won't conform to the standard.

unsigned int CRC_Reflect(unsigned int ref, char ch)
{// Used only by Init_CRC32_Table()

	unsigned int value = 0;

	// Swap bit 0 for bit 7
	// bit 1 for bit 6, etc.
	for(int i = 1; i < (ch + 1); i++)
	{
		if(ref & 1)
			value |= 1 << (ch - i);

		ref >>= 1;
	}

	return value;
}
// ----------------------------------------------------------------------------
// Call this function only once to initialize the CRC table.
void CRC_BuildTable()
{
	// This is the official polynomial used by CRC-32 
	// in PKZip, WinZip and Ethernet. 
	unsigned int ulPolynomial = 0x04c11db7;

	// 256 values representing ASCII character codes.
	for(int i = 0; i <= 0xFF; i++)
	{
		crc32_table[i] = CRC_Reflect(i, 8) << 24;

		for (int j = 0; j < 8; j++)
			crc32_table[i] = (crc32_table[i] << 1) ^ (crc32_table[i] & (1 << 31) ? ulPolynomial : 0);

		crc32_table[i] = CRC_Reflect(crc32_table[i], 32);
	}
}
// ----------------------------------------------------------------------------
// This function uses the crc32_table lookup table
// to generate a CRC for csData
// ----------------------------------------------------------------------------
unsigned int GetCRC(void* Data,unsigned int Len)
{
	if (bFirstInit)
	{
		CRC_BuildTable();
		bFirstInit = false;
	}

	// Be sure to use unsigned variables,
	// because negative values introduce high bits
	// where zero bits are required.
	unsigned int crc		= 0xffffffff;
	unsigned char* buffer	= (unsigned char*)Data;

	// Perform the algorithm on each character
	// in the string, using the lookup table values.
	while(Len--)
		crc = (crc >> 8) ^ crc32_table[(crc & 0xFF) ^ *buffer++];

	// Exclusive OR the result with the beginning value.
	return crc ^ 0xffffffff;
}
// ----------------------------------------------------------------------------