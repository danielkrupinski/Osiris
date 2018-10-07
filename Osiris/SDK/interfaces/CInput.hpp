#pragma once

#include "../Misc/CUserCmd.hpp"

#define MULTIPLAYER_BACKUP 150

class bf_write;
class bf_read;

class CInput
{
public:
    virtual void  Init_All(void);
    virtual void  Shutdown_All(void);
    virtual int   GetButtonBits(int);
    virtual void  CreateMove(int sequence_number, float input_sample_frametime, bool active);
    virtual void  ExtraMouseSample(float frametime, bool active);
    virtual bool  WriteUsercmdDeltaToBuffer(bf_write *buf, int from, int to, bool isnewcommand);
    virtual void  EncodeUserCmdToBuffer(bf_write& buf, int slot);
    virtual void  DecodeUserCmdFromBuffer(bf_read& buf, int slot);


    inline CUserCmd* GetUserCmd(int sequence_number);
    inline CVerifiedUserCmd* GetVerifiedCmd(int sequence_number);

    bool                m_fTrackIRAvailable;            //0x04
    bool                m_fMouseInitialized;            //0x05
    bool                m_fMouseActive;                 //0x06
    bool                m_fJoystickAdvancedInit;        //0x07
    char                pad_0x08[0x2C];                 //0x08
    void*               m_pKeys;                        //0x34
    char                pad_0x38[0x6C];                 //0x38
    bool                m_fCameraInterceptingMouse;     //0x9C
    bool                m_fCameraInThirdPerson;         //0x9D
    bool                m_fCameraMovingWithMouse;       //0x9E
    Vector              m_vecCameraOffset;              //0xA0
    bool                m_fCameraDistanceMove;          //0xAC
    int                 m_nCameraOldX;                  //0xB0
    int                 m_nCameraOldY;                  //0xB4
    int                 m_nCameraX;                     //0xB8
    int                 m_nCameraY;                     //0xBC
    bool                m_CameraIsOrthographic;         //0xC0
    QAngle              m_angPreviousViewAngles;        //0xC4
    QAngle              m_angPreviousViewAnglesTilt;    //0xD0
    float               m_flLastForwardMove;            //0xDC
    int                 m_nClearInputState;             //0xE0
    CUserCmd*           m_pCommands;                    //0xEC
    CVerifiedUserCmd*   m_pVerifiedCommands;            //0xF0
};

CUserCmd* CInput::GetUserCmd(int sequence_number)
{
    return &m_pCommands[sequence_number % MULTIPLAYER_BACKUP];
}

CVerifiedUserCmd* CInput::GetVerifiedCmd(int sequence_number)
{
    return &m_pVerifiedCommands[sequence_number % MULTIPLAYER_BACKUP];
}
