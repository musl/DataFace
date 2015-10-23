/*
 * DataFace
 * A functional, minimal watchface for hacker-ish types.
 *
 * The MIT License (MIT)
 * 
 * Copyright (c) 2015 Michael Hix
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */


// The URL for the configuration page.
var url = 'https://hix.io/misc/dataface.html';


// Stuff we're going to put in local storage on the phone.
//
var keys = [
    'KEY_API',
    'KEY_TEMP_UNIT'
];


// Utility for checking for emptyness.
//
function isEmpty(obj) {
    if (obj === null) return true;
    if (obj.length === 0) return true;
    if (obj.length > 0) return false;
    
    for (var key in obj) {
        if (hasOwnProperty.call(obj, key)) return false;
    }
    
    return true;
}


// Read the current data out of local storage if available
// and display the configuration page.
//
Pebble.addEventListener('showConfiguration', function(e) {
    var  i, query, value;
    
    query = [];
    
    for(i = 0; i < keys.length; i++) {
        value = localStorage.getItem(keys[i]);
        if(value) query.push(keys[i] + '=' + encodeURIComponent(value));
    }
    
    if(query.length === 0) {
        Pebble.openURL(url);
        return;
    }
    
    Pebble.openURL(url + '?' + query.join('&'));
});


// Handle configuration changes.
//
Pebble.addEventListener('webviewclosed', function(e) {
    var data, config;
    
    config = {};
    data = JSON.parse(decodeURIComponent(e.response));
    //console.log('Config window returned: ', JSON.stringify(config));

    // Only store keys we know about.
    //
    for(var i = 0; i < keys.length; i++) {
        if(data[keys[i]]) {
            localStorage.setItem(keys[i], data[keys[i]]);
            config[keys[i]] = data[keys[i]];
        }
    }
    
    if(isEmpty(config)) return;

    Pebble.sendAppMessage(config, function() {
        console.log('Sent config data to Pebble');  
    }, function() {
        console.log('Failed to send config data!');
    });
});

