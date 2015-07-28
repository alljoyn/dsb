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
 * The corresponding flag dbgAJSVCAPP is defined in the containing sample app.
 */
#define AJ_MODULE AJSVCAPP

#include <alljoyn.h>
#include <aj_debug.h>
#include <aj_config.h>
#include <aj_creds.h>
#include <aj_nvram.h>
#include <aj_link_timeout.h>
#include <alljoyn/services_common/ServicesCommon.h>
#include <alljoyn/services_common/ServicesHandlers.h>
#include <alljoyn/time/TimeServiceCommon.h>
#include <alljoyn/time/TimeServiceClient.h>
#include <alljoyn/services_common/PropertyStore.h>
#if (defined(__linux) || defined(_WIN32) || defined(__MACH__))
#include <TimeClientSampleUtil.h>
#else
#define Client_GetActionFromUser(...) do { } while (0)
#endif

/*
 * Logger definition
 */
#ifndef NDEBUG
#ifndef ER_DEBUG_AJSVCAPP
#define ER_DEBUG_AJSVCAPP 0
#endif
AJ_EXPORT uint8_t dbgAJSVCAPP = ER_DEBUG_AJSVCAPP;
#endif

/*
 * Define timeout/pause values. Values are in milli seconds.
 * The following may be tuned according to platform requirements such as battery usage.
 */
#define AJAPP_BUS_LINK_TIMEOUT    60
#define AJAPP_CONNECT_TIMEOUT     (1000 * 60 * 10) // Override AJ_CONNECT_TIMEOUT to wait longer for a successful connection to a Routing Node
#define AJAPP_CONNECT_PAUSE       (1000 * 2)       // Override AJ_CONNECT_PAUSE to be more responsive
#define AJAPP_SLEEP_TIME          (1000 * 2)       // A little pause to let things settle
#define AJAPP_UNMARSHAL_TIMEOUT   (1000 * 1)       // Override AJ_UNMARSHAL_TIMEOUT to be more responsive

/**
 * Application wide globals
 */

#define ROUTING_NODE_NAME "org.alljoyn.BusNode"
static uint8_t isBusConnected = FALSE;
static AJ_BusAttachment g_busAttachment;

static uint8_t g_reportTime = FALSE;

typedef struct _PeerData {
    char peerName[16];
    char appId[UUID_LENGTH * 2 + 1];
    RequestContext clockContext;
    RequestContext clockAuthorityContext;
    RequestContext alarm1Context;
    RequestContext timer1Context;
    uint32_t sessionId;
} PeerData;

static PeerData g_peer1 = { "", "", { "", 0 }, { "", 0 }, { "", 0 }, { "", 0 }, 0 };

/**
 * Application wide callbacks
 */

static uint32_t MyBusAuthPwdCB(uint8_t* buf, uint32_t bufLen)
{
    const char* myRoutingNodePwd = "000000";
    strncpy((char*)buf, myRoutingNodePwd, bufLen);
    return (uint32_t)strlen(myRoutingNodePwd);
}

static uint32_t PasswordCallback(uint8_t* buffer, uint32_t bufLen)
{
    AJ_Status status = AJ_OK;
    const char* hexPassword = "303030303030";
    size_t hexPasswordLen;
    uint32_t len = 0;

    AJ_InfoPrintf(("Configured password=%s\n", hexPassword));
    hexPasswordLen = strlen(hexPassword);
    len = (uint32_t)hexPasswordLen / 2;
    status = AJ_HexToRaw(hexPassword, hexPasswordLen, buffer, bufLen);
    if (status == AJ_ERR_RESOURCES) {
        len = 0;
    }

    return len;
}

/**
 * Application handlers
 */

typedef enum {
    INIT_START = 0,
    INIT_SERVICES = INIT_START,
    INIT_FINISHED
} enum_init_state_t;

static enum_init_state_t currentServicesInitializationState = INIT_START;
static enum_init_state_t nextServicesInitializationState = INIT_START;

static AJ_Status AJApp_ConnectedHandler(AJ_BusAttachment* busAttachment)
{
    AJ_Status status = AJ_OK;

    if (AJ_GetUniqueName(busAttachment)) {
        if (currentServicesInitializationState == nextServicesInitializationState) {
            switch (currentServicesInitializationState) {
            case INIT_SERVICES:
                status = AJSVC_ConnectedHandler(busAttachment);
                if (status != AJ_OK) {
                    goto ErrorExit;
                }
                status = AJ_BusSetSignalRuleFlags(busAttachment, "interface='org.alljoyn.About',member='Announce',sessionless='t'", AJ_BUS_SIGNAL_ALLOW, AJ_FLAG_NO_REPLY_EXPECTED);
                if (status != AJ_OK) {
                    goto ErrorExit;
                }

                currentServicesInitializationState = nextServicesInitializationState = INIT_FINISHED;
                break;

            case INIT_FINISHED:
            default:
                break;
            }
        }
    }
    return status;

ErrorExit:

    AJ_ErrPrintf(("Application ConnectedHandler returned an error %s\n", (AJ_StatusText(status))));
    return status;
}

static AJSVC_ServiceStatus AJApp_MessageProcessor(AJ_BusAttachment* busAttachment, AJ_Message* msg, AJ_Status* status)
{
    AJSVC_ServiceStatus serviceStatus = AJSVC_SERVICE_STATUS_NOT_HANDLED;

    switch (currentServicesInitializationState) {
    case INIT_FINISHED:
        {
            switch (msg->msgId) {
            default:
                switch (msg->hdr->msgType) {
                case AJ_MSG_METHOD_RET:
                    {
                        uint32_t replySerial = msg->replySerial;

                        if (replySerial == g_peer1.clockContext.replySerial) {
                            *status = AJTS_Client_ReplyHandler(msg, &g_peer1.clockContext);
                            serviceStatus = AJSVC_SERVICE_STATUS_HANDLED;
                        } else if (replySerial == g_peer1.clockAuthorityContext.replySerial) {
                            *status = AJTS_Client_ReplyHandler(msg, &g_peer1.clockAuthorityContext);
                            serviceStatus = AJSVC_SERVICE_STATUS_HANDLED;
                        } else if (replySerial == g_peer1.alarm1Context.replySerial) {
                            *status = AJTS_Client_ReplyHandler(msg, &g_peer1.alarm1Context);
                            serviceStatus = AJSVC_SERVICE_STATUS_HANDLED;
                        } else if (replySerial == g_peer1.timer1Context.replySerial) {
                            *status = AJTS_Client_ReplyHandler(msg, &g_peer1.timer1Context);
                            serviceStatus = AJSVC_SERVICE_STATUS_HANDLED;
                        }
                    }
                    break;

                case AJ_MSG_ERROR:
                    AJ_AlwaysPrintf(("Error receiving a message, reply serial:%d msgId:%x error message:%s\n", msg->replySerial, msg->msgId, msg->error));
                    break;

                default:
                    break;
                }
                break;
            }
            if (serviceStatus != AJSVC_SERVICE_STATUS_HANDLED) {
                serviceStatus = AJSVC_MessageProcessorAndDispatcher(busAttachment, msg, status);
            }
        }
        break;

    default:
        break;
    }


    return serviceStatus;
}

static AJ_Status AJApp_DisconnectHandler(AJ_BusAttachment* busAttachment, uint8_t restart)
{
    AJ_Status status = AJ_OK;

    currentServicesInitializationState = nextServicesInitializationState = INIT_START;

    status = AJSVC_DisconnectHandler(busAttachment);
    return status;
}

/**
 * Services Provisioning
 */

/**
 * PropertyStore stub implementation for About feature
 */
AJ_Status AJSVC_PropertyStore_ReadAll(AJ_Message* reply, AJSVC_PropertyStoreCategoryFilter filter, int8_t langIndex) {
    return AJ_OK;
}
int8_t AJSVC_PropertyStore_GetLanguageIndex(const char* const language) {
    return AJSVC_PROPERTY_STORE_ERROR_LANGUAGE_INDEX;
}
int8_t AJSVC_PropertyStore_GetCurrentDefaultLanguageIndex() {
    return AJSVC_PROPERTY_STORE_ERROR_LANGUAGE_INDEX;
}

/**
 *  Time Client Provisioning
 */

static AJ_Status OnTimeSync(const char*peerName, const char*objPath)
{
    AJ_AlwaysPrintf(("OnTimeSync received from '%s//%s'\n", peerName, objPath));
    return AJ_OK;
}

static AJ_Status OnAlarmReached(const char*peerName, const char*objPath)
{
    AJ_AlwaysPrintf(("OnAlarmReached received from '%s//%s'\n", peerName, objPath));

    return AJ_OK;
}

static AJ_Status OnTimerEvent(const char*peerName, const char*objPath)
{
    AJ_AlwaysPrintf(("OnTimerEvent received from '%s//%s'\n", peerName, objPath));

    return AJ_OK;
}

static AJ_Status OnTimerRunStateChanged(const char*peerName, const char*objPath, const uint8_t runState)
{
    AJ_AlwaysPrintf(("OnTimerRunStateChanged received from '%s//%s', runstate:%d\n", peerName, objPath, runState));

    return AJ_OK;
}

AJ_Status IsSetHandler(const char*peerName, const RequestContext*context, uint8_t isSet, const AJ_Status status)
{
    AJ_AlwaysPrintf(("isSet:%d on:%s", isSet, context->objPath));

    g_reportTime = isSet;

    return AJ_OK;
}

AJ_Status GetDateTimeHandler(const char*peerName, const RequestContext*context, const TS_Date* date, const TS_Time* time, const int16_t utcOffset, const AJ_Status status)
{

    AJ_AlwaysPrintf(("%02u/%02u/%u %02u:%02u:%02u.%u\n", date->day, date->month, date->year, time->hour, time->minute, time->second, time->milliseconds));

    return AJ_OK;
}

AJ_Status TimerResetHandler(const char*peerName, const RequestContext*context, const AJ_Status status)
{
    AJ_AlwaysPrintf(("TimerStartHandler status:%d\n", status));
    return AJ_OK;
}


static AJ_Status SetPropReplyHandler(AJ_Message*replyMsg)
{
    AJ_AlwaysPrintf(("Got reply on set property from '%s', reply Id:%d", replyMsg->sender, replyMsg->replySerial));

    return AJ_OK;
}

static AJ_Status GetAuthorityTypeHandler(const char*peerName, const RequestContext*context, uint8_t authorityType, const AJ_Status status)
{
    AJ_AlwaysPrintf(("GetAuthorityTypeHandler status:%d\n", status));

    return AJ_OK;
}

static AJ_Status GetAlaramTitleHandler(const char*peerName, const RequestContext*context, char*title, const AJ_Status status)
{
    AJ_AlwaysPrintf(("GetAlaramTitleHandler status:%d\n", status));

    return AJ_OK;
}

AJ_Status GetTimerTitleHandler(const char*peerName, const RequestContext*context, char* title, const AJ_Status status)
{
    if (status == AJ_OK) {
        AJ_AlwaysPrintf(("GetTimerTitleHandler title:%s\n", title));
    } else {
        AJ_ErrPrintf(("AJTS_Client_GetTimerTitle returned an error %s\n", (AJ_StatusText(status))));
    }

    return AJ_OK;
}

AJ_Status GetTimerTimeLeftHandler(const char*peerName, const RequestContext*context, TS_Timer_Period* time, const AJ_Status status)
{
    AJ_Status localStatus;

    AJ_AlwaysPrintf(("GetTimerTimeLeftHandler:%d:%d:%d.%d\n", time->hour, time->minute, time->second, time->milliseconds));

    localStatus = AJTS_Client_GetTimerTitle(&g_busAttachment, &g_peer1.timer1Context);
    if (localStatus != AJ_OK) {
        AJ_ErrPrintf(("AJTS_Client_GetTimerTitle returned an error %s\n", (AJ_StatusText(localStatus))));
    }

    return AJ_OK;
}


AJ_Status GetTimerRepeatHandler(const char*peerName, const RequestContext*context, uint16_t repeat, const AJ_Status status)
{
    AJ_Status localStatus;

    AJ_AlwaysPrintf(("GetTimerRepeatHandler:%d\n", repeat));

    localStatus = AJTS_Client_GetTimerTimeLeft(&g_busAttachment, &g_peer1.timer1Context);
    if (localStatus != AJ_OK) {
        AJ_ErrPrintf(("AJTS_Client_GetTimerRepeat returned an error %s\n", (AJ_StatusText(localStatus))));
    }

    return AJ_OK;
}

AJ_Status GetTimerIntervalHandler(const char*peerName, const RequestContext*context, TS_Timer_Period* interval, const AJ_Status status)
{
    AJ_Status localStatus;

    AJ_AlwaysPrintf(("GetTimerIntervalHandler:%d:%d:%d.%d\n", interval->hour, interval->minute, interval->second, interval->milliseconds));

    localStatus = AJTS_Client_GetTimerRepeat(&g_busAttachment, &g_peer1.timer1Context);
    if (localStatus != AJ_OK) {
        AJ_ErrPrintf(("AJTS_Client_GetTimerRepeat returned an error %s\n", (AJ_StatusText(localStatus))));
    }

    return AJ_OK;
}

static AJ_Status GetTimerIsRunningHandler(const char*peerName, const RequestContext*context, uint8_t isRunning, const AJ_Status status)
{
    AJ_AlwaysPrintf(("GetTimerIsRunningHandler status:%d\n", status));

    return AJ_OK;
}

AJ_Status GetAlaramEnabledHandler(const char*peerName, const RequestContext*context, uint8_t isEnabled, const AJ_Status status)
{
    AJ_AlwaysPrintf(("GetAlaramEnabledHandler:%d\n", isEnabled));

    return AJ_OK;
}

AJ_Status GetAlaramScheduleHandler(const char*peerName, const RequestContext*context, TS_Time* time, uint8_t weekdaysBitmap, const AJ_Status status)
{
    AJ_Status localStatus;

    AJ_AlwaysPrintf(("GetAlaramScheduleHandler:%d:%d:%d.%d\n", time->hour, time->minute, time->second, time->milliseconds));

    localStatus = AJTS_Client_GetAlaramEnabled(&g_busAttachment, &g_peer1.alarm1Context);
    if (localStatus != AJ_OK) {
        AJ_ErrPrintf(("AJTS_Client_GetAlaramEnabled returned an error %s\n", (AJ_StatusText(localStatus))));
    }

    return AJ_OK;
}

const char* clockIFaces[] = { AJTS_CLOCK_IFACE_NAME };
const char* timeAuthorityClockIFaces[] = { AJTS_TIMEAUTHORITY_IFACE_NAME };
const char* alarmIFaces[] = { AJTS_ALARM_IFACE_NAME };
const char* timerIFaces[] = { AJTS_TIMER_IFACE_NAME };

#define SAFE_COPY_STRING(src, dst) { strncpy(dst, src, sizeof(dst)); \
                                     dst[sizeof(dst) - 1] = '\0'; }

static uint8_t handlePeerIsRelevant(const char* peerName)
{
    return TRUE;
}

static void handleObjectDescription(const char* peerName, const AJ_AboutObjectDescription* aboutObjectDescription)
{
}

static void handleMandatoryProps(const char* peerName,
                                 const char* appId,
                                 const char* appName,
                                 const char* deviceId,
                                 const char* deviceName,
                                 const char* manufacturer,
                                 const char* modelNumber,
                                 const char* defaultLanguage) {
    if (strcmp(g_peer1.peerName, peerName) == 0) {
        if (strcmp(g_peer1.appId, appId) != 0) {
            AJ_AlwaysPrintf(("Mandatory Properties for %s\n", peerName));
            AJ_AlwaysPrintf(("Mandatory property: appId=\"%s\"\n", appId));
            AJ_AlwaysPrintf(("Mandatory property: deviceName=\"%s\"\n", deviceName));
            AJ_AlwaysPrintf(("Mandatory property: deviceId=\"%s\"\n", deviceId));
            AJ_AlwaysPrintf(("Mandatory property: appName=\"%s\"\n", appName));
            AJ_AlwaysPrintf(("Mandatory property: manufacturer=\"%s\"\n", manufacturer));
            AJ_AlwaysPrintf(("Mandatory property: modelNumber=\"%s\"\n", modelNumber));
            AJ_AlwaysPrintf(("Mandatory property: defaultLanguage=\"%s\"\n", defaultLanguage));
            SAFE_COPY_STRING(appId, g_peer1.appId);
        }
    }
}

static void handleOptionalProperty(const char* peerName, const char* key, const char* sig, const AJ_Arg* value) {
    if (strcmp(g_peer1.peerName, peerName) == 0) {
        if (strcmp(sig, "s") == 0) {
            AJ_AlwaysPrintf(("Optional Prop: %s=\"%s\"\n", key, value->val.v_string));
        } else {
            AJ_AlwaysPrintf(("Optional Prop: %s=[Not A String]\n", key));
        }
    }
}

static uint8_t FoundNewClock(uint16_t version, uint16_t port, const char* peerName, const char* objPath);
static uint8_t FoundNewTimeAuthorityClock(uint16_t version, uint16_t port, const char* peerName, const char* objPath);
static uint8_t FoundNewAlarm(uint16_t version, uint16_t port, const char* peerName, const char* objPath);
static uint8_t FoundNewTimer(uint16_t version, uint16_t port, const char* peerName, const char* objPath);

static AJ_AboutPeerDescription timeServerPeers[] = {
    { clockIFaces, (uint16_t)(sizeof(clockIFaces) / sizeof(*clockIFaces)), FoundNewClock, handlePeerIsRelevant, handleObjectDescription, handleMandatoryProps, handleOptionalProperty },
    { timeAuthorityClockIFaces, (uint16_t)(sizeof(char*) / sizeof(*timeAuthorityClockIFaces)), FoundNewTimeAuthorityClock, handlePeerIsRelevant, handleObjectDescription, handleMandatoryProps, handleOptionalProperty },
    { alarmIFaces, (uint16_t)(sizeof(alarmIFaces) / sizeof(*alarmIFaces)), FoundNewAlarm, handlePeerIsRelevant, handleObjectDescription, handleMandatoryProps, handleOptionalProperty },
    { timerIFaces, (uint16_t)(sizeof(timerIFaces) / sizeof(*timerIFaces)), FoundNewTimer, handlePeerIsRelevant, handleObjectDescription, handleMandatoryProps, handleOptionalProperty },
};

static uint8_t FoundNewClock(uint16_t version, uint16_t port, const char* peerName, const char* objPath)
{
    AJ_AlwaysPrintf(("FoundNewClock: version:%u port:%u name:%s path=%s\n", version, port, peerName, objPath));

    if (g_peer1.peerName[0] == '\0') {
        SAFE_COPY_STRING(peerName, g_peer1.peerName)
    }

    if ((strcmp(peerName, g_peer1.peerName) == 0) && (g_peer1.clockContext.objPath[0] == '\0')) {
        SAFE_COPY_STRING(objPath, g_peer1.clockContext.objPath)
    }

    return TRUE;
}

static uint8_t FoundNewTimeAuthorityClock(uint16_t version, uint16_t port, const char* peerName, const char* objPath)
{
    AJ_AlwaysPrintf(("FoundNewTimeAuthorityClock: version:%u port:%u name:%s path=%s\n", version, port, peerName, objPath));

    if (g_peer1.peerName[0] == '\0') {
        SAFE_COPY_STRING(peerName, g_peer1.peerName)
    }

    if ((strcmp(peerName, g_peer1.peerName) == 0) && (g_peer1.clockAuthorityContext.objPath[0] == '\0')) {
        SAFE_COPY_STRING(objPath, g_peer1.clockAuthorityContext.objPath)
    }

    return TRUE;
}

static uint8_t FoundNewAlarm(uint16_t version, uint16_t port, const char* peerName, const char* objPath)
{
    AJ_AlwaysPrintf(("FoundNewAlarm: version:%u port:%u name:%s path=%s\n", version, port, peerName, objPath));

    if (g_peer1.peerName[0] == '\0') {
        SAFE_COPY_STRING(peerName, g_peer1.peerName)
    }

    if ((strcmp(peerName, g_peer1.peerName) == 0) && (g_peer1.alarm1Context.objPath[0] == '\0')) {
        SAFE_COPY_STRING(objPath, g_peer1.alarm1Context.objPath)
    }

    return TRUE;
}

static uint8_t FoundNewTimer(uint16_t version, uint16_t port, const char* peerName, const char* objPath)
{
    AJ_AlwaysPrintf(("FoundNewTimer: version:%u port:%u name:%s path=%s\n", version, port, peerName, objPath));

    if (g_peer1.peerName[0] == '\0') {
        SAFE_COPY_STRING(peerName, g_peer1.peerName)
    }

    if ((strcmp(peerName, g_peer1.peerName) == 0) && (g_peer1.timer1Context.objPath[0] == '\0')) {
        SAFE_COPY_STRING(objPath, g_peer1.timer1Context.objPath)
    }

    return TRUE;
}

static AJ_Status Client_Init()
{
    AJ_Status status;

    AJ_AboutRegisterAnnounceHandlers(timeServerPeers, (uint16_t)(sizeof(timeServerPeers) / sizeof(*timeServerPeers)));
    status = AJTS_Client_Start(OnTimeSync,
                               OnAlarmReached,
                               OnTimerEvent,
                               OnTimerRunStateChanged,
                               SetPropReplyHandler,
                               GetDateTimeHandler,
                               IsSetHandler,
                               GetAuthorityTypeHandler,
                               GetAlaramScheduleHandler,
                               GetAlaramTitleHandler,
                               GetAlaramEnabledHandler,
                               TimerResetHandler,
                               GetTimerIntervalHandler,
                               GetTimerTimeLeftHandler,
                               GetTimerIsRunningHandler,
                               GetTimerRepeatHandler,
                               GetTimerTitleHandler
                               );

    return status;
}

void AuthCallback(const void* context, AJ_Status status)
{
    AJ_AlwaysPrintf(("AuthCallback status:%d\n", status));

    if (status != AJ_OK) {
        return;
    }
}

void OnSessionReply(uint32_t sessionId, const char*peerName)
{
    AJ_Status authStatus = AJ_ERR_NULL;

    authStatus = AJ_BusAuthenticatePeer(&g_busAttachment, peerName, AuthCallback, &authStatus);

    if (authStatus != AJ_OK) {
        AJ_Disconnect(&g_busAttachment);
        return;
    }
    //SendPing(&busAttachment, sessionId);

    if (!strcmp(peerName, g_peer1.peerName)) {
        g_peer1.sessionId = sessionId;

        if (!g_peer1.sessionId) {
            memset(&g_peer1, 0, sizeof(PeerData));

            // we lost an active session and assume we lost connection to our peer. Since we are creating a session only when the first Time service announcement is received this means we will not be able to connect to this peer since it was already discovered and handled in the OnAnnouncementHandler function. If a new time service is not discovered and the lost peer is required, restart that server to received it's announcement again.
        }
    }
}

void AJTS_DoWork(AJ_BusAttachment* busAttachment)
{
    AJ_Status status = AJ_OK;
    uint8_t action = 9;

    if (g_peer1.peerName[0] != '\0' && (g_peer1.sessionId == 0)) {
        status = AJTS_Client_CreateSessionWithServer(busAttachment, g_peer1.peerName, OnSessionReply);
    }

    if (g_peer1.sessionId == 0) {
        return;
    }

    Client_GetActionFromUser(g_peer1.peerName, &action);

    switch (action) {
    case 0:
        if (g_peer1.sessionId) {
            if (g_peer1.clockContext.objPath[0] != '\0') {

                TS_Date date = { 2011, 10, 10 };

                TS_Time time = { 6, 10, 10, 0 };

                status = AJTS_Client_SetDateTime(&g_busAttachment, &g_peer1.clockContext, &date, &time, 0, FALSE);

                if (status != AJ_OK) {
                    AJ_ErrPrintf(("AJTS_Client_SetDateTime returned an error %s\n", (AJ_StatusText(status))));
                }
            }
        }
        break;

    case 1:
        status = AJTS_Client_IsSet(busAttachment, &g_peer1.clockContext, TRUE);
        if (status != AJ_OK) {
            AJ_ErrPrintf(("AJTS_Client_IsSet returned an error %s\n", (AJ_StatusText(status))));
        } else if (g_reportTime) {
            AJ_Status status = AJTS_Client_GetDateTime(busAttachment, &g_peer1.clockContext, FALSE);

            if (status != AJ_OK) {
                AJ_ErrPrintf(("AJTS_Client_GetDateTime returned an error %s\n", (AJ_StatusText(status))));
            }
        }
        break;

    case 2:
        {
            TS_Timer_Period interval = { 0, 0, 15, 0 };

            status = AJTS_Client_SetTimerTitle(busAttachment, &g_peer1.timer1Context, "Thin Library client title");

            status = AJTS_Client_SetTimerInterval(busAttachment, &g_peer1.timer1Context, &interval);
            if (status != AJ_OK) {
                AJ_ErrPrintf(("AJTS_Client_SetTimerInterval returned an error %s\n", (AJ_StatusText(status))));
            } else {
                status = AJTS_Client_SetTimerRepeat(busAttachment, &g_peer1.timer1Context, 1);
                if (status != AJ_OK) {
                    AJ_ErrPrintf(("AJTS_Client_SetTimerRepeat returned an error %s\n", (AJ_StatusText(status))));
                } else {
                    status = AJTS_Client_TimerStart(busAttachment, &g_peer1.timer1Context);
                    if (status != AJ_OK) {
                        AJ_ErrPrintf(("AJTS_Client_TimerStart returned an error %s\n", (AJ_StatusText(status))));
                    }
                }
            }
        }
        break;

    case 3:
        {
            status = AJTS_Client_GetTimerInterval(busAttachment, &g_peer1.timer1Context);
            if (status != AJ_OK) {
                AJ_ErrPrintf(("AJTS_Client_GetTimerInterval returned an error %s\n", (AJ_StatusText(status))));
            }
        }
        break;

    case 4:
        {
            uint8_t weekdaysBitmap = TS_ALARM_SUNDAY | TS_ALARM_MONDAY | TS_ALARM_TUESDAY | TS_ALARM_WEDNESDAY | TS_ALARM_THURSDAY;
            TS_Time time = { 6, 11, 10, 0 };
            status = AJTS_Client_SetAlaramSchedule(busAttachment, &g_peer1.alarm1Context, &time, weekdaysBitmap);
            if (status != AJ_OK) {
                AJ_ErrPrintf(("AJTS_Client_SetAlaramSchedule returned an error %s\n", (AJ_StatusText(status))));
            } else {
                status = AJTS_Client_SetAlaramEnabled(busAttachment, &g_peer1.alarm1Context, TRUE);
                if (status != AJ_OK) {
                    AJ_ErrPrintf(("AJTS_Client_SetAlaramEnabled returned an error %s\n", (AJ_StatusText(status))));
                }
            }
        }
        break;

    case 5:
        status = AJTS_Client_GetAlaramSchedule(busAttachment, &g_peer1.alarm1Context);
        if (status != AJ_OK) {
            AJ_ErrPrintf(("AJTS_Client_GetAlaramSchedule returned an error %s\n", (AJ_StatusText(status))));
        }
        break;

    case 6:
        {
            status = AJTS_Client_TimerPause(busAttachment, &g_peer1.timer1Context);
            if (status != AJ_OK) {
                AJ_ErrPrintf(("AJTS_Client_TimerPause returned an error %s\n", (AJ_StatusText(status))));
            }
        }
        break;

    case 7:
        {
            status = AJTS_Client_TimerReset(busAttachment, &g_peer1.timer1Context);
            if (status != AJ_OK) {
                AJ_ErrPrintf(("AJTS_Client_TimerPause returned an error %s\n", (AJ_StatusText(status))));
            }
        }
        break;

    default:
        break;
    }
}

/**
 * The AllJoyn Message Loop
 */

int AJ_Main(void)
{
    AJ_Status status = AJ_OK;
    uint8_t isUnmarshalingSuccessful = FALSE;
    AJSVC_ServiceStatus serviceStatus;
    AJ_Message msg;
    uint8_t forcedDisconnnect = FALSE;
    uint8_t rebootRequired = FALSE;

    AJ_Initialize();

    status = Client_Init();
    if (status != AJ_OK) {
        goto Exit;
    }

    SetBusAuthPwdCallback(MyBusAuthPwdCB);

    while (TRUE) {
        status = AJ_OK;
        serviceStatus = AJSVC_SERVICE_STATUS_NOT_HANDLED;

        if (!isBusConnected) {
            status = AJSVC_RoutingNodeConnect(&g_busAttachment, ROUTING_NODE_NAME, AJAPP_CONNECT_TIMEOUT, AJAPP_CONNECT_PAUSE, AJAPP_BUS_LINK_TIMEOUT, &isBusConnected);
            if (!isBusConnected) { // Failed to connect to Routing Node?
                continue; // Retry establishing connection to Routing Node.
            }

            /* Setup password based authentication listener for secured peer to peer connections */
            AJ_BusSetPasswordCallback(&g_busAttachment, PasswordCallback);
        }

        status = AJApp_ConnectedHandler(&g_busAttachment);

        if (status == AJ_OK) {
            status = AJ_UnmarshalMsg(&g_busAttachment, &msg, AJAPP_UNMARSHAL_TIMEOUT);
            isUnmarshalingSuccessful = (status == AJ_OK);

            if (status == AJ_ERR_TIMEOUT) {
                if (AJ_ERR_LINK_TIMEOUT == AJ_BusLinkStateProc(&g_busAttachment)) {
                    status = AJ_ERR_READ;             // something's not right. force disconnect
                } else {
                    AJTS_DoWork(&g_busAttachment);
                    continue;
                }
            }

            if (isUnmarshalingSuccessful) {
                if (serviceStatus == AJSVC_SERVICE_STATUS_NOT_HANDLED) {
                    serviceStatus = AJApp_MessageProcessor(&g_busAttachment, &msg, &status);
                }
                if (serviceStatus == AJSVC_SERVICE_STATUS_NOT_HANDLED) {
                    //Pass to the built-in bus message handlers
                    status = AJ_BusHandleBusMessage(&msg);
                }
                AJ_NotifyLinkActive();
            }

            //Unmarshaled messages must be closed to free resources
            AJ_CloseMsg(&msg);
        }

        if (status == AJ_ERR_READ || status == AJ_ERR_RESTART || status == AJ_ERR_RESTART_APP) {
            if (isBusConnected) {
                forcedDisconnnect = (status != AJ_ERR_READ);
                rebootRequired = (status == AJ_ERR_RESTART_APP);
                AJApp_DisconnectHandler(&g_busAttachment, forcedDisconnnect);
                AJSVC_RoutingNodeDisconnect(&g_busAttachment, forcedDisconnnect, AJAPP_SLEEP_TIME, AJAPP_SLEEP_TIME, &isBusConnected);
                if (rebootRequired) {
                    AJ_Reboot();
                }
            }
        }
    }     // while (TRUE)

    return 0;

Exit:

    return (1);
}

#ifdef AJ_MAIN
int main()
{
    return AJ_Main();
}
#endif


