/**
* @file
*/
/******************************************************************************
* * 
*    Copyright (c) 2016 Open Connectivity Foundation and AllJoyn Open
*    Source Project Contributors and others.
*    
*    All rights reserved. This program and the accompanying materials are
*    made available under the terms of the Apache License, Version 2.0
*    which accompanies this distribution, and is available at
*    http://www.apache.org/licenses/LICENSE-2.0

******************************************************************************/

#include "StartupTask.h"

#include "ajs.h"
#include "aj_init.h"
#include "aj_status.h"


// Debug flags - set to 1 to see their debug logs
#ifndef NDEBUG
extern "C" {
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
}
#endif


using namespace Platform;
using namespace Windows::ApplicationModel::Background;
using namespace AllJoynJSBackgroundService;
using namespace Windows::Storage;

// The Background Application template is documented at http://go.microsoft.com/fwlink/?LinkID=533884&clcid=0x409

#ifndef NDEBUG
#define AJS_STACK_SIZE 1024*256
#else
#define AJS_STACK_SIZE 0    // Use default
#endif

extern "C" {
    AJ_Status AJS_Main(const char* deviceName);
    void AJ_SetNVRAM_FilePath(const char* path);
}

DWORD WINAPI AllJoynJS_Start(void* _context)
{
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

    return 0;
}


void StartupTask::Run(IBackgroundTaskInstance^ taskInstance)
{
    if (taskInstance != nullptr)
    {
        deferral = taskInstance->GetDeferral();
    }

#ifndef NDEBUG
    // Enable debug logs
    dbgMSG = dbgBUS = dbgABOUT = dbgINTROSPECT = dbgAJCPS = dbgAJS = 1;
    AJ_DbgLevel = AJ_DEBUG_INFO;
#endif

    HANDLE h = CreateThread(nullptr, AJS_STACK_SIZE, AllJoynJS_Start, nullptr, 0, nullptr);
    if (h != NULL)
    {
        CloseHandle(h);
    }
}