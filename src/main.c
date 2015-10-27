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
 * Display the date in the tersest format possible.
 */
static void update_date() {
    
    time_t gmt = time(NULL);
    struct tm *local_time = localtime(&gmt);
    static char buff[9];
    
    strftime(buff, sizeof(buff), "%Y%m%d", local_time);
    text_layer_set_text(s_date_layer, buff);
    
}


/*
 * Display some text about devices that I care about inside the watch.
 */
static void update_tech() {
    
    static char batt_buff[9];
    static char blth_buff[9];
    static char *bt;
    static int bs;
    static int p;
    
    if(s_bluetooth_state) {
        bt = "bt";
        text_layer_set_text_color(s_blth_layer, color_c);
    } else {
        bt = "nc";
        text_layer_set_text_color(s_blth_layer, color_error);
    }
    
    if(s_battery_state.is_charging) {
        bs = (int) 'c';
    } else if(s_battery_state.is_charging) {
        bs = (int) 'p';
    } else {
        bs = (int) '%';    
    }
    
    p = s_battery_state.charge_percent;
    if(p > 20) {
        text_layer_set_text_color(s_batt_layer, color_c);
    } else if(p > 10) {
        text_layer_set_text_color(s_batt_layer, color_warn);
    } else {
        text_layer_set_text_color(s_batt_layer, color_error);
    }
    
    // To make text rendering uniform across the layers, use padding
    // in the format strings.  These layers overlap, but have transparent
    // backgrounds.
    //
    snprintf(batt_buff, sizeof(batt_buff), "    %3d%c", p, bs);
    text_layer_set_text(s_batt_layer, batt_buff);
    snprintf(blth_buff, sizeof(blth_buff), "%2s      ", bt);
    text_layer_set_text(s_blth_layer, blth_buff);
    
}


/*
 * Display the time in the tersest format possible.
 */
static void update_time() {
    
    time_t gmt = time(NULL);
    struct tm *local_time = localtime(&gmt);
    static char buff[9];
    
    if(clock_is_24h_style() == true) {
        strftime(buff, sizeof(buff), "%H:%M:%S", local_time);
    } else {
        strftime(buff, sizeof(buff), "%I:%M:%S", local_time);
    }
    text_layer_set_text(s_time_layer, buff);
    
}


/*
 * Display a word about conditions and temperature.
 */
static void update_wthr() {
    
    static char buff[9];
    static float temp;

	if(s_temp == 0) return;
    
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
    text_layer_set_text_color(s_wthr_layer, color_b);
    text_layer_set_text(s_wthr_layer, buff);
    
}


/*
 *
 */
static void update_cldr() {
    
    time_t gmt = time(NULL);
    struct tm *local_time = localtime(&gmt);    
    static char buff[9];
    
    strftime(buff, sizeof(buff), "ww%U %a", local_time);
    text_layer_set_text(s_cldr_layer, buff);
    
}


/********************************************************************************
 * Callbacks - Functions in our code that get called by the API.
 *******************************************************************************/
 // Alphabetical order.


/*
 * Handle battery state changes.
 */
static void batt_cb(BatteryChargeState state) {
    
    s_battery_state = state;
    update_tech();
    
}


/*
 * Handle bluetooth state changes.
 */
static void blth_cb(bool state) {
    
    s_bluetooth_state = state;
    update_tech();
    
}


/*
 * Handle messages from the AppMessage API.
 */
static void inbox_cb(DictionaryIterator *iterator, void *context) {
    
    bool need_wthr;
    Tuple *t = dict_read_first(iterator);

    while(t != NULL) {
        switch(t->key) {
            case KEY_TEMPERATURE:
                s_temp = ((int)t->value->int32);
                need_wthr = true;
                break;
            case KEY_TEMP_UNIT:
                strncpy(s_temp_unit, t->value->cstring, sizeof(s_temp_unit));
                need_wthr = true;
                break;
            case KEY_CONDITIONS:
                strncpy(s_cond, t->value->cstring, sizeof(s_cond));
                need_wthr = true;
                break;
            case KEY_API:
                fetch_weather();
                break;
			case KEY_WEATHER_FAIL:
				text_layer_set_text(s_wthr_layer, "API KEY?");
				text_layer_set_text_color(s_wthr_layer, color_error);
				break;
            default:
                APP_LOG(APP_LOG_LEVEL_ERROR, "Key %d not recognized!", (int)t->key);
                break;
        }
        t = dict_read_next(iterator);
    }
    
    if(need_wthr) { update_wthr(); }
    
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
    
    color_a = GColorWhite;
    color_b = GColorScreaminGreen;
    color_c = GColorIslamicGreen;
    color_error = GColorRed;
    color_warn = GColorYellow;
    color_info = GColorPictonBlue;

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
    text_layer_set_text_color(s_cldr_layer, color_c);
    text_layer_set_font(s_cldr_layer, s_font);
    text_layer_set_text_alignment(s_cldr_layer, GTextAlignmentLeft);
    layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_cldr_layer));
    
    s_date_layer = make_row(2);
    text_layer_set_background_color(s_date_layer, GColorClear);
    text_layer_set_text_color(s_date_layer, color_b);
    text_layer_set_font(s_date_layer, s_font);
    text_layer_set_text_alignment(s_date_layer, GTextAlignmentLeft);
    layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_date_layer));
    
    s_time_layer = make_row(3);
    text_layer_set_background_color(s_time_layer, GColorClear);
    text_layer_set_text_color(s_time_layer, color_a);
    text_layer_set_font(s_time_layer, s_font);
    text_layer_set_text_alignment(s_time_layer, GTextAlignmentLeft);
    layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_time_layer));
    
    s_wthr_layer = make_row(4);
    text_layer_set_background_color(s_wthr_layer, GColorClear);
    text_layer_set_text_color(s_wthr_layer, color_info);
    text_layer_set_font(s_wthr_layer, s_font);
    text_layer_set_text_alignment(s_wthr_layer, GTextAlignmentLeft);
    text_layer_set_text(s_wthr_layer, s_weather_fetching);
    layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_wthr_layer));
    
    s_blth_layer = make_row(5);
    text_layer_set_background_color(s_blth_layer, GColorClear);
    text_layer_set_text_color(s_blth_layer, color_c);
    text_layer_set_font(s_blth_layer, s_font);
    text_layer_set_text_alignment(s_blth_layer, GTextAlignmentLeft);
    layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_blth_layer));
    
    s_batt_layer = make_row(5);
    text_layer_set_background_color(s_batt_layer, GColorClear);
    text_layer_set_text_color(s_batt_layer, color_c);
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
        update_time();
    }

    if(bit_is_set(units_changed, MINUTE_UNIT)) {
        update_date();
        update_tech();
        if(tick_time->tm_min % 15 == 0) fetch_weather();
    }

    if(bit_is_set(units_changed, HOUR_UNIT)) {
        update_cldr();
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
    
    // Build the main window.
    //
    s_main_window = window_create();
    window_set_window_handlers(s_main_window, (WindowHandlers) {
        .load = load_cb,
        .unload = unload_cb
    });
    window_stack_push(s_main_window, true);
    
    // Handle config defaults
    //
    if(persist_exists(KEY_TEMP_UNIT)) {
        persist_read_string(KEY_TEMP_UNIT, s_temp_unit, sizeof(s_temp_unit));
    } else {
        strncpy(s_temp_unit, DEFAULT_TEMP_UNIT, sizeof(s_temp_unit));    
        persist_write_string(KEY_TEMP_UNIT, s_temp_unit);
    }
    
    // Update tech stats.
    //
    s_battery_state = battery_state_service_peek();
    s_bluetooth_state = bluetooth_connection_service_peek();
    
    // Draw everything.
    //
    update_time();
    update_date();
    update_cldr();
    update_tech();
    
    // Subscribe to device events.
    //
    battery_state_service_subscribe(batt_cb);
    bluetooth_connection_service_subscribe(blth_cb);
    tick_timer_service_subscribe(SECOND_UNIT | MINUTE_UNIT | HOUR_UNIT, tick_cb);
    
    // Register interest in AppMessage events.
    //
    app_message_register_inbox_received(inbox_cb);
    app_message_register_inbox_dropped(inbox_dropped_cb);
    app_message_register_outbox_failed(outbox_failed_cb);
    app_message_register_outbox_sent(outbox_sent_cb);
    
    // Listen for messages.
    app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());
    
}


/*
 * Scortched earth. Well-organized destruction.
 */
static void deinit() {
    
    persist_write_string(KEY_TEMP_UNIT, s_temp_unit);
    
    // I'm not sure if the unsubscriptions are necessary.
    //
    tick_timer_service_unsubscribe();
    bluetooth_connection_service_unsubscribe();
    battery_state_service_unsubscribe();
    
    window_destroy(s_main_window);
    
}

