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

// API Key Source: http://openweathermap.org/appid
var myAPIKey = "379d77efb8dd4fc21c0f896d53a1b75c";

var xhrRequest = function (url, type, callback) {
    var xhr;
    
    xhr = new XMLHttpRequest();
    xhr.onload = function () { callback(this.responseText); };
    xhr.open(type, url);
    xhr.send();
};

Pebble.addEventListener('ready', function(e) {
    //console.log('PebbleKit JS ready!');
    getWeather();
});

Pebble.addEventListener('appmessage', function(e) {
    //console.log('AppMessage received!');
    getWeather();
});

function locationSuccess(pos) {
    var url;
    
    url = 'http://api.openweathermap.org/data/2.5/weather?lat=' +
          pos.coords.latitude +
          '&lon=' + 
          pos.coords.longitude +
          '&appid=' +
          myAPIKey;

    xhrRequest(url, 'GET',  function(responseText) {
        var json, data;
        
        json = JSON.parse(responseText);
        //console.log(responseText);
        
        data = {
            'KEY_TEMPERATURE': Math.round(json.main.temp),
            'KEY_CONDITIONS': json.weather[0].main.replace(/[aeiouy]/ig, '').substring(0, 3)
        };
        
        /*
        console.log('Temperature is ' + data.KEY_TEMPERATURE);
        console.log('Conditions are ' + data.KEY_CONDITIONS);
        */
        Pebble.sendAppMessage(data, function(e) {
            //console.log('Weather info sent to Pebble successfully!');
        }, function(e) {
            //console.log('Error sending weather info to Pebble!');
        });
    });
}

function locationError(err) {
    //console.log('Error requesting location!');
}

function getWeather() {
    navigator.geolocation.getCurrentPosition(
        locationSuccess,
        locationError,
        {timeout: 15000, maximumAge: 60000}
    );
}

