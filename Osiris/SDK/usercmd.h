#pragma once
#include "crc32.h"
class CUserCmd
{
public:
	CRC32_t GetChecksum(void)
	{
		CRC32_t crc;

		gCRC.CRC32_Init(&crc);
		gCRC.CRC32_ProcessBuffer(&crc, &command_number, sizeof(command_number));
		gCRC.CRC32_ProcessBuffer(&crc, &tick_count, sizeof(tick_count));
		gCRC.CRC32_ProcessBuffer(&crc, &viewangles, sizeof(viewangles));
		gCRC.CRC32_ProcessBuffer(&crc, &aimdirection, sizeof(aimdirection));
		gCRC.CRC32_ProcessBuffer(&crc, &forwardmove, sizeof(forwardmove));
		gCRC.CRC32_ProcessBuffer(&crc, &sidemove, sizeof(sidemove));
		gCRC.CRC32_ProcessBuffer(&crc, &upmove, sizeof(upmove));
		gCRC.CRC32_ProcessBuffer(&crc, &buttons, sizeof(buttons));
		gCRC.CRC32_ProcessBuffer(&crc, &impulse, sizeof(impulse));
		gCRC.CRC32_ProcessBuffer(&crc, &weaponselect, sizeof(weaponselect));
		gCRC.CRC32_ProcessBuffer(&crc, &weaponsubtype, sizeof(weaponsubtype));
		gCRC.CRC32_ProcessBuffer(&crc, &random_seed, sizeof(random_seed));
		gCRC.CRC32_ProcessBuffer(&crc, &mousedx, sizeof(mousedx));
		gCRC.CRC32_ProcessBuffer(&crc, &mousedy, sizeof(mousedy));
		//CRC32_ProcessBuffer( &crc, &hasbeenpredicted, sizeof( hasbeenpredicted ) );
		//CRC32_ProcessBuffer( &crc, &headangles, sizeof( headangles ) );
		//CRC32_ProcessBuffer( &crc, &headoffset, sizeof( headoffset ) );        
		gCRC.CRC32_Final(&crc);

		return crc;
	}
	char pad_0x0000[0x4]; //0x0000	
	int		command_number;		// 0x04 For matching server and client commands for debugging
	int		tick_count;			// 0x08 the tick the client created this command
	QAngle	viewangles;			// 0x0C Player instantaneous view angles.
	Vector	aimdirection;		// 0x18
	float	forwardmove;		// 0x24
	float	sidemove;			// 0x28
	float	upmove;				// 0x2C
	int		buttons;			// 0x30 Attack button states
	byte    impulse;			// 0x34
	int		weaponselect;		// 0x38 Current weapon id
	int		weaponsubtype;		// 0x3C
	int		random_seed;		// 0x40 For shared random functions
	short	mousedx;			// 0x44 mouse accum in x from create move
	short	mousedy;			// 0x46 mouse accum in y from create move
	bool	hasbeenpredicted;	// 0x48 Client only, tracks whether we've predicted this command at least once
	char	pad_0x4C[0x18];		// 0x4C Current sizeof( usercmd ) =  100  = 0x64

};
/*
class CVerifiedUserCmd
{
public:
void* m_cmd; //0x0000
__int32 m_crc; //0x0004

};//Size=0x0008*/
class CVerifiedUserCmd
{
public:
	CUserCmd	m_cmd;
	unsigned long	m_crc;
};
