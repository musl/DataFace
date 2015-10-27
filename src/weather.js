/* vim: set nosta noet ts=4 sw=4 ft=javascript:
 *
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

var owm_url = 'http://api.openweathermap.org/data/2.5/weather';

var xhr = function (url, type, callback) {
    var req;
    
    req = new XMLHttpRequest();
    req.onload = function () { callback(this.responseText); };
    req.open(type, url);
    req.send();
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
    var appid, url;
    
    appid = localStorage.getItem('KEY_API');
    if(!appid) return;
    
    url = owm_url +
		  '?lat=' + pos.coords.latitude +
          '&lon=' + pos.coords.longitude +
          '&appid=' + appid;

    xhr(url, 'GET',  function(responseText) {
        var json;
        
		try {
        	json = JSON.parse(responseText);
			Pebble.sendAppMessage({
				'KEY_TEMPERATURE': Math.round(json.main.temp),
				'KEY_CONDITIONS': json.weather[0].main.replace(/[aeiou]/ig, '').substring(0, 4)
			});
		} catch(error) {
			console.log(error);
			Pebble.sendAppMessage({'KEY_WEATHER_FAIL': true});
		}
    });
}

function locationError(err) {
    console.log('Error requesting location: ' + err);
	Pebble.sendAppMessage({'KEY_WEATHER_FAIL': true});
}

function getWeather() {
    navigator.geolocation.getCurrentPosition(
        locationSuccess,
        locationError,
        {timeout: 15000, maximumAge: 60000}
    );
}

