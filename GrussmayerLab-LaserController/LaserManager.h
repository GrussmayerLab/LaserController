//////////////////////////////////////////////////////////////////////////////
// FILE:          Arduino.h
// PROJECT:       Micro-Manager
// SUBSYSTEM:     DeviceAdapters
//-----------------------------------------------------------------------------
// DESCRIPTION:   Adapter for Arduino board
//                Needs accompanying firmware to be installed on the board
// COPYRIGHT:     University of California, San Francisco, 2008
// LICENSE:       LGPL
//
// AUTHOR:        Nico Stuurman, nico@cmp.ucsf.edu, 11/09/2008
//                automatic device detection by Karl Hoover
//
//

#ifndef _Arduino_H_
#define _Arduino_H_

#include "MMDevice.h"
#include "DeviceBase.h"
#include "Protocol.h"
#include <string>
#include <map>
#include <mutex>

//////////////////////////////////////////////////////////////////////////////
// Error codes
//
#define ERR_UNKNOWN_POSITION 101
#define ERR_INITIALIZE_FAILED 102
#define ERR_WRITE_FAILED 103
#define ERR_CLOSE_FAILED 104
#define ERR_BOARD_NOT_FOUND 105
#define ERR_PORT_OPEN_FAILED 106
#define ERR_COMMUNICATION 107
#define ERR_NO_PORT_SET 108
#define ERR_VERSION_MISMATCH 109


class LaserManagerHub : public HubBase<LaserManagerHub>
{
public:
    LaserManagerHub();
    ~LaserManagerHub();

    int Initialize();
    int Shutdown();
    void GetName(char* pszName) const;
    bool Busy();

    bool SupportsDeviceDetection(void);
    MM::DeviceDetectionStatus DetectDevice(void);
    int DetectInstalledDevices();

    // property handlers
    int OnPort(MM::PropertyBase* pPropt, MM::ActionType eAct);
    int OnVersion(MM::PropertyBase* pPropt, MM::ActionType eAct);
    unsigned int GetMaxNumPatterns() {
        return 0;
    };

    // custom interface for child devices
    bool IsPortAvailable() { return portAvailable_; }
    bool IsLogicInverted() { return invertedLogic_; }
    bool IsTimedOutputActive() { return timedOutputActive_; }
    void SetTimedOutput(bool active) { timedOutputActive_ = active; }

    int PurgeComPortH() { return PurgeComPort(port_.c_str()); }
    int WriteToComPortH(const unsigned char* command, unsigned len) { return WriteToComPort(port_.c_str(), command, len); }
    int ReadFromComPortH(unsigned char* answer, unsigned maxLen, unsigned long& bytesRead)
    {
        return ReadFromComPort(port_.c_str(), answer, maxLen, bytesRead);
    }
    std::mutex& GetLock() { return mutex_; }


private:
    int GetControllerVersion(int&);
    Protocol protocol_;
    std::string port_;
    bool initialized_;
    bool portAvailable_;
    bool invertedLogic_;
    bool timedOutputActive_;
    int version_;
    std::mutex mutex_;
};

class LaserManagerDAC : public CSignalIOBase<LaserManagerDAC>
{
public:
    LaserManagerDAC(int channel);
    ~LaserManagerDAC();

    // MMDevice API
    // ------------
    int Initialize();
    int Shutdown();

    void GetName(char* pszName) const;
    bool Busy() { return busy_; }

    // DA API
    int SetGateOpen(bool open);
    int GetGateOpen(bool& open) { open = gateOpen_; return DEVICE_OK; };
    int SetSignal(double volts);
    int GetSignal(double& volts) { volts_ = volts; return DEVICE_UNSUPPORTED_COMMAND; }
    int GetLimits(double& minVolts, double& maxVolts) { minVolts = minV_; maxVolts = maxV_; return DEVICE_OK; }

    int IsDASequenceable(bool& isSequenceable) const { isSequenceable = false; return DEVICE_OK; }

    // action interface
    // ----------------
    int OnVolts(MM::PropertyBase* pProp, MM::ActionType eAct);
    int OnMaxVolt(MM::PropertyBase* pProp, MM::ActionType eAct);
    int OnChannel(MM::PropertyBase* pProp, MM::ActionType eAct);
    int OnState(MM::PropertyBase* pProp, MM::ActionType eAct);


private:
    int WriteToPort(unsigned long lnValue);
    int WriteSignal(double volts);
    bool initialized_;
    bool busy_;
    double minV_;
    double maxV_;
    double volts_;
    double gatedVolts_;
    unsigned channel_;
    unsigned maxChannel_;
    bool gateOpen_;
    Protocol protocol_;
    std::string name_;
};

class LaserManagerPWM : public CSignalIOBase<LaserManagerPWM>
{
public:
    LaserManagerPWM(int channel);
    ~LaserManagerPWM();

    // MMDevice API
    // ------------
    int Initialize();
    int Shutdown();

    void GetName(char* pszName) const;
    bool Busy() { return busy_; }

    // DA API
    int SetGateOpen(bool open);
    int GetGateOpen(bool& open) { open = gateOpen_; return DEVICE_OK; };
    int SetSignal(double volts);
    int GetSignal(double& volts) { volts_ = volts; return DEVICE_UNSUPPORTED_COMMAND; }
    int GetLimits(double& minVolts, double& maxVolts) { minVolts = minV_; maxVolts = maxV_; return DEVICE_OK; }

    int IsDASequenceable(bool& isSequenceable) const { isSequenceable = false; return DEVICE_OK; }

    // action interface
    // ----------------
    int OnVolts(MM::PropertyBase* pProp, MM::ActionType eAct);
    int OnMaxVolt(MM::PropertyBase* pProp, MM::ActionType eAct);
    int OnChannel(MM::PropertyBase* pProp, MM::ActionType eAct);
    int OnState(MM::PropertyBase* pProp, MM::ActionType eAct);


private:
    int WriteToPort(unsigned long lnValue);
    int WriteSignal(double volts);
    bool initialized_;
    bool busy_;
    double minV_;
    double maxV_;
    double volts_;
    double gatedVolts_;
    unsigned channel_;
    unsigned maxChannel_;
    bool gateOpen_;
    Protocol protocol_;
    std::string name_;
};


#endif //_Arduino_H_
