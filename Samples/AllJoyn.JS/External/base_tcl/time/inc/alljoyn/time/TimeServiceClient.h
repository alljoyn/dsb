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

#ifndef _TIMESERVICECLIENT_H_
#define _TIMESERVICECLIENT_H_

/** @defgroup TimeServiceClient Time Service Framework
 *
 *  @{
 */

#include <alljoyn.h>
#include <alljoyn/services_common/ServicesCommon.h>
#include <alljoyn/time/TimeServiceCommon.h>

/**
 * Following definitions are read by the application.
 */

/**
 * Published Time BusObjects and Interfaces.
 */

/**
 * Clock and Authority proxy objects
 */

/**
 * Clock obj path name
 */
extern char AJTS_OBJ_PATH_CLOCK_PROXY[AJTS_MAX_OBJ_PATH_LENGTH + 1];

/**
 * Time Authority obj path name
 */
extern char AJTS_OBJ_PATH_CLOCK_TIME_AUTHORITY_PROXY[AJTS_MAX_OBJ_PATH_LENGTH + 1];

/**
 * AJ_Object definition for the clock proxy
 */
#define AJTS_Clock_Proxy { AJTS_OBJ_PATH_CLOCK_PROXY, AJTS_ClockInterfaces, AJ_OBJ_FLAG_HIDDEN | AJ_OBJ_FLAG_DISABLED }

/**
 * AJ_Object definition for the time authority proxy, which include the clock and the time authority interface
 */
#define AJTS_TimeAuthority_Proxy { AJTS_OBJ_PATH_CLOCK_TIME_AUTHORITY_PROXY, AJTS_TimeAuthorityInterfaces, AJ_OBJ_FLAG_HIDDEN | AJ_OBJ_FLAG_DISABLED }

/**
 * Request context for the application. The application should maintain one object for each distinct conversation with the server (clock, alarm,timer,etc.). Only one request can be sent to each of these objects on the server side at any given time.
 */

typedef struct _Request_Context {
    char objPath[33];       /**< path to alljoyn object */
    uint32_t replySerial;   /**< serial number of alljoyn request. populated by service */
    uint32_t propId;        /**< property Id of request. populated by service */
}RequestContext;

/**
 * Following definitions are used by the application.
 */

/**
 * Callbacks
 */
typedef AJ_Status (*AJTS_SetPropReplyHandler)(AJ_Message*replyMsg);

/**
 * Actions to perform when GetDateTime is requested.
 * Performed by the application.
 * @param peerName sender name
 * @param context object context
 * @param date date information retreived
 * @param time time information retreived
 * @param utcOffset offset in minutes
 * @param isAuthority use the authority proxy object
 * @return status
 */
typedef AJ_Status (*AJTS_GetDateTimeHandler)(const char*peerName, const RequestContext*context, const TS_Date* date, const TS_Time* time, const int16_t utcOffset, const AJ_Status status);

/**
 * GetDateTime request.
 * @param bus bus attachment the client uses
 * @param request object context
 * @param isAuthority is this a time authority service or just a regular clock
 * @return status
 */
AJ_Status AJTS_Client_GetDateTime(AJ_BusAttachment* bus, RequestContext*request, uint8_t isAuthority);


/**
 * Actions to perform when SetDateTime is requested.
 * Performed by the application.
 * @param bus bus attachment to send request through
 * @param request object context
 * @param date date to set
 * @param time time to set
 * @param utcOffset offset in minutes to set
 * @param isAuthority use the authority proxy object
 * @return status
 */

AJ_Status AJTS_Client_SetDateTime(AJ_BusAttachment* bus, RequestContext*request, const TS_Date* date, const TS_Time* time, const int16_t utcOffset, uint8_t isAuthority);

/**
 * Actions to perform when IsSet is requested.
 * Performed by the application.
 * @param peerName sender name
 * @param context object context
 * @param isSet has the clock been set since last reboot
 * @param isAuthority use the authority proxy object
 * @param status the status of the request as returned from the server. AJ_OK if successfully handled, otherwise the returned information should be ignored.
 * @return status
 */
typedef AJ_Status (*AJTS_IsSetHandler)(const char*peerName, const RequestContext*context, uint8_t isSet, const AJ_Status status);

/**
 * IsSet request.
 * @param bus bus attachment the client uses
 * @param request object context
 * @param isAuthority is this a time authority service or just a regular clock
 * @return status
 */
AJ_Status AJTS_Client_IsSet(AJ_BusAttachment* bus, RequestContext*request, uint8_t isAuthority);

/**
 * Actions to perform when GetAuthorityType is requested.
 * Performed by the application.
 * @param peerName sender name
 * @param context object context
 * @param authorityType authority type
 * @param status the status of the request as returned from the server. AJ_OK if successfully handled, otherwise the returned information should be ignored.
 * @return status
 */
typedef AJ_Status (*AJTS_GetAuthorityTypeHandler)(const char*peerName, const RequestContext*context, uint8_t authorityType, const AJ_Status status);

/**
 * GetAuthorityType request.
 * @param bus bus attachment the client uses
 * @param request object context
 * @return status
 */
AJ_Status AJTS_Client_GetAuthorityType(AJ_BusAttachment* bus, RequestContext*request);

/**
 * Actions to perform when GetAlaramSchedule is requested.
 * Performed by the application.
 * @param peerName sender name
 * @param context object context
 * @param time time to populate
 * @param weekdaysBitmap bitmap of weekdays
 * @param status the status of the request as returned from the server. AJ_OK if successfully handled, otherwise the returned information should be ignored.
 * @return status
 */
typedef AJ_Status (*AJTS_GetAlaramScheduleHandler)(const char*peerName, const RequestContext*context, TS_Time* time, uint8_t weekdaysBitmap, const AJ_Status status);

/**
 * GetAlaramSchedule request.
 * @param bus bus attachment the client uses
 * @param request object context
 * @return status
 */
AJ_Status AJTS_Client_GetAlaramSchedule(AJ_BusAttachment* bus, RequestContext*request);

/**
 * Actions to perform when SetAlaramSchedule is requested.
 * Performed by the application.
 * @param bus bus attachment the client uses
 * @param context object context
 * @param time time to set
 * @param weekdaysBitmap of weekdays to set
 * @return status
 */
AJ_Status AJTS_Client_SetAlaramSchedule(AJ_BusAttachment* bus, RequestContext*context, const TS_Time* time, const uint8_t weekdaysBitmap);

/**
 * Actions to perform when GetAlaramTitle is requested.
 * Performed by the application.
 * @param peerName sender name
 * @param context object context
 * @param title title of alarm
 * @param status the status of the request as returned from the server. AJ_OK if successfully handled, otherwise the returned information should be ignored.
 * @return status
 */
typedef AJ_Status (*AJTS_GetAlaramTitleHandler)(const char*peerName, const RequestContext*context, char*title, const AJ_Status status);

/**
 * GetAlaramTitle request.
 * @param bus bus attachment the client uses
 * @param request object context
 * @return status
 */
AJ_Status AJTS_Client_GetAlaramTitle(AJ_BusAttachment* bus, RequestContext*request);

/**
 * Actions to perform when SetAlaramTitle is requested.
 * Performed by the application.
 * @param bus bus attachment the client uses
 * @param request object context
 * @param title title of alarm to set
 * @return status
 */
AJ_Status AJTS_Client_SetAlaramTitle(AJ_BusAttachment* bus, RequestContext*request, const char title[AJTS_MAX_TITLE_LENGTH + 1]);

/**
 * Actions to perform when GetAlaramEnabled is requested.
 * Performed by the application.
 * @param peerName sender name
 * @param context object context
 * @param status the status of the request as returned from the server. AJ_OK if successfully handled, otherwise the returned information should be ignored.
 * @return status
 */
typedef AJ_Status (*AJTS_GetAlaramEnabledHandler)(const char*peerName, const RequestContext*context, uint8_t isEnabled, const AJ_Status status);

/**
 * GetAlaramEnabled request.
 * @param bus bus attachment the client uses
 * @param request object context
 * @return status
 */
AJ_Status AJTS_Client_GetAlaramEnabled(AJ_BusAttachment* bus, RequestContext*request);

/**
 * Actions to perform when SetAlaramEnabled is requested.
 * Performed by the application.
 * @param bus bus attachment the client uses
 * @param request object context
 * @param isEnabled is the alarm enabled
 * @return status
 */
AJ_Status AJTS_Client_SetAlaramEnabled(AJ_BusAttachment* bus, RequestContext*request, const uint8_t isEnabled);

/**
 * Actions to perform when TimerStart is requested.
 * Performed by the application.
 * @param bus bus attachment the client uses
 * @param request object context
 * @return status
 */
AJ_Status AJTS_Client_TimerStart(AJ_BusAttachment* bus, RequestContext*request);

/**
 * Actions to perform when TimerPause is requested.
 * Performed by the application.
 * @param bus bus attachment the client uses
 * @param request object context
 * @return status
 */
AJ_Status AJTS_Client_TimerPause(AJ_BusAttachment* bus, RequestContext*request);

/**
 * Actions to perform when TimerReset is requested.
 * Performed by the application.
 * @param peerName sender name
 * @param context object context
 * @param status the status of the request as returned from the server. AJ_OK if successfully handled, otherwise the returned information should be ignored.
 * @return status
 */
typedef AJ_Status (*AJTS_TimerResetHandler)(const char*peerName, const RequestContext*context, const AJ_Status status);

/**
 * TimerReset request.
 * @param bus bus attachment the client uses
 * @param request object context
 * @return status
 */
AJ_Status AJTS_Client_TimerReset(AJ_BusAttachment* bus, RequestContext*request);

/**
 * Actions to perform when GetTimerInterval is requested.
 * Performed by the application.
 * @param peerName sender name
 * @param interval object path
 * @param status the status of the request as returned from the server. AJ_OK if successfully handled, otherwise the returned information should be ignored.
 * @return status
 */
typedef AJ_Status (*AJTS_GetTimerIntervalHandler)(const char*peerName, const RequestContext*context, TS_Timer_Period* interval, const AJ_Status status);

/**
 * GetTimerInterval request.
 * @param bus bus attachment the client uses
 * @param request object context
 * @return status
 */
AJ_Status AJTS_Client_GetTimerInterval(AJ_BusAttachment* bus, RequestContext*request);

/**
 * Actions to perform when SetTimerInterval is requested.
 * Performed by the application.
 * @param bus bus attachment the client uses
 * @param request object context
 * @param time
 * @return status
 */
AJ_Status AJTS_Client_SetTimerInterval(AJ_BusAttachment* bus, RequestContext*request, const TS_Timer_Period* time);

/**
 * Actions to perform when GetTimerTimeLeft is requested.
 * Performed by the application.
 * @param peerName sender name
 * @param context object context
 * @param time time left for this timer
 * @param status the status of the request as returned from the server. AJ_OK if successfully handled, otherwise the returned information should be ignored.
 * @return status
 */
typedef AJ_Status (*AJTS_GetTimerTimeLeftHandler)(const char*peerName, const RequestContext*context, TS_Timer_Period* time, const AJ_Status status);

/**
 * GetTimerTimeLeft request.
 * @param bus bus attachment the client uses
 * @param request object context
 * @return status
 */
AJ_Status AJTS_Client_GetTimerTimeLeft(AJ_BusAttachment* bus, RequestContext*request);

/**
 * Actions to perform when GetTimerIsRunning is requested.
 * Performed by the application.
 * @param peerName sender name
 * @param context object context
 * @param isRunning is this timer running
 * @param status the status of the request as returned from the server. AJ_OK if successfully handled, otherwise the returned information should be ignored.
 * @return status
 */
typedef AJ_Status (*AJTS_GetTimerIsRunningHandler)(const char*peerName, const RequestContext*context, uint8_t isRunning, const AJ_Status status);

/**
 * GetTimerIsRunning request.
 * @param bus bus attachment the client uses
 * @param request object context
 * @return status
 */
AJ_Status AJTS_Client_GetTimerIsRunning(AJ_BusAttachment* bus, RequestContext*request);

/**
 * Actions to perform when GetTimerRepeat is requested.
 * Performed by the application.
 * @param peerName sender name
 * @param context object context
 * @param repeat how many times to repeat this timer
 * @param status the status of the request as returned from the server. AJ_OK if successfully handled, otherwise the returned information should be ignored.
 * @return status
 */
typedef AJ_Status (*AJTS_GetTimerRepeatHandler)(const char*peerName, const RequestContext*context, uint16_t repeat, const AJ_Status status);

/**
 * GetTimerRepeat request.
 * @param bus bus attachment the client uses
 * @param request object context
 * @return status
 */
AJ_Status AJTS_Client_GetTimerRepeat(AJ_BusAttachment* bus, RequestContext*request);

/**
 * Actions to perform when SetTimerRepeat is requested.
 * Performed by the application.
 * @param bus bus attachment the client uses
 * @param request object context
 * @param repeat how many times to repeat this timer
 * @return status
 */
AJ_Status AJTS_Client_SetTimerRepeat(AJ_BusAttachment* bus, RequestContext*request, const uint16_t repeat);

/**
 * Actions to perform when GetTimerTitle is requested.
 * Performed by the application.
 * @param peerName sender name
 * @param context object context
 * @param title of the timer
 * @param status the status of the request as returned from the server. AJ_OK if successfully handled, otherwise the returned information should be ignored.
 * @return status
 */
typedef AJ_Status (*AJTS_GetTimerTitleHandler)(const char*peerName, const RequestContext*context, char* title, const AJ_Status status);

/**
 * GetTimerTitle request.
 * @param bus bus attachment the client uses
 * @param request object context
 * @return status
 */
AJ_Status AJTS_Client_GetTimerTitle(AJ_BusAttachment* bus, RequestContext*request);

/**
 * Actions to perform when SetTimerTitle is requested.
 * Performed by the application.
 * @param bus bus attachment the client uses
 * @param request object context
 * @param title title of timer to set
 * @return status
 */
AJ_Status AJTS_Client_SetTimerTitle(AJ_BusAttachment* bus, RequestContext*request, const char title[AJTS_MAX_TITLE_LENGTH + 1]);

/**
 * Client_OnSignal implemented by the application
 * @param peerName sender name
 * @param objPath object's path
 * @return status success/failure
 */
typedef AJ_Status (*AJTS_Client_OnSignal)(const char*peerName, const char*objPath);

/**
 * Client_OnTimerRunStateChanged implemented by the application
 * @param peerName sender name
 * @param objPath object's path
 * @param runState  Signal sent after Start or Pause is called. The boolean argument is true if the timer is running, false if it is not.
 * @return status success/failure
 */
typedef AJ_Status (*AJTS_Client_OnTimerRunStateChanged)(const char*peerName, const char*objPath, const uint8_t runState);

extern char AJTS_OBJ_PATH_ALARM_PROXY[AJTS_MAX_OBJ_PATH_LENGTH + 1];

/**
 * Alarm proxy object
 */

#define AJTS_Alarm_Proxy { AJTS_OBJ_PATH_ALARM_PROXY, AJTS_AlarmInterfaces, AJ_OBJ_FLAG_HIDDEN | AJ_OBJ_FLAG_DISABLED }

extern char AJTS_OBJ_PATH_TIMER_PROXY[AJTS_MAX_OBJ_PATH_LENGTH + 1];

/**
 * Timer proxy object
 */


#define AJTS_Timer_Proxy { AJTS_OBJ_PATH_TIMER_PROXY, AJTS_TimerInterfaces, AJ_OBJ_FLAG_HIDDEN | AJ_OBJ_FLAG_DISABLED }


/**
 * Start time service framework client mode and callbacks
 * @return status
 */
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
                            AJTS_GetTimerTitleHandler getTimerTitleHandler);
/**
 * Consumer_SetSignalRules, to add the correct filter for the required interface
 * @param busAttachment
 * @return status
 */
AJ_Status AJTS_Client_SetSignalRules(AJ_BusAttachment* busAttachment);

/**
 * TimeSyncSignalHandler - receives message, unmarshals it and calls handleNotify
 * @return status
 */
AJ_Status AJTS_Client_TimeSyncSignalHandler(AJ_Message* msg);

/**
 * AlarmReachedSignalHandler - receives message, unmarshals it and calls handleDismiss
 * @return status
 */
AJ_Status AJTS_Client_AlarmReachedSignalHandler(AJ_Message* msg);

/**
 * TimerEventSignalHandler - receives message, unmarshals it and calls handleDismiss
 * @return status
 */
AJ_Status AJTS_Client_TimerEventSignalHandler(AJ_Message* msg);

/**
 * RunStateChangedSignalHandler - receives message, unmarshals it and calls handleDismiss
 * @param msg
 * @return status
 */
AJ_Status AJTS_Client_RunStateChangedSignalHandler(AJ_Message* msg);

/**
 * AJTS_OnSessionReply - handle session events after join request
 * @param sessionId - if this is 0 the session join request failed or lost
 * @param peerName - peer name to connect to
 * @return status
 */

typedef void (*AJTS_OnSessionReply)(uint32_t sessionId, const char*peerName);

/**
 * AJTS_Client_CreateSessionWithServer - create new session with peer. only one session is allowed at a given time
 * @param busAttachment
 * @param peerName
 * @param onSessionReply
 * @return status
 */
AJ_Status AJTS_Client_CreateSessionWithServer(AJ_BusAttachment* busAttachment, const char* peerName, AJTS_OnSessionReply onSessionReply);

/**
 * AJTS_Client_LeaveSessionWithServer - leave the session.
 * @param busAttachment
 * @return status
 */
AJ_Status AJTS_Client_LeaveSessionWithServer(AJ_BusAttachment* busAttachment);

/**
 * AJTS_Client_ReplyHandler - handle reply from server
 * @param replyMsg
 * @param context
 * @return the Alljoyn status
 */
AJ_Status AJTS_Client_ReplyHandler(AJ_Message* replyMsg, RequestContext* context);

/**
 * Function called after service connects
 * @param busAttachment
 * @return the Alljoyn status
 */
AJ_Status AJTS_Client_ConnectedHandler(AJ_BusAttachment* busAttachment);

/**
 * Process the message received
 * @param busAttachment
 * @param msg
 * @param msgStatus
 * @return status - was message handled
 */
AJSVC_ServiceStatus AJTS_Client_MessageProcessor(AJ_BusAttachment* busAttachment, AJ_Message* msg, AJ_Status* msgStatus);

/**
 * Handle successful session join
 * @param busAttachment
 * @param sessionId
 * @param replySerialNum
 * @return status - was message handled
 */
AJSVC_ServiceStatus AJTS_Client_SessionJoinedHandler(AJ_BusAttachment* busAttachment, uint32_t sessionId, uint32_t replySerialNum);

/**
 * Handle failed session join
 * @param busAttachment
 * @param replySerialNum
 * @param replyCode
 * @return status - was message handled
 */
AJSVC_ServiceStatus AJTS_Client_SessionRejectedHandler(AJ_BusAttachment* busAttachment, uint32_t replySerialNum, uint32_t replyCode);

/**
 * Handle session loss
 * @param busAttachment
 * @param sessionId
 * @param reason
 * @return status - was message handled
 */
AJSVC_ServiceStatus AJTS_Client_SessionLostHandler(AJ_BusAttachment* busAttachment, uint32_t sessionId, uint32_t reason);

/**
 * Function called after service disconnects
 * @param busAttachment
 * @return the Alljoyn status
 */
AJ_Status AJTS_Client_DisconnectHandler(AJ_BusAttachment* busAttachment);



/** @} */ // End of group 'TimeServiceClient'
#endif // _TIMESERVICECLIENT_H_
