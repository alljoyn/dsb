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
var AJ = require('AllJoyn');

AJ.interfaceDefinition['org.alljoyn.event_test'] =
{
    my_event:{ type:AJ.SIGNAL, description:'This is a test event' },
};

AJ.objectDefinition['/org/alljoyn/event_test'] = {
    interfaces:['org.alljoyn.event_test']
};

var sendInterval;

AJ.onAttach = function()
{
    print('Attached');
    sendInterval = setInterval(function() {
           var myEvent = AJ.signal('/org/alljoyn/event_test', 'my_event');
           myEvent.sessionless = true;
           myEvent.timeToLive = 30;
           myEvent.send();
        }     
        , 5000);
}

AJ.onDetach = function()
{
    clearInterval(sendInterval);
}


