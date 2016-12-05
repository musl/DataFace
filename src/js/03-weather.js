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

      console.log('url: ' + url);
      return url;
    },
    fetch: function(params, fn) {
      console.log('fetch: ' + JSON.stringify(params));
      xhr(self.impl.url(params), self.impl.method, self.impl.process(fn));
    },
    process: function(fn) {
      return function(body) {
        console.log('process: ' + body);
        fn(self.impl.unpack(body));
      }
    },
    unpack: function(body) {
      return body;
    }
  };

  this.impl = Object.assign({}, common, impl);
};

DataFace.WeatherProvider.prototype.fetch = function(params, fn) {
  return this.impl.fetch(params, fn);
};

DataFace.WeatherProviders = {
  dark_skies: new DataFace.WeatherProvider({
    // Requires HTTPS
    url_template: 'https://api.darksky.net/forecast/{key}/{lat},{lon}',
    unpack: function(body) {
      var json = JSON.parse(body);
      var obj = {
        'KEY_TEMPERATURE': Math.round((parseFloat(json['currently']['temperature']) - 32.0) * 5.0 / 9.0 + 273.15),
        'KEY_CONDITIONS': json['currently']['summary'].replace(/[aeiouyl ]/ig, '').substring(0, 4)
      };

      return obj;
    },
  }),
  open_weather_map: new DataFace.WeatherProvider({
    // Disallows HTTPS
    url_template: 'http://api.openweathermap.org/data/2.5/weather/?lat={lat}&lon={lon}&appid={key}',
    unpack: function(body) {
      var json = JSON.parse(body);
      var obj = {
        'KEY_TEMPERATURE': Math.round(json.main.temp),
        'KEY_CONDITIONS': json.weather[0].main.replace(/[aeiouyl ]/ig, '').substring(0, 4),
      };

      return obj;
    },
  }),
  weather_underground: new DataFace.WeatherProvider({
    // Optional HTTPS
    url_template: 'https://api.wunderground.com/api/{key}/conditions/q/{lat},{lon}.json',
    unpack: function(body) {
      var json = JSON.parse(body);
      var obj = {
        'KEY_TEMPERATURE': Math.round((parseFloat(json['current_observation']['temp_f']) - 32.0) * 5.0 / 9.0 + 273.15),
        'KEY_CONDITIONS': json['current_observation']['weather'].replace(/[aeiouyl ]/ig, '').substring(0, 4)
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
			if(!api) return;

			provider = DataFace.WeatherProviders[api];
			if(!provider) return;

			key = localStorage.getItem('KEY_CONFIG_API_KEY');
			if(!appid) return;

			location = localStorage.getItem('KEY_CONFIG_LOCATION') || "";
			location = location.split(',');

			if(location.length == 2) {
				obj = provider.fetch({lat: location[0].trim(), lon: location[1].trim(), key: key}, send_message);
			} else {
				DataFace.Location.get(function(pos) {
					provider.fetch({lat: location[0].trim(), lon: location[1].trim(), key: key}, send_message);
				});
			}
		},
	};

	Pebble.addEventListener('ready', function(e) {
		self.get();
	});

	Pebble.addEventListener('appmessage', function(e) {
		if(e.payload[0] == 0) self.get();
	});

	return self;

})();

