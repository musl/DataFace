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

DataFace.Config = (function(){

	var keys, self, url;
	
	// The URL for the configuration page.
   	url = 'https://hix.io/misc/dataface/';

	// Stuff we're going to put in local storage on the phone.
	//
	keys = [
		'KEY_CONFIG_API_KEY',
		'KEY_CONFIG_TEMP_UNIT',
		'KEY_CONFIG_LOCATION',
		'KEY_CONFIG_THEME'
	];

	self = {

		// Open the configuration URL, passing configuration as URI
		// encoded query parameters.
		//
		open: function() {
			var query, str; 

			query = [];

			for(i = 0; i < keys.length; i++) {
				value = localStorage.getItem(keys[i]);
				if(value) query.push(keys[i] + '=' + encodeURIComponent(value));
			}

			if(query.length === 0) Pebble.openURL(url);

		 	Pebble.openURL(url + '?' + query.join('&'));
		},

		// When the configuration page has been closed, receive any
		// configurtion items that are passed back in the URI string.
		//
		close: function(e) {
			var data, config;

			//console.log('Window returned: ' + e.response);

			config = {};
			data = JSON.parse(decodeURIComponent(e.response));

			// Only store and send keys we know about.
			//
			for(var i = 0; i < keys.length; i++) {
				if(typeof data[keys[i]] !== 'undefined') {
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
		}
	};

	// Hook up events.
	//
	Pebble.addEventListener('showConfiguration', self.open);
	Pebble.addEventListener('webviewclosed', self.close);

	return self;

})();

