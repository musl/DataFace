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

DataFace.Weather = (function() {

	var request, response, eelf;

	request = function(lat, lon) {
		var appid, url;

		url = localStorage.getItem('KEY_CONFIG_API_URL');
		if(!url) return;

		appid = localStorage.getItem('KEY_CONFIG_API_KEY');
		if(!appid) return;

		url += '?lat=' + lat + '&lon=' + lon + '&appid=' + appid;

		//console.log('URL: ' + url);

		xhr(url, 'GET', response);
	};

	response = function(body) {

		//console.log("Body: " + body);
		
		try {
			var json = JSON.parse(body);
			Pebble.sendAppMessage({
				'KEY_TEMPERATURE': Math.round(json.main.temp),
				'KEY_CONDITIONS': json.weather[0].main.replace(/[aeiou]/ig, '').substring(0, 4)
			});
		} catch(error) {
			Pebble.sendAppMessage({
				'KEY_WEATHER_FAIL': true
			});
		}
	};

	self = {
		get: function() {
			var location;

			location = localStorage.getItem('KEY_CONFIG_LOCATION') || "";
			location = location.split(',');

			//console.log("Location: " + location);

			if(location.length == 2) {
				request(location[0].trim(), location[1].trim());
			} else {
				DataFace.Location.get(function(pos) {
					request(pos.coords.latitude, pos.coords.longitude);
				});
			}
		},
	};

	// Hook up events.
	//
	Pebble.addEventListener('ready', function(e) {
		self.get();
	});

	Pebble.addEventListener('appmessage', function(e) {
		if(e.payload[0] == 0) self.get();
	});

	return self;

})();

