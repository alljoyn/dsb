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

#include "ajs.h"
#include "ajs_io.h"

AJ_Status AJS_TargetIO_AdcOpen(uint16_t channel, void** adcCtx)
{
    return AJ_ERR_UNEXPECTED;
}

AJ_Status AJS_TargetIO_AdcClose(void* adcCtx)
{
    return AJ_ERR_UNEXPECTED;
}

uint32_t AJS_TargetIO_AdcRead(void* adcCtx)
{
    return 0;
}

AJ_Status AJS_TargetIO_DacOpen(uint16_t pin, void** dacCtx)
{
    return AJ_ERR_UNEXPECTED;
}

AJ_Status AJS_TargetIO_DacClose(void* dacCtx)
{
    return AJ_ERR_UNEXPECTED;
}

void AJS_TargetIO_DacWrite(void* dacCtx, uint32_t val)
{
}

uint32_t AJS_TargetIO_UartRead(void* uartCtx, uint8_t* buf, uint32_t length)
{
    AJ_InfoPrintf(("AJS_TargetIO_UartRead()"));
    return 0;
}
AJ_Status AJS_TargetIO_UartWrite(void* uartCtx, uint8_t* data, uint32_t length)
{
    AJ_InfoPrintf(("AJS_TargetIO_UartWrite(): Wrote %u bytes\n", length));
    return AJ_OK;
}
AJ_Status AJS_TargetIO_UartOpen(uint8_t txPin, uint8_t rxPin, uint32_t baud, void** uartCtx, int32_t* rxTrigId)
{
    return AJ_ERR_UNEXPECTED;
}
AJ_Status AJS_TargetIO_UartClose(void* uartCtx)
{
    return AJ_ERR_UNEXPECTED;
}