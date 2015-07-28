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
#include "PropertyStoreOEMProvisioning.h"
#include <alljoyn/services_common/PropertyStore.h>
#include <alljoyn/services_common/ServicesCommon.h>
#include <alljoyn/services_common/ServicesHandlers.h>
#include <alljoyn/time/TimeServiceCommon.h>
#include <alljoyn/time/TimeServiceServer.h>
#ifdef TIME_H_IMPLEMENTED
#include <time.h>
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
static AJ_BusAttachment busAttachment;
#define AJ_ABOUT_SERVICE_PORT 900

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
    INIT_SERVICES_PORT,
    INIT_ABOUT,
    INIT_CHECK_ANNOUNCE,
    INIT_FINISHED = INIT_CHECK_ANNOUNCE
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
                currentServicesInitializationState = nextServicesInitializationState = INIT_SERVICES_PORT;
                break;

            case INIT_SERVICES_PORT:
                status = AJ_BusBindSessionPort(busAttachment, AJ_ABOUT_SERVICE_PORT, NULL, 0);
                if (status != AJ_OK) {
                    goto ErrorExit;
                }
                nextServicesInitializationState = INIT_ABOUT;
                break;

            case INIT_ABOUT:
                status = AJ_AboutInit(busAttachment, AJ_ABOUT_SERVICE_PORT);
                if (status != AJ_OK) {
                    goto ErrorExit;
                }
                currentServicesInitializationState = nextServicesInitializationState = INIT_CHECK_ANNOUNCE;
                break;

            case INIT_CHECK_ANNOUNCE:
                status = AJ_AboutAnnounce(busAttachment);
                if (status != AJ_OK) {
                    goto ErrorExit;
                }
                break;

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
    AJSVC_ServiceStatus serviceStatus = AJSVC_SERVICE_STATUS_HANDLED;
    uint16_t port;
    char* joiner;
    uint32_t sessionId = 0;
    uint8_t session_accepted = FALSE;

    if (msg->msgId == AJ_METHOD_ACCEPT_SESSION) {    // Process all incoming request to join a session and pass request for acceptance by all services
        *status = AJ_UnmarshalArgs(msg, "qus", &port, &sessionId, &joiner);
        if (*status != AJ_OK) {
            return serviceStatus;
        }
        session_accepted |= (port == AJ_ABOUT_SERVICE_PORT);
        session_accepted |= AJSVC_CheckSessionAccepted(port, sessionId, joiner);
        *status = AJ_BusReplyAcceptSession(msg, session_accepted);
        AJ_AlwaysPrintf(("%s session session_id=%u joiner=%s for port %u\n", (session_accepted ? "Accepted" : "Rejected"), sessionId, joiner, port));
    } else {
        switch (currentServicesInitializationState) {
        case INIT_SERVICES_PORT:
            if (msg->msgId == AJ_REPLY_ID(AJ_METHOD_BIND_SESSION_PORT)) {
                currentServicesInitializationState = nextServicesInitializationState;
            }
            break;

        default:
            serviceStatus = AJSVC_MessageProcessorAndDispatcher(busAttachment, msg, status);
            break;
        }
    }

    return serviceStatus;
}

static AJ_Status AJApp_DisconnectHandler(AJ_BusAttachment* busAttachment, uint8_t restart)
{
    AJ_Status status = AJ_OK;

    if (restart) {
        AJ_BusUnbindSession(busAttachment, AJ_ABOUT_SERVICE_PORT);
    }

    AJ_AboutSetShouldAnnounce();
    currentServicesInitializationState = nextServicesInitializationState = INIT_START;

    return status;
}

/**
 * Services Provisioning
 */

const char* deviceManufactureName = "COMPANY";
const char* deviceProductName = "ORLOGIN";

/**
 * Config supported PropertyStore provisioning
 */

static const char DEFAULT_LANGUAGE[] = "en";
static const char* DEFAULT_LANGUAGES[] = { DEFAULT_LANGUAGE };
static const char* SUPPORTED_LANGUAGES[] = { DEFAULT_LANGUAGE, NULL };
const char* const* propertyStoreDefaultLanguages = SUPPORTED_LANGUAGES;

/**
 * property array of structure with defaults
 */
static const char DEFAULT_DEVICE_NAME_LANG1[] = { "Company A - \"ORLOGIN\"" };
static const char* DEFAULT_DEVICE_NAMES[] = { DEFAULT_DEVICE_NAME_LANG1 };
static const char* DEFAULT_APP_NAMES[] = { "TimeServer" };
static const char DEFAULT_DESCRIPTION_LANG1[] = "watch";
static const char* DEFAULT_DESCRIPTIONS[] = { DEFAULT_DESCRIPTION_LANG1 };
static const char DEFAULT_MANUFACTURER_LANG1[] = "Company A(EN)";
static const char* DEFAULT_MANUFACTURERS[] = { DEFAULT_MANUFACTURER_LANG1 };
static const char* DEFAULT_DEVICE_MODELS[] = { "0.0.1" };
static const char* DEFAULT_DATE_OF_MANUFACTURES[] = { "2014-08-18" };
static const char* DEFAULT_SOFTWARE_VERSIONS[] = { "0.0.1" };
static const char* DEFAULT_HARDWARE_VERSIONS[] = { "0.0.1" };
static const char DEFAULT_SUPPORT_URL_LANG1[] = "http://www.company_a.com";
static const char* DEFAULT_SUPPORT_URLS[] = { DEFAULT_SUPPORT_URL_LANG1 };

const char** propertyStoreDefaultValues[AJSVC_PROPERTY_STORE_NUMBER_OF_KEYS] =
{
// "Default Values per language",                    "Key Name"
    NULL,                                           /*DeviceId*/
    NULL,                                           /*AppId*/
    DEFAULT_DEVICE_NAMES,                           /*DeviceName*/
    DEFAULT_LANGUAGES,                              /*DefaultLanguage*/
// Add other runtime keys above this line
    DEFAULT_APP_NAMES,                              /*AppName*/
    DEFAULT_DESCRIPTIONS,                           /*Description*/
    DEFAULT_MANUFACTURERS,                          /*Manufacturer*/
    DEFAULT_DEVICE_MODELS,                          /*ModelNumber*/
    DEFAULT_DATE_OF_MANUFACTURES,                   /*DateOfManufacture*/
    DEFAULT_SOFTWARE_VERSIONS,                      /*SoftwareVersion*/
    NULL,                                           /*AJSoftwareVersion*/
// Add other mandatory about keys above this line
    DEFAULT_HARDWARE_VERSIONS,                      /*HardwareVersion*/
    DEFAULT_SUPPORT_URLS,                           /*SupportUrl*/
// Add other optional about keys above this line
};

/**
 * properties array of runtime values' buffers
 */
static char machineIdVar[MACHINE_ID_LENGTH + 1] = { 0 };
static char* machineIdVars[] = { machineIdVar };
static char deviceNameVarLang1[DEVICE_NAME_VALUE_LENGTH + 1] = { 0 };
static char* deviceNameVars[] = { deviceNameVarLang1 };


PropertyStoreConfigEntry propertyStoreRuntimeValues[AJSVC_PROPERTY_STORE_NUMBER_OF_RUNTIME_KEYS] =
{
//  {"Buffers for Values per language", "Buffer Size"},                  "Key Name"
    { machineIdVars,             MACHINE_ID_LENGTH + 1 },               /*DeviceId*/
    { machineIdVars,             MACHINE_ID_LENGTH + 1 },               /*AppId*/
    { deviceNameVars,            DEVICE_NAME_VALUE_LENGTH + 1 }         /*DeviceName*/
// Add other runtime keys above this line
};

/**
 * AboutIcon Provisioning
 */

static const char* aboutIconMimetype = { "image/png" };
static const uint8_t aboutIconContent[] =
{ 0x89, 0x50, 0x4e, 0x47, 0x0d, 0x0a, 0x1a, 0x0a, 0x00, 0x00, 0x00, 0x0d, 0x49, 0x48, 0x44, 0x52 \
    , 0x00, 0x00, 0x00, 0x32, 0x00, 0x00, 0x00, 0x32, 0x02, 0x03, 0x00, 0x00, 0x00, 0x63, 0x51, 0x60 \
    , 0x22, 0x00, 0x00, 0x00, 0x0c, 0x50, 0x4c, 0x54, 0x45, 0x65, 0x2d, 0x67, 0xeb, 0x00, 0x88, 0xec \
    , 0x06, 0x8d, 0xf1, 0x44, 0xaa, 0x1f, 0x54, 0xd3, 0x5b, 0x00, 0x00, 0x00, 0x01, 0x74, 0x52, 0x4e \
    , 0x53, 0x00, 0x40, 0xe6, 0xd8, 0x66, 0x00, 0x00, 0x00, 0xe8, 0x49, 0x44, 0x41, 0x54, 0x28, 0xcf \
    , 0x95, 0x92, 0x51, 0x0a, 0xc3, 0x20, 0x0c, 0x86, 0xd3, 0xc0, 0x40, 0x84, 0xdd, 0x61, 0xec, 0x49 \
    , 0x72, 0x9f, 0xde, 0x47, 0xfa, 0x34, 0x3c, 0x45, 0x18, 0x85, 0x49, 0x4e, 0xb9, 0x18, 0xab, 0xd6 \
    , 0xb1, 0x97, 0x06, 0xb4, 0x7e, 0xa6, 0x26, 0xbf, 0x89, 0x00, 0x97, 0xec, 0xb6, 0x9e, 0xc9, 0x8b \
    , 0x0e, 0xee, 0x04, 0x40, 0x92, 0x1b, 0x49, 0x04, 0x7a, 0xcb, 0x01, 0x28, 0x20, 0xc4, 0xd4, 0x7c \
    , 0x0f, 0x90, 0x11, 0x04, 0x39, 0xd0, 0x29, 0x24, 0xd3, 0x39, 0x41, 0x0c, 0x53, 0x3e, 0x4c, 0x1b \
    , 0x4b, 0x4f, 0x87, 0x29, 0x65, 0x49, 0x7b, 0x89, 0x01, 0x64, 0x91, 0x44, 0xf6, 0x2a, 0xc4, 0x26 \
    , 0xf1, 0x1f, 0x5d, 0x10, 0xbb, 0xba, 0xe5, 0x77, 0x93, 0x15, 0x4c, 0x40, 0xb5, 0x64, 0xc1, 0x9a \
    , 0x66, 0x37, 0x91, 0x2d, 0x10, 0xda, 0xf5, 0x9e, 0xba, 0xc0, 0xad, 0x39, 0x31, 0xea, 0xc0, 0xfe \
    , 0xab, 0x2b, 0x5b, 0x9d, 0x42, 0x11, 0x3e, 0xd0, 0x68, 0x5c, 0x18, 0x13, 0x74, 0xf2, 0x01, 0x4b \
    , 0x71, 0xea, 0x95, 0x3d, 0x05, 0x56, 0xcc, 0x5a, 0xb9, 0xb2, 0x19, 0x20, 0xfb, 0xa8, 0x5f, 0x3e \
    , 0x0a, 0xcd, 0xc4, 0x07, 0x89, 0xd3, 0x84, 0xcd, 0xb7, 0xa8, 0x8b, 0x4c, 0x4f, 0x39, 0xb7, 0x68 \
    , 0xd6, 0x1a, 0xbc, 0xcc, 0xf7, 0x58, 0x7c, 0xad, 0x43, 0x77, 0x8d, 0xf3, 0xd2, 0x72, 0x0c, 0xd2 \
    , 0x16, 0x0d, 0x95, 0x34, 0x91, 0xfa, 0x46, 0x67, 0x21, 0x45, 0xcb, 0xd0, 0x1a, 0x56, 0xc7, 0x41 \
    , 0x7a, 0xc6, 0xe7, 0x89, 0xe4, 0x3f, 0x81, 0x51, 0xfc, 0x79, 0x3f, 0xc3, 0x96, 0xf5, 0xda, 0x5b \
    , 0x84, 0x2f, 0x85, 0x3b, 0x47, 0x0d, 0xe8, 0x0d, 0xca, 0xd3, 0x00, 0x00, 0x00, 0x00, 0x49, 0x45 \
    , 0x4e, 0x44, 0xae, 0x42, 0x60, 0x82, 0x82 };
static const size_t aboutIconContentSize = sizeof(aboutIconContent);
static const char* aboutIconUrl = { "https://www.alljoyn.org/sites/all/themes/at_alljoyn/images/img-alljoyn-logo.png" };

/**
 * Time functions
 */

#define NUM_OF_SECS_IN_MIN 60
#define NUM_OF_SECS_IN_HOUR (60 * NUM_OF_SECS_IN_MIN)
#define NUM_OF_SECS_IN_DAY (24 * NUM_OF_SECS_IN_HOUR)
#define NUM_OF_SECS_IN_YEAR (365 * NUM_OF_SECS_IN_DAY)
static uint32_t ACCUMULATED_DAYS_FOR_MONTH[] = { 0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365 };

static uint8_t LookupMonth(uint32_t daysOffset, uint8_t isLeapYear) {
    uint8_t month = 0;
    uint32_t previousMonthAccumulatedDays = ACCUMULATED_DAYS_FOR_MONTH[month];
    for (; month < sizeof(ACCUMULATED_DAYS_FOR_MONTH); month++) {
        uint32_t accumulatedDays = ACCUMULATED_DAYS_FOR_MONTH[month + 1] + (((month + 1) > 0 && isLeapYear) ? 1 : 0);
        if (daysOffset >= previousMonthAccumulatedDays && daysOffset < accumulatedDays) {
            break;
        }
        previousMonthAccumulatedDays = accumulatedDays;
    }
    return month + 1;
}

// note that this function is not a fully capable conversion function! do not use outside of this sample!
void AJTime2TSDateTime(const AJ_Time* aj_time, TS_Date* ts_date, TS_Time* ts_time)
{
#ifdef TIME_H_IMPLEMENTED
    time_t epochTime = aj_time->seconds;

    struct tm* parsedTime = gmtime(&epochTime);

    ts_date->year  = parsedTime->tm_year + 1900;
    ts_date->month = parsedTime->tm_mon + 1;
    ts_date->day   = parsedTime->tm_mday + 1;
    ts_time->hour  = parsedTime->tm_hour;
    ts_time->minute = parsedTime->tm_min;
    ts_time->second = parsedTime->tm_sec;
    ts_time->milliseconds = aj_time->milliseconds;
#else
    uint32_t yearsOffestFrom1970;
    uint32_t leapYearsFrom1970;
    uint8_t isLeapYear;
    uint32_t secsOffsetSinceStartOfYear;
    uint32_t daysOffsetSinceStartOfYear;
    uint32_t secsOffsetSinceStartOfMonth;
    uint32_t secsOffsetSinceStartOfDay;
    uint32_t secsOffsetSinceStartOfHour;

    yearsOffestFrom1970 = aj_time->seconds / NUM_OF_SECS_IN_YEAR;
    ts_date->year = 1970 + yearsOffestFrom1970;

    leapYearsFrom1970 = yearsOffestFrom1970 / 4; // number of leap years since 1970
    isLeapYear = ((yearsOffestFrom1970 - 2) % 4) == 0; // flag whether this is a leap year
    secsOffsetSinceStartOfYear = aj_time->seconds - yearsOffestFrom1970 * NUM_OF_SECS_IN_YEAR - leapYearsFrom1970 * NUM_OF_SECS_IN_DAY;
    daysOffsetSinceStartOfYear = secsOffsetSinceStartOfYear / NUM_OF_SECS_IN_DAY;
    ts_date->month = LookupMonth(daysOffsetSinceStartOfYear, isLeapYear);

    secsOffsetSinceStartOfMonth = secsOffsetSinceStartOfYear - (ACCUMULATED_DAYS_FOR_MONTH[ts_date->month - 1] * NUM_OF_SECS_IN_DAY);
    ts_date->day = secsOffsetSinceStartOfMonth / NUM_OF_SECS_IN_DAY + 1;

    secsOffsetSinceStartOfDay = secsOffsetSinceStartOfMonth - (ts_date->day - 1) * NUM_OF_SECS_IN_DAY;
    ts_time->hour = secsOffsetSinceStartOfDay / NUM_OF_SECS_IN_HOUR;

    secsOffsetSinceStartOfHour = secsOffsetSinceStartOfDay - ts_time->hour * NUM_OF_SECS_IN_HOUR;
    ts_time->minute = secsOffsetSinceStartOfHour / NUM_OF_SECS_IN_MIN;

    ts_time->second = secsOffsetSinceStartOfHour - ts_time->minute * NUM_OF_SECS_IN_MIN;

    ts_time->milliseconds = aj_time->milliseconds;
#endif
}

// return week day (1-7)
uint8_t TSDateTime2AJTime(const TS_Date* ts_date, const TS_Time* ts_time, AJ_Time* aj_time)
{
#ifdef TIME_H_IMPLEMENTED
    struct tm inputTime;

    memset(&inputTime, 0, sizeof(struct tm));

    inputTime.tm_year  =    ts_date->year - 1900;
    inputTime.tm_mon   =    ts_date->month - 1;
    inputTime.tm_mday  =    ts_date->day - 1;
    inputTime.tm_hour  =    ts_time->hour;
    inputTime.tm_min   =    ts_time->minute;
    inputTime.tm_sec   =    ts_time->second;

    time_t epochTime = timegm(&inputTime);

    aj_time->seconds = (uint32_t)epochTime;
    aj_time->milliseconds = ts_time->milliseconds;

    return inputTime.tm_wday + 1;
#else
    uint32_t yearsOffestFrom1970 = ts_date->year - 1970;
    uint32_t leapYearsFrom1970 = yearsOffestFrom1970 / 4; // number of leap years since 1970

    uint8_t isLeapYear = ((yearsOffestFrom1970 - 2) % 4) == 0; // flag whether this is a leap year
    uint32_t accumulatedDays = ACCUMULATED_DAYS_FOR_MONTH[ts_date->month - 1] + (((ts_date->month > 2) && isLeapYear) ? 1 : 0);

    uint8_t dayOfTheWeek = 4; // 1st January 1970 was a Thursday i.e. 4 in 0 index based

    aj_time->seconds = yearsOffestFrom1970 * NUM_OF_SECS_IN_YEAR + leapYearsFrom1970 * NUM_OF_SECS_IN_DAY;

    aj_time->seconds += accumulatedDays * NUM_OF_SECS_IN_DAY;

    aj_time->seconds += (ts_date->day - 1) * NUM_OF_SECS_IN_DAY;

    dayOfTheWeek = (dayOfTheWeek + aj_time->seconds / NUM_OF_SECS_IN_DAY) % 7;

    aj_time->seconds += ts_time->hour * NUM_OF_SECS_IN_HOUR;

    aj_time->seconds += ts_time->minute * NUM_OF_SECS_IN_MIN;

    aj_time->seconds += ts_time->second;

    aj_time->milliseconds = ts_time->milliseconds;

    return dayOfTheWeek + 1; // Since we are 1 index based and not 0 index based
#endif
}

void TestTimeConversion()
{
    TS_Time time;
    TS_Date date;
    AJ_Time ajtime;
    uint8_t dow;
    const char* WEEKDAYS[] = { "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat" };

    ajtime.seconds = 1409262479; // Thu, 28 Aug 2014 21:47:59 GMT
    ajtime.milliseconds = 66;
    AJ_AlwaysPrintf(("epoch %u.%03u\n", ajtime.seconds, ajtime.milliseconds));
    AJTime2TSDateTime(&ajtime, &date, &time);
    dow = TSDateTime2AJTime(&date, &time, &ajtime);
    AJ_AlwaysPrintf(("%s %02u/%02u/%02u %02u:%02u:%02u.%03u\n", WEEKDAYS[dow - 1], date.day, date.month, date.year, time.hour, time.minute, time.second, time.milliseconds));
    AJ_AlwaysPrintf(("epoch %u.%03u\n", ajtime.seconds, ajtime.milliseconds));
}

/*
 * Time Server Provisioning
 */

static TS_Date g_ts_date = { 1900, 1, 1 };
static TS_Time g_ts_time = { 0, 0, 0, 0 };
static int16_t g_utcOffset = 0;
static uint8_t g_isSet = FALSE;
static AJ_Time g_aj_time = { 0, 0 };
static AJ_Time g_lastSavedTime = { 0, 0 };

static void UpdateTime()
{
    uint32_t diff;

    if (!g_isSet) {
        return; // time is not set, no need to update yet

    }
    diff = AJ_GetElapsedTime(&g_aj_time, 0) / 1000;

    g_lastSavedTime.milliseconds = 0;
    g_lastSavedTime.seconds = 0;

    TSDateTime2AJTime(&g_ts_date, &g_ts_time, &g_lastSavedTime);

    g_lastSavedTime.seconds += diff;

    AJTime2TSDateTime(&g_lastSavedTime, &g_ts_date, &g_ts_time);
}

static AJ_Status GetDateTime(const uint8_t index, TS_Date* date, TS_Time* time, int16_t* utcOffset) {

    UpdateTime();

    memcpy(date, &g_ts_date, sizeof(TS_Date));
    memcpy(time, &g_ts_time, sizeof(TS_Time));
    *utcOffset = g_utcOffset;

    if (date->year == 0) {
        date->year = 1900;
        date->month = 1;
        date->day = 1;
    }

    printf("GetDateTime Called on index %d\n", index);
    return AJ_OK;
}

static AJ_Status SetDateTime(const uint8_t index, const TS_Date* date, const TS_Time* time, const int16_t* utcOffset) {

    AJ_InitTimer(&g_aj_time);

    memcpy(&g_ts_date, date, sizeof(TS_Date));
    memcpy(&g_ts_time, time, sizeof(TS_Time));
    g_utcOffset = *utcOffset;

    g_isSet = TRUE;

    printf("SetDateTime Called on index %d\n", index);
    return AJ_OK;
}


static AJ_Status IsSet(const uint8_t index, uint8_t*isSet) {
    *isSet = g_isSet;
    printf("IsSet Called on index %d\n", index);
    return AJ_OK;
}

static AJ_Status GetAuthorityType(const uint8_t index, uint8_t*authorityType) {

    *authorityType = 0;

    printf("GetAuthorityType Called on index %d\n", index);
    return AJ_OK;
}

static TS_Time g_ts_alarm_time;
static uint8_t g_alarm_weekdaysBitmap;
static char g_alarm_title[AJTS_MAX_TITLE_LENGTH + 1] = "";
static uint8_t g_alarm_enabled = FALSE;

static AJ_Status GetAlaramSchedule(const uint8_t index, TS_Time* time, uint8_t*weekdaysBitmap) {

    *time = g_ts_alarm_time;
    *weekdaysBitmap = g_alarm_weekdaysBitmap;

    printf("GetAlaramSchedule Called on index %d\n", index);
    return AJ_OK;
}

static AJ_Status SetAlaramSchedule(const uint8_t index, const TS_Time* time, const uint8_t weekdaysBitmap) {

    g_ts_alarm_time = *time;
    g_alarm_weekdaysBitmap = weekdaysBitmap;
    g_alarm_enabled = TRUE;


    printf("SetAlaramSchedule Called on index %d\n", index);
    return AJ_OK;
}

static AJ_Status GetAlaramTitle(const uint8_t index,  char title[AJTS_MAX_TITLE_LENGTH + 1]) {

    strncpy(title, g_alarm_title, AJTS_MAX_TITLE_LENGTH);

    printf("GetAlaramTitle Called on index %d\n", index);
    return AJ_OK;
}

static AJ_Status SetAlaramTitle(const uint8_t index, const char title[AJTS_MAX_TITLE_LENGTH + 1]) {

    strncpy(g_alarm_title, title, AJTS_MAX_TITLE_LENGTH);

    printf("SetAlaramTitle Called on index %d\n", index);
    return AJ_OK;
}

static AJ_Status GetAlaramEnabled(const uint8_t index,  uint8_t*isEnabled) {

    *isEnabled = g_alarm_enabled;

    printf("GetAlaramEnabled Called on index %d\n", index);
    return AJ_OK;
}

static AJ_Status SetAlaramEnabled(const uint8_t index, const uint8_t isEnabled) {

    g_alarm_enabled = isEnabled;

    printf("SetAlaramEnabled Called on index %d\n", index);
    return AJ_OK;
}

static AJ_Time g_aj_timer_time = { 0, 0 };
static TS_Timer_Period g_ts_timer_interval = { 0, 0, 0, 0 };
static uint8_t g_timer_runs = FALSE;
static uint16_t g_timer_repeat = 0;
static TS_Timer_Period g_ts_timer_timeleft = { 0, 0, 0, 0 };
static char g_timer_title[AJTS_MAX_TITLE_LENGTH + 1] = "";

void UpdateTimer(AJ_BusAttachment*busAttachment)
{
    if (g_timer_runs) {

        uint32_t elapsed = AJ_GetElapsedTime(&g_aj_timer_time, 0);

        uint32_t timerLeftMilliSec = g_ts_timer_timeleft.milliseconds + ((g_ts_timer_timeleft.second + g_ts_timer_timeleft.minute * NUM_OF_SECS_IN_MIN + g_ts_timer_timeleft.hour * 24 * NUM_OF_SECS_IN_HOUR) * 1000);

        if (elapsed >= timerLeftMilliSec) { // emit timer signal and substarct 1 from repeat count and reset timer
            if (busAttachment != NULL) {
                AJTS_Server_TimerEvent(busAttachment, AJTS_TIMER_OBJECT_INDEX);
            }

            if ((int16_t)-- g_timer_repeat > 0) {
                g_ts_timer_timeleft = g_ts_timer_interval;
            } else {
                g_timer_runs = FALSE;
                AJTS_Server_Timer_RunStateChanged(busAttachment, AJTS_TIMER_OBJECT_INDEX, FALSE);
                g_timer_repeat = 0;
            }

        } else {

            timerLeftMilliSec -= elapsed;

            g_ts_timer_timeleft.milliseconds = timerLeftMilliSec % 1000;

            timerLeftMilliSec /= 1000; // reduce to seconds

            g_ts_timer_timeleft.second = timerLeftMilliSec % 60;

            timerLeftMilliSec /= 60; // reduce to minutes

            g_ts_timer_timeleft.minute = timerLeftMilliSec % 60;

            timerLeftMilliSec /= 60; // reduce to hours

            g_ts_timer_timeleft.hour = timerLeftMilliSec;

        }
    }
}

static AJ_Status TimerStart(const uint8_t index) {

    g_timer_runs = TRUE;
    AJ_InitTimer(&g_aj_timer_time);

    AJTS_Server_Timer_RunStateChanged(&busAttachment, AJTS_TIMER_OBJECT_INDEX, TRUE);

    printf("TimerStart Called on index %d\n", index);
    return AJ_OK;
}

static AJ_Status TimerPause(const uint8_t index) {

    g_timer_runs = FALSE;

    AJTS_Server_Timer_RunStateChanged(&busAttachment, AJTS_TIMER_OBJECT_INDEX, FALSE);

    printf("TimerPause Called on index %d\n", index);
    return AJ_OK;
}

static AJ_Status TimerReset(const uint8_t index) {

    g_timer_runs = FALSE;
    g_ts_timer_timeleft = g_ts_timer_interval;

    printf("TimerReset Called on index %d\n", index);
    return AJ_OK;
}


static AJ_Status GetTimerInterval(const uint8_t index,  TS_Timer_Period* interval) {

    *interval = g_ts_timer_interval;

    printf("GetTimerInterval Called on index %d\n", index);
    return AJ_OK;
}

static AJ_Status SetTimerInterval(const uint8_t index, const TS_Timer_Period* interval) {

    g_ts_timer_interval = *interval;
    g_ts_timer_timeleft = *interval;

    printf("SetTimerInterval Called on index %d\n", index);
    return AJ_OK;
}

static AJ_Status GetTimerTimeLeft(const uint8_t index,  TS_Timer_Period* timeLeft) {

    UpdateTimer(NULL);

    *timeLeft = g_ts_timer_timeleft;

    printf("GetTimerTimeLeft Called on index %d\n", index);
    return AJ_OK;
}

static AJ_Status GetTimerIsRunning(const uint8_t index,  uint8_t*isRunning) {

    *isRunning = g_timer_runs;

    printf("GetTimerIsRunning Called on index %d\n", index);
    return AJ_OK;
}

static AJ_Status GetTimerRepeat(const uint8_t index,  uint16_t*repeat) {

    *repeat = g_timer_repeat;

    printf("GetTimerRepeat Called on index %d\n", index);
    return AJ_OK;
}

static AJ_Status SetTimerRepeat(const uint8_t index, const uint16_t*repeat) {

    g_timer_repeat = *repeat;

    printf("SetTimerRepeat Called on index %d\n", index);
    return AJ_OK;
}

static AJ_Status GetTimerTitle(const uint8_t index,  char title[AJTS_MAX_TITLE_LENGTH + 1]) {

    strncpy(title, g_timer_title, AJTS_MAX_TITLE_LENGTH);


    printf("GetTimerTitle Called on index %d\n", index);
    return AJ_OK;
}

static AJ_Status SetTimerTitle(const uint8_t index, const char title[AJTS_MAX_TITLE_LENGTH + 1]) {

    strncpy(g_timer_title, title, AJTS_MAX_TITLE_LENGTH);

    printf("SetTimerTitle Called on index %d\n", index);
    return AJ_OK;
}

static AJ_Status TimeServer_Init()
{

    AJ_Status status = AJTS_Server_Start(GetDateTime, SetDateTime, IsSet,
                                         GetAuthorityType,
                                         GetAlaramSchedule, SetAlaramSchedule, GetAlaramTitle,  SetAlaramTitle, GetAlaramEnabled, SetAlaramEnabled,
                                         GetTimerInterval, SetTimerInterval, GetTimerTimeLeft, GetTimerIsRunning, GetTimerRepeat, SetTimerRepeat, GetTimerTitle, SetTimerTitle,
                                         TimerStart, TimerPause, TimerReset);

    return status;
}

void AJTS_DoWork(AJ_BusAttachment* busAttachment)
{
    UpdateTime();

    if (g_alarm_enabled && g_isSet) {
        AJ_Time aj_alarm_time;

        int weekDay = TSDateTime2AJTime(&g_ts_date, &g_ts_alarm_time, &aj_alarm_time);

        uint8_t todayBitmap = (1 << (weekDay - 1));
        if ((g_alarm_weekdaysBitmap & todayBitmap) || (g_alarm_weekdaysBitmap == 0)) {
            if (aj_alarm_time.seconds < g_lastSavedTime.seconds) { // no check for milliseconds in sample

                AJ_AlwaysPrintf(("Emitting Alarm reached signal. aj_alarm_time.seconds:%u, g_lastSavedTime.seconds:%u\n", aj_alarm_time.seconds, g_lastSavedTime.seconds));
                //emit alarm
                AJTS_Server_AlarmReached(busAttachment, AJTS_ALARM_OBJECT_INDEX);

                g_alarm_weekdaysBitmap &= ~todayBitmap;     // note that in this sample we can only get alarms in the span of one week. every day the alarm is fired we take the day off the g_alarm_weekdaysBitmap variable.

                if (g_alarm_weekdaysBitmap == 0) {
                    g_alarm_enabled = FALSE;
                }
            }
        }
    }

    UpdateTimer(busAttachment);

//    t++;
//
//    t = t % 10;
//
//    if (!t) {
//        AJTS_Server_TimeAuthority_TimeSync(busAttachment, 1);
//    }
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

//    TestTimeConversion();
//    return(0);

    memset(&g_ts_date, 0, sizeof(TS_Date));
    memset(&g_ts_time, 0, sizeof(TS_Time));

    AJ_Initialize();

    AJ_AboutSetIcon(aboutIconContent, aboutIconContentSize, aboutIconMimetype, aboutIconUrl);

    status = PropertyStore_Init();
    if (status != AJ_OK) {
        goto Exit;
    }

    AJ_RegisterDescriptionLanguages(SUPPORTED_LANGUAGES);

    status = TimeServer_Init();
    if (status != AJ_OK) {
        goto Exit;
    }

    SetBusAuthPwdCallback(MyBusAuthPwdCB);

    while (TRUE) {
        status = AJ_OK;
        serviceStatus = AJSVC_SERVICE_STATUS_NOT_HANDLED;

        if (!isBusConnected) {
            status = AJSVC_RoutingNodeConnect(&busAttachment, ROUTING_NODE_NAME, AJAPP_CONNECT_TIMEOUT, AJAPP_CONNECT_PAUSE, AJAPP_BUS_LINK_TIMEOUT, &isBusConnected);
            if (!isBusConnected) { // Failed to connect to Routing Node.
                continue; // Retry establishing connection to Routing Node.
            }
            /* Setup password based authentication listener for secured peer to peer connections */
            AJ_BusSetPasswordCallback(&busAttachment, PasswordCallback);
        }

        status = AJApp_ConnectedHandler(&busAttachment);

        if (status == AJ_OK) {
            status = AJ_UnmarshalMsg(&busAttachment, &msg, AJAPP_UNMARSHAL_TIMEOUT);
            isUnmarshalingSuccessful = (status == AJ_OK);

            if (status == AJ_ERR_TIMEOUT) {
                if (AJ_ERR_LINK_TIMEOUT == AJ_BusLinkStateProc(&busAttachment)) {
                    status = AJ_ERR_READ;             // something's not right. force disconnect
                } else {                              // nothing on bus, do our own thing
                    AJTS_DoWork(&busAttachment);
                    continue;
                }
            }

            if (isUnmarshalingSuccessful) {
                if (serviceStatus == AJSVC_SERVICE_STATUS_NOT_HANDLED) {
                    serviceStatus = AJApp_MessageProcessor(&busAttachment, &msg, &status);
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
                AJApp_DisconnectHandler(&busAttachment, forcedDisconnnect);
                AJSVC_RoutingNodeDisconnect(&busAttachment, forcedDisconnnect, AJAPP_SLEEP_TIME, AJAPP_SLEEP_TIME, &isBusConnected);
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
