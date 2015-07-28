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
#include <aj_util.h>
#include <alljoyn/time/TimeServiceCommon.h>
#include <alljoyn/services_common/ServicesCommon.h>
#include <alljoyn/time/TimeServiceServer.h>
#include <alljoyn/time/TimeServiceClient.h>

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

/**
 * Published Time BusObjects and Interfaces.
 */

const uint16_t AJTS_Version = 1;

/**
 * Clock and Authority interfaces
 */

const char* const AJTS_TimeAuthorityInterface[] = {
    AJTS_TIMEAUTHORITY_IFACE_NAME,
    "@Version>q",
    "@AuthorityType>y",
    "!&TimeSync",
    NULL
};

const char* const AJTS_TimeClockInterface[] = {
    MAKE_SECURE_IFACE_NAME(AJTS_CLOCK_IFACE_NAME),
    "@Version>q",
    "@DateTime=((qyy)(yyyq)n)",
    "@IsSet>b",
    NULL
};

const AJ_InterfaceDescription AJTS_TimeAuthorityInterfaces[] = {
    AJ_PropertiesIface,
    AJTS_TimeClockInterface,
    AJTS_TimeAuthorityInterface,
    NULL
};

const AJ_InterfaceDescription AJTS_ClockInterfaces[] = {
    AJ_PropertiesIface,
    AJTS_TimeClockInterface,
    NULL
};


/**
 * Alarm interfaces
 */


const char* const AJTS_AlarmInterface[] = {
    AJTS_ALARM_IFACE_NAME,
    "@Version>q",
    "@Schedule=((yyyq)y)",
    "@Title=s",
    "@Enabled=b",
    "!&AlarmReached", //TODO: later in the code add descriptions/consult HLD
    NULL
};

const AJ_InterfaceDescription AJTS_AlarmInterfaces[] = {
    AJ_PropertiesIface,
    AJTS_AlarmInterface,
    NULL
};


/**
 * Timer interfaces
 */

const char* const AJTS_TimerInterface[] = {
    AJTS_TIMER_IFACE_NAME,
    "@Version>q",
    "@Interval=(uyyq)",
    "@TimeLeft>(uyyq)",
    "?Start",   //org.freedesktop.DBus.Method.NoReply
    "?Pause",   //org.freedesktop.DBus.Method.NoReply
    "?Reset",
    "@IsRunning>b",
    "@Repeat=q",
    "@Title=s",
    "!&TimerEvent", //TODO: later in the code add descriptions/consult HLD
    "!RunStateChanged runState>b",
    NULL
};

const AJ_InterfaceDescription AJTS_TimerInterfaces[] = {
    AJ_PropertiesIface,
    AJTS_TimerInterface,
    NULL
};

/**
 * Clock and Authority objects
 */

char AJTS_OBJ_PATH_CLOCK1[AJTS_MAX_OBJ_PATH_LENGTH + 1];
char AJTS_OBJ_PATH_CLOCK2[AJTS_MAX_OBJ_PATH_LENGTH + 1];

/**
 * Alarm objects
 */

char AJTS_OBJ_PATH_ALARM1[AJTS_MAX_OBJ_PATH_LENGTH + 1];

/**
 * Timer objects
 */

char AJTS_OBJ_PATH_TIMER1[AJTS_MAX_OBJ_PATH_LENGTH + 1];

/**
 * Clock and Authority proxy objects
 */

char AJTS_OBJ_PATH_CLOCK_PROXY[AJTS_MAX_OBJ_PATH_LENGTH + 1] = "!";
char AJTS_OBJ_PATH_CLOCK_TIME_AUTHORITY_PROXY[AJTS_MAX_OBJ_PATH_LENGTH + 1] = "!";

/**
 * Alarm proxy object
 */

char AJTS_OBJ_PATH_ALARM_PROXY[AJTS_MAX_OBJ_PATH_LENGTH + 1] = "!";

/**
 * Timer proxy object
 */

char AJTS_OBJ_PATH_TIMER_PROXY[AJTS_MAX_OBJ_PATH_LENGTH + 1] = "!";

/*
 * Objects List
 */

AJ_Object AJTS_ObjectList[] = {
    AJTS_Clocks,
    AJTS_Alarms,
    AJTS_Timers,
    AJTS_Clock_Proxy,
    AJTS_TimeAuthority_Proxy,
    AJTS_Alarm_Proxy,
    AJTS_Timer_Proxy,
    { NULL }
};

























