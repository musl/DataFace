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
    
    static char batt_buff[5];
    static char blth_buff[5];
    static char *bt;
    static int bs;
    static int p;
    
    if(s_bluetooth_state) {
        bt = "bt";
        text_layer_set_text_color(s_blth_layer, GColorDarkGray);
    } else {
        bt = "nc";
        text_layer_set_text_color(s_blth_layer, GColorRed);
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
        text_layer_set_text_color(s_batt_layer, GColorDarkGray);
    } else if(p > 10) {
        text_layer_set_text_color(s_batt_layer, GColorOrange);
    } else {
        text_layer_set_text_color(s_batt_layer, GColorRed);
    }
    
    snprintf(batt_buff, sizeof(batt_buff), "%3d%c", p, bs);
    text_layer_set_text(s_batt_layer, batt_buff);
    
    snprintf(blth_buff, sizeof(blth_buff), " %2s ", bt);
    text_layer_set_text(s_blth_layer, blth_buff);
    
}


/*
 * Display the time in the tersest format possible.
 */
static void update_time() {
    
    time_t gmt = time(NULL);
    struct tm *local_time = localtime(&gmt);
    static char buff[7];
    
    if(clock_is_24h_style() == true) {
        strftime(buff, sizeof(buff), "%H%M%S", local_time);
    } else {
        strftime(buff, sizeof(buff), "%I%M%S", local_time);
    }
    text_layer_set_text(s_time_layer, buff);
    
}


/*
 * Display a word about conditions and temperature.
 */
static void update_wthr() {
    
    static char buff[7];
    static const char *unit = "C";
    
    snprintf(buff, sizeof(buff), "%3s%2d%1s", s_cond, s_temp, unit);
    text_layer_set_text(s_wthr_layer, buff);
    
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
    
    static bool needs_update;
    Tuple *t = dict_read_first(iterator);

    needs_update = false;
    while(t != NULL) {
        switch(t->key) {
            case KEY_TEMPERATURE:
                // TODO - temperature unit config.
                s_temp = ((int)t->value->int32) - 273; // Maybe I could store this internally as a float?
                needs_update = true;
                break;
            case KEY_CONDITIONS:
                // TODO - abbreviate / translate conditions
                strncpy(s_cond, t->value->cstring, sizeof(s_cond - 1));
                s_cond[sizeof(s_cond) - 1] = '\0';
                needs_update = true;
                break;
            default:
                APP_LOG(APP_LOG_LEVEL_ERROR, "Key %d not recognized!", (int)t->key);
                break;
        }
        t = dict_read_next(iterator);
    }
    
    if(needs_update) { update_wthr(); }
    
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
    
    s_scp_a = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_SCP_SB_28));
    s_scp_b = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_SCP_SB_38));

    s_bg_layer = bitmap_layer_create(GRect(0, 0, 144, 168));
    bitmap_layer_set_background_color(s_bg_layer, GColorBlack);
    layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(s_bg_layer));
    
    s_date_layer = text_layer_create(GRect(0, 0, 144, 28));
    text_layer_set_background_color(s_date_layer, GColorClear);
    text_layer_set_text_color(s_date_layer, GColorLightGray);
    text_layer_set_font(s_date_layer, s_scp_a);
    text_layer_set_text_alignment(s_date_layer, GTextAlignmentCenter);
    layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_date_layer));
    
    s_time_layer = text_layer_create(GRect(2, 26, 142, 38));
    text_layer_set_background_color(s_time_layer, GColorClear);
    text_layer_set_text_color(s_time_layer, GColorWhite);
    text_layer_set_font(s_time_layer, s_scp_b);
    text_layer_set_text_alignment(s_time_layer, GTextAlignmentCenter);
    layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_time_layer));
    
    s_arty_layer = text_layer_create(GRect(2, 58, 142, 38));
    text_layer_set_background_color(s_arty_layer, GColorClear);
    text_layer_set_text_color(s_arty_layer, GColorRed);
    text_layer_set_font(s_arty_layer, s_scp_b);
    text_layer_set_text_alignment(s_arty_layer, GTextAlignmentCenter);
    text_layer_set_text(s_arty_layer, "~~~~~~");
    layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_arty_layer));
    
    s_wthr_layer = text_layer_create(GRect(2, 92, 142, 38));
    text_layer_set_background_color(s_wthr_layer, GColorClear);
    text_layer_set_text_color(s_wthr_layer, GColorWhite);
    text_layer_set_font(s_wthr_layer, s_scp_b);
    text_layer_set_text_alignment(s_wthr_layer, GTextAlignmentCenter);
    layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_wthr_layer));
    
    s_batt_layer = text_layer_create(GRect(2, 130, 70, 28));
    text_layer_set_background_color(s_batt_layer, GColorClear);
    text_layer_set_text_color(s_batt_layer, GColorLightGray);
    text_layer_set_font(s_batt_layer, s_scp_a);
    text_layer_set_text_alignment(s_batt_layer, GTextAlignmentCenter);
    layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_batt_layer));
    
    s_blth_layer = text_layer_create(GRect(74, 132, 70, 28));
    text_layer_set_background_color(s_blth_layer, GColorClear);
    text_layer_set_text_color(s_blth_layer, GColorLightGray);
    text_layer_set_font(s_blth_layer, s_scp_a);
    text_layer_set_text_alignment(s_blth_layer, GTextAlignmentCenter);
    layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_blth_layer));
    
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
    
    if(units_changed == SECOND_UNIT) {
        update_time();
    }
    
    if(units_changed == MINUTE_UNIT) {
        update_date();
        update_tech();
    }
    
    // update the weather on the hour.
    //
    if(units_changed == HOUR_UNIT) {
        DictionaryIterator *iter;
        app_message_outbox_begin(&iter);
        dict_write_uint8(iter, 0, 0);
        app_message_outbox_send();
    }
    
}


/*
 * Handle window unloading.
 */
static void unload_cb(Window *window) {
    
    text_layer_destroy(s_date_layer);
    text_layer_destroy(s_time_layer);
    text_layer_destroy(s_arty_layer);
    text_layer_destroy(s_wthr_layer);
    text_layer_destroy(s_batt_layer);
    text_layer_destroy(s_blth_layer);
    
    bitmap_layer_destroy(s_bg_layer);
    
    fonts_unload_custom_font(s_scp_a);
    fonts_unload_custom_font(s_scp_b);
    
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
    
    // Update tech stats.
    //
    s_battery_state = battery_state_service_peek();
    s_bluetooth_state = bluetooth_connection_service_peek();

    // Draw everything.
    //
    update_time();
    update_date();
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
    
    // I'm not sure if the unsubscriptions are necessary.
    //
    tick_timer_service_unsubscribe();
    bluetooth_connection_service_unsubscribe();
    battery_state_service_unsubscribe();
    
    window_destroy(s_main_window);
    
}

