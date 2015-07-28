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

#ifndef _TIMESERVICECOMMON_H_
#define _TIMESERVICECOMMON_H_



/** @defgroup TimeServiceCommon Time Service Framework
 *
 *  @{
 */

/** Time service port */
#define AJTS_Port 1030

/** Maximum object path length */
#define AJTS_MAX_OBJ_PATH_LENGTH 256

/** Max title length */
#define AJTS_MAX_TITLE_LENGTH 32

/**
 * Time service Data structure
 */
typedef struct _TS_Date {
    uint16_t year; /**< year */
    uint8_t month; /**< month */
    uint8_t day;   /**< day */
} TS_Date;

/**
 * Time service Time structure
 */
typedef struct _TS_Time {
    uint8_t hour;   /**< hour */
    uint8_t minute; /**< minute */
    uint8_t second; /**< second */
    uint16_t milliseconds; /**< milliseconds */
} TS_Time;

/**
 * Time service Period structure for the timer functions
 */
typedef struct _TS_Timer_Period {
    uint32_t hour; /**< hour */
    uint8_t minute; /**< minute */
    uint8_t second; /**< second */
    uint16_t milliseconds; /**< milliseconds */
} TS_Timer_Period;

// alarm weekdays bitmap

#define TS_ALARM_SUNDAY       0x01 /**< bitmap for sunday */
#define TS_ALARM_MONDAY       0x02 /**< bitmap for monday */
#define TS_ALARM_TUESDAY      0x04 /**< bitmap for tuesday */
#define TS_ALARM_WEDNESDAY    0x08 /**< bitmap for wednesday */
#define TS_ALARM_THURSDAY     0x10 /**< bitmap for thursday */
#define TS_ALARM_FRIDAY       0x20 /**< bitmap for friday */
#define TS_ALARM_SATURDAY     0x40 /**< bitmap for saturday */

extern const uint16_t AJTS_Version;

/**
 * create a declaration of secured interface by adding $ to the interface name string
 */
#define MAKE_SECURE_IFACE_NAME(name) "$"name

/**
 * Time authority interface name
 */
#define AJTS_TIMEAUTHORITY_IFACE_NAME "org.allseen.Time.TimeAuthority"

/** time authority interface definition extern */
extern const char* const AJTS_TimeAuthorityInterface[];

/**
 * Clock interface name
 */
#define AJTS_CLOCK_IFACE_NAME "org.allseen.Time.Clock"

/** clock interface definition extern */
extern const char* const AJTS_TimeClockInterface[];

/** time authority interface extern */
extern const AJ_InterfaceDescription AJTS_TimeAuthorityInterfaces[];

/** clock interfaces extern */
extern const AJ_InterfaceDescription AJTS_ClockInterfaces[];

/**
 * Alarm interface name
 */
#define AJTS_ALARM_IFACE_NAME "org.allseen.Time.Alarm"

/** alarm interface definition extern */
extern const char* const AJTS_AlarmInterface[];

/** alarm interfaces extern */
extern const AJ_InterfaceDescription AJTS_AlarmInterfaces[];


/**
 * Timer interface name
 */
#define AJTS_TIMER_IFACE_NAME "org.allseen.Time.Timer"

/** timer interface definition extern */
extern const char* const AJTS_TimerInterface[];

/** timer interfaces extern */
extern const AJ_InterfaceDescription AJTS_TimerInterfaces[];

/**
 * extern to all of the time service objects. this include both the server and client ('proxy') sides
 */
extern AJ_Object AJTS_ObjectList[];

/* server side */
//if the below is changed, you need to change the AJTS_TimeClocks array accordingly


#define AJTS_NUM_OF_CLOCKS 2 /**< number of clocks, one is regular clock and the other time authority */

#define AJTS_NUM_OF_ALARMS 1 /**< number of alarms, one is regular clock and the other time authority */

#define AJTS_NUM_OF_TIMERS 1 /**< number of timers, one is regular clock and the other time authority */

#define AJTS_OBJECT_INDEX  0 /**< first object in the aj objects  */

#define AJTS_CLOCK_OBJECT_INDEX                             AJTS_OBJECT_INDEX               /**< index of the first clock  */

#define AJTS_CLOCK_AUTHORITY_OBJECT_INDEX                   AJTS_CLOCK_OBJECT_INDEX + 1     /**< index of the second clock */

#define AJTS_ALARM_OBJECT_INDEX                             AJTS_CLOCK_OBJECT_INDEX + AJTS_NUM_OF_CLOCKS    /**< index of the first alarm service */

#define AJTS_TIMER_OBJECT_INDEX                             AJTS_ALARM_OBJECT_INDEX + AJTS_NUM_OF_ALARMS    /**< index of the first timer service */

#define AJTS_SERVER_NUM_OF_OBJECTS  (AJTS_NUM_OF_CLOCKS + AJTS_NUM_OF_ALARMS + AJTS_NUM_OF_TIMERS)          /**< number of the object entries in AJTS_ObjectList */

/* client side */

#define AJTS_CLIENT_OBJECT_INDEX (AJTS_SERVER_NUM_OF_OBJECTS)   /**< first index of the client ('proxy') object in AJTS_ObjectList */

#define AJTS_CLOCK_PROXY_OBJECT_INDEX                   AJTS_CLIENT_OBJECT_INDEX    /**< first index of the clock proxy object in AJTS_ObjectList */

#define AJTS_TIME_AUTHORITY_PROXY_OBJECT_INDEX          AJTS_CLOCK_PROXY_OBJECT_INDEX + 1   /**< first index of the time authority proxy object in AJTS_ObjectList */

#define AJTS_ALARM_PROXY_OBJECT_INDEX                   AJTS_TIME_AUTHORITY_PROXY_OBJECT_INDEX + 1  /**< first index of the alarm proxy object in AJTS_ObjectList */

#define AJTS_TIMER_PROXY_OBJECT_INDEX                   AJTS_ALARM_PROXY_OBJECT_INDEX + 1   /**< first index of the timer proxy object in AJTS_ObjectList */


/** @} */ // End of group 'TimeServiceCommon'
#endif // _TIMESERVICECOMMON_H_
