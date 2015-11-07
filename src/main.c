// vim: set nosta noet ts=4 sw=4 ft=c:
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

// Organize ALL THE THINGS.
#include "main.h"

/********************************************************************************
 * Display - Functions that display stuff. It's arts and crafts time!
 *******************************************************************************/
// Alphabetical order.


/*
 * Manually update the ui.
 */
static void update_all() {

	BatteryChargeState battery_state;
	bool bluetooth_state;
	time_t gmt;
	struct tm *local_time;

	// Prepare to update the UI manually.
	//
	battery_state = battery_state_service_peek();
	bluetooth_state = bluetooth_connection_service_peek();
	gmt = time(NULL);
	local_time = localtime(&gmt);

	// Draw everything that we can, so that there isn't a blank screen
	// on start-up.
	//
	update_time(local_time);
	update_date(local_time);
	update_cldr(local_time);
	update_batt(battery_state);
	update_blth(bluetooth_state);

}


/*
 * Display a word about the battery state.
 */
static void update_batt(BatteryChargeState battery_state) {

	static char buff[9];
	static char state;
	static int charge;

	if(battery_state.is_charging) {
		state = 'c';
	} else if(battery_state.is_plugged) {
		state = 'p';
	} else {
		state = '%';    
	}

	charge = battery_state.charge_percent;
	if(charge > 20) {
		text_layer_set_text_color(s_batt_layer, theme_tertiary);
	} else if(charge > 10) {
		text_layer_set_text_color(s_batt_layer, theme_warn);
	} else {
		text_layer_set_text_color(s_batt_layer, theme_error);
	}

	// Shared row with the bluetooth status.
	//
	snprintf(buff, sizeof(buff), "    %3d%c", charge, state);
	text_layer_set_text(s_batt_layer, buff);

}


/*
 * Display a word about bluetooth state.
 */
static void update_blth(bool bluetooth_state) {

	static char buff[9];
	static char *state;

	if(bluetooth_state) {
		state = "bt";
		text_layer_set_text_color(s_blth_layer, theme_tertiary);
	} else {
		state = "nc";
		text_layer_set_text_color(s_blth_layer, theme_error);
	}

	// Shared row with the battery status.
	//
	snprintf(buff, sizeof(buff), "%2s      ", state);
	text_layer_set_text(s_blth_layer, buff);

}


/*
 * Display a couple of words about the calendar.
 */
static void update_cldr(struct tm *local_time) {

	static char buff[9];

	strftime(buff, sizeof(buff), "ww%U %a", local_time);
	text_layer_set_text_color(s_cldr_layer, theme_tertiary);
	text_layer_set_text(s_cldr_layer, buff);

}


/*
 * Display the date in the tersest format possible.
 */
static void update_date(struct tm *local_time) {

	static char buff[9];

	strftime(buff, sizeof(buff), "%Y%m%d", local_time);
	text_layer_set_text_color(s_date_layer, theme_secondary);
	text_layer_set_text(s_date_layer, buff);

}


/*
 * Display the time in the tersest format possible.
 */
static void update_time(struct tm *local_time) {

	static char buff[9];

	if(clock_is_24h_style() == true) {
		strftime(buff, sizeof(buff), "%H:%M:%S", local_time);
	} else {
		strftime(buff, sizeof(buff), "%I:%M:%S", local_time);
	}

	text_layer_set_text_color(s_time_layer, theme_primary);
	text_layer_set_text(s_time_layer, buff);

}


/*
 * Display a word about conditions and temperature.
 */
static void update_wthr() {

	static char buff[9];
	static float temp;

	// TODO maintain a weather state - no key, errored, fetching, fresh, stale
	if(s_temp <= 0) {
		text_layer_set_text_color(s_wthr_layer, theme_info);
		text_layer_set_text(s_wthr_layer, s_weather_fetching);
		return;
	}

	switch(s_temp_unit[0]) {
		case 'C':
			temp = s_temp - 273.15f;
			break;
		case 'F':
			temp = (s_temp - 273.15f) * 1.8f + 32.0f;
			break;
		case 'K':
			temp = s_temp;
			break;
		default:
			return;
	}

	snprintf(buff, sizeof(buff), "%-4s%3d%1c", s_cond, (int)temp, s_temp_unit[0]);
	text_layer_set_text_color(s_wthr_layer, theme_secondary);
	text_layer_set_text(s_wthr_layer, buff);

}


/********************************************************************************
 * Callbacks - Functions in our code that get called by the API.
 *******************************************************************************/
// Alphabetical order.


/*
 * Handle messages from the AppMessage API. This is responsible for
 * handling messages about weather and configuration. Configuration
 * values that persist on the watch are stored here.
 */
static void inbox_cb(DictionaryIterator *iterator, void *context) {

	static int theme;
	static bool need_wthr;
	static bool need_refresh;
	Tuple *t;

	t = dict_read_first(iterator);
	need_wthr = need_refresh = false;

	while(t != NULL) {
		switch(t->key) {

			/*
			 * App Config
			 */
			case KEY_CONFIG_API_KEY:
				fetch_weather();
				break;

			case KEY_CONFIG_LATITUDE:
				break;

			case KEY_CONFIG_LONGITUDE:
				break;

			case KEY_CONFIG_TEMP_UNIT:
				strncpy(s_temp_unit, t->value->cstring, sizeof(s_temp_unit));
				// TODO: validation
				persist_write_string(KEY_CONFIG_TEMP_UNIT, s_temp_unit);
				need_wthr = true;
				break;

			case KEY_CONFIG_THEME:
				theme = atoi(t->value->cstring);
				if(theme < 0 || theme >= THEME_COUNT) break;
				s_theme = theme;
				persist_write_int(KEY_CONFIG_THEME, s_theme);
				need_refresh = true;
				need_wthr = true;
				break;

				/*
				 * App Data
				 */
			case KEY_CONDITIONS:
				strncpy(s_cond, t->value->cstring, sizeof(s_cond));
				need_wthr = true;
				break;

			case KEY_TEMPERATURE:
				s_temp = (float)t->value->int32;
				need_wthr = true;
				break;

				/*
				 * Errors, Exceptional Cases
				 */
			case KEY_WEATHER_FAIL:
				text_layer_set_text(s_wthr_layer, "API KEY?");
				text_layer_set_text_color(s_wthr_layer, theme_error);
				break;

			default:
				APP_LOG(APP_LOG_LEVEL_ERROR, "Key %d not recognized!", (int)t->key);
				break;

		}
		t = dict_read_next(iterator);
	}

	if(need_refresh) {
		update_all();
	}

	if(need_wthr) {
		update_wthr();
	}

}


/*
 * Handle dropped messages from the AppMessage API.
 */
static void inbox_dropped_cb(AppMessageResult reason, void *context) {

	APP_LOG(APP_LOG_LEVEL_ERROR, "Message dropped.");

}


/*:
 * Handle window loading.
 */
static void load_cb(Window *window) {

	// Right Margin
	static int m = 4;
	// Font Size
	static int s = 31;
	// First Row Offset
	static int o = 0;
	// Row Top Padding
	static int p = 2;

	// Screen Size
	static int w = 144;
	static int h = 168;

	// Load resources we'll have to destroy later.
	s_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_TARGA_MS_31));

	// NO DESCENDERS FOR YOU
#define make_row(x) text_layer_create(GRect(m, (x - 1) * (s + p) + o, w, s))

	// Window Background
	//
	s_bg_layer = bitmap_layer_create(GRect(0, 0, w, h));
	bitmap_layer_set_background_color(s_bg_layer, GColorBlack);
	layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(s_bg_layer));

	s_cldr_layer = make_row(1);
	text_layer_set_background_color(s_cldr_layer, GColorClear);
	text_layer_set_text_color(s_cldr_layer, theme_tertiary);
	text_layer_set_font(s_cldr_layer, s_font);
	text_layer_set_text_alignment(s_cldr_layer, GTextAlignmentLeft);
	layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_cldr_layer));

	s_date_layer = make_row(2);
	text_layer_set_background_color(s_date_layer, GColorClear);
	text_layer_set_text_color(s_date_layer, theme_secondary);
	text_layer_set_font(s_date_layer, s_font);
	text_layer_set_text_alignment(s_date_layer, GTextAlignmentLeft);
	layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_date_layer));

	s_time_layer = make_row(3);
	text_layer_set_background_color(s_time_layer, GColorClear);
	text_layer_set_text_color(s_time_layer, theme_primary);
	text_layer_set_font(s_time_layer, s_font);
	text_layer_set_text_alignment(s_time_layer, GTextAlignmentLeft);
	layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_time_layer));

	s_wthr_layer = make_row(4);
	text_layer_set_background_color(s_wthr_layer, GColorClear);
	text_layer_set_text_color(s_wthr_layer, theme_info);
	text_layer_set_font(s_wthr_layer, s_font);
	text_layer_set_text_alignment(s_wthr_layer, GTextAlignmentLeft);
	layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_wthr_layer));

	s_blth_layer = make_row(5);
	text_layer_set_background_color(s_blth_layer, GColorClear);
	text_layer_set_text_color(s_blth_layer, theme_tertiary);
	text_layer_set_font(s_blth_layer, s_font);
	text_layer_set_text_alignment(s_blth_layer, GTextAlignmentLeft);
	layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_blth_layer));

	s_batt_layer = make_row(5);
	text_layer_set_background_color(s_batt_layer, GColorClear);
	text_layer_set_text_color(s_batt_layer, theme_tertiary);
	text_layer_set_font(s_batt_layer, s_font);
	text_layer_set_text_alignment(s_batt_layer, GTextAlignmentLeft);
	layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_batt_layer));

#undef make_row

}


/*
 * Handle failed sends.
 */
static void outbox_failed_cb(DictionaryIterator *iterator, AppMessageResult reason, void *context) {

	APP_LOG(APP_LOG_LEVEL_ERROR, "Outbox send failed.");

}


/*
 * Handle successful sends.
 */
static void outbox_sent_cb(DictionaryIterator *iterator, void *context) {

	APP_LOG(APP_LOG_LEVEL_INFO, "Outbox send succeeded.");

}


/*
 * Handle time changes.
 */
static void tick_cb(struct tm *tick_time, TimeUnits units_changed) {

	if(bit_is_set(units_changed, SECOND_UNIT)) {
		update_cldr(tick_time);
		update_date(tick_time);
		update_time(tick_time);
	}

	if(bit_is_set(units_changed, MINUTE_UNIT)) {
		if(tick_time->tm_min % 15 == 0) fetch_weather();
	}

}


/*
 * Handle window unloading.
 */
static void unload_cb(Window *window) {

	text_layer_destroy(s_date_layer);
	text_layer_destroy(s_time_layer);
	text_layer_destroy(s_cldr_layer);
	text_layer_destroy(s_wthr_layer);
	text_layer_destroy(s_batt_layer);
	text_layer_destroy(s_blth_layer);

	bitmap_layer_destroy(s_bg_layer);

	fonts_unload_custom_font(s_font);

}


/********************************************************************************
 * Utilities
 *******************************************************************************/
// Alphabetical order.

/*
 * Send an AppMessage to fetch the current weather.
 */
static void fetch_weather() {

	DictionaryIterator *iter;
	app_message_outbox_begin(&iter);
	dict_write_uint8(iter, 0, 0);
	app_message_outbox_send();

}

/********************************************************************************
 * Lifecycle - A basic sketch of the life of a watchface.
 *******************************************************************************/
// Mostly in execution order. Thanks, prototypes. Thprototypes!


/*
 * Main entry point. Painfully, standardly boring.
 */
int main( void ) {

	init();
	app_event_loop();
	deinit();

}


/*
 * A place for everything and everything it its place. This is
 * mainly here because main() would be messy otherwise.
 */
static void init() {

	int theme;

	/* 
	 * Read the stored configuration keys or write defaults if they
	 * don't exist so that the config is properly loaded.
	 */

	if(persist_exists(KEY_CONFIG_TEMP_UNIT)) {
		// TODO validation
		persist_read_string(KEY_CONFIG_TEMP_UNIT, s_temp_unit, sizeof(s_temp_unit));
	} else {
		strncpy(s_temp_unit, DEFAULT_TEMP_UNIT, sizeof(s_temp_unit));    
		persist_write_string(KEY_CONFIG_TEMP_UNIT, s_temp_unit);
	}

	if(persist_exists(KEY_CONFIG_THEME)) {
		theme = persist_read_int(KEY_CONFIG_THEME);
		s_theme = (theme >= 0 && theme < THEME_COUNT) ? theme : DEFAULT_THEME;
	} else {
		s_theme = DEFAULT_THEME;
		persist_write_int(KEY_CONFIG_THEME, s_theme);
	}

	// Build the main window and register callbacks so that the UI gets
	// drawn.
	//
	s_main_window = window_create();
	window_set_window_handlers(s_main_window, (WindowHandlers) {
			.load = load_cb,
			.unload = unload_cb
			});
	window_stack_push(s_main_window, true);

	// Draw the watchface for the first time.
	update_all();

	// Subscribe to device events so that changes to the battery state,
	// bluetooth state, or time cause the UI to update.
	//
	battery_state_service_subscribe(update_batt);
	bluetooth_connection_service_subscribe(update_blth);
	tick_timer_service_subscribe(SECOND_UNIT | MINUTE_UNIT, tick_cb);

	// Register interest in AppMessage events so that handlers get
	// called when messages arrive, leave, or fail.
	//
	app_message_register_inbox_received(inbox_cb);
	app_message_register_inbox_dropped(inbox_dropped_cb);
	app_message_register_outbox_failed(outbox_failed_cb);
	app_message_register_outbox_sent(outbox_sent_cb);

	// Listen for messages so that the weather and configuration get
	// updated.
	//
	app_message_open(
			app_message_inbox_size_maximum(),
			app_message_outbox_size_maximum());

}


/*
 * Scortched earth. Well-organized destruction.
 */
static void deinit() {

	tick_timer_service_unsubscribe();
	bluetooth_connection_service_unsubscribe();
	battery_state_service_unsubscribe();

	window_destroy(s_main_window);

}

