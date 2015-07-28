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
#include <alljoyn/time/TimeServiceCommon.h>
#include <alljoyn/services_common/ServicesCommon.h>
#include <alljoyn/time/TimeServiceServer.h>
#include <alljoyn/time/TimeServiceClient.h>
#include <aj_config.h>


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
uint8_t dbgAJTS = ER_DEBUG_AJTS || ER_DEBUG_AJSVCALL;
#endif

uint32_t currentSessionId = 0;

/*
 * Message identifiers for the method calls this service implements
 */

// Clock interface
#define TIME_CLOCK_GET_PROP                                         AJ_ENCODE_MESSAGE_ID(AJTS_OBJECT_LIST_INDEX, AJTS_CLOCK_OBJECT_INDEX, 0, AJ_PROP_GET)
#define TIME_CLOCK_SET_PROP                                         AJ_ENCODE_MESSAGE_ID(AJTS_OBJECT_LIST_INDEX, AJTS_CLOCK_OBJECT_INDEX, 0, AJ_PROP_SET)

#define TIME_CLOCK_VERSION_PROP                                     AJ_ENCODE_PROPERTY_ID(AJTS_OBJECT_LIST_INDEX, AJTS_CLOCK_OBJECT_INDEX, 1, 0) // note that these entries are in time authority as well since the AJTS_TimeAuthorityInterfaces include the clock interface as well
#define TIME_CLOCK_DATETIME_PROP                                    AJ_ENCODE_PROPERTY_ID(AJTS_OBJECT_LIST_INDEX, AJTS_CLOCK_OBJECT_INDEX, 1, 1)
#define TIME_CLOCK_ISSET_PROP                                       AJ_ENCODE_PROPERTY_ID(AJTS_OBJECT_LIST_INDEX, AJTS_CLOCK_OBJECT_INDEX, 1, 2)

// Clock with time authority interface

#define TIME_CLOCK_AUTHORITY_GET_PROP                               AJ_ENCODE_MESSAGE_ID(AJTS_OBJECT_LIST_INDEX, AJTS_CLOCK_AUTHORITY_OBJECT_INDEX, 0, AJ_PROP_GET)
#define TIME_CLOCK_AUTHORITY_SET_PROP                               AJ_ENCODE_MESSAGE_ID(AJTS_OBJECT_LIST_INDEX, AJTS_CLOCK_AUTHORITY_OBJECT_INDEX, 0, AJ_PROP_SET)

#define TIME_CLOCK_AUTHORITY_VERSION_PROP                           AJ_ENCODE_PROPERTY_ID(AJTS_OBJECT_LIST_INDEX, AJTS_CLOCK_AUTHORITY_OBJECT_INDEX, 1, 0) // see above comment
#define TIME_CLOCK_AUTHORITY_DATETIME_PROP                          AJ_ENCODE_PROPERTY_ID(AJTS_OBJECT_LIST_INDEX, AJTS_CLOCK_AUTHORITY_OBJECT_INDEX, 1, 1)
#define TIME_CLOCK_AUTHORITY_ISSET_PROP                             AJ_ENCODE_PROPERTY_ID(AJTS_OBJECT_LIST_INDEX, AJTS_CLOCK_AUTHORITY_OBJECT_INDEX, 1, 2)

#define TIME_AUTHORITY_VERSION_PROP                                 AJ_ENCODE_PROPERTY_ID(AJTS_OBJECT_LIST_INDEX, AJTS_CLOCK_AUTHORITY_OBJECT_INDEX, 2, 0) // see above comment
#define TIME_AUTHORITY_TYPE_PROP                                    AJ_ENCODE_PROPERTY_ID(AJTS_OBJECT_LIST_INDEX, AJTS_CLOCK_AUTHORITY_OBJECT_INDEX, 2, 1)
#define TIME_AUTHORITY_TIMESYNC_SIGNAL                              AJ_ENCODE_MESSAGE_ID(AJTS_OBJECT_LIST_INDEX, AJTS_CLOCK_AUTHORITY_OBJECT_INDEX, 2, 2)

// Alarm interface

#define TIME_ALARM1_GET_PROP                                        AJ_ENCODE_MESSAGE_ID(AJTS_OBJECT_LIST_INDEX, AJTS_ALARM_OBJECT_INDEX + 0, 0, AJ_PROP_GET)
#define TIME_ALARM1_SET_PROP                                        AJ_ENCODE_MESSAGE_ID(AJTS_OBJECT_LIST_INDEX, AJTS_ALARM_OBJECT_INDEX + 0, 0, AJ_PROP_SET)

#define TIME_ALARM1_VERSION_PROP                                    AJ_ENCODE_PROPERTY_ID(AJTS_OBJECT_LIST_INDEX, AJTS_ALARM_OBJECT_INDEX + 0, 1, 0) // note that these entries are in time authority as well since the AJTS_TimeAuthorityInterfaces include the clock interface as well
#define TIME_ALARM1_SCHEDULE_PROP                                   AJ_ENCODE_PROPERTY_ID(AJTS_OBJECT_LIST_INDEX, AJTS_ALARM_OBJECT_INDEX + 0, 1, 1)
#define TIME_ALARM1_TITLE_PROP                                      AJ_ENCODE_PROPERTY_ID(AJTS_OBJECT_LIST_INDEX, AJTS_ALARM_OBJECT_INDEX + 0, 1, 2)
#define TIME_ALARM1_ENABLED_PROP                                    AJ_ENCODE_PROPERTY_ID(AJTS_OBJECT_LIST_INDEX, AJTS_ALARM_OBJECT_INDEX + 0, 1, 3)

#define TIME_ALARM1_ALARMREACHED_SIGNAL                             AJ_ENCODE_MESSAGE_ID(AJTS_OBJECT_LIST_INDEX, AJTS_ALARM_OBJECT_INDEX + 0, 1, 4)

// Timer interface

#define TIME_TIMER1_GET_PROP                                        AJ_ENCODE_MESSAGE_ID(AJTS_OBJECT_LIST_INDEX, AJTS_TIMER_OBJECT_INDEX + 0, 0, AJ_PROP_GET)
#define TIME_TIMER1_SET_PROP                                        AJ_ENCODE_MESSAGE_ID(AJTS_OBJECT_LIST_INDEX, AJTS_TIMER_OBJECT_INDEX + 0, 0, AJ_PROP_SET)

#define TIME_TIMER1_VERSION_PROP                                    AJ_ENCODE_PROPERTY_ID(AJTS_OBJECT_LIST_INDEX, AJTS_TIMER_OBJECT_INDEX + 0, 1, 0) // note that these entries are in time authority as well since the AJTS_TimeAuthorityInterfaces include the clock interface as well
#define TIME_TIMER1_INTERVAL_PROP                                   AJ_ENCODE_PROPERTY_ID(AJTS_OBJECT_LIST_INDEX, AJTS_TIMER_OBJECT_INDEX + 0, 1, 1)
#define TIME_TIMER1_TIMELEFT_PROP                                   AJ_ENCODE_PROPERTY_ID(AJTS_OBJECT_LIST_INDEX, AJTS_TIMER_OBJECT_INDEX + 0, 1, 2)

#define TIME_TIMER1_START                                           AJ_ENCODE_MESSAGE_ID(AJTS_OBJECT_LIST_INDEX, AJTS_TIMER_OBJECT_INDEX + 0, 1, 3)
#define TIME_TIMER1_PAUSE                                           AJ_ENCODE_MESSAGE_ID(AJTS_OBJECT_LIST_INDEX, AJTS_TIMER_OBJECT_INDEX + 0, 1, 4)
#define TIME_TIMER1_RESET                                           AJ_ENCODE_MESSAGE_ID(AJTS_OBJECT_LIST_INDEX, AJTS_TIMER_OBJECT_INDEX + 0, 1, 5)

#define TIME_TIMER1_ISRUNNING_PROP                                  AJ_ENCODE_PROPERTY_ID(AJTS_OBJECT_LIST_INDEX, AJTS_TIMER_OBJECT_INDEX + 0, 1, 6) // note that these entries are in time authority as well since the AJTS_TimeAuthorityInterfaces include the clock interface as well
#define TIME_TIMER1_REPEAT_PROP                                     AJ_ENCODE_PROPERTY_ID(AJTS_OBJECT_LIST_INDEX, AJTS_TIMER_OBJECT_INDEX + 0, 1, 7)
#define TIME_TIMER1_TITLE_PROP                                      AJ_ENCODE_PROPERTY_ID(AJTS_OBJECT_LIST_INDEX, AJTS_TIMER_OBJECT_INDEX + 0, 1, 8)

#define TIME_TIMER1_TIMEREVENT_SIGNAL                               AJ_ENCODE_MESSAGE_ID(AJTS_OBJECT_LIST_INDEX, AJTS_TIMER_OBJECT_INDEX + 0, 1, 9)
#define TIME_TIMER1_RUNSTATECHANGED_SIGNAL                          AJ_ENCODE_MESSAGE_ID(AJTS_OBJECT_LIST_INDEX, AJTS_TIMER_OBJECT_INDEX + 0, 1, 10)

#define TIME_CLOCK_OBJECT_DESC                                      AJ_DESC_ID_FROM_OBJ_INDEX(AJTS_CLOCK_OBJECT_INDEX)
#define TIME_AUTHORITY_OBJECT_DESC                                  AJ_DESC_ID_FROM_OBJ_INDEX(AJTS_CLOCK_AUTHORITY_OBJECT_INDEX)
#define TIME_AUTHORITY_TIMESYNC_SIGNAL_DESC                         AJ_DESC_ID_FROM_MSG_ID(TIME_AUTHORITY_TIMESYNC_SIGNAL, 0)
#define TIME_ALARM1_OBJECT_DESC                                     AJ_DESC_ID_FROM_OBJ_INDEX(AJTS_ALARM_OBJECT_INDEX + 0)
#define TIME_ALARM1_ALARMREACHED_DESC                               AJ_DESC_ID_FROM_MSG_ID(TIME_ALARM1_ALARMREACHED_SIGNAL, 0)
#define TIME_TIMER1_OBJECT_DESC                                     AJ_DESC_ID_FROM_OBJ_INDEX(AJTS_TIMER_OBJECT_INDEX + 0)
#define TIME_TIMER1_TIMEREVENT_SIGNAL_DESC                          AJ_DESC_ID_FROM_MSG_ID(TIME_TIMER1_TIMEREVENT_SIGNAL, 0)
#define TIME_TIMER1_RUNSTATECHANGED_SIGNAL_DESC                     AJ_DESC_ID_FROM_MSG_ID(TIME_TIMER1_RUNSTATECHANGED_SIGNAL, 0)

static const char* DescriptionLookup(uint32_t descId, const char* lang)
{
    switch (descId) {
    case  (TIME_CLOCK_OBJECT_DESC):
        return "Clock Service";
        break;

    case  (TIME_AUTHORITY_OBJECT_DESC):
        return "Time Authority Service";
        break;

    case (TIME_AUTHORITY_TIMESYNC_SIGNAL_DESC):
        return "Time Authority Sync Signal";
        break;

    case  (TIME_ALARM1_OBJECT_DESC):
        return "Alarm1 Service";
        break;

    case  (TIME_TIMER1_OBJECT_DESC):
        return "Timer1 Service";
        break;

    case (TIME_ALARM1_ALARMREACHED_DESC):
        return "Alarm Reached Signal";
        break;

    case (TIME_TIMER1_TIMEREVENT_SIGNAL_DESC):
        return "Timer Event Signal";
        break;

    case (TIME_TIMER1_RUNSTATECHANGED_SIGNAL_DESC):
        return "Timer Run State Changed Signal";
        break;

    default:
        AJ_AlwaysPrintf(("0x%x \n", descId));
        // snprintf(description, DESCRIPTION_LENGTH, "default 0x%x",descId);

        return NULL;

        break;
    }
    return NULL;
}


static AJ_Status RegisterObjectList()
{
    uint8_t index = 0;

    for (index = AJTS_CLOCK_OBJECT_INDEX; index < (AJTS_CLOCK_OBJECT_INDEX + AJTS_NUM_OF_CLOCKS); index++) {
        AJTS_ObjectList[index].flags &= ~(AJ_OBJ_FLAG_HIDDEN | AJ_OBJ_FLAG_DISABLED);
        AJTS_ObjectList[index].flags |= AJ_OBJ_FLAG_DESCRIBED;

                #ifdef _WIN32
        _snprintf((char*)AJTS_ObjectList[index].path, AJTS_MAX_OBJ_PATH_LENGTH, "/Clock%d", index);
                #else
        snprintf((char*)AJTS_ObjectList[index].path, AJTS_MAX_OBJ_PATH_LENGTH, "/Clock%d", index);
                #endif
    }

    for (index = AJTS_ALARM_OBJECT_INDEX; index < (AJTS_ALARM_OBJECT_INDEX + AJTS_NUM_OF_ALARMS); index++) {
        AJTS_ObjectList[index].flags &= ~(AJ_OBJ_FLAG_HIDDEN | AJ_OBJ_FLAG_DISABLED);
        AJTS_ObjectList[index].flags |= AJ_OBJ_FLAG_DESCRIBED;

                #ifdef _WIN32
        _snprintf((char*)AJTS_ObjectList[index].path, AJTS_MAX_OBJ_PATH_LENGTH, "/Alarm%d", index);
                #else
        snprintf((char*)AJTS_ObjectList[index].path, AJTS_MAX_OBJ_PATH_LENGTH, "/Alarm%d", index);
                #endif
    }

    for (index = AJTS_TIMER_OBJECT_INDEX; index < (AJTS_TIMER_OBJECT_INDEX + AJTS_NUM_OF_TIMERS); index++) {
        AJTS_ObjectList[index].flags &= ~(AJ_OBJ_FLAG_HIDDEN | AJ_OBJ_FLAG_DISABLED);
        AJTS_ObjectList[index].flags |= AJ_OBJ_FLAG_DESCRIBED;

                #ifdef _WIN32
        _snprintf((char*)AJTS_ObjectList[index].path, AJTS_MAX_OBJ_PATH_LENGTH, "/Timer%d", index);
                #else
        snprintf((char*)AJTS_ObjectList[index].path, AJTS_MAX_OBJ_PATH_LENGTH, "/Timer%d", index);
                #endif
    }

    return AJ_RegisterObjectListWithDescriptions(AJTS_ObjectList, AJTS_OBJECT_LIST_INDEX, DescriptionLookup);
}

/*
 * Application registered Callbacks
 */

static AJTS_GetDateTime AppGetDateTime      = NULL;
static AJTS_SetDateTime AppSetDateTime      = NULL;
static AJTS_IsSet AppIsSet            = NULL;

static AJTS_GetAuthorityType AppGetAuthorityType = NULL;

static AJTS_GetAlaramSchedule AppGetAlaramSchedule = NULL;
static AJTS_SetAlaramSchedule AppSetAlaramSchedule = NULL;
static AJTS_GetAlaramTitle AppGetAlaramTitle   = NULL;
static AJTS_SetAlaramTitle AppSetAlaramTitle   = NULL;
static AJTS_GetAlaramEnabled AppGetAlaramEnabled = NULL;
static AJTS_SetAlaramEnabled AppSetAlaramEnabled = NULL;

static AJTS_GetTimerInterval AppGetTimerInterval = NULL;
static AJTS_SetTimerInterval AppSetTimerInterval = NULL;
static AJTS_GetTimerTimeLeft AppGetTimerTimeLeft = NULL;
static AJTS_GetTimerIsRunning AppGetTimerIsRunning = NULL;
static AJTS_GetTimerRepeat AppGetTimerRepeat   = NULL;
static AJTS_SetTimerRepeat AppSetTimerRepeat   = NULL;
static AJTS_GetTimerTitle AppGetTimerTitle    = NULL;
static AJTS_SetTimerTitle AppSetTimerTitle    = NULL;
static AJTS_TimerStart AppTimerStart       = NULL;
static AJTS_TimerPause AppTimerPause       = NULL;
static AJTS_TimerReset AppTimerReset       = NULL;

AJ_Status AJTS_Server_Start(AJTS_GetDateTime getDateTime, AJTS_SetDateTime setDateTime, AJTS_IsSet isSet,
                            AJTS_GetAuthorityType getAuthorityType,
                            AJTS_GetAlaramSchedule getAlaramSchedule, AJTS_SetAlaramSchedule setAlaramSchedule, AJTS_GetAlaramTitle getAlaramTitle,  AJTS_SetAlaramTitle setAlaramTitle, AJTS_GetAlaramEnabled getAlaramEnabled, AJTS_SetAlaramEnabled setAlaramEnabled,
                            AJTS_GetTimerInterval getTimerInterval, AJTS_SetTimerInterval setTimerInterval, AJTS_GetTimerTimeLeft getTimerTimeLeft, AJTS_GetTimerIsRunning getTimerIsRunning, AJTS_GetTimerRepeat getTimerRepeat, AJTS_SetTimerRepeat setTimerRepeat, AJTS_GetTimerTitle getTimerTitle, AJTS_SetTimerTitle setTimerTitle,
                            AJTS_TimerStart timerStart, AJTS_TimerPause timerPause, AJTS_TimerReset timerReset)
{
    AJ_Status status = AJ_OK;

    AppGetDateTime      = getDateTime;
    AppSetDateTime      = setDateTime;
    AppIsSet            = isSet;

    AppGetAuthorityType = getAuthorityType;

    AppGetAlaramSchedule = getAlaramSchedule;
    AppSetAlaramSchedule = setAlaramSchedule;
    AppGetAlaramTitle   = getAlaramTitle;
    AppSetAlaramTitle   = setAlaramTitle;
    AppGetAlaramEnabled = getAlaramEnabled;
    AppSetAlaramEnabled = setAlaramEnabled;

    AppGetTimerInterval = getTimerInterval;
    AppSetTimerInterval = setTimerInterval;
    AppGetTimerTimeLeft = getTimerTimeLeft;
    AppGetTimerIsRunning = getTimerIsRunning;
    AppGetTimerRepeat   = getTimerRepeat;
    AppSetTimerRepeat   = setTimerRepeat;
    AppGetTimerTitle    = getTimerTitle;
    AppSetTimerTitle    = setTimerTitle;
    AppTimerStart       = timerStart;
    AppTimerPause       = timerPause;
    AppTimerReset       = timerReset;

    status = RegisterObjectList();

    return status;
}

AJ_Status AJTS_Server_ConnectedHandler(AJ_BusAttachment* busAttachment)
{

    AJ_Status status = AJ_OK;
    AJ_SessionOpts sessionOpts = {
        AJ_SESSION_TRAFFIC_MESSAGES,
        AJ_SESSION_PROXIMITY_ANY,
        AJ_TRANSPORT_ANY,
        TRUE
    };
    uint8_t serviceStarted;
    AJ_Message msg;

    status = AJ_BusBindSessionPort(busAttachment, AJTS_Port, &sessionOpts, 0);
    if (status != AJ_OK) {
        AJ_ErrPrintf(("Failed to send bind session port message\n"));
    }

    serviceStarted = FALSE;
    while (!serviceStarted && (status == AJ_OK)) {

        status = AJ_UnmarshalMsg(busAttachment, &msg, AJ_UNMARSHAL_TIMEOUT);
        if (status == AJ_ERR_NO_MATCH) {
            status = AJ_OK;
            continue;
        } else if (status != AJ_OK) {
            break;
        }

        switch (msg.msgId) {
        case AJ_REPLY_ID(AJ_METHOD_BIND_SESSION_PORT):
            if (msg.hdr->msgType == AJ_MSG_ERROR) {
                status = AJ_ERR_FAILURE;
            } else {
                serviceStarted = TRUE;
            }
            break;

        default:
            /*
             * Pass to the built-in bus message handlers
             */
            status = AJ_BusHandleBusMessage(&msg);
            break;
        }
        AJ_CloseMsg(&msg);
    }

    if (status != AJ_OK) {
        AJ_ErrPrintf(("AllJoyn disconnect bus status=%d\n", status));
        status = AJ_ERR_READ;
    }
    return status;

}

AJ_Status AJTS_Server_PropGetHandler(AJ_Message* replyMsg, uint32_t propId, void* context)
{
    AJ_Status status = AJ_OK;

    uint8_t objectIndex = (uint8_t)(propId >> 16);

    switch (propId) {
    case TIME_CLOCK_VERSION_PROP:
    case TIME_CLOCK_AUTHORITY_VERSION_PROP:
    case TIME_AUTHORITY_VERSION_PROP:
    case TIME_ALARM1_VERSION_PROP:
    case TIME_TIMER1_VERSION_PROP:

        return AJ_MarshalArgs(replyMsg, "q", AJTS_Version);

        break;

    case TIME_CLOCK_DATETIME_PROP:
    case TIME_CLOCK_AUTHORITY_DATETIME_PROP:
        {
            TS_Date date;
            TS_Time time;
            int16_t utcOffset;
            AJ_Arg arg_struct;

            status = AppGetDateTime(objectIndex, &date, &time, &utcOffset);
            if (status != AJ_OK) {
                AJ_ErrPrintf(("AppGetDateTime returned '%s'", AJ_StatusText(status)));
                goto Exit;
            }


            status = AJ_MarshalContainer(replyMsg, &arg_struct, AJ_ARG_STRUCT);
            if (status != AJ_OK) {
                goto Exit;
            }

            status = AJ_MarshalArgs(replyMsg, "(qyy)", date.year, date.month, date.day);
            if (status != AJ_OK) {
                goto Exit;
            }
            status = AJ_MarshalArgs(replyMsg, "(yyyq)", time.hour, time.minute, time.second, time.milliseconds);
            if (status != AJ_OK) {
                goto Exit;
            }
            status = AJ_MarshalArgs(replyMsg, "n", utcOffset);
            if (status != AJ_OK) {
                goto Exit;
            }

            return AJ_MarshalCloseContainer(replyMsg, &arg_struct);
        }
        break;

    case TIME_CLOCK_ISSET_PROP:
    case TIME_CLOCK_AUTHORITY_ISSET_PROP:
        {
            uint8_t isSet;

            status = AppIsSet(objectIndex, &isSet);

            if (status != AJ_OK) {
                AJ_ErrPrintf(("AppIsSet returned '%s'", AJ_StatusText(status)));
                goto Exit;
            }

            return AJ_MarshalArgs(replyMsg, "b", isSet);
        }
        break;

    case TIME_AUTHORITY_TYPE_PROP:
        {
            uint8_t authorityType;

            status = AppGetAuthorityType(objectIndex, &authorityType);

            if (status != AJ_OK) {
                AJ_ErrPrintf(("AppGetAuthorityType returned '%s'", AJ_StatusText(status)));
                goto Exit;
            }

            return AJ_MarshalArgs(replyMsg, "y", authorityType);

        }
        break;

    case TIME_ALARM1_SCHEDULE_PROP:
        {
            TS_Time time;
            uint8_t weekdaysBitmap;
            AJ_Arg arg_struct;

            status = AppGetAlaramSchedule(objectIndex, &time, &weekdaysBitmap);
            if (status != AJ_OK) {
                AJ_ErrPrintf(("AppGetAlaramSchedule returned '%s'", AJ_StatusText(status)));
                goto Exit;
            }

            status = AJ_MarshalContainer(replyMsg, &arg_struct, AJ_ARG_STRUCT);
            if (status != AJ_OK) {
                goto Exit;
            }

            status = AJ_MarshalArgs(replyMsg, "(yyyq)", time.hour, time.minute, time.second, time.milliseconds);
            if (status != AJ_OK) {
                goto Exit;
            }
            status = AJ_MarshalArgs(replyMsg, "y", weekdaysBitmap);
            if (status != AJ_OK) {
                goto Exit;
            }

            return AJ_MarshalCloseContainer(replyMsg, &arg_struct);
        }
        break;

    case TIME_ALARM1_TITLE_PROP:
        {
            char title[AJTS_MAX_TITLE_LENGTH + 1];

            status = AppGetAlaramTitle(objectIndex, title);
            if (status != AJ_OK) {
                AJ_ErrPrintf(("AppGetAlaramTitle returned '%s'", AJ_StatusText(status)));
                goto Exit;
            }

            return AJ_MarshalArgs(replyMsg, "s", title);
        }
        break;

    case TIME_ALARM1_ENABLED_PROP:
        {
            uint8_t isEnabled;

            status = AppGetAlaramEnabled(objectIndex, &isEnabled);

            if (status != AJ_OK) {
                AJ_ErrPrintf(("AppGetAlaramEnabled returned '%s'", AJ_StatusText(status)));
                goto Exit;
            }

            return AJ_MarshalArgs(replyMsg, "b", isEnabled);
        }
        break;

    case TIME_TIMER1_INTERVAL_PROP:
        {
            TS_Timer_Period interval;

            status = AppGetTimerInterval(objectIndex, &interval);
            if (status != AJ_OK) {
                AJ_ErrPrintf(("AppGetTimerInterval returned '%s'", AJ_StatusText(status)));
                goto Exit;
            }

            return AJ_MarshalArgs(replyMsg, "(uyyq)", interval.hour, interval.minute, interval.second, interval.milliseconds);

        }
        break;

    case TIME_TIMER1_TIMELEFT_PROP:
        {
            TS_Timer_Period timeLeft;

            status = AppGetTimerTimeLeft(objectIndex, &timeLeft);
            if (status != AJ_OK) {
                AJ_ErrPrintf(("AppGetTimerTimeLeft returned '%s'", AJ_StatusText(status)));
                goto Exit;
            }

            return AJ_MarshalArgs(replyMsg, "(uyyq)", timeLeft.hour, timeLeft.minute, timeLeft.second, timeLeft.milliseconds);


        }
        break;

    case TIME_TIMER1_ISRUNNING_PROP:
        {
            uint8_t isRunning;

            status = AppGetTimerIsRunning(objectIndex, &isRunning);

            if (status != AJ_OK) {
                AJ_ErrPrintf(("AppGetTimerIsRunning returned '%s'", AJ_StatusText(status)));
                goto Exit;
            }

            return AJ_MarshalArgs(replyMsg, "b", isRunning);

        }
        break;

    case TIME_TIMER1_REPEAT_PROP:
        {
            uint16_t repeat;

            status = AppGetTimerRepeat(objectIndex, &repeat);

            if (status != AJ_OK) {
                AJ_ErrPrintf(("AppGetTimerRepeat returned '%s'", AJ_StatusText(status)));
                goto Exit;
            }

            return AJ_MarshalArgs(replyMsg, "q", repeat);

        }
        break;

    case TIME_TIMER1_TITLE_PROP:
        {
            char title[AJTS_MAX_TITLE_LENGTH + 1];

            status = AppGetTimerTitle(objectIndex, title);
            if (status != AJ_OK) {
                AJ_ErrPrintf(("AppGetTimerTitle returned '%s'", AJ_StatusText(status)));
                goto Exit;
            }

            return AJ_MarshalArgs(replyMsg, "s", title);

        }
        break;

    default:
        return AJ_ERR_UNEXPECTED;
        break;
    }

Exit:
    return status;
}

AJ_Status AJTS_Server_PropSetHandler(AJ_Message* replyMsg, uint32_t propId, void* context)
{
    AJ_Status status = AJ_OK;

    uint8_t objectIndex = (uint8_t)(propId >> 16);

    switch (propId) {
    case TIME_CLOCK_DATETIME_PROP:
    case TIME_CLOCK_AUTHORITY_DATETIME_PROP:
        {
            TS_Date date;
            TS_Time time;
            int16_t utcOffset;
            AJ_Arg structArg;

            status = AJ_UnmarshalContainer(replyMsg, &structArg, AJ_ARG_STRUCT);
            if (status != AJ_OK) {
                return status;
            }

            status = AJ_UnmarshalArgs(replyMsg, "(qyy)", &date.year, &date.month, &date.day);
            if (status != AJ_OK) {
                goto Exit;
            }

            status = AJ_UnmarshalArgs(replyMsg, "(yyyq)", &time.hour, &time.minute, &time.second, &time.milliseconds);
            if (status != AJ_OK) {
                goto Exit;
            }

            status = AJ_UnmarshalArgs(replyMsg, "n", &utcOffset);
            if (status != AJ_OK) {
                goto Exit;
            }

            status = AJ_UnmarshalCloseContainer(replyMsg, &structArg);
            if (status != AJ_OK) {
                goto Exit;
            }

            status = AppSetDateTime(objectIndex, &date, &time, &utcOffset);

            if (status != AJ_OK) {
                AJ_ErrPrintf(("AppSetDateTime returned '%s'", AJ_StatusText(status)));
                goto Exit;
            }

        }
        break;


    case TIME_ALARM1_SCHEDULE_PROP:
        {
            TS_Time time;
            uint8_t weekdaysBitmap;
            AJ_Arg structArg;

            status = AJ_UnmarshalContainer(replyMsg, &structArg, AJ_ARG_STRUCT);
            if (status != AJ_OK) {
                return status;
            }

            status = AJ_UnmarshalArgs(replyMsg, "(yyyq)", &time.hour, &time.minute, &time.second, &time.milliseconds);

            if (status != AJ_OK) {
                goto Exit;
            }

            status = AJ_UnmarshalArgs(replyMsg, "y", &weekdaysBitmap);

            if (status != AJ_OK) {
                goto Exit;
            }

            status = AJ_UnmarshalCloseContainer(replyMsg, &structArg);
            if (status != AJ_OK) {
                goto Exit;
            }

            status = AppSetAlaramSchedule(objectIndex, &time, weekdaysBitmap);
            if (status != AJ_OK) {
                AJ_ErrPrintf(("AppSetAlaramSchedule returned '%s'", AJ_StatusText(status)));
                goto Exit;
            }

        }
        break;

    case TIME_ALARM1_TITLE_PROP:
        {
            char*title;

            status = AJ_UnmarshalArgs(replyMsg, "s", &title);
            if (status != AJ_OK) {
                goto Exit;
            }

            status = AppSetAlaramTitle(objectIndex, title); // note that title is stored locally , so the callback can't just take the pointer and run with it..
            if (status != AJ_OK) {
                AJ_ErrPrintf(("AppSetAlaramTitle returned '%s'", AJ_StatusText(status)));
                goto Exit;
            }
        }
        break;

    case TIME_ALARM1_ENABLED_PROP:
        {
            uint32_t isEnabled;

            status = AJ_UnmarshalArgs(replyMsg, "b", &isEnabled);
            if (status != AJ_OK) {
                goto Exit;
            }

            status = AppSetAlaramEnabled(objectIndex, isEnabled);

            if (status != AJ_OK) {
                AJ_ErrPrintf(("AppSetAlaramEnabled returned '%s'", AJ_StatusText(status)));
                goto Exit;
            }
        }
        break;

    case TIME_TIMER1_INTERVAL_PROP:
        {
            TS_Timer_Period interval;

            status = AJ_UnmarshalArgs(replyMsg, "(uyyq)", &interval.hour, &interval.minute, &interval.second, &interval.milliseconds);
            if (status != AJ_OK) {
                goto Exit;
            }

            status = AppSetTimerInterval(objectIndex, &interval);
            if (status != AJ_OK) {
                AJ_ErrPrintf(("AppSetTimerInterval returned '%s'", AJ_StatusText(status)));
                goto Exit;
            }
        }
        break;

    case TIME_TIMER1_REPEAT_PROP:
        {
            uint16_t repeat;

            status = AJ_UnmarshalArgs(replyMsg, "q", &repeat);
            if (status != AJ_OK) {
                goto Exit;
            }

            status = AppSetTimerRepeat(objectIndex, &repeat);

            if (status != AJ_OK) {
                AJ_ErrPrintf(("AppSetTimerRepeat returned '%s'", AJ_StatusText(status)));
                goto Exit;
            }
        }
        break;

    case TIME_TIMER1_TITLE_PROP:
        {
            char*title;

            status = AJ_UnmarshalArgs(replyMsg, "s", &title);
            if (status != AJ_OK) {
                goto Exit;
            }

            status = AppSetTimerTitle(objectIndex, title);
            if (status != AJ_OK) {
                AJ_ErrPrintf(("AppSetTimerTitle returned '%s'", AJ_StatusText(status)));
                goto Exit;
            }
        }
        break;

    default:
        return AJ_ERR_UNEXPECTED;

        break;
    }

Exit:
    return status;
}

static AJ_Status TimerStartHandler(AJ_Message* msg)
{
    AJ_Status status = AJ_OK;

    uint8_t objectIndex = (uint8_t)(msg->msgId >> 16);

    status = AppTimerStart(objectIndex);

    if (status != AJ_OK) {
        AJ_ErrPrintf(("AJTS_Server_TimerStart returned '%s'", AJ_StatusText(status)));

        goto Exit;
    }

Exit:
    return status;
}

static AJ_Status TimerPauseHandler(AJ_Message* msg)
{
    AJ_Status status = AJ_OK;

    uint8_t objectIndex = (uint8_t)(msg->msgId >> 16);

    status = AppTimerPause(objectIndex);

    if (status != AJ_OK) {
        AJ_ErrPrintf(("AJTS_Server_TimerPause returned '%s'", AJ_StatusText(status)));

        goto Exit;
    }

Exit:
    return status;
}

static AJ_Status TimerResetHandler(AJ_Message* msg)
{
    AJ_Status status = AJ_OK;
    AJ_Message replyMsg;

    uint8_t objectIndex = (uint8_t)(msg->msgId >> 16);

    status = AppTimerReset(objectIndex);

    if (status != AJ_OK) {
        AJ_ErrPrintf(("AJTS_Server_TimerReset returned '%s'", AJ_StatusText(status)));

        status = AJ_MarshalStatusMsg(msg, &replyMsg,  status);
        goto Exit;
    }

    status = AJ_MarshalReplyMsg(msg, &replyMsg);

    if (status != AJ_OK) {
        AJ_ErrPrintf(("AJ_MarshalReplyMsg returned '%s'", AJ_StatusText(status)));
        goto Exit;
    }

    status = AJ_DeliverMsg(&replyMsg);
    if (status != AJ_OK) {
        AJ_ErrPrintf(("AJ_DeliverMsg returned '%s'", AJ_StatusText(status)));
        goto Exit;
    }

Exit:
    return status;
}


AJSVC_ServiceStatus AJTS_Server_MessageProcessor(AJ_BusAttachment* bus, AJ_Message* msg, AJ_Status* msgStatus)
{
    AJSVC_ServiceStatus serviceStatus = AJSVC_SERVICE_STATUS_HANDLED;

    switch (msg->msgId) {

    case TIME_CLOCK_GET_PROP:
    case TIME_CLOCK_AUTHORITY_GET_PROP:
    case TIME_ALARM1_GET_PROP:
    case TIME_TIMER1_GET_PROP:
        *msgStatus = AJ_BusPropGet(msg, AJTS_Server_PropGetHandler, NULL);
        break;

    case TIME_CLOCK_SET_PROP:
    case TIME_CLOCK_AUTHORITY_SET_PROP:
    case TIME_ALARM1_SET_PROP:
    case TIME_TIMER1_SET_PROP:
        *msgStatus = AJ_BusPropSet(msg, AJTS_Server_PropSetHandler, NULL);
        break;

    case TIME_TIMER1_START:
        *msgStatus = TimerStartHandler(msg);
        break;

    case TIME_TIMER1_PAUSE:
        *msgStatus = TimerPauseHandler(msg);
        break;

    case TIME_TIMER1_RESET:
        *msgStatus = TimerResetHandler(msg);
        break;

    default:
        serviceStatus = AJSVC_SERVICE_STATUS_NOT_HANDLED;
        break;
    }

    return serviceStatus;
}

uint8_t AJTS_CheckSessionAccepted(uint16_t port, uint32_t sessionId, char* joiner)
{
    if (port != AJTS_Port) {
        return FALSE;
    }
    currentSessionId = sessionId;
    return TRUE;
}

AJ_Status AJTS_Server_DisconnectHandler(AJ_BusAttachment* busAttachment)
{
    AJ_Status status = AJ_OK;
    //    status = AJ_BusUnbindSession(busAttachment, AJTS_Port);
    //    if (status != AJ_OK) {
    //        AJ_ErrPrintf(("Failed to send unbind session port=%d\n", AJTS_Port));
    //    }
    return status;
}

// Time service server API
AJ_Status AJTS_Server_TimeAuthority_TimeSync(AJ_BusAttachment* busAttachment, uint8_t objectIndex)
{
    AJ_Status status = AJ_OK;
    AJ_Message msg;

    status = AJ_MarshalSignal(busAttachment, &msg, TIME_AUTHORITY_TIMESYNC_SIGNAL, NULL, 0, AJ_FLAG_SESSIONLESS, AJTS_TIME_AUTHORITY_TIMESYNC_SIGNAL_TTL);

    if (status != AJ_OK) {
        AJ_ErrPrintf(("AJ_MarshalSignal returned '%s'", AJ_StatusText(status)));
        goto Exit;
    }

    status = AJ_DeliverMsg(&msg);

    if (status != AJ_OK) {
        AJ_ErrPrintf(("AJ_DeliverMsg returned '%s'", AJ_StatusText(status)));
        goto Exit;
    }

Exit:
    return status;
}


AJ_Status AJTS_Server_AlarmReached(AJ_BusAttachment* busAttachment, uint8_t objectIndex)
{
    AJ_Status status = AJ_OK;
    AJ_Message msg;

    status = AJ_MarshalSignal(busAttachment, &msg, TIME_ALARM1_ALARMREACHED_SIGNAL, NULL, 0, AJ_FLAG_SESSIONLESS, AJTS_ALARM_AND_TIMER_SIGNAL_TTL);

    if (status != AJ_OK) {
        AJ_ErrPrintf(("AJ_MarshalSignal returned '%s'", AJ_StatusText(status)));
        goto Exit;
    }

    status = AJ_DeliverMsg(&msg);

    if (status != AJ_OK) {
        AJ_ErrPrintf(("AJ_DeliverMsg returned '%s'", AJ_StatusText(status)));
        goto Exit;
    }

Exit:
    return status;
}

AJ_Status AJTS_Server_TimerEvent(AJ_BusAttachment* busAttachment, uint8_t objectIndex)
{
    AJ_Status status = AJ_OK;
    AJ_Message msg;

    status = AJ_MarshalSignal(busAttachment, &msg, TIME_TIMER1_TIMEREVENT_SIGNAL, NULL, 0, AJ_FLAG_SESSIONLESS, AJTS_ALARM_AND_TIMER_SIGNAL_TTL);

    if (status != AJ_OK) {
        AJ_ErrPrintf(("AJ_MarshalSignal returned '%s'", AJ_StatusText(status)));
        goto Exit;
    }

    status = AJ_DeliverMsg(&msg);

    if (status != AJ_OK) {
        AJ_ErrPrintf(("AJ_DeliverMsg returned '%s'", AJ_StatusText(status)));
        goto Exit;
    }

Exit:
    return status;
}

AJ_Status AJTS_Server_Timer_RunStateChanged(AJ_BusAttachment* busAttachment, uint8_t objectIndex, uint8_t isRunning)
{
    AJ_Status status = AJ_OK;
    AJ_Message msg;

    status = AJ_MarshalSignal(busAttachment, &msg, TIME_TIMER1_RUNSTATECHANGED_SIGNAL, NULL, currentSessionId, 0, 0);

    if (status != AJ_OK) {
        AJ_ErrPrintf(("AJ_MarshalSignal returned '%s'", AJ_StatusText(status)));
        goto Exit;
    }

    status = AJ_MarshalArgs(&msg, "b", isRunning);
    if (status != AJ_OK) {
        AJ_ErrPrintf(("AJ_MarshalArgs returned '%s'", AJ_StatusText(status)));
        goto Exit;
    }

    status = AJ_DeliverMsg(&msg);

    if (status != AJ_OK) {
        AJ_ErrPrintf(("AJ_DeliverMsg returned '%s'", AJ_StatusText(status)));
        goto Exit;
    }

Exit:
    return status;
}