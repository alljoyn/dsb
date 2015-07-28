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

#include <aj_target.h>
#include <aj_status.h>
#include <aj_util.h>
#include <stdlib.h>
#include "TimeClientSampleUtil.h"

#define BUF_SIZE 500

static uint8_t GetCmdLine(char* buf, size_t num)
{
    while (AJ_GetCmdLine(buf, num) == NULL) {
    }
    return (strlen(buf) > 0);
}

void Client_GetActionFromUser(char*servername, uint8_t* action)
{
    char buf[BUF_SIZE];
    uint8_t defaultAction = *action;
    AJ_StartReadFromStdIn();
    AJ_Printf("Please enter an action to perform on the time server %s\n\t0=set datetime,\n\t1=get datetime,\n\t2=set timer,\n\t3=get timer\n\t4=set alarm,\n\t5=get alarm,\n\t6=pause timer,\n\t7=reset timer\n", servername);
    AJ_Printf("Empty string or invalid input will default to %u\n", defaultAction);
    if (GetCmdLine(buf, BUF_SIZE)) {
        char stringType[10];
        *action = (uint8_t)atoi(buf);
        sprintf(stringType, "%u", *action);
        if (!(strcmp(buf, stringType) == 0)) {             //they do not match, it is not int
            AJ_Printf("Action is not an integer value. Defaulting to %u\n", defaultAction);
            *action = defaultAction;
        }
    }
    AJ_StopReadFromStdIn();
}
