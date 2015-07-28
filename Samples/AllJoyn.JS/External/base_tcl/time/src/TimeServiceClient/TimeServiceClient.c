/******************************************************************************
 * Copyright AllSeen Alliance. All rights reserved.
 *
 *    Permission to use, copy, modify, and/or distribute this software for any
 *    purpose with or without fee is hereby granted, provided that the above
 *    copyright notice and this permission notice appear in all copies.
 *
 *    THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 *    WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 *    MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 *    ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 *    WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 *    ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 *    OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 ******************************************************************************/

/**
 * Per-module definition of the current module for debug logging.  Must be defined
 * prior to first inclusion of aj_debug.h.
 * The corresponding flag dbgAJTS is declared below.
 */
#define AJ_MODULE AJTS
#include <aj_debug.h>

#include <alljoyn.h>
#include <aj_config.h>
#include <alljoyn/time/TimeServiceCommon.h>
#include <alljoyn/time/TimeServiceClient.h>
#include <alljoyn/services_common/ServicesCommon.h>

/**
 * Turn on per-module debug printing by setting this variable to non-zero value
 * (usually in debugger).
 */
#ifndef NDEBUG
#ifndef ER_DEBUG_AJSVCALL
#define ER_DEBUG_AJSVCALL 0
#endif
#ifndef ER_DEBUG_AJTS
#define ER_DEBUG_AJTS 0
#endif
static uint8_t dbgAJTS = ER_DEBUG_AJTS || ER_DEBUG_AJSVCALL;
#endif

/* global params for session handling - only one session can be open at a time */

static uint32_t g_lastSessionRequestSerialNum = 0;
static uint32_t g_sessionId; // there is at most one active session at any given time. if a change in peer is needed, close the session and wait for session lost before opening a new session.
static char g_peerName[16];
static AJTS_OnSessionReply g_onSessionReply = NULL;

/* Time ProxyObject bus registration */

/*
 * Message identifiers for the method calls this service implements
 */

// Clock interface
#define TIME_CLOCK_PROXY_GET_PROP                                         AJ_ENCODE_MESSAGE_ID(AJTS_OBJECT_LIST_INDEX, AJTS_CLOCK_PROXY_OBJECT_INDEX, 0, AJ_PROP_GET)
#define TIME_CLOCK_PROXY_SET_PROP                                         AJ_ENCODE_MESSAGE_ID(AJTS_OBJECT_LIST_INDEX, AJTS_CLOCK_PROXY_OBJECT_INDEX, 0, AJ_PROP_SET)

#define TIME_CLOCK_VERSION_PROP                                           AJ_ENCODE_PROPERTY_ID(AJTS_OBJECT_LIST_INDEX, AJTS_CLOCK_PROXY_OBJECT_INDEX, 1, 0)
#define TIME_CLOCK_PROXY_DATETIME_PROP                                    AJ_ENCODE_PROPERTY_ID(AJTS_OBJECT_LIST_INDEX, AJTS_CLOCK_PROXY_OBJECT_INDEX, 1, 1)
#define TIME_CLOCK_PROXY_ISSET_PROP                                       AJ_ENCODE_PROPERTY_ID(AJTS_OBJECT_LIST_INDEX, AJTS_CLOCK_PROXY_OBJECT_INDEX, 1, 2)

// Clock with time authority interface

#define TIME_CLOCK_AUTHORITY_PROXY_GET_PROP                               AJ_ENCODE_MESSAGE_ID(AJTS_OBJECT_LIST_INDEX, AJTS_TIME_AUTHORITY_PROXY_OBJECT_INDEX, 0, AJ_PROP_GET)
#define TIME_CLOCK_AUTHORITY_PROXY_SET_PROP                               AJ_ENCODE_MESSAGE_ID(AJTS_OBJECT_LIST_INDEX, AJTS_TIME_AUTHORITY_PROXY_OBJECT_INDEX, 0, AJ_PROP_SET)

#define TIME_CLOCK_AUTHORITY_VERSION_PROXY_PROP                           AJ_ENCODE_PROPERTY_ID(AJTS_OBJECT_LIST_INDEX, AJTS_TIME_AUTHORITY_PROXY_OBJECT_INDEX, 1, 0)
#define TIME_CLOCK_AUTHORITY_DATETIME_PROXY_PROP                          AJ_ENCODE_PROPERTY_ID(AJTS_OBJECT_LIST_INDEX, AJTS_TIME_AUTHORITY_PROXY_OBJECT_INDEX, 1, 1)
#define TIME_CLOCK_AUTHORITY_ISSET_PROXY_PROP                             AJ_ENCODE_PROPERTY_ID(AJTS_OBJECT_LIST_INDEX, AJTS_TIME_AUTHORITY_PROXY_OBJECT_INDEX, 1, 2)

#define TIME_AUTHORITY_VERSION_PROXY_PROP                                 AJ_ENCODE_PROPERTY_ID(AJTS_OBJECT_LIST_INDEX, AJTS_TIME_AUTHORITY_PROXY_OBJECT_INDEX, 2, 0)
#define TIME_AUTHORITY_TYPE_PROXY_PROP                                    AJ_ENCODE_PROPERTY_ID(AJTS_OBJECT_LIST_INDEX, AJTS_TIME_AUTHORITY_PROXY_OBJECT_INDEX, 2, 1)
#define TIME_AUTHORITY_TIMESYNC_PROXY_SIGNAL                              AJ_ENCODE_MESSAGE_ID(AJTS_OBJECT_LIST_INDEX, AJTS_TIME_AUTHORITY_PROXY_OBJECT_INDEX, 2, 2)

// Alarm interface

#define TIME_ALARM1_GET_PROXY_PROP                                        AJ_ENCODE_MESSAGE_ID(AJTS_OBJECT_LIST_INDEX, AJTS_ALARM_PROXY_OBJECT_INDEX + 0, 0, AJ_PROP_GET)
#define TIME_ALARM1_SET_PROXY_PROP                                        AJ_ENCODE_MESSAGE_ID(AJTS_OBJECT_LIST_INDEX, AJTS_ALARM_PROXY_OBJECT_INDEX + 0, 0, AJ_PROP_SET)

#define TIME_ALARM1_VERSION_PROP                                          AJ_ENCODE_PROPERTY_ID(AJTS_OBJECT_LIST_INDEX, AJTS_ALARM_OBJECT_INDEX + 0, 1, 0)
#define TIME_ALARM1_SCHEDULE_PROXY_PROP                                   AJ_ENCODE_PROPERTY_ID(AJTS_OBJECT_LIST_INDEX, AJTS_ALARM_PROXY_OBJECT_INDEX + 0, 1, 1)
#define TIME_ALARM1_TITLE_PROXY_PROP                                      AJ_ENCODE_PROPERTY_ID(AJTS_OBJECT_LIST_INDEX, AJTS_ALARM_PROXY_OBJECT_INDEX + 0, 1, 2)
#define TIME_ALARM1_ENABLED_PROXY_PROP                                    AJ_ENCODE_PROPERTY_ID(AJTS_OBJECT_LIST_INDEX, AJTS_ALARM_PROXY_OBJECT_INDEX + 0, 1, 3)

#define TIME_ALARM1_ALARMREACHED_PROXY_SIGNAL                             AJ_ENCODE_MESSAGE_ID(AJTS_OBJECT_LIST_INDEX, AJTS_ALARM_PROXY_OBJECT_INDEX + 0, 1, 4)

// Timer interface

#define TIME_TIMER1_GET_PROXY_PROP                                        AJ_ENCODE_MESSAGE_ID(AJTS_OBJECT_LIST_INDEX, AJTS_TIMER_PROXY_OBJECT_INDEX + 0, 0, AJ_PROP_GET)
#define TIME_TIMER1_SET_PROXY_PROP                                        AJ_ENCODE_MESSAGE_ID(AJTS_OBJECT_LIST_INDEX, AJTS_TIMER_PROXY_OBJECT_INDEX + 0, 0, AJ_PROP_SET)

#define TIME_TIMER1_VERSION_PROXY_PROP                                    AJ_ENCODE_PROPERTY_ID(AJTS_OBJECT_LIST_INDEX, AJTS_TIMER_PROXY_OBJECT_INDEX + 0, 1, 0)
#define TIME_TIMER1_INTERVAL_PROXY_PROP                                   AJ_ENCODE_PROPERTY_ID(AJTS_OBJECT_LIST_INDEX, AJTS_TIMER_PROXY_OBJECT_INDEX + 0, 1, 1)
#define TIME_TIMER1_TIMELEFT_PROXY_PROP                                   AJ_ENCODE_PROPERTY_ID(AJTS_OBJECT_LIST_INDEX, AJTS_TIMER_PROXY_OBJECT_INDEX + 0, 1, 2)

#define TIME_TIMER1_PROXY_START                                           AJ_ENCODE_MESSAGE_ID(AJTS_OBJECT_LIST_INDEX, AJTS_TIMER_PROXY_OBJECT_INDEX + 0, 1, 3)
#define TIME_TIMER1_PROXY_PAUSE                                           AJ_ENCODE_MESSAGE_ID(AJTS_OBJECT_LIST_INDEX, AJTS_TIMER_PROXY_OBJECT_INDEX + 0, 1, 4)
#define TIME_TIMER1_PROXY_RESET                                           AJ_ENCODE_MESSAGE_ID(AJTS_OBJECT_LIST_INDEX, AJTS_TIMER_PROXY_OBJECT_INDEX + 0, 1, 5)

#define TIME_TIMER1_ISRUNNING_PROXY_PROP                                  AJ_ENCODE_PROPERTY_ID(AJTS_OBJECT_LIST_INDEX, AJTS_TIMER_PROXY_OBJECT_INDEX + 0, 1, 6)
#define TIME_TIMER1_REPEAT_PROXY_PROP                                     AJ_ENCODE_PROPERTY_ID(AJTS_OBJECT_LIST_INDEX, AJTS_TIMER_PROXY_OBJECT_INDEX + 0, 1, 7)
#define TIME_TIMER1_TITLE_PROXY_PROP                                      AJ_ENCODE_PROPERTY_ID(AJTS_OBJECT_LIST_INDEX, AJTS_TIMER_PROXY_OBJECT_INDEX + 0, 1, 8)

#define TIME_TIMER1_TIMEREVENT_PROXY_SIGNAL                               AJ_ENCODE_MESSAGE_ID(AJTS_OBJECT_LIST_INDEX, AJTS_TIMER_PROXY_OBJECT_INDEX + 0, 1, 9)
#define TIME_TIMER1_RUNSTATECHANGED_PROXY_SIGNAL                          AJ_ENCODE_MESSAGE_ID(AJTS_OBJECT_LIST_INDEX, AJTS_TIMER_PROXY_OBJECT_INDEX + 0, 1, 10)



AJSVC_ServiceStatus AJTS_Client_MessageProcessor(AJ_BusAttachment* busAttachment, AJ_Message* msg, AJ_Status* msgStatus)
{
    switch (msg->msgId) {
    case TIME_AUTHORITY_TIMESYNC_PROXY_SIGNAL:
        AJ_InfoPrintf(("Received timesync signal.\n"));
        *msgStatus = AJTS_Client_TimeSyncSignalHandler(msg);
        break;


    case TIME_ALARM1_ALARMREACHED_PROXY_SIGNAL:
        AJ_InfoPrintf(("Received alarm reached signal.\n"));
        *msgStatus = AJTS_Client_AlarmReachedSignalHandler(msg);
        break;

    case TIME_TIMER1_TIMEREVENT_PROXY_SIGNAL:
        AJ_InfoPrintf(("Received timer event signal.\n"));
        *msgStatus = AJTS_Client_TimerEventSignalHandler(msg);
        break;

    case TIME_TIMER1_RUNSTATECHANGED_PROXY_SIGNAL:
        AJ_InfoPrintf(("Received timer state changed signal.\n"));
        *msgStatus = AJTS_Client_RunStateChangedSignalHandler(msg);
        break;

    default:
        return AJSVC_SERVICE_STATUS_NOT_HANDLED;
    }

    return AJSVC_SERVICE_STATUS_HANDLED;
}



static AJ_Status RegisterObjectList()
{
    AJTS_ObjectList[AJTS_CLOCK_PROXY_OBJECT_INDEX].flags &= ~(AJ_OBJ_FLAG_HIDDEN | AJ_OBJ_FLAG_DISABLED);
    AJTS_ObjectList[AJTS_CLOCK_PROXY_OBJECT_INDEX].flags |= AJ_OBJ_FLAG_IS_PROXY;

    AJTS_ObjectList[AJTS_TIME_AUTHORITY_PROXY_OBJECT_INDEX].flags &= ~(AJ_OBJ_FLAG_HIDDEN | AJ_OBJ_FLAG_DISABLED);
    AJTS_ObjectList[AJTS_TIME_AUTHORITY_PROXY_OBJECT_INDEX].flags |= AJ_OBJ_FLAG_IS_PROXY;

    AJTS_ObjectList[AJTS_ALARM_PROXY_OBJECT_INDEX].flags &= ~(AJ_OBJ_FLAG_HIDDEN | AJ_OBJ_FLAG_DISABLED);
    AJTS_ObjectList[AJTS_ALARM_PROXY_OBJECT_INDEX].flags |= AJ_OBJ_FLAG_IS_PROXY;

    AJTS_ObjectList[AJTS_TIMER_PROXY_OBJECT_INDEX].flags &= ~(AJ_OBJ_FLAG_HIDDEN | AJ_OBJ_FLAG_DISABLED);
    AJTS_ObjectList[AJTS_TIMER_PROXY_OBJECT_INDEX].flags |= AJ_OBJ_FLAG_IS_PROXY;

    return AJ_RegisterObjectList(AJTS_ObjectList, AJTS_OBJECT_LIST_INDEX);
}

static AJTS_Client_OnSignal appOnTimeSync = NULL;
static AJTS_Client_OnSignal appOnAlarmReached = NULL;
static AJTS_Client_OnSignal appOnTimerEvent = NULL;
static AJTS_Client_OnTimerRunStateChanged appOnTimerRunStateChanged = NULL;
static AJTS_SetPropReplyHandler appSetPropReplyHandler = NULL;
static AJTS_GetDateTimeHandler appGetDateTimeHandler = NULL;
static AJTS_IsSetHandler appIsSetHandler = NULL;
static AJTS_GetAuthorityTypeHandler appGetAuthorityTypeHandler = NULL;
static AJTS_GetAlaramScheduleHandler appGetAlaramScheduleHandler = NULL;
static AJTS_GetAlaramTitleHandler appGetAlaramTitleHandler = NULL;
static AJTS_GetAlaramEnabledHandler appGetAlaramEnabledHandler = NULL;
static AJTS_TimerResetHandler appTimerResetHandler = NULL;
static AJTS_GetTimerIntervalHandler appGetTimerIntervalHandler = NULL;
static AJTS_GetTimerTimeLeftHandler appGetTimerTimeLeftHandler = NULL;
static AJTS_GetTimerIsRunningHandler appGetTimerIsRunningHandler = NULL;
static AJTS_GetTimerRepeatHandler appGetTimerRepeatHandler = NULL;
static AJTS_GetTimerTitleHandler appGetTimerTitleHandler = NULL;



AJ_Status AJTS_Client_Start(AJTS_Client_OnSignal onTimeSync, AJTS_Client_OnSignal onAlarmReached, AJTS_Client_OnSignal onTimerEvent, AJTS_Client_OnTimerRunStateChanged onTimerRunStateChanged, AJTS_SetPropReplyHandler setPropReplyHandler,
                            AJTS_GetDateTimeHandler getDateTimeHandler,
                            AJTS_IsSetHandler isSetHandler,
                            AJTS_GetAuthorityTypeHandler getAuthorityTypeHandler,
                            AJTS_GetAlaramScheduleHandler getAlaramScheduleHandler,
                            AJTS_GetAlaramTitleHandler getAlaramTitleHandler,
                            AJTS_GetAlaramEnabledHandler getAlaramEnabledHandler,
                            AJTS_TimerResetHandler timerResetHandler,
                            AJTS_GetTimerIntervalHandler getTimerIntervalHandler,
                            AJTS_GetTimerTimeLeftHandler getTimerTimeLeftHandler,
                            AJTS_GetTimerIsRunningHandler getTimerIsRunningHandler,
                            AJTS_GetTimerRepeatHandler getTimerRepeatHandler,
                            AJTS_GetTimerTitleHandler getTimerTitleHandler)
{
    AJ_Status status = AJ_OK;

    appOnTimeSync = onTimeSync;
    appOnAlarmReached = onAlarmReached;
    appOnTimerEvent = onTimerEvent;
    appOnTimerRunStateChanged = onTimerRunStateChanged;
    appSetPropReplyHandler = setPropReplyHandler;

    appGetDateTimeHandler = getDateTimeHandler;
    appIsSetHandler = isSetHandler;
    appGetAuthorityTypeHandler = getAuthorityTypeHandler;
    appGetAlaramScheduleHandler = getAlaramScheduleHandler;
    appGetAlaramTitleHandler = getAlaramTitleHandler;
    appGetAlaramEnabledHandler = getAlaramEnabledHandler;
    appTimerResetHandler = timerResetHandler;
    appGetTimerIntervalHandler = getTimerIntervalHandler;
    appGetTimerTimeLeftHandler = getTimerTimeLeftHandler;
    appGetTimerIsRunningHandler = getTimerIsRunningHandler;
    appGetTimerRepeatHandler = getTimerRepeatHandler;
    appGetTimerTitleHandler = getTimerTitleHandler;


    status = RegisterObjectList();

    return status;
}

static const char timeSyncSignalsMatch[] = "interface='org.allseen.Time.TimeAuthority',sessionless='t'";
static const char alarmReachedSignalsMatch[] = "interface='org.allseen.Time.Alarm',sessionless='t'";
static const char timerEventSignalsMatch[] = "interface='org.allseen.Time.Timer',sessionless='t'";

static const char timerRunStateChangedMatch[] = "interface='org.allseen.Time.Timer'";
static const char wildCard[] = "!"; /**< Signifies an object path which is also a wild card, every object is defaulted to this so sessionless signals will work */

AJ_Status AJTS_Client_SetSignalRules(AJ_BusAttachment* busAttachment)
{
    AJ_Status status = AJ_OK;

    AJ_InfoPrintf(("In SetSignalRules()\n"));
    AJ_InfoPrintf(("Adding time service signal match rules.\n"));
    status = AJ_BusSetSignalRuleFlags(busAttachment, timeSyncSignalsMatch, AJ_BUS_SIGNAL_ALLOW, AJ_FLAG_NO_REPLY_EXPECTED);
    if (status != AJ_OK) {
        AJ_ErrPrintf(("Could not set time service Interface AddMatch\n"));
        return status;
    }

    status = AJ_BusSetSignalRuleFlags(busAttachment, alarmReachedSignalsMatch, AJ_BUS_SIGNAL_ALLOW, AJ_FLAG_NO_REPLY_EXPECTED);
    if (status != AJ_OK) {
        AJ_ErrPrintf(("Could not set time service Interface AddMatch\n"));
        return status;
    }

    status = AJ_BusSetSignalRuleFlags(busAttachment, timerEventSignalsMatch, AJ_BUS_SIGNAL_ALLOW, AJ_FLAG_NO_REPLY_EXPECTED);
    if (status != AJ_OK) {
        AJ_ErrPrintf(("Could not set time service Interface AddMatch\n"));
        return status;
    }

    AJ_InfoPrintf(("Adding timerRunStateChangedMatch interface match.\n"));

    status = AJ_BusSetSignalRuleFlags(busAttachment, timerRunStateChangedMatch, AJ_BUS_SIGNAL_ALLOW, AJ_FLAG_NO_REPLY_EXPECTED);
    if (status != AJ_OK) {
        AJ_ErrPrintf(("Could not set timerRunStateChangedMatch Interface AddMatch\n"));
        return status;
    }

    return status;
}

AJ_Status AJTS_Client_TimeSyncSignalHandler(AJ_Message* msg)
{
    AJ_Status status = AJ_OK;

    if (appOnTimeSync) {
        (*appOnTimeSync)(msg->sender, msg->objPath);
    }

    return status;
}

AJ_Status AJTS_Client_AlarmReachedSignalHandler(AJ_Message* msg)
{
    AJ_Status status = AJ_OK;

    if (appOnAlarmReached) {
        (*appOnAlarmReached)(msg->sender, msg->objPath);
    }

    return status;
}


AJ_Status AJTS_Client_TimerEventSignalHandler(AJ_Message* msg)
{
    AJ_Status status = AJ_OK;

    if (appOnTimerEvent) {
        (*appOnTimerEvent)(msg->sender, msg->objPath);
    }

    return status;
}

AJ_Status AJTS_Client_RunStateChangedSignalHandler(AJ_Message* msg)
{
    AJ_Status status = AJ_OK;
    uint32_t state;

    status = AJ_UnmarshalArgs(msg, "b", &state);

    if (status != AJ_OK) {
        goto Exit;
    }

    if (appOnTimerRunStateChanged) {
        (*appOnTimerRunStateChanged)(msg->sender, msg->objPath, state);
    }

Exit:

    return status;
}

AJ_Status AJTS_Client_CreateSessionWithServer(AJ_BusAttachment* busAttachment, const char* peerName, AJTS_OnSessionReply onSessionReply)
{
    AJ_Status status = AJ_OK;
    AJ_SessionOpts sessionOpts = {
        AJ_SESSION_TRAFFIC_MESSAGES,
        AJ_SESSION_PROXIMITY_ANY,
        AJ_TRANSPORT_ANY,
        TRUE
    };

    if (g_lastSessionRequestSerialNum) {
        return AJ_OK;
    }

    AJ_InfoPrintf(("Inside CreateSessionWithServer()\n"));
    g_lastSessionRequestSerialNum = busAttachment->serial;
    AJ_InfoPrintf(("CreateSessionWithServer(): Joining session with %s on port %u with serial number %u\n", peerName, AJTS_Port, g_lastSessionRequestSerialNum));
    status = AJ_BusJoinSession(busAttachment, peerName, AJTS_Port, &sessionOpts);
    AJ_InfoPrintf(("CreateSessionWithServer(): AJ_BusJoinSession() returned with status %s\n", AJ_StatusText(status)));

    g_onSessionReply = onSessionReply;
    strncpy(g_peerName, peerName, 16);
    g_peerName[15] = '\0';

    return status;
}

AJ_Status AJTS_Client_LeaveSessionWithServer(AJ_BusAttachment* busAttachment)
{
    AJ_Status status = AJ_BusLeaveSession(busAttachment, g_sessionId);

    if (status == AJ_OK) {
        g_sessionId = 0;
    }

    return status;
}

AJ_Status SendGetProp(AJ_BusAttachment* bus, AJ_Message*msg, RequestContext*request, uint32_t propId)
{
    AJ_Status status = AJ_OK;

    uint32_t msgId = propId & 0xFFFF0000; // FFFF is the indices of the entry in the full object list, and the entry in the specific time service list, 00 is the dbus property interface in the interfaces array and the last 00 is corresponding to the "get" operation. so we get: AJ_ENCODE_MESSAGE_ID(AJTS_OBJECT_LIST_INDEX, AJTS_CLOCK_PROXY_OBJECT_INDEX, 0, AJ_PROP_GET)

    status = AJ_MarshalMethodCall(bus, msg, msgId, g_peerName, g_sessionId, 0, AJ_METHOD_TIMEOUT);
    if (status == AJ_OK) {
        status = AJ_MarshalPropertyArgs(msg, propId);

        if (status == AJ_OK) {
            request->replySerial = msg->hdr->serialNum;
            request->propId = propId;


            status = AJ_DeliverMsg(msg);
        }
    }
    return status;
}

AJ_Status SendSetProp(AJ_BusAttachment* bus, AJ_Message*msg, uint32_t propId)
{
    AJ_Status status = AJ_OK;

    uint32_t msgId = propId & 0xFFFF0000; // FFFF is the indices of the entry in the full object list, and the entry in the specific time service list, 00 is the dbus property interface in the interfaces array and the last 00 is corresponding to the "get" operation. at the next line we will set the member index to AJ_PROP_SET.

    msgId |= AJ_PROP_SET; // mark this method to be the set property method.

    status = AJ_MarshalMethodCall(bus, msg, msgId, g_peerName, g_sessionId, 0, AJ_METHOD_TIMEOUT);
    if (status == AJ_OK) {
        status = AJ_MarshalPropertyArgs(msg, propId);
    }

    return status;
}

AJ_Status AJTS_Client_SetDateTime(AJ_BusAttachment* bus, RequestContext*request, const TS_Date* date, const TS_Time* time, const int16_t utcOffset, uint8_t isAuthority)
{
    AJ_Status status = AJ_OK;
    AJ_Message msg;
    AJ_Arg structArg;
    uint32_t propId = (isAuthority ? TIME_CLOCK_AUTHORITY_DATETIME_PROXY_PROP : TIME_CLOCK_PROXY_DATETIME_PROP);

    strncpy((char*)AJTS_ObjectList[(isAuthority ? AJTS_TIME_AUTHORITY_PROXY_OBJECT_INDEX : AJTS_CLOCK_PROXY_OBJECT_INDEX)].path, request->objPath, AJTS_MAX_OBJ_PATH_LENGTH);
    status = SendSetProp(bus, &msg, propId);

    if (status == AJ_OK) {

        status = AJ_MarshalContainer(&msg, &structArg, AJ_ARG_STRUCT);
        if (status != AJ_OK) {
            goto Exit;
        }

        status = AJ_MarshalArgs(&msg, "(qyy)", date->year, date->month, date->day);
        if (status != AJ_OK) {
            goto Exit;
        }

        status = AJ_MarshalArgs(&msg, "(yyyq)", time->hour, time->minute, time->second, time->milliseconds);
        if (status != AJ_OK) {
            goto Exit;
        }

        status = AJ_MarshalArgs(&msg, "n", utcOffset);
        if (status != AJ_OK) {
            goto Exit;
        }

        status = AJ_MarshalCloseContainer(&msg, &structArg);
        if (status != AJ_OK) {
            goto Exit;
        }

    } else {
        AJ_AlwaysPrintf((">>>>>>>> SendSetProp() AJ_MarshalPropertyArgs() returned status = 0x%04x\n", status));
    }

    if (status == AJ_OK) {
        request->replySerial = msg.hdr->serialNum;
        request->propId = propId;

        status = AJ_DeliverMsg(&msg);
    }

Exit:
    strcpy((char*)AJTS_ObjectList[(isAuthority ? AJTS_TIME_AUTHORITY_PROXY_OBJECT_INDEX : AJTS_CLOCK_PROXY_OBJECT_INDEX)].path, wildCard);

    return status;
}

AJ_Status AJTS_Client_GetDateTime(AJ_BusAttachment* bus, RequestContext*request, uint8_t isAuthority)
{
    AJ_Status status = AJ_OK;
    AJ_Message msg;
    uint32_t propId = (isAuthority ? TIME_CLOCK_AUTHORITY_DATETIME_PROXY_PROP : TIME_CLOCK_PROXY_DATETIME_PROP);

    strncpy((char*)AJTS_ObjectList[(isAuthority ? AJTS_TIME_AUTHORITY_PROXY_OBJECT_INDEX : AJTS_CLOCK_PROXY_OBJECT_INDEX)].path, request->objPath, AJTS_MAX_OBJ_PATH_LENGTH);

    status = SendGetProp(bus, &msg, request, propId);

    strcpy((char*)AJTS_ObjectList[(isAuthority ? AJTS_TIME_AUTHORITY_PROXY_OBJECT_INDEX : AJTS_CLOCK_PROXY_OBJECT_INDEX)].path, wildCard);

    return status;

}

AJ_Status AJTS_Client_IsSet(AJ_BusAttachment* bus, RequestContext*request, uint8_t isAuthority)
{
    AJ_Status status = AJ_OK;
    AJ_Message msg;
    uint32_t propId = (isAuthority ? TIME_CLOCK_AUTHORITY_ISSET_PROXY_PROP : TIME_CLOCK_PROXY_ISSET_PROP);

    strncpy((char*)AJTS_ObjectList[(isAuthority ? AJTS_TIME_AUTHORITY_PROXY_OBJECT_INDEX : AJTS_CLOCK_PROXY_OBJECT_INDEX)].path, request->objPath, AJTS_MAX_OBJ_PATH_LENGTH);

    status = SendGetProp(bus, &msg, request, propId);

    strcpy((char*)AJTS_ObjectList[(isAuthority ? AJTS_TIME_AUTHORITY_PROXY_OBJECT_INDEX : AJTS_CLOCK_PROXY_OBJECT_INDEX)].path, wildCard);

    return status;
}

AJ_Status AJTS_Client_GetAuthorityType(AJ_BusAttachment* bus, RequestContext*request)
{
    AJ_Status status = AJ_OK;
    AJ_Message msg;
    uint32_t propId = TIME_AUTHORITY_TYPE_PROXY_PROP;

    strncpy((char*)AJTS_ObjectList[AJTS_TIME_AUTHORITY_PROXY_OBJECT_INDEX].path, request->objPath, AJTS_MAX_OBJ_PATH_LENGTH);

    status = SendGetProp(bus, &msg, request, propId);

    strcpy((char*)AJTS_ObjectList[AJTS_TIME_AUTHORITY_PROXY_OBJECT_INDEX].path, wildCard);

    return status;

}

AJ_Status AJTS_Client_GetAlaramSchedule(AJ_BusAttachment* bus, RequestContext*request)
{
    AJ_Status status = AJ_OK;
    AJ_Message msg;
    uint32_t propId = TIME_ALARM1_SCHEDULE_PROXY_PROP;

    strncpy((char*)AJTS_ObjectList[AJTS_ALARM_PROXY_OBJECT_INDEX].path, request->objPath, AJTS_MAX_OBJ_PATH_LENGTH);

    status = SendGetProp(bus, &msg, request, propId);

    strcpy((char*)AJTS_ObjectList[AJTS_ALARM_PROXY_OBJECT_INDEX].path, wildCard);

    return status;
}

AJ_Status AJTS_Client_SetAlaramSchedule(AJ_BusAttachment* bus, RequestContext*request, const TS_Time* time, const uint8_t weekdaysBitmap)
{
    AJ_Status status = AJ_OK;
    AJ_Message msg;
    uint32_t propId = TIME_ALARM1_SCHEDULE_PROXY_PROP;

    strncpy((char*)AJTS_ObjectList[AJTS_ALARM_PROXY_OBJECT_INDEX].path, request->objPath, AJTS_MAX_OBJ_PATH_LENGTH);

    status = SendSetProp(bus, &msg, propId);

    if (status == AJ_OK) {

        status = AJ_MarshalArgs(&msg, "((yyyq)y)", time->hour, time->minute, time->second, time->milliseconds, weekdaysBitmap);
        if (status != AJ_OK) {
            goto Exit;
        }

    } else {
        AJ_AlwaysPrintf((">>>>>>>> SendSetProp() AJ_MarshalPropertyArgs() returned status = 0x%04x\n", status));
    }

    if (status == AJ_OK) {
        request->replySerial = msg.hdr->serialNum;
        request->propId = propId;

        status = AJ_DeliverMsg(&msg);
    }

Exit:
    strcpy((char*)AJTS_ObjectList[AJTS_ALARM_PROXY_OBJECT_INDEX].path, wildCard);

    return status;
}

AJ_Status AJTS_Client_GetAlaramTitle(AJ_BusAttachment* bus, RequestContext*request)
{
    AJ_Status status = AJ_OK;
    AJ_Message msg;
    uint32_t propId = TIME_ALARM1_TITLE_PROXY_PROP;

    strncpy((char*)AJTS_ObjectList[AJTS_ALARM_PROXY_OBJECT_INDEX].path, request->objPath, AJTS_MAX_OBJ_PATH_LENGTH);

    status = SendGetProp(bus, &msg, request, propId);

    strcpy((char*)AJTS_ObjectList[AJTS_ALARM_PROXY_OBJECT_INDEX].path, wildCard);

    return status;
}

AJ_Status AJTS_Client_SetAlaramTitle(AJ_BusAttachment* bus, RequestContext*request, const char title[AJTS_MAX_TITLE_LENGTH + 1])
{
    AJ_Status status = AJ_OK;
    AJ_Message msg;
    uint32_t propId = TIME_ALARM1_TITLE_PROXY_PROP;

    strncpy((char*)AJTS_ObjectList[AJTS_ALARM_PROXY_OBJECT_INDEX].path, request->objPath, AJTS_MAX_OBJ_PATH_LENGTH);

    status = SendSetProp(bus, &msg, propId);

    if (status == AJ_OK) {

        status = AJ_MarshalArgs(&msg, "s", title);
        if (status != AJ_OK) {
            goto Exit;
        }

    } else {
        AJ_AlwaysPrintf((">>>>>>>> SendSetProp() AJ_MarshalPropertyArgs() returned status = 0x%04x\n", status));
    }

    if (status == AJ_OK) {
        request->replySerial = msg.hdr->serialNum;
        request->propId = propId;

        status = AJ_DeliverMsg(&msg);
    }

Exit:
    strcpy((char*)AJTS_ObjectList[AJTS_ALARM_PROXY_OBJECT_INDEX].path, wildCard);

    return status;
}

AJ_Status AJTS_Client_GetAlaramEnabled(AJ_BusAttachment* bus, RequestContext*request)
{
    AJ_Status status = AJ_OK;
    AJ_Message msg;
    uint32_t propId = TIME_ALARM1_ENABLED_PROXY_PROP;

    strncpy((char*)AJTS_ObjectList[AJTS_ALARM_PROXY_OBJECT_INDEX].path, request->objPath, AJTS_MAX_OBJ_PATH_LENGTH);

    status = SendGetProp(bus, &msg, request, propId);

    strcpy((char*)AJTS_ObjectList[AJTS_ALARM_PROXY_OBJECT_INDEX].path, wildCard);

    return status;
}

AJ_Status AJTS_Client_SetAlaramEnabled(AJ_BusAttachment* bus, RequestContext*request, const uint8_t isEnabled)
{
    AJ_Status status = AJ_OK;
    AJ_Message msg;
    uint32_t propId = TIME_ALARM1_ENABLED_PROXY_PROP;

    strncpy((char*)AJTS_ObjectList[AJTS_ALARM_PROXY_OBJECT_INDEX].path, request->objPath, AJTS_MAX_OBJ_PATH_LENGTH);

    status = SendSetProp(bus, &msg, propId);

    if (status == AJ_OK) {

        status = AJ_MarshalArgs(&msg, "b", isEnabled);
        if (status != AJ_OK) {
            goto Exit;
        }

    } else {
        AJ_AlwaysPrintf((">>>>>>>> SendSetProp() AJ_MarshalPropertyArgs() returned status = 0x%04x\n", status));
    }

    if (status == AJ_OK) {
        request->replySerial = msg.hdr->serialNum;
        request->propId = propId;

        status = AJ_DeliverMsg(&msg);
    }

Exit:
    strcpy((char*)AJTS_ObjectList[AJTS_ALARM_PROXY_OBJECT_INDEX].path, wildCard);

    return status;
}


AJ_Status AJTS_Client_TimerStart(AJ_BusAttachment* bus, RequestContext*request)
{
    AJ_Status status = AJ_OK;
    AJ_Message msg;

    strncpy((char*)AJTS_ObjectList[AJTS_TIMER_PROXY_OBJECT_INDEX].path, request->objPath, AJTS_MAX_OBJ_PATH_LENGTH);

    status = AJ_MarshalMethodCall(bus, &msg, TIME_TIMER1_PROXY_START, g_peerName, g_sessionId, 0, AJ_METHOD_TIMEOUT);

    if (status == AJ_OK) {
        request->replySerial = msg.hdr->serialNum;
        request->propId = 0; // no prop id for methods!

        status = AJ_DeliverMsg(&msg);
    }

    strcpy((char*)AJTS_ObjectList[AJTS_TIMER_PROXY_OBJECT_INDEX].path, wildCard);

    return status;
}
AJ_Status AJTS_Client_TimerPause(AJ_BusAttachment* bus, RequestContext*request)
{
    AJ_Status status = AJ_OK;
    AJ_Message msg;

    strncpy((char*)AJTS_ObjectList[AJTS_TIMER_PROXY_OBJECT_INDEX].path, request->objPath, AJTS_MAX_OBJ_PATH_LENGTH);

    status = AJ_MarshalMethodCall(bus, &msg, TIME_TIMER1_PROXY_PAUSE, g_peerName, g_sessionId, 0, AJ_METHOD_TIMEOUT);

    if (status == AJ_OK) {
        request->replySerial = msg.hdr->serialNum;
        request->propId = 0; // no prop id for methods!

        status = AJ_DeliverMsg(&msg);
    }

    strcpy((char*)AJTS_ObjectList[AJTS_TIMER_PROXY_OBJECT_INDEX].path, wildCard);

    return status;
}

AJ_Status AJTS_Client_TimerReset(AJ_BusAttachment* bus, RequestContext*request)
{
    AJ_Status status = AJ_OK;
    AJ_Message msg;

    strncpy((char*)AJTS_ObjectList[AJTS_TIMER_PROXY_OBJECT_INDEX].path, request->objPath, AJTS_MAX_OBJ_PATH_LENGTH);

    status = AJ_MarshalMethodCall(bus, &msg, TIME_TIMER1_PROXY_RESET, g_peerName, g_sessionId, 0, AJ_METHOD_TIMEOUT);

    if (status == AJ_OK) {
        request->replySerial = msg.hdr->serialNum;
        request->propId = 0; // no prop id for methods!

        status = AJ_DeliverMsg(&msg);
    }

    strcpy((char*)AJTS_ObjectList[AJTS_TIMER_PROXY_OBJECT_INDEX].path, wildCard);

    return status;
}

AJ_Status AJTS_Client_GetTimerInterval(AJ_BusAttachment* bus, RequestContext*request)
{
    AJ_Status status = AJ_OK;
    AJ_Message msg;
    uint32_t propId = TIME_TIMER1_INTERVAL_PROXY_PROP;

    strncpy((char*)AJTS_ObjectList[AJTS_TIMER_PROXY_OBJECT_INDEX].path, request->objPath, AJTS_MAX_OBJ_PATH_LENGTH);

    status = SendGetProp(bus, &msg, request, propId);

    strcpy((char*)AJTS_ObjectList[AJTS_TIMER_PROXY_OBJECT_INDEX].path, wildCard);

    return status;
}

AJ_Status AJTS_Client_SetTimerInterval(AJ_BusAttachment* bus, RequestContext*request, const TS_Timer_Period* time)
{
    AJ_Status status = AJ_OK;
    AJ_Message msg;
    uint32_t propId = TIME_TIMER1_INTERVAL_PROXY_PROP;

    strncpy((char*)AJTS_ObjectList[AJTS_TIMER_PROXY_OBJECT_INDEX].path, request->objPath, AJTS_MAX_OBJ_PATH_LENGTH);

    status = SendSetProp(bus, &msg, propId);

    if (status == AJ_OK) {

        status = AJ_MarshalArgs(&msg, "(uyyq)", time->hour, time->minute, time->second, time->milliseconds);
        if (status != AJ_OK) {
            goto Exit;
        }

    } else {
        AJ_AlwaysPrintf((">>>>>>>> SendSetProp() AJ_MarshalPropertyArgs() returned status = 0x%04x\n", status));
    }

    if (status == AJ_OK) {
        request->replySerial = msg.hdr->serialNum;
        request->propId = propId;

        status = AJ_DeliverMsg(&msg);
    }

Exit:
    strcpy((char*)AJTS_ObjectList[AJTS_TIMER_PROXY_OBJECT_INDEX].path, wildCard);

    return status;
}

AJ_Status AJTS_Client_GetTimerTimeLeft(AJ_BusAttachment* bus, RequestContext*request)
{
    AJ_Status status = AJ_OK;
    AJ_Message msg;
    uint32_t propId = TIME_TIMER1_TIMELEFT_PROXY_PROP;

    strncpy((char*)AJTS_ObjectList[AJTS_TIMER_PROXY_OBJECT_INDEX].path, request->objPath, AJTS_MAX_OBJ_PATH_LENGTH);

    status = SendGetProp(bus, &msg, request, propId);

    strcpy((char*)AJTS_ObjectList[AJTS_TIMER_PROXY_OBJECT_INDEX].path, wildCard);

    return status;
}

AJ_Status AJTS_Client_GetTimerIsRunning(AJ_BusAttachment* bus, RequestContext*request)
{
    AJ_Status status = AJ_OK;
    AJ_Message msg;
    uint32_t propId = TIME_TIMER1_ISRUNNING_PROXY_PROP;

    strncpy((char*)AJTS_ObjectList[AJTS_TIMER_PROXY_OBJECT_INDEX].path, request->objPath, AJTS_MAX_OBJ_PATH_LENGTH);

    status = SendGetProp(bus, &msg, request, propId);

    strcpy((char*)AJTS_ObjectList[AJTS_TIMER_PROXY_OBJECT_INDEX].path, wildCard);

    return status;
}

AJ_Status AJTS_Client_GetTimerRepeat(AJ_BusAttachment* bus, RequestContext*request)
{
    AJ_Status status = AJ_OK;
    AJ_Message msg;
    uint32_t propId = TIME_TIMER1_REPEAT_PROXY_PROP;

    strncpy((char*)AJTS_ObjectList[AJTS_TIMER_PROXY_OBJECT_INDEX].path, request->objPath, AJTS_MAX_OBJ_PATH_LENGTH);

    status = SendGetProp(bus, &msg, request, propId);

    strcpy((char*)AJTS_ObjectList[AJTS_TIMER_PROXY_OBJECT_INDEX].path, wildCard);

    return status;
}

AJ_Status AJTS_Client_SetTimerRepeat(AJ_BusAttachment* bus, RequestContext*request, const uint16_t repeat)
{
    AJ_Status status = AJ_OK;
    AJ_Message msg;
    uint32_t propId = TIME_TIMER1_REPEAT_PROXY_PROP;

    strncpy((char*)AJTS_ObjectList[AJTS_TIMER_PROXY_OBJECT_INDEX].path, request->objPath, AJTS_MAX_OBJ_PATH_LENGTH);

    status = SendSetProp(bus, &msg, propId);

    if (status == AJ_OK) {

        status = AJ_MarshalArgs(&msg, "q", repeat);
        if (status != AJ_OK) {
            goto Exit;
        }

    } else {
        AJ_AlwaysPrintf((">>>>>>>> SendSetProp() AJ_MarshalPropertyArgs() returned status = 0x%04x\n", status));
    }

    if (status == AJ_OK) {
        request->replySerial = msg.hdr->serialNum;
        request->propId = propId;

        status = AJ_DeliverMsg(&msg);
    }

Exit:
    strcpy((char*)AJTS_ObjectList[AJTS_TIMER_PROXY_OBJECT_INDEX].path, wildCard);

    return status;
}

AJ_Status AJTS_Client_GetTimerTitle(AJ_BusAttachment* bus, RequestContext*request)
{
    AJ_Status status = AJ_OK;
    AJ_Message msg;
    uint32_t propId = TIME_TIMER1_TITLE_PROXY_PROP;

    strncpy((char*)AJTS_ObjectList[AJTS_TIMER_PROXY_OBJECT_INDEX].path, request->objPath, AJTS_MAX_OBJ_PATH_LENGTH);

    status = SendGetProp(bus, &msg, request, propId);


    strcpy((char*)AJTS_ObjectList[AJTS_TIMER_PROXY_OBJECT_INDEX].path, wildCard);

    return status;
}

AJ_Status AJTS_Client_SetTimerTitle(AJ_BusAttachment* bus, RequestContext*request, const char title[AJTS_MAX_TITLE_LENGTH + 1])
{
    AJ_Status status = AJ_OK;
    AJ_Message msg;
    uint32_t propId = TIME_TIMER1_TITLE_PROXY_PROP;

    if (strlen(title) > AJTS_MAX_TITLE_LENGTH) {
        status = AJ_ERR_INVALID;

        AJ_AlwaysPrintf(("AJTS_Client_SetTimerTitle returned status = 0x%04x\n", status));

        goto Exit;
    }

    strncpy((char*)AJTS_ObjectList[AJTS_TIMER_PROXY_OBJECT_INDEX].path, request->objPath, AJTS_MAX_OBJ_PATH_LENGTH);

    status = SendSetProp(bus, &msg, propId);

    if (status == AJ_OK) {

        status = AJ_MarshalArgs(&msg, "s", title);
        if (status != AJ_OK) {
            goto Exit;
        }

    } else {
        AJ_AlwaysPrintf((">>>>>>>> SendSetProp() AJ_MarshalPropertyArgs() returned status = 0x%04x\n", status));
    }

    if (status == AJ_OK) {
        request->replySerial = msg.hdr->serialNum;
        request->propId = propId;

        status = AJ_DeliverMsg(&msg);
    }

Exit:
    strcpy((char*)AJTS_ObjectList[AJTS_TIMER_PROXY_OBJECT_INDEX].path, wildCard);

    return status;
}



AJ_Status AJTS_Client_ConnectedHandler(AJ_BusAttachment* busAttachment)
{
    return AJTS_Client_SetSignalRules(busAttachment);
}


AJSVC_ServiceStatus AJTS_Client_SessionJoinedHandler(AJ_BusAttachment* busAttachment, uint32_t sessionId, uint32_t replySerialNum)
{
    AJSVC_ServiceStatus serviceStatus = AJSVC_SERVICE_STATUS_NOT_HANDLED;

    if (g_lastSessionRequestSerialNum != 0 && g_lastSessionRequestSerialNum == replySerialNum) { // Check if this is a reply to our request
        AJ_InfoPrintf(("HandleSessionJoined(): Got reply serial number %u that matches last request serial number %u\n", replySerialNum, g_lastSessionRequestSerialNum));
        if (g_sessionId == 0) {
            g_sessionId = sessionId;
            serviceStatus = AJSVC_SERVICE_STATUS_HANDLED;

            g_onSessionReply(sessionId, g_peerName);
        }
    }

    return serviceStatus;
}

AJSVC_ServiceStatus AJTS_Client_SessionRejectedHandler(AJ_BusAttachment* busAttachment, uint32_t replySerialNum, uint32_t replyCode)
{
    AJSVC_ServiceStatus serviceStatus = AJSVC_SERVICE_STATUS_NOT_HANDLED;

    if (g_lastSessionRequestSerialNum != 0 && g_lastSessionRequestSerialNum == replySerialNum) { // Check if this is a reply to our request
        AJ_InfoPrintf(("HandleSessionRejected(): Got reply serial number %u that matches last request serial number %u\n", replySerialNum, g_lastSessionRequestSerialNum));
        if (g_sessionId == 0) {
            g_lastSessionRequestSerialNum = 0;
            serviceStatus = AJSVC_SERVICE_STATUS_HANDLED;
        }
    }

    return serviceStatus;
}

AJSVC_ServiceStatus AJTS_Client_SessionLostHandler(AJ_BusAttachment* busAttachment, uint32_t sessionId, uint32_t reason)
{
    AJSVC_ServiceStatus serviceStatus = AJSVC_SERVICE_STATUS_NOT_HANDLED;

    if ((g_lastSessionRequestSerialNum != 0) && (g_sessionId == sessionId)) {
        g_lastSessionRequestSerialNum = 0;
        g_sessionId = 0;
        serviceStatus = AJSVC_SERVICE_STATUS_HANDLED;
    }

    return serviceStatus;
}

AJ_Status AJTS_Client_DisconnectHandler(AJ_BusAttachment* busAttachment)
{
    return AJ_OK;
}

AJ_Status AJTS_Client_PropGetHandler(AJ_Message* replyMsg, RequestContext* context)
{
    AJ_Status status = AJ_OK;
    switch (context->propId) {
    case TIME_CLOCK_PROXY_DATETIME_PROP:
    case TIME_CLOCK_AUTHORITY_DATETIME_PROXY_PROP:
        {
            TS_Date date;
            TS_Time time;
            int16_t utcOffset;

            status = AJ_UnmarshalArgs(replyMsg, "v", "((qyy)(yyyq)n)", &date.year, &date.month, &date.day, &time.hour, &time.minute, &time.second, &time.milliseconds, &utcOffset);

            if (status != AJ_OK) {
                return appGetDateTimeHandler(replyMsg->sender, context, NULL, NULL, 0, AJ_OK); // TODO: is this a good pattern ?
            }

            return appGetDateTimeHandler(replyMsg->sender, context, &date, &time, utcOffset, AJ_OK);
        }

        break;

    case TIME_CLOCK_PROXY_ISSET_PROP:
    case TIME_CLOCK_AUTHORITY_ISSET_PROXY_PROP:
        {
            uint32_t isSet;
            status = AJ_UnmarshalArgs(replyMsg, "v", "b", &isSet);
            if (status != AJ_OK) {
                return appIsSetHandler(replyMsg->sender, context, -1, status);
            }

            return appIsSetHandler(replyMsg->sender, context, isSet, AJ_OK);
        }
        break;

    case TIME_AUTHORITY_TYPE_PROXY_PROP:
        {
            uint8_t type;
            status = AJ_UnmarshalArgs(replyMsg, "v", "y", &type);
            if (status != AJ_OK) {
                return appGetAuthorityTypeHandler(replyMsg->sender, context, (uint8_t)-1, status);
            }

            return appGetAuthorityTypeHandler(replyMsg->sender, context, type, AJ_OK);
        }

        break;

    case TIME_ALARM1_SCHEDULE_PROXY_PROP:
        {
            TS_Time time;
            int8_t weekdaysBitmap;

            status = AJ_UnmarshalArgs(replyMsg, "v", "((yyyq)y)", &time.hour, &time.minute, &time.second, &time.milliseconds, &weekdaysBitmap);

            if (status != AJ_OK) {
                return appGetAlaramScheduleHandler(replyMsg->sender, context, NULL, 0, AJ_OK); // TODO: is this a good pattern ?
            }

            return appGetAlaramScheduleHandler(replyMsg->sender, context, &time, weekdaysBitmap, AJ_OK);
        }

        break;

    case TIME_ALARM1_TITLE_PROXY_PROP:
        {
            char*title;

            status = AJ_UnmarshalArgs(replyMsg, "v", "s", &title);

            if (status != AJ_OK) {
                return appGetAlaramTitleHandler(replyMsg->sender, context, NULL, AJ_OK); // TODO: is this a good pattern ?
            }

            return appGetAlaramTitleHandler(replyMsg->sender, context, title, AJ_OK);
        }
        break;

    case TIME_ALARM1_ENABLED_PROXY_PROP:
        {
            uint32_t enabled;
            status = AJ_UnmarshalArgs(replyMsg, "v", "b", &enabled);
            if (status != AJ_OK) {
                return appGetAlaramEnabledHandler(replyMsg->sender, context, -1, status);
            }

            return appGetAlaramEnabledHandler(replyMsg->sender, context, enabled, AJ_OK);
        }

        break;

    case TIME_TIMER1_INTERVAL_PROXY_PROP:
        {
            TS_Timer_Period interval;

            status = AJ_UnmarshalArgs(replyMsg, "v", "(uyyq)", &interval.hour, &interval.minute, &interval.second, &interval.milliseconds);

            if (status != AJ_OK) {
                return appGetTimerIntervalHandler(replyMsg->sender, context, NULL, AJ_OK); // TODO: is this a good pattern ?
            }

            return appGetTimerIntervalHandler(replyMsg->sender, context, &interval, AJ_OK);
        }
        break;

    case TIME_TIMER1_TIMELEFT_PROXY_PROP:
        {
            TS_Timer_Period interval;

            status = AJ_UnmarshalArgs(replyMsg, "v", "(uyyq)", &interval.hour, &interval.minute, &interval.second, &interval.milliseconds);

            if (status != AJ_OK) {
                return appGetTimerTimeLeftHandler(replyMsg->sender, context, NULL, AJ_OK); // TODO: is this a good pattern ?
            }

            return appGetTimerTimeLeftHandler(replyMsg->sender, context, &interval, AJ_OK);
        }
        break;

    case TIME_TIMER1_ISRUNNING_PROXY_PROP:
        {
            uint32_t enabled;
            status = AJ_UnmarshalArgs(replyMsg, "v", "b", &enabled);
            if (status != AJ_OK) {
                return appGetTimerIsRunningHandler(replyMsg->sender, context, -1, status);
            }

            return appGetTimerIsRunningHandler(replyMsg->sender, context, enabled, AJ_OK);
        }
        break;

    case TIME_TIMER1_REPEAT_PROXY_PROP:
        {
            uint16_t repeat;
            status = AJ_UnmarshalArgs(replyMsg, "v", "q", &repeat);
            if (status != AJ_OK) {
                return appGetTimerRepeatHandler(replyMsg->sender, context, -1, status);
            }

            return appGetTimerRepeatHandler(replyMsg->sender, context, repeat, AJ_OK);
        }
        break;

    case TIME_TIMER1_TITLE_PROXY_PROP:
        {
            char*title;

            status = AJ_UnmarshalArgs(replyMsg, "v", "s", &title);

            if (status != AJ_OK) {
                return appGetTimerTitleHandler(replyMsg->sender, context, NULL, AJ_OK); // TODO: is this a good pattern ?
            }

            return appGetTimerTitleHandler(replyMsg->sender, context, title, AJ_OK);
        }

        break;

    default:
        break;

    }

    return status;
}

AJ_Status AJTS_Client_PropSetHandler(AJ_Message* replyMsg, RequestContext* context)
{
    AJ_Status status = AJ_OK;

    switch (context->propId) {
    case TIME_CLOCK_PROXY_DATETIME_PROP:
    case TIME_CLOCK_PROXY_ISSET_PROP:
    case TIME_CLOCK_AUTHORITY_VERSION_PROXY_PROP:
    case TIME_CLOCK_AUTHORITY_DATETIME_PROXY_PROP:
    case TIME_CLOCK_AUTHORITY_ISSET_PROXY_PROP:
    case TIME_AUTHORITY_TYPE_PROXY_PROP:
    case TIME_ALARM1_SCHEDULE_PROXY_PROP:
    case TIME_ALARM1_TITLE_PROXY_PROP:
    case TIME_ALARM1_ENABLED_PROXY_PROP:
    case TIME_TIMER1_INTERVAL_PROXY_PROP:
    case TIME_TIMER1_TIMELEFT_PROXY_PROP:
    case TIME_TIMER1_ISRUNNING_PROXY_PROP:
    case TIME_TIMER1_REPEAT_PROXY_PROP:
    case TIME_TIMER1_TITLE_PROXY_PROP:
        status = appSetPropReplyHandler(replyMsg);
        break;

    default:
        break;
    }

    return status;
}

AJ_Status AJTS_Client_ReplyHandler(AJ_Message* replyMsg, RequestContext* context)
{
    AJ_Status status = AJ_OK;

    switch (replyMsg->msgId) {
    case AJ_REPLY_ID(TIME_CLOCK_PROXY_GET_PROP):
    case AJ_REPLY_ID(TIME_CLOCK_AUTHORITY_PROXY_GET_PROP):
    case AJ_REPLY_ID(TIME_ALARM1_GET_PROXY_PROP):
    case AJ_REPLY_ID(TIME_TIMER1_GET_PROXY_PROP):
        status = AJTS_Client_PropGetHandler(replyMsg, context);

        break;

    case AJ_REPLY_ID(TIME_CLOCK_PROXY_SET_PROP):
    case AJ_REPLY_ID(TIME_CLOCK_AUTHORITY_PROXY_SET_PROP):
    case AJ_REPLY_ID(TIME_ALARM1_SET_PROXY_PROP):
    case AJ_REPLY_ID(TIME_TIMER1_SET_PROXY_PROP):
        status = AJTS_Client_PropSetHandler(replyMsg, context);
        break;

    case AJ_REPLY_ID(TIME_TIMER1_PROXY_RESET):
        status = appTimerResetHandler(replyMsg->sender, context, AJ_OK);

        break;

    default:
        break;
    }

    return status;
}

