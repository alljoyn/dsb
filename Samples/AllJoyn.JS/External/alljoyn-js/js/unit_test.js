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

var args_object = {
    sessionless_sig1: "sessionless_sig1",
    sessionless_sig2: 123456,
    sessionless_sig3: 1234.5678,
    sessionless_sig4: true,
    sessionless_sig5: ['one', 'two', 'three', 'four', 'five', 'six'],
    sessionless_sig6: [1111, 2222, 3333, 4444, 5555, 6666],
    sessionless_sig7: [111.111, 222.222, 333.333, 444.444, 555.555, 666.666],
    sessionless_sig8: [true, false, true, false, true, false]
}

AJ.interfaceDefinition['org.alljoyn.unit_test'] =
{
    sessionless_sig1:{ type:AJ.SIGNAL, args:["s"] },
    sessionless_sig2:{ type:AJ.SIGNAL, args:["u"] },
    sessionless_sig3:{ type:AJ.SIGNAL, args:["d"] },
    sessionless_sig4:{ type:AJ.SIGNAL, args:["b"] },
    sessionless_sig5:{ type:AJ.SIGNAL, args:["as"] },
    sessionless_sig6:{ type:AJ.SIGNAL, args:["au"] },
    sessionless_sig7:{ type:AJ.SIGNAL, args:["ad"] },
    sessionless_sig8:{ type:AJ.SIGNAL, args:["ab"] },
};

AJ.objectDefinition['/org/alljoyn/unit_test'] =
{
    interfaces:['org.alljoyn.unit_test']
};

var MAX_SIG_ITERATIONS = 5;
var self_sls_count = 0;
var sls_count = 0;

AJ.onSignal = function()
{
    if (this.member == 'sessionless_sig1') {
        if (arguments[0] == args_object.sessionless_sig1) {
            print('PASS: Recv: sessionless_sig1');
            self_sls_count++;
        } else {
            alert('ERROR: sessionless_sig1: invalid argument: ' + arguments[0]);
        }
    } else if (this.member == 'sessionless_sig2') {
        if (arguments[0] == args_object.sessionless_sig2) {
            print('PASS: Recv: sessionless_sig2');
            self_sls_count++;
        } else {
            alert('ERROR: sessionless_sig2: invalid argument: ' + arguments[0]);
        }
    } else if (this.member == 'sessionless_sig3') {
        if (arguments[0] == args_object.sessionless_sig3) {
            print('PASS: Recv: sessionless_sig3');
            self_sls_count++;
        } else {
            alert('ERROR: sessionless_sig3: invalid argument: ' + arguments[0]);
        }
    } else if (this.member == 'sessionless_sig4') {
        if (arguments[0] == args_object.sessionless_sig4) {
            print('PASS: Recv: sessionless_sig4');
            self_sls_count++;
        } else {
            alert('ERROR: sessionless_sig1: invalid argument: ' + arguments[0]);
        }
    } else if (this.member == 'sessionless_sig5') {
        var pass = true;
        for (var i = 0; i < arguments[0].length; i++) {
            if (arguments[0][i] != args_object.sessionless_sig5[i]) {
                alert('ERROR: sessionless_sig5: invalid argument: ' + arguments[0]);
                pass = false;
            }
        }
        if (pass) {
            print('PASS: Recv: sessionless_sig5');
            self_sls_count++;
        }
    } else if (this.member == 'sessionless_sig6') {
        var pass = true;
        for (var i = 0; i < arguments[0].length; i++) {
            if (arguments[0][i] != args_object.sessionless_sig6[i]) {
                alert('ERROR: sessionless_sig6: invalid argument: ' + arguments[0]);
                pass = false;
            }
        }
        if (pass) {
            print('PASS: Recv: sessionless_sig6');
            self_sls_count++;
        }
    } else if (this.member == 'sessionless_sig7') {
        var pass = true;
        for (var i = 0; i < arguments[0].length; i++) {
            if (arguments[0][i] != args_object.sessionless_sig7[i]) {
                alert('ERROR: sessionless_sig7: invalid argument: ' + arguments[0]);
                pass = false;
            }
        }
        if (pass) {
            print('PASS: Recv: sessionless_sig7');
            self_sls_count++;
        }
    } else if (this.member == 'sessionless_sig8') {
        var pass = true;
        for (var i = 0; i < arguments[0].length; i++) {
            if (arguments[0][i] != args_object.sessionless_sig8[i]) {
                alert('ERROR: sessionless_sig8: invalid argument: ' + arguments[0]);
                pass = false;
            }
        }
        if (pass) {
            print('PASS: Recv: sessionless_sig8');
            self_sls_count++;
        }
    } else if (this.member == 'notif') {
        JSON.stringify(arguments);
    }
}

function sendSLS(iter)
{
    if (iter >= MAX_SIG_ITERATIONS) {
        print('PASS: sessionless_sig - count: ' + sls_count);
        return;
    }
    AJ.signal('/org/alljoyn/unit_test', {sessionless_sig1:'org.alljoyn.unit_test'}).send(args_object.sessionless_sig1);
    AJ.signal('/org/alljoyn/unit_test', {sessionless_sig2:'org.alljoyn.unit_test'}).send(args_object.sessionless_sig2);
    AJ.signal('/org/alljoyn/unit_test', {sessionless_sig3:'org.alljoyn.unit_test'}).send(args_object.sessionless_sig3);
    AJ.signal('/org/alljoyn/unit_test', {sessionless_sig4:'org.alljoyn.unit_test'}).send(args_object.sessionless_sig4);
    AJ.signal('/org/alljoyn/unit_test', {sessionless_sig5:'org.alljoyn.unit_test'}).send(args_object.sessionless_sig5);
    AJ.signal('/org/alljoyn/unit_test', {sessionless_sig6:'org.alljoyn.unit_test'}).send(args_object.sessionless_sig6);
    AJ.signal('/org/alljoyn/unit_test', {sessionless_sig7:'org.alljoyn.unit_test'}).send(args_object.sessionless_sig7);
    AJ.signal('/org/alljoyn/unit_test', {sessionless_sig8:'org.alljoyn.unit_test'}).send(args_object.sessionless_sig8);
    sls_count++;
    sendSLS(iter + 1);
}

AJ.onAttach = function()
{
    AJ.addMatch('org.alljoyn.unit_test', 'sessionless_sig1');
    AJ.addMatch('org.alljoyn.unit_test', 'sessionless_sig2');
    AJ.addMatch('org.alljoyn.unit_test', 'sessionless_sig3');
    AJ.addMatch('org.alljoyn.unit_test', 'sessionless_sig4');
    AJ.addMatch('org.alljoyn.unit_test', 'sessionless_sig5');
    AJ.addMatch('org.alljoyn.unit_test', 'sessionless_sig6');
    AJ.addMatch('org.alljoyn.unit_test', 'sessionless_sig7');
    AJ.addMatch('org.alljoyn.unit_test', 'sessionless_sig8');
    print("Starting SLS test");
    sendSLS(0);
}

var store_obj = {
    int_val:100,
    str_val:"string",
    obj_val:{
        obj_int:10,
        obj_str:"obj_string"
    }
}

var store_int = 60;
var store_str = "simple_string"
var nvram_pass = true;
    
    
print("===== Starting Unit Tests =====");
/*
 * Store/Load tests
 */
AJ.store('int_store', store_int);
AJ.store('str_store', store_str);
AJ.store('obj_store', store_obj);

var out_int = AJ.load('int_store');
if (out_int != store_int) {
    alert('Store/Load test error: integer value did not match');
    nvram_pass = false;
}
var out_str = AJ.load('str_store');
if (out_str != store_str) {
    alert('Store/Load test error: string value did not match');
    nvram_pass = false;
}
var out_obj = AJ.load('obj_store');
if (out_obj.int_val != store_obj.int_val) {
    alert('Store/Load test error: object.int_val value did not match');
    nvram_pass = false;
}
if (out_obj.str_val != store_obj.str_val) {
    alert('Store/Load test error: object.str_val value did not match');
    nvram_pass = false;
}
if (out_obj.obj_val.obj_int != store_obj.obj_val.obj_int) {
    alert('Store/Load test error: object.obj_val.obj_int value did not match');
    nvram_pass = false;
}
if (out_obj.obj_val.obj_str != store_obj.obj_val.obj_str) {
    alert('Store/Load test error: object.obj_val.obj_str value did not match');
    nvram_pass = false;
}

if (nvram_pass == false) {
    alert('ERROR: Store/Load test failed');
}
/*
 * Time tests
 */
var counter = 0;

function checkCounter()
{
    if (counter > 5) {
        print("Error, setInterval was called too many times");
        return;
    }
    clearInterval(interval);
    print('PASS: Counter was correct');
}

function incCounter()
{
    counter++;
}
/*
 * Set a counter to increment every 100ms
 * Set a timeout for 500ms
 * if the counter is > 5 then timers don't work correctly.
 */
var interval = setInterval(incCounter, 100);
setTimeout(checkCounter, 500);

