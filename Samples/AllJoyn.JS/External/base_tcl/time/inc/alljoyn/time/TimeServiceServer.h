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

#ifndef _TIMESERVICESERVER_H_
#define _TIMESERVICESERVER_H_

/** @defgroup TimeServiceServer Time Service Framework
 *
 *  @{
 */

#include <alljoyn.h>
#include <alljoyn/services_common/ServicesCommon.h>
#include <alljoyn/time/TimeServiceCommon.h>

/**
 * Published Time BusObjects and Interfaces.
 */

/**
 * Clock and Authority objects
 */

/** clock obj path */
extern char AJTS_OBJ_PATH_CLOCK1[AJTS_MAX_OBJ_PATH_LENGTH + 1];
/** time authority and clock obj path */
extern char AJTS_OBJ_PATH_CLOCK2[AJTS_MAX_OBJ_PATH_LENGTH + 1];

/** clocks (regular and time authority) object definitions */
#define AJTS_Clocks     { AJTS_OBJ_PATH_CLOCK1, AJTS_ClockInterfaces, AJ_OBJ_FLAG_HIDDEN | AJ_OBJ_FLAG_DISABLED }, \
    { AJTS_OBJ_PATH_CLOCK2, AJTS_TimeAuthorityInterfaces, AJ_OBJ_FLAG_HIDDEN | AJ_OBJ_FLAG_DISABLED }

/**
 * Alarm objects
 */
/** alarm obj path */
extern char AJTS_OBJ_PATH_ALARM1[AJTS_MAX_OBJ_PATH_LENGTH + 1];

/** alarms object definitions */
#define AJTS_Alarms { AJTS_OBJ_PATH_ALARM1, AJTS_AlarmInterfaces, AJ_OBJ_FLAG_HIDDEN | AJ_OBJ_FLAG_DISABLED }

/**
 * Timer objects
 */
/** timer obj path */
extern char AJTS_OBJ_PATH_TIMER1[AJTS_MAX_OBJ_PATH_LENGTH + 1];

/** timer object definitions */
#define AJTS_Timers    { AJTS_OBJ_PATH_TIMER1, AJTS_TimerInterfaces, AJ_OBJ_FLAG_HIDDEN | AJ_OBJ_FLAG_DISABLED }

/**
 * Following definitions are used by the application.
 */

/**
 * Actions to perform when GetDateTime is requested.
 * Performed by the application.
 * @param index index of alljoyn object in AJTS_ObjectList
 * @param date date information retreived
 * @param time time information retreived
 * @param utcOffset offset in minutes
 * @return status
 */
typedef AJ_Status (*AJTS_GetDateTime)(const uint8_t index, TS_Date* date, TS_Time* time, int16_t* utcOffset);

/**
 * Actions to perform when SetDateTime is requested.
 * Performed by the application.
 * @param index index of alljoyn object in AJTS_ObjectList
 * @param date date to set
 * @param time time to set
 * @param utcOffset offset in minutes to set
 * @return status
 */
typedef AJ_Status (*AJTS_SetDateTime)(const uint8_t index, const TS_Date* date, const TS_Time* time, const int16_t* utcOffset);

/**
 * Actions to perform when TimerStart is requested.
 * Performed by the application.
 * @param index index of alljoyn object in AJTS_ObjectList
 * @return status
 */
typedef AJ_Status (*AJTS_TimerStart)(const uint8_t index);

/**
 * Actions to perform when TimerPause is requested.
 * Performed by the application.
 * @param index index of alljoyn object in AJTS_ObjectList
 * @return status
 */
typedef AJ_Status (*AJTS_TimerPause)(const uint8_t index);

/**
 * Actions to perform when TimerReset is requested.
 * Performed by the application.
 * @param index index of alljoyn object in AJTS_ObjectList
 * @return status
 */
typedef AJ_Status (*AJTS_TimerReset)(const uint8_t index);

/**
 * Actions to perform when IsSet is requested.
 * Performed by the application.
 * @param index index of alljoyn object in AJTS_ObjectList
 * @param isSet has the clock been set since last reboot
 * @return status
 */
typedef AJ_Status (*AJTS_IsSet)(const uint8_t index, uint8_t*isSet);

/**
 * Actions to perform when GetAuthorityType is requested.
 * Performed by the application.
 * @param index index of alljoyn object in AJTS_ObjectList
 * @param authorityType authority type
 * @return status
 */
typedef AJ_Status (*AJTS_GetAuthorityType)(const uint8_t index, uint8_t*authorityType);

/**
 * Actions to perform when GetAlaramSchedule is requested.
 * Performed by the application.
 * @param index index of alljoyn object in AJTS_ObjectList
 * @param time time to populate
 * @param bitmap of weekdays
 * @return status
 */
typedef AJ_Status (*AJTS_GetAlaramSchedule)(const uint8_t index, TS_Time* time, uint8_t*weekdaysBitmap);

/**
 * Actions to perform when SetAlaramSchedule is requested.
 * Performed by the application.
 * @param index index of alljoyn object in AJTS_ObjectList
 * @param time time to set
 * @param bitmap of weekdays to set
 * @return status
 */
typedef AJ_Status (*AJTS_SetAlaramSchedule)(const uint8_t index, const TS_Time* time, const uint8_t weekdaysBitmap);

/**
 * Actions to perform when GetAlaramTitle is requested.
 * Performed by the application.
 * @param index index of alljoyn object in AJTS_ObjectList
 * @return status
 */
typedef AJ_Status (*AJTS_GetAlaramTitle)(const uint8_t index,  char title[AJTS_MAX_TITLE_LENGTH + 1]);

/**
 * Actions to perform when SetAlaramTitle is requested.
 * Performed by the application.
 * @param index index of alljoyn object in AJTS_ObjectList
 * @return status
 */
typedef AJ_Status (*AJTS_SetAlaramTitle)(const uint8_t index, const char title[AJTS_MAX_TITLE_LENGTH + 1]);

/**
 * Actions to perform when GetAlaramEnabled is requested.
 * Performed by the application.
 * @param index index of alljoyn object in AJTS_ObjectList
 * @return status
 */
typedef AJ_Status (*AJTS_GetAlaramEnabled)(const uint8_t index,  uint8_t*isEnabled);

/**
 * Actions to perform when SetAlaramEnabled is requested.
 * Performed by the application.
 * @param index index of alljoyn object in AJTS_ObjectList
 * @return status
 */
typedef AJ_Status (*AJTS_SetAlaramEnabled)(const uint8_t index, const uint8_t isEnabled);

/**
 * Actions to perform when GetTimerInterval is requested.
 * Performed by the application.
 * @param index index of alljoyn object in AJTS_ObjectList
 * @return status
 */
typedef AJ_Status (*AJTS_GetTimerInterval)(const uint8_t index,  TS_Timer_Period* interval);

/**
 * Actions to perform when SetTimerInterval is requested.
 * Performed by the application.
 * @param index index of alljoyn object in AJTS_ObjectList
 * @return status
 */
typedef AJ_Status (*AJTS_SetTimerInterval)(const uint8_t index, const TS_Timer_Period* time);

/**
 * Actions to perform when GetTimerTimeLeft is requested.
 * Performed by the application.
 * @param index index of alljoyn object in AJTS_ObjectList
 * @return status
 */
typedef AJ_Status (*AJTS_GetTimerTimeLeft)(const uint8_t index,  TS_Timer_Period* time);

/**
 * Actions to perform when GetTimerIsRunning is requested.
 * Performed by the application.
 * @param index index of alljoyn object in AJTS_ObjectList
 * @return status
 */
typedef AJ_Status (*AJTS_GetTimerIsRunning)(const uint8_t index,  uint8_t*isEnabled);

/**
 * Actions to perform when GetTimerRepeat is requested.
 * Performed by the application.
 * @param index index of alljoyn object in AJTS_ObjectList
 * @return status
 */
typedef AJ_Status (*AJTS_GetTimerRepeat)(const uint8_t index,  uint16_t*repeat);

/**
 * Actions to perform when SetTimerRepeat is requested.
 * Performed by the application.
 * @param index index of alljoyn object in AJTS_ObjectList
 * @return status
 */
typedef AJ_Status (*AJTS_SetTimerRepeat)(const uint8_t index, const uint16_t*repeat);

/**
 * Actions to perform when GetTimerTitle is requested.
 * Performed by the application.
 * @param index index of alljoyn object in AJTS_ObjectList
 * @return status
 */
typedef AJ_Status (*AJTS_GetTimerTitle)(const uint8_t index,  char title[AJTS_MAX_TITLE_LENGTH + 1]);

/**
 * Actions to perform when SetTimerTitle is requested.
 * Performed by the application.
 * @param index index of alljoyn object in AJTS_ObjectList
 * @return status
 */
typedef AJ_Status (*AJTS_SetTimerTitle)(const uint8_t index, const char title[AJTS_MAX_TITLE_LENGTH + 1]);


/**
 * Server Start function. Register all callbacks here
 * @param getDateTime
 * @param setDateTime
 * @param isSet
 * @param getAuthorityType
 * @param getAlaramSchedule
 * @param setAlaramSchedule
 * @param getAlaramTitle
 * @param setAlaramTitle
 * @param getAlaramEnabled
 * @param setAlaramEnabled
 * @param getTimerInterval
 * @param setTimerInterval
 * @param getTimerTimeLeft
 * @param getTimerIsRunning
 * @param getTimerRepeat
 * @param setTimerRepeat
 * @param getTimerTitle
 * @param setTimerTitle
 * @param timerStart
 * @param timerPause
 * @param timerReset
 * @return status
 */

AJ_Status AJTS_Server_Start(AJTS_GetDateTime getDateTime, AJTS_SetDateTime setDateTime, AJTS_IsSet isSet,
                            AJTS_GetAuthorityType getAuthorityType,
                            AJTS_GetAlaramSchedule getAlaramSchedule, AJTS_SetAlaramSchedule setAlaramSchedule, AJTS_GetAlaramTitle getAlaramTitle,  AJTS_SetAlaramTitle setAlaramTitle, AJTS_GetAlaramEnabled getAlaramEnabled, AJTS_SetAlaramEnabled setAlaramEnabled,
                            AJTS_GetTimerInterval getTimerInterval, AJTS_SetTimerInterval setTimerInterval, AJTS_GetTimerTimeLeft getTimerTimeLeft, AJTS_GetTimerIsRunning getTimerIsRunning, AJTS_GetTimerRepeat getTimerRepeat, AJTS_SetTimerRepeat setTimerRepeat, AJTS_GetTimerTitle getTimerTitle, AJTS_SetTimerTitle setTimerTitle,
                            AJTS_TimerStart timerStart, AJTS_TimerPause timerPause, AJTS_TimerReset timerReset);

/**
 * AJTS_Server_ConnectedHandler - entry point to be called after connection from ServicesHandlers.c
 * @param busAttachment
 * @return status
 */
AJ_Status AJTS_Server_ConnectedHandler(AJ_BusAttachment* busAttachment);

/**
 * AJTS_Server_MessageProcessor - message processor to be called on each message from ServicesHandlers.c
 * @param bus
 * @param msg message to evaluate
 * @param msgStatus
 * @return AJSVC_ServiceStatus
 */
AJSVC_ServiceStatus AJTS_Server_MessageProcessor(AJ_BusAttachment* bus, AJ_Message* msg, AJ_Status* msgStatus);

/**
 * AJTS_CheckSessionAccepted -  to be called from ServicesHandlers.c after a session is created
 * @param port
 * @param sessionId
 * @param joiner
 * @return status - boolean
 */
uint8_t AJTS_CheckSessionAccepted(uint16_t port, uint32_t sessionId, char* joiner);

/**
 * AJTS_Server_DisconnectHandler -  to be called from ServicesHandlers.c
 * @param busAttachment
 * @return status
 */
AJ_Status AJTS_Server_DisconnectHandler(AJ_BusAttachment* busAttachment);

#define AJTS_TIME_AUTHORITY_TIMESYNC_SIGNAL_TTL (1000 * 180) /**< 180 seconds */
#define AJTS_ALARM_AND_TIMER_SIGNAL_TTL         (1000 * 50) /**< seconds */

/**
 * Signal a time authority service with time sync
 * @param busAttachment
 * @param objectIndex the index of the time autority enabled object in AJTS_ObjectList
 * @return status
 */
AJ_Status AJTS_Server_TimeAuthority_TimeSync(AJ_BusAttachment* busAttachment, uint8_t objectIndex);

/**
 * Signal that an alarm has reached
 * @param busAttachment
 * @param objectIndex the index of the alarm enabled object in AJTS_ObjectList
 * @return status
 */
AJ_Status AJTS_Server_AlarmReached(AJ_BusAttachment* busAttachment, uint8_t objectIndex);

/**
 * Signal that a timer event occured
 * @param busAttachment
 * @param objectIndex the index of the alarm enabled object in AJTS_ObjectList
 * @return status
 */
AJ_Status AJTS_Server_TimerEvent(AJ_BusAttachment* busAttachment, uint8_t objectIndex);

/**
 * Signal that a timer run status have changed
 * @param busAttachment
 * @param objectIndex the index of the alarm enabled object in AJTS_ObjectList
 * @param isRunning true if the timer is running, false if it is not.
 * @return status
 */
AJ_Status AJTS_Server_Timer_RunStateChanged(AJ_BusAttachment* busAttachment, uint8_t objectIndex, uint8_t isRunning);

/** @} */ // End of group 'TimeServiceServer'
#endif //_TIMESERVICESERVER_H_
