#pragma once

typedef unsigned long CRC32;

void CRC32_Init(CRC32* pulCRC);

void CRC32_ProcessBuffer(CRC32* pulCRC, const void* p, int len);

void CRC32_Final(CRC32* pulCRC);

CRC32 CRC32_GetTableEntry(unsigned int slot);

inline CRC32 CRC32_ProcessSingleBuffer(const void* p, int len)
{
	CRC32 crc;

	CRC32_Init(&crc);
	CRC32_ProcessBuffer(&crc, p, len);
	CRC32_Final(&crc);

	return crc;
}