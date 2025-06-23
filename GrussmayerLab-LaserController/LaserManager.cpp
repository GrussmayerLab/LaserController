#include "LaserManager.h"
#include "Protocol.h"
#include "DeviceBase.h" // Ensure this header is included for MM::Device
#include <sstream>
#include <cstdio>

#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

const char *g_DeviceNameHub = "LaserController-Hub";
const char *g_DeviceNameDAC = "LaserController-DAC";
const char *g_DeviceNamePWM = "LaserController-PWM";

// Global info about the state of the Arduino.  This should be folded into a class
const int g_Min_MMVersion = 1;
const int g_Max_MMVersion = 3;
const char *g_versionProp = "Version";
const char *g_normalLogicString = "Normal";
const char *g_invertedLogicString = "Inverted";

const char *g_On = "On";
const char *g_Off = "Off";

///////////////////////////////////////////////////////////////////////////////
// Exported MMDevice API
///////////////////////////////////////////////////////////////////////////////
MODULE_API void InitializeModuleData()
{
    RegisterDevice(g_DeviceNameHub, MM::HubDevice, "Hub (required)");
    RegisterDevice("LaserController-DAC0", MM::SignalIODevice, "DAC channel 0");
    RegisterDevice("LaserController-DAC1", MM::SignalIODevice, "DAC channel 1");
    RegisterDevice("LaserController-DAC2", MM::SignalIODevice, "DAC channel 2");
    RegisterDevice("LaserController-DAC3", MM::SignalIODevice, "DAC channel 3");
    RegisterDevice("LaserController-DAC4", MM::SignalIODevice, "DAC channel 4");
    //RegisterDevice("LaserController-DAC5", MM::SignalIODevice, "DAC channel 5");
    RegisterDevice(g_DeviceNamePWM, MM::SignalIODevice, "PWM channel");
}

MODULE_API MM::Device *CreateDevice(const char *deviceName)
{
    if (deviceName == 0)
        return 0;

    if (strcmp(deviceName, g_DeviceNameHub) == 0)
    {
        return new LaserManagerHub;
    }
    else if (strcmp(deviceName, "LaserController-DAC0") == 0)
    {
        return new LaserManagerDAC(1);
    }
    else if (strcmp(deviceName, "LaserController-DAC1") == 0)
    {
        return new LaserManagerDAC(2);
    }
    else if (strcmp(deviceName, "LaserController-DAC2") == 0)
    {
        return new LaserManagerDAC(3);
    }
    else if (strcmp(deviceName, "LaserController-DAC3") == 0)
    {
        return new LaserManagerDAC(4);
    }
    else if (strcmp(deviceName, "LaserController-DAC4") == 0)
    {
        return new LaserManagerDAC(5);
    }
    else if (strcmp(deviceName, "LaserController-DAC5") == 0)
    {
        return new LaserManagerDAC(6);
    }
    else if (strcmp(deviceName, g_DeviceNamePWM) == 0)
    {
        return new LaserManagerPWM(1);
    }
    return 0;
}

MODULE_API void DeleteDevice(MM::Device *pDevice)
{
    delete pDevice;
}

LaserManagerHub::LaserManagerHub() : initialized_(false),
                                     version_(0)
{
    portAvailable_ = false;
    invertedLogic_ = false;
    timedOutputActive_ = false;
    std::cout << "test1" << std::endl;

    InitializeDefaultErrorMessages();

    SetErrorText(ERR_PORT_OPEN_FAILED, "Failed opening USB device");
    SetErrorText(ERR_BOARD_NOT_FOUND, "Did not find an board with the correct firmware.  Is the Arduino board connected to this serial port?");
    SetErrorText(ERR_NO_PORT_SET, "Hub Device not found.  The Hub device is needed to create this device");
    std::ostringstream errorText;
    errorText << "The firmware version on the board is not compatible with this adapter.  Please use firmware version ";
    errorText << g_Min_MMVersion << " to " << g_Max_MMVersion;
    SetErrorText(ERR_VERSION_MISMATCH, errorText.str().c_str());
    std::cout << "test2" << std::endl;

    CPropertyAction *pAct = new CPropertyAction(this, &LaserManagerHub::OnPort);
    CreateProperty(MM::g_Keyword_Port, "Undefined", MM::String, false, pAct, true);
    std::cout << "test3" << std::endl;
}

LaserManagerHub::~LaserManagerHub()
{
    Shutdown();
}

void LaserManagerHub::GetName(char *Name) const
{
    CDeviceUtils::CopyLimitedString(Name, g_DeviceNameHub);
}

bool LaserManagerHub::Busy()
{
    return false;
}

int LaserManagerHub::GetControllerVersion(int &version)
{
    std::vector<uint8_t> message = protocol_.createGetDeviceMessage();

    int ret = WriteToComPort(port_.c_str(), message.data(), static_cast<unsigned int>(message.size()));
    if (ret != DEVICE_OK)
        return ret;

    std::string ans;

    ret = GetSerialAnswer(port_.c_str(), "\x7F", ans);
    ans = ans + "\x7F";
    std::vector<uint8_t> buffer(ans.begin(), ans.end());
    std::ostringstream oss;
    for (uint8_t b : buffer)
    {
        oss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(b) << " ";
    }
    LogMessage("Message: " + oss.str(), true);

    bool success = protocol_.validateDeviceMessage(buffer);

    version = 1;

    if (success)
        return DEVICE_OK;
    else
        LogMessage(protocol_.getError());
    return DEVICE_NOT_CONNECTED;
}

bool LaserManagerHub::SupportsDeviceDetection(void)
{
    return true;
}

MM::DeviceDetectionStatus LaserManagerHub::DetectDevice(void)
{
    if (initialized_)
        return MM::CanCommunicate;

    // all conditions must be satisfied...
    MM::DeviceDetectionStatus result = MM::Misconfigured;
    char answerTO[MM::MaxStrLength];

    try
    {
        std::string portLowerCase = port_;
        for (std::string::iterator its = portLowerCase.begin(); its != portLowerCase.end(); ++its)
        {
            *its = (char)tolower(*its);
        }
        if (0 < portLowerCase.length() && 0 != portLowerCase.compare("undefined") && 0 != portLowerCase.compare("unknown"))
        {
            result = MM::CanNotCommunicate;
            // record the default answer time out
            GetCoreCallback()->GetDeviceProperty(port_.c_str(), "AnswerTimeout", answerTO);

            // device specific default communication parameters
            GetCoreCallback()->SetDeviceProperty(port_.c_str(), MM::g_Keyword_Handshaking, g_Off);
            GetCoreCallback()->SetDeviceProperty(port_.c_str(), MM::g_Keyword_BaudRate, "115200");
            GetCoreCallback()->SetDeviceProperty(port_.c_str(), MM::g_Keyword_StopBits, "1");
            // Arduino timed out in GetControllerVersion even if AnswerTimeout  = 300 ms
            GetCoreCallback()->SetDeviceProperty(port_.c_str(), "AnswerTimeout", "500.0");
            GetCoreCallback()->SetDeviceProperty(port_.c_str(), "DelayBetweenCharsMs", "0");
            MM::Device *pS = GetCoreCallback()->GetDevice(this, port_.c_str());
            pS->Initialize();
            // The first second or so after opening the serial port, the Arduino is waiting for firmwareupgrades.  Simply sleep 1 second.
            CDeviceUtils::SleepMs(1000);
            const std::lock_guard<std::mutex> lock(mutex_);
            PurgeComPort(port_.c_str());
            int v = 0;
            int ret = GetControllerVersion(v);
            // later, Initialize will explicitly check the version #
            if (DEVICE_OK != ret)
            {
                LogMessageCode(ret, true);
            }
            else
            {
                // to succeed must reach here....
                result = MM::CanCommunicate;
            }
            pS->Shutdown();
            // always restore the AnswerTimeout to the default
            GetCoreCallback()->SetDeviceProperty(port_.c_str(), "AnswerTimeout", answerTO);
        }
    }
    catch (...)
    {
        LogMessage("Exception in DetectDevice!", false);
    }

    return result;
}

int LaserManagerHub::Initialize()
{
    std::cout << "test" << std::endl;

    // Name
    int ret = CreateProperty(MM::g_Keyword_Name, g_DeviceNameHub, MM::String, true);
    if (DEVICE_OK != ret)
        return ret;

    // The first second or so after opening the serial port, the Arduino is waiting for firmwareupgrades.  Simply sleep 1 second.
    CDeviceUtils::SleepMs(1000);

    const std::lock_guard<std::mutex> lock(mutex_);

    // Check that we have a controller:
    PurgeComPort(port_.c_str());
    ret = GetControllerVersion(version_);
    if (DEVICE_OK != ret)
    {
        std::cout << "get version error" << std::endl;
        return ret;
    }

    CPropertyAction *pAct = new CPropertyAction(this, &LaserManagerHub::OnVersion);

    CreateProperty(g_versionProp, "1.0", MM::Integer, true, pAct);

    initialized_ = true;
    return DEVICE_OK;
}

int LaserManagerHub::DetectInstalledDevices()
{
    if (MM::CanCommunicate == DetectDevice())
    {
        std::vector<std::string> peripherals;
        peripherals.clear();
        for (int i = 0; i < 9; i++)
        {
            peripherals.push_back(g_DeviceNameDAC + std::to_string(i));
        }
        peripherals.push_back(g_DeviceNamePWM);
        for (size_t i = 0; i < peripherals.size(); i++)
        {
            MM::Device *pDev = ::CreateDevice(peripherals[i].c_str());
            if (pDev)
            {
                AddInstalledDevice(pDev);
            }
        }
    }

    return DEVICE_OK;
}

int LaserManagerHub::Shutdown()
{
    initialized_ = false;
    return DEVICE_OK;
}

int LaserManagerHub::OnPort(MM::PropertyBase *pProp, MM::ActionType pAct)
{
    if (pAct == MM::BeforeGet)
    {
        pProp->Set(port_.c_str());
    }
    else if (pAct == MM::AfterSet)
    {
        pProp->Get(port_);
        portAvailable_ = true;
    }
    return DEVICE_OK;
}

int LaserManagerHub::OnVersion(MM::PropertyBase *pProp, MM::ActionType pAct)
{
    if (pAct == MM::BeforeGet)
    {
        pProp->Set((long)version_);
    }
    return DEVICE_OK;
}

///////////////////////////////////////////////////////////////////////////////
// LaserManagerDAC implementation
// ~~~~~~~~~~~~~~~~~~~~~~~~~~
LaserManagerDAC::LaserManagerDAC(int channel) : busy_(false),
                                                minV_(0.0),
                                                maxV_(5.0),
                                                volts_(0.0),
                                                gatedVolts_(0.0),
                                                channel_(channel),
                                                maxChannel_(8),
                                                gateOpen_(false),
                                                initialized_(false)
{
    InitializeDefaultErrorMessages();

    // add custom error messages
    SetErrorText(ERR_UNKNOWN_POSITION, "Invalid position (state) specified");
    SetErrorText(ERR_INITIALIZE_FAILED, "Initialization of the device failed");
    SetErrorText(ERR_WRITE_FAILED, "Failed to write data to the device");
    SetErrorText(ERR_CLOSE_FAILED, "Failed closing the device");
    SetErrorText(ERR_NO_PORT_SET, "Hub Device not found.  The device is needed to create this device");

    CPropertyAction *pAct = new CPropertyAction(this, &LaserManagerDAC::OnMaxVolt);
    CreateProperty("MaxVolt", "5.0", MM::Float, false, pAct, true);

    name_ = g_DeviceNameDAC + std::to_string(channel_);

    // Description
    int nRet = CreateProperty(MM::g_Keyword_Description, "DAC driver", MM::String, true);
    assert(DEVICE_OK == nRet);

    // Name
    nRet = CreateProperty(MM::g_Keyword_Name, name_.c_str(), MM::String, true);
    assert(DEVICE_OK == nRet);

    // parent ID display
    CreateHubIDProperty();
}

LaserManagerDAC::~LaserManagerDAC()
{
    Shutdown();
}

void LaserManagerDAC::GetName(char *name) const
{
    CDeviceUtils::CopyLimitedString(name, name_.c_str());
}

int LaserManagerDAC::Initialize()
{
    LaserManagerHub *hub = static_cast<LaserManagerHub *>(GetParentHub());
    if (!hub || !hub->IsPortAvailable())
    {
        return ERR_NO_PORT_SET;
    }
    char hubLabel[MM::MaxStrLength];
    hub->GetLabel(hubLabel);
    SetParentID(hubLabel); // for backward comp.

    // set property list
    // -----------------

    // State
    // -----
    CPropertyAction *pActV = new CPropertyAction(this, &LaserManagerDAC::OnVolts);
    int nRet = CreateProperty("Volts", "0.0", MM::Float, false, pActV);
    if (nRet != DEVICE_OK)
        return nRet;
    SetPropertyLimits("Volts", minV_, maxV_);

    CPropertyAction *pActB = new CPropertyAction(this, &LaserManagerDAC::OnState);
    nRet = CreateProperty(MM::g_Keyword_State, "0", MM::Integer, false, pActB);
    if (nRet != DEVICE_OK)
        return nRet;

    nRet = UpdateStatus();
    if (nRet != DEVICE_OK)
        return nRet;

    initialized_ = true;

    return DEVICE_OK;
}

int LaserManagerDAC::Shutdown()
{
    initialized_ = false;
    return DEVICE_OK;
}

int LaserManagerDAC::WriteToPort(unsigned long value)
{
    LaserManagerHub *hub = static_cast<LaserManagerHub *>(GetParentHub());
    if (!hub || !hub->IsPortAvailable())
        return ERR_NO_PORT_SET;

    const std::lock_guard<std::mutex> lock(hub->GetLock());

    hub->PurgeComPortH();

    Protocol::SetLaserCommand laserCommand{
        (uint8_t)channel_,
        (uint16_t)value};

    std::vector<uint8_t> message = protocol_.createSetLaserMessage(laserCommand);

    // Convert full message to hex string
    std::ostringstream msgHexStream;
    msgHexStream << "[";
    for (size_t i = 0; i < message.size(); ++i)
    {
        msgHexStream << "0x" << std::hex << std::uppercase << std::setw(2) << std::setfill('0')
                     << static_cast<int>(message[i]);
        if (i != message.size() - 1)
            msgHexStream << " ";
    }
    msgHexStream << "]";

    LogMessage(msgHexStream.str());

    hub->WriteToComPortH(message.data(), static_cast<unsigned int>(message.size()));

    return DEVICE_OK;
}

int LaserManagerDAC::WriteSignal(double volts)
{
    long value = (long)((volts - minV_) / maxV_ * 4095);

    std::ostringstream os;
    os << "Volts: " << volts << " Max Voltage: " << maxV_ << " digital value: " << value;
    LogMessage(os.str().c_str(), true);

    return WriteToPort(value);
}

int LaserManagerDAC::SetSignal(double volts)
{
    volts_ = volts;
    if (gateOpen_)
    {
        gatedVolts_ = volts_;
        return WriteSignal(volts_);
    }
    else
    {
        gatedVolts_ = 0;
    }

    return DEVICE_OK;
}

int LaserManagerDAC::SetGateOpen(bool open)
{
    if (open)
    {
        gateOpen_ = true;
        gatedVolts_ = volts_;
        return WriteSignal(volts_);
    }
    gateOpen_ = false;
    gatedVolts_ = 0;
    return WriteSignal(0.0);
}

///////////////////////////////////////////////////////////////////////////////
// Action handlers
///////////////////////////////////////////////////////////////////////////////

int LaserManagerDAC::OnVolts(MM::PropertyBase *pProp, MM::ActionType eAct)
{
    if (eAct == MM::BeforeGet)
    {
        // nothing to do, let the caller use cached property
    }
    else if (eAct == MM::AfterSet)
    {
        double volts;
        pProp->Get(volts);
        return SetSignal(volts);
    }

    return DEVICE_OK;
}

int LaserManagerDAC::OnMaxVolt(MM::PropertyBase *pProp, MM::ActionType eAct)
{
    if (eAct == MM::BeforeGet)
    {
        pProp->Set(maxV_);
    }
    else if (eAct == MM::AfterSet)
    {
        pProp->Get(maxV_);
        if (HasProperty("Volts"))
            SetPropertyLimits("Volts", 0.0, maxV_);
    }
    return DEVICE_OK;
}

int LaserManagerDAC::OnChannel(MM::PropertyBase *pProp, MM::ActionType eAct)
{
    if (eAct == MM::BeforeGet)
    {
        pProp->Set((long int)channel_);
    }
    else if (eAct == MM::AfterSet)
    {
        long channel;
        pProp->Get(channel);
        if (channel >= 1 && ((unsigned)channel <= maxChannel_))
            channel_ = channel;
    }
    return DEVICE_OK;
}

int LaserManagerDAC::OnState(MM::PropertyBase *pProp, MM::ActionType eAct)
{
    if (eAct == MM::BeforeGet)
    {
        // nothing to do, let the caller use cached property
    }
    else if (eAct == MM::AfterSet)
    {
        long gate;
        pProp->Get(gate);
        bool open = static_cast<bool>(gate);
        std::ostringstream result;
        result << "Gate value: " << gate << ", Open: " << (open ? "true" : "false");
        LogMessage(result.str());
        return SetGateOpen(open);
    }

    return DEVICE_OK;
}

///////////////////////////////////////////////////////////////////////////////
// LaserManagerPWM implementation
// ~~~~~~~~~~~~~~~~~~~~~~~~~~
LaserManagerPWM::LaserManagerPWM(int channel) : busy_(false),
                                                minV_(0.0),
                                                maxV_(1.0),
                                                volts_(0.0),
                                                gatedVolts_(0.0),
                                                channel_(channel),
                                                maxChannel_(8),
                                                gateOpen_(false),
                                                initialized_(false)
{
    InitializeDefaultErrorMessages();

    // add custom error messages
    SetErrorText(ERR_UNKNOWN_POSITION, "Invalid position (state) specified");
    SetErrorText(ERR_INITIALIZE_FAILED, "Initialization of the device failed");
    SetErrorText(ERR_WRITE_FAILED, "Failed to write data to the device");
    SetErrorText(ERR_CLOSE_FAILED, "Failed closing the device");
    SetErrorText(ERR_NO_PORT_SET, "Hub Device not found.  The device is needed to create this device");

    CPropertyAction *pAct = new CPropertyAction(this, &LaserManagerPWM::OnMaxVolt);
    CreateProperty("MaxVolt", "1.0", MM::Float, false, pAct, true);

    name_ = g_DeviceNamePWM;

    // Description
    int nRet = CreateProperty(MM::g_Keyword_Description, "PWM driver", MM::String, true);
    assert(DEVICE_OK == nRet);

    // Name
    nRet = CreateProperty(MM::g_Keyword_Name, name_.c_str(), MM::String, true);
    assert(DEVICE_OK == nRet);

    // parent ID display
    CreateHubIDProperty();
}

LaserManagerPWM::~LaserManagerPWM()
{
    Shutdown();
}

void LaserManagerPWM::GetName(char *name) const
{
    CDeviceUtils::CopyLimitedString(name, name_.c_str());
}

int LaserManagerPWM::Initialize()
{
    LaserManagerHub *hub = static_cast<LaserManagerHub *>(GetParentHub());
    if (!hub || !hub->IsPortAvailable())
    {
        return ERR_NO_PORT_SET;
    }
    char hubLabel[MM::MaxStrLength];
    hub->GetLabel(hubLabel);
    SetParentID(hubLabel); // for backward comp.

    // set property list
    // -----------------

    // State
    // -----
    CPropertyAction *pActV = new CPropertyAction(this, &LaserManagerPWM::OnVolts);
    int nRet = CreateProperty("Volts", "0.0", MM::Float, false, pActV);
    if (nRet != DEVICE_OK)
        return nRet;
    SetPropertyLimits("Volts", minV_, maxV_);

    CPropertyAction *pActB = new CPropertyAction(this, &LaserManagerPWM::OnState);
    nRet = CreateProperty(MM::g_Keyword_State, "0", MM::Integer, false, pActB);
    if (nRet != DEVICE_OK)
        return nRet;

    nRet = UpdateStatus();
    if (nRet != DEVICE_OK)
        return nRet;

    initialized_ = true;

    return DEVICE_OK;
}

int LaserManagerPWM::Shutdown()
{
    initialized_ = false;
    return DEVICE_OK;
}

int LaserManagerPWM::WriteToPort(unsigned long value)
{
    LaserManagerHub *hub = static_cast<LaserManagerHub *>(GetParentHub());
    if (!hub || !hub->IsPortAvailable())
        return ERR_NO_PORT_SET;

    const std::lock_guard<std::mutex> lock(hub->GetLock());

    hub->PurgeComPortH();

    Protocol::SetMotorCommand motorCommand{
        (uint16_t)value};

    std::vector<uint8_t> message = protocol_.createSetMotorMessage(motorCommand);

    // Convert full message to hex string
    std::ostringstream msgHexStream;
    msgHexStream << "[";
    for (size_t i = 0; i < message.size(); ++i)
    {
        msgHexStream << "0x" << std::hex << std::uppercase << std::setw(2) << std::setfill('0')
                     << static_cast<int>(message[i]);
        if (i != message.size() - 1)
            msgHexStream << " ";
    }
    msgHexStream << "]";

    LogMessage(msgHexStream.str());

    hub->WriteToComPortH(message.data(), static_cast<unsigned int>(message.size()));

    return DEVICE_OK;
}

int LaserManagerPWM::WriteSignal(double volts)
{
    long value = (long)((volts - minV_) / maxV_ * UINT16_MAX);

    std::ostringstream os;
    os << "Volts: " << volts << " Max Voltage: " << maxV_ << " digital value: " << value;
    LogMessage(os.str().c_str(), true);

    return WriteToPort(value);
}

int LaserManagerPWM::SetSignal(double volts)
{
    volts_ = volts;
    if (gateOpen_)
    {
        gatedVolts_ = volts_;
        return WriteSignal(volts_);
    }
    else
    {
        gatedVolts_ = 0;
    }

    return DEVICE_OK;
}

int LaserManagerPWM::SetGateOpen(bool open)
{
    if (open)
    {
        gateOpen_ = true;
        gatedVolts_ = volts_;
        return WriteSignal(volts_);
    }
    gateOpen_ = false;
    gatedVolts_ = 0;
    return WriteSignal(0.0);
}

///////////////////////////////////////////////////////////////////////////////
// Action handlers
///////////////////////////////////////////////////////////////////////////////

int LaserManagerPWM::OnVolts(MM::PropertyBase *pProp, MM::ActionType eAct)
{
    if (eAct == MM::BeforeGet)
    {
        // nothing to do, let the caller use cached property
    }
    else if (eAct == MM::AfterSet)
    {
        double volts;
        pProp->Get(volts);
        return SetSignal(volts);
    }

    return DEVICE_OK;
}

int LaserManagerPWM::OnMaxVolt(MM::PropertyBase *pProp, MM::ActionType eAct)
{
    if (eAct == MM::BeforeGet)
    {
        pProp->Set(maxV_);
    }
    else if (eAct == MM::AfterSet)
    {
        pProp->Get(maxV_);
        if (HasProperty("Volts"))
            SetPropertyLimits("Volts", 0.0, maxV_);
    }
    return DEVICE_OK;
}

int LaserManagerPWM::OnChannel(MM::PropertyBase *pProp, MM::ActionType eAct)
{
    if (eAct == MM::BeforeGet)
    {
        pProp->Set((long int)channel_);
    }
    else if (eAct == MM::AfterSet)
    {
        long channel;
        pProp->Get(channel);
        if (channel >= 1 && ((unsigned)channel <= maxChannel_))
            channel_ = channel;
    }
    return DEVICE_OK;
}

int LaserManagerPWM::OnState(MM::PropertyBase *pProp, MM::ActionType eAct)
{
    if (eAct == MM::BeforeGet)
    {
        // nothing to do, let the caller use cached property
    }
    else if (eAct == MM::AfterSet)
    {
        long gate;
        pProp->Get(gate);
        bool open = static_cast<bool>(gate);
        std::ostringstream result;
        result << "Gate value: " << gate << ", Open: " << (open ? "true" : "false");
        LogMessage(result.str());
        return SetGateOpen(open);
    }

    return DEVICE_OK;
}