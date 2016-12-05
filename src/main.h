/* vim: set nosta noet ts=4 sw=4 ft=c:
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

#pragma once

#include <pebble.h>

// AppMessage Keys. Make sure these match those in appinfo.json!
//
enum {
	// Weather Config
	KEY_CONFIG_API_KEY = 100,
	KEY_CONFIG_LOCATION,
	KEY_CONFIG_TEMP_UNIT,
	KEY_CONFIG_API,
	// Look & Feel Config
	KEY_CONFIG_THEME = 110,
	// Data Messages
	KEY_CONDITIONS = 200,
	KEY_TEMPERATURE,
	// Exceptions and Errors
	KEY_WEATHER_FAIL = 500
};

// Defaults
//
#define DEFAULT_TEMP_UNIT "C"
#define DEFAULT_THEME 0

// Macros
#define bit_is_set(value, mask) (value & mask) != 0

/*
 * Yeah, mamma said not to use globals, but sheesh. This is a tiny, static
 * watch face that will hove no consuming third-party code. Meh. I hope this
 * runs in some kind of sandbox.
 */

// App State
//
static float s_temp;
static char s_temp_unit[2];
static char s_cond[7];

// App Messages
//
#define MSG_WTHR_API_KEY  "API KEY?"
#define MSG_WTHR_LOCATION "NO LOC? "
#define MSG_WTHR_FETCHING "FETCHING"

// App Resources
//
static Window *s_main_window;
static BitmapLayer *s_bg_layer;
static TextLayer *s_batt_layer;
static TextLayer *s_blth_layer;
static TextLayer *s_date_layer;
static TextLayer *s_cldr_layer;
static TextLayer *s_time_layer;
static TextLayer *s_wthr_layer;
static GFont s_font;

#define THEME_COUNT 10
#define THEME_SIZE 6
#define theme_primary *s_themes[s_theme][0]
#define theme_secondary *s_themes[s_theme][1]
#define theme_tertiary *s_themes[s_theme][2]
#define theme_error *s_themes[s_theme][3]
#define theme_warn *s_themes[s_theme][4]
#define theme_info *s_themes[s_theme][5]
static int s_theme;
static GColor *s_themes[THEME_COUNT][THEME_SIZE] = {
	{ &GColorWhite, &GColorMelon, &GColorRed, &GColorRed, &GColorYellow, &GColorPictonBlue },
	{ &GColorWhite, &GColorRajah, &GColorOrange, &GColorRed, &GColorYellow, &GColorPictonBlue },
	{ &GColorWhite, &GColorYellow, &GColorLimerick, &GColorRed, &GColorYellow, &GColorPictonBlue },
	{ &GColorWhite, &GColorScreaminGreen, &GColorIslamicGreen, &GColorRed, &GColorYellow, &GColorPictonBlue },
	{ &GColorWhite, &GColorCyan, &GColorTiffanyBlue, &GColorRed, &GColorYellow, &GColorPictonBlue },
	{ &GColorWhite, &GColorVividCerulean, &GColorBlue, &GColorRed, &GColorYellow, &GColorPictonBlue },
	{ &GColorWhite, &GColorLavenderIndigo, &GColorPurple, &GColorRed, &GColorYellow, &GColorPictonBlue },
	{ &GColorWhite, &GColorShockingPink, &GColorFashionMagenta, &GColorRed, &GColorYellow, &GColorPictonBlue },
	{ &GColorWhite, &GColorLightGray, &GColorDarkGray, &GColorRed, &GColorYellow, &GColorPictonBlue },
	{ &GColorWhite, &GColorWhite, &GColorWhite, &GColorRed, &GColorYellow, &GColorPictonBlue }
};

/*
 * If you care enough to organize everything for an app with such a defined
 * workflow into separate methods, you might as well use prototypes.
 */

// Organizational Functions 
//
static void deinit();
static void init();

// Display
//
static void update_all();
static void update_batt(BatteryChargeState battery_state);
static void update_blth(bool bluetooth_state);
static void update_cldr(struct tm *local_time);
static void update_date(struct tm *local_time);
static void update_time(struct tm *local_time);
static void update_wthr();

// Window API
//
static void load_cb(Window *window);
static void unload_cb(Window *window);

// TickTimerService API
static void tick_cb(struct tm *tick_time, TimeUnits units_changed);

// AppMessage API
//
static void inbox_cb(DictionaryIterator *iterator, void *context);
static void inbox_dropped_cb(AppMessageResult reason, void *context);
static void outbox_failed_cb(DictionaryIterator *iterator, AppMessageResult reason, void *context);
static void outbox_sent_cb(DictionaryIterator *iterator, void *context);

// Utilities
//
static void fetch_weather();

