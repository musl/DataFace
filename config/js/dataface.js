/*
 * Namespace, IIFE, and all we need to run. No JS in the markup, and
 * no markup in the JS. VanillaJS FTW!
 *
 * Put at the end of the body tag.
 */
var DataFace = (function() {
	var config, inputs, keys, self, save_button;

	// Storage for configuration options.
	//
	config = {
		return_to: 'pebblejs://close#'
	};

	// Singleton instance to get/set config.
	//
	self = {
		get: function(key) {
			return config[key];
		},
		set: function(key, value) {
			config[key] = value;
		},
		close: function() {
			var uri, base;
			
			uri = config.return_to;
			delete config.return_to;
			uri += encodeURIComponent(JSON.stringify(config));
			console.log(uri);
			location.href = uri;
		},
		cancel: function() {
			location.href = config.return_to + "%7B%7D";
		},
		bind_button: function(id, fn) {
			document.getElementById(id).addEventListener('click', fn.bind(this));
		},
		bind_tag: function(tag) {
			elements = document.getElementsByTagName(tag);
			for(var i = 0; i < elements.length; i++) {
				var element, fn;

				element = elements[i];
				fn = function(event) {
					this.set(event.target.id, event.target.value);
					//console.log('Set: ', event.target.id, ' to: ', this.get(event.target.id)); 
					//console.log(config);
				}.bind(this);
				element.addEventListener('change', fn);

				if(config[element.id]) element.value = config[element.id];
			}
		}
	};

	// Load config from the app.
	//
	keys = location.search.substring(1).split('&');
	for(var i = 0; i < keys.length; i++) {
		var pair = keys[i].split('=');
		if(pair[0] === "") continue;
		config[pair[0]] = decodeURIComponent(pair[1]);
	}
	
	// Hook up events.
	//
	self.bind_tag('input');
	self.bind_tag('select');
	self.bind_button('cancel', self.cancel);
	self.bind_button('save', self.close);

	// Fire in the hole!
	return self;
})();

