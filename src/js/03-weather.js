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


DataFace.WeatherProvider = function(impl) {
	var common, self;

	self = this;

	common = {
		method: 'GET',
		url_template: '',
		url: function(params) {
			var url;

			url = self.impl.url_template;
			for (var key in params) {
				if (params.hasOwnProperty(key)) {
					url = url.replace('{' + key + '}', params[key]);
				}
			}

			//console.log('url: ' + url);
			return url;
		},
		fetch: function(params, fn) {
			//console.log('fetch: ' + JSON.stringify(params));
			xhr(self.impl.url(params), self.impl.method, self.impl.process(fn));
		},
		process: function(fn) {
			return function(body) {
				//console.log('process: ' + body);
				fn(self.impl.unpack(body));
			}
		},
		unpack: function(body) {
			return body;
		}
	};

	// Apparently Object.assign isn't a thing in pebblejs...
	// this.impl = Object.assign({}, common, impl);

	// ACK, BY HAND, YOU HEATHENS
	this.impl = common;
	if(impl.url_template) this.impl.url_template = impl.url_template;
	if(impl.unpack) this.impl.unpack = impl.unpack;
	if(impl.method) this.impl.method = impl.method;
};

DataFace.WeatherProvider.prototype.fetch = function(params, fn) {
	return this.impl.fetch(params, fn);
};

DataFace.squash_summary = function(str) {
	var s, l, n, changed;

	// Width of the display field.
	n = 4;

	// Only operate on a limited number of characters to avoid
	// problems.
	s = str.substr(0, 64);

	// Replace common word separators with spaces.
	s = s.replace(/[_\-]/g, ' ');

	// Normalize words.
	s = s.replace(/[^a-z ]/ig, '');

	// Reduce the length of the summary as much as possible by
	// removing the most frequently used vowels in order of
	// relative frequency of use in English.
	// 
	l = '';
	while (s.length > n && s !== l) {
		l = s;
		s = s.replace(/\B[eaoiuy]/i, '');
	}

	// Replace descenders for readability reasons.
	// 
	s = s.replace(/[gjpqy]/g, function(l) {
		return l.toUpperCase();
	});

	// Split words and take substrings of words to make a word
	// that fits into the display width. This is somewhat subjective
	// and based on my preferences and limited testing.
	// 
	s = s.trim().split(' ', 2);
	switch (s.length) {
		case 1:
			s = s[0].substring(0, n);
			break;
		case 2:
			s = s[0].substring(0, 1) + s[1].substring(0, n - 1);
			break;
	}

	// console.log(str + ' -> ' + s);
	return s;
};

DataFace.WeatherProviders = {
	dark_sky: new DataFace.WeatherProvider({
		// Requires HTTPS
		url_template: 'https://api.darksky.net/forecast/{key}/{lat},{lon}',
		unpack: function(body) {
			//console.log('ds unpack');
			var json = JSON.parse(body);
			var obj = {
				'KEY_TEMPERATURE': Math.round((parseFloat(json['currently']['temperature']) - 32.0) * 5.0 / 9.0 + 273.15),
				'KEY_CONDITIONS': DataFace.squash_summary(json['currently']['summary']),
			};

			return obj;
		},
	}),
	open_weather_map: new DataFace.WeatherProvider({
		// Disallows HTTPS
		url_template: 'http://api.openweathermap.org/data/2.5/weather/?lat={lat}&lon={lon}&appid={key}',
		unpack: function(body) {
			//console.log('ds unpack');
			var json = JSON.parse(body);
			var obj = {
				'KEY_TEMPERATURE': Math.round(json.main.temp),
				'KEY_CONDITIONS': DataFace.squash_summary(json.weather[0].main),
			};

			return obj;
		},
	}),
	weather_underground: new DataFace.WeatherProvider({
		// Optional HTTPS
		url_template: 'https://api.wunderground.com/api/{key}/conditions/q/{lat},{lon}.json',
		unpack: function(body) {
			//console.log('ds unpack');
			var json = JSON.parse(body);
			var obj = {
				'KEY_TEMPERATURE': Math.round((parseFloat(json['current_observation']['temp_f']) - 32.0) * 5.0 / 9.0 + 273.15),
				'KEY_CONDITIONS': DataFace.squash_summary(json['current_observation']['weather']),
			};

			return obj;
		},
	}),
};

DataFace.Weather = (function() {

	var send_message, self;

	send_message = function(message) {
		//console.log("Body: " + JSON.stringify(body));

		try {
			Pebble.sendAppMessage(message);
		} catch(error) {
			Pebble.sendAppMessage({ 'KEY_WEATHER_FAIL': true });
		}
	};

	self = {
		get: function() {
			var api, key, location, obj, provider;

			api = localStorage.getItem('KEY_CONFIG_API');
			if(!api) {
				console.log('Unable to load KEY_CONFIG_API');
				return
			}

			provider = DataFace.WeatherProviders[api];
			if(!provider) {
				console.log('Unable find provider for: ' + api);
				return
			}

			key = localStorage.getItem('KEY_CONFIG_API_KEY');
			if(!key) {
				console.log('Unable to load KEY_CONFIG_API_KEY');
				return
			}

			location = localStorage.getItem('KEY_CONFIG_LOCATION') || "";
			location = location.split(',');

			//console.log(api, provider.url, location);

			if(location.length == 2) {
				provider.fetch({lat: ('' + location[0]).trim(), lon: ('' + location[1]).trim(), key: key}, send_message);
			} else {
				DataFace.Location.get(function(pos) {
					provider.fetch({lat: pos.coords.latitude, lon: pos.coords.longitude, key: key}, send_message);
				});
			}
		},
	};

	Pebble.addEventListener('ready', function(e) {
		//console.log('ready');
		self.get();
	});

	Pebble.addEventListener('appmessage', function(e) {
		//console.log('appmessage: ' + e.payload[0]);
		if(e.payload[0] == 0) self.get();
	});

	return self;

})();

