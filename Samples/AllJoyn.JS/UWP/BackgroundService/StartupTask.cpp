#include "StartupTask.h"

#include "ajs.h"
#include "aj_init.h"
#include "aj_status.h"
#include "aj_target_nvram.h"


// Debug flags - set to 1 to see their debug logs
#ifndef NDEBUG
extern uint8_t dbgMSG;
extern uint8_t dbgHELPER;
extern uint8_t dbgBUS;
extern uint8_t dbgABOUT;
extern uint8_t dbgINTROSPECT;
extern uint8_t dbgAJCPS;
extern uint8_t dbgAJS;
extern uint8_t dbgHEAP;
extern uint8_t dbgNET;
extern uint8_t dbgHEAPDUMP;
extern uint8_t dbgCONSOLE;
extern uint8_t dbgGPIO;
extern uint8_t dbgDEBUGGER;

extern uint8_t dbgALL;
extern AJ_DebugLevel AJ_DbgLevel;
#endif


using namespace Platform;
using namespace Windows::ApplicationModel::Background;
using namespace AllJoynJSBackgroundService;
using namespace Windows::Storage;

// The Background Application template is documented at http://go.microsoft.com/fwlink/?LinkID=533884&clcid=0x409

void StartupTask::Run(IBackgroundTaskInstance^ taskInstance)
{
#ifndef NDEBUG
    // Enable debug logs
    dbgMSG = dbgBUS = dbgABOUT = dbgINTROSPECT = dbgAJCPS = dbgAJS = 1;
    AJ_DbgLevel = AJ_DEBUG_INFO;
#endif

    // Set NVRAM file to local storage
    String^ nvFile = "ajtcl.nvram";
    String^ nvFilePath = String::Concat(ApplicationData::Current->LocalFolder->Path, "\\");
    nvFilePath += nvFile;
    std::string nvFilePathA(nvFilePath->Begin(), nvFilePath->End());
    AJ_SetNVRAM_FilePath(nvFilePathA.c_str());

    AJ_Initialize();

    AJ_Status status;
    do {
        status = AJS_Main(nullptr);
    } while (status == AJ_ERR_RESTART);
}
