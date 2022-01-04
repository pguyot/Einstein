#include <Includes/Frames/Objects.h>
#include <Includes/Frames/NewtonScript.h>
#include <Includes/Utilities/Unicode.h>
#include <SerialChipRegistry.h>
#include <CommErrors.h>

#define kCMOSerialEinsteinLoc 'eloc'

class THMOSerialEinsteinHardware : public TOption
{
    public:
        THMOSerialEinsteinHardware();
        ULong   fLocationID;    // hardware location
        ULong   fType;          // host side driver type
        ULong   fDataSize;      // size of configuration data
        UChar   fData[60];      // configuration data
};

THMOSerialEinsteinHardware::THMOSerialEinsteinHardware()
{
    SetAsOption(kCMOSerialEinsteinLoc);
    SetLength(sizeof(THMOSerialEinsteinHardware));
}

extern "C" Ref Enable(RefArg inRcvr, RefArg inLocation, RefArg inType, RefArg inConfig)
{
    THMOSerialEinsteinHardware option;
    ULong location = 0;
    char name[5];

    WITH_LOCKED_BINARY(inLocation, ptr)
        UniChar *l = static_cast<UniChar *>(ptr);
        for (size_t i = 0; i < 4; i++) {
            name[i] = l[i];
            location = (location << 8) + l[i];
        }
    END_WITH_LOCKED_BINARY(inLocation)
    name[4] = 0;

    option.fDataSize = 0;
    if (IsBinary(inConfig)) {
        option.fData[sizeof(option.fData) - 1] = 0;
        WITH_LOCKED_BINARY(inConfig, ptr)
            UniChar *l = static_cast<UniChar *>(ptr);
            for (size_t i = 0; i < sizeof(option.fData) - 1 && l[i] != 0; i++) {
                option.fData[i] = l[i];
                option.fDataSize++;
            }
        END_WITH_LOCKED_BINARY(inConfig)
    }

    TSerialChip *einsteinChip = TSerialChip::New("TSerialChipEinstein");
    if (einsteinChip == NULL) {
        return MakeInt(kCommErrMethodNotImplemented);
    }

    PSerialChipRegistry *registry = GetSerialChipRegistry();
    SerialChipID id = registry->FindByLocation(location);
    TSerialChip *existingChip = registry->GetChipPtr(id);
    if (existingChip != NULL) {
        registry->UnRegister(existingChip);
    }
    TUNameServer nameServer;
    nameServer.UnRegisterName(name, "TSerialChip");

    option.fLocationID = location;
    if (IsNumber(inType)) {
        option.fType = RefToInt(inType);
    } else {
        option.fType = -1;
    }
    einsteinChip->ProcessOption(&option);
    registry->Register(einsteinChip, location);

    return MakeInt(0);
}

extern "C" Ref Disable(RefArg inRcvr, RefArg inLocation)
{
    THMOSerialEinsteinHardware option;
    ULong location = 0;
    char name[5];

    DebugStr("Disabling the serial driver");
    WITH_LOCKED_BINARY(inLocation, ptr)
        UniChar *l = static_cast<UniChar *>(ptr);
        for (size_t i = 0; i < 4; i++) {
            name[i] = l[i];
            location = (location << 8) + l[i];
        }
    END_WITH_LOCKED_BINARY(inLocation)
    name[4] = 0;

    PSerialChipRegistry *registry = GetSerialChipRegistry();
    SerialChipID id = registry->FindByLocation(location);
    TSerialChip *einsteinChip = registry->GetChipPtr(id);
    if (einsteinChip != NULL) {
        registry->UnRegister(einsteinChip);
    }
    TUNameServer nameServer;
    nameServer.UnRegisterName(name, "TSerialChip");

    einsteinChip->Delete();

    return MakeInt(0);
}
