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

DataFace.Location = (function() {

	var self;

	self = {
		get: function(success, error) {
			success = success || function(pos) {
				console.log('Location: ' + pos);
			};

			error = error || function(err) {
				console.log('Error requesting location: ' + err);
			};

			navigator.geolocation.getCurrentPosition(
				success,
				error,
				{
					timeout: 15000,
					maximumAge: 60000
				}
			);
		}
	};

	return self;

})();

