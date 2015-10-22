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


#pragma once


#include <pebble.h>


// Macros for weather info.
//
enum {
    KEY_TEMPERATURE,
    KEY_CONDITIONS
};

/*
 * Yeah, mamma said not to use globals, but sheesh. This is a tiny, static
 * watch face that will hove no consuming third-party code. Meh. I hope this
 * runs in some kind of sandbox.
 */

// App State
//
static bool s_bluetooth_state;
static BatteryChargeState s_battery_state;
static int s_temp;
static char s_cond[5];

// App Resources
//
static Window *s_main_window;
static BitmapLayer *s_bg_layer;
static TextLayer *s_batt_layer;
static TextLayer *s_blth_layer;
static TextLayer *s_date_layer;
static TextLayer *s_arty_layer;
static TextLayer *s_time_layer;
static TextLayer *s_wthr_layer;
static GFont s_scp_a;
static GFont s_scp_b;

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
static void update_date();
static void update_tech();
static void update_time();
static void update_wthr();

// Window API
//
static void load_cb(Window *window);
static void unload_cb(Window *window);

// Battery, Bluetooth, and Time APIs
//
static void batt_cb(BatteryChargeState state);
static void blth_cb(bool state);
static void tick_cb(struct tm *tick_time, TimeUnits units_changed);

// AppMessage API
//
static void inbox_cb(DictionaryIterator *iterator, void *context);
static void inbox_dropped_cb(AppMessageResult reason, void *context);
static void outbox_failed_cb(DictionaryIterator *iterator, AppMessageResult reason, void *context);
static void outbox_sent_cb(DictionaryIterator *iterator, void *context);

