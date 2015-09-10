#include <pebble.h>

// Static variable declarations
static Window *s_main_window;
static TextLayer *s_time_layer;
static GFont s_fira_sans;

// Updating the time displayed.
static void update_time() {
    // Get a tm structure
    time_t temp = time(NULL);
    struct tm *tick_time = localtime(&temp);
    
    // Create a (persistant) character buffer
    static char buffer[] = "00:00";
    
    // Write the current time into the buffer (instead of 00:00, which the time likely isn't)
    if (clock_is_24h_style() == true) {
        // Use 24h format
        strftime(buffer, sizeof("00:00"), "%H:%M", tick_time);
    } else {
        strftime(buffer, sizeof("00:00"), "%I:%M", tick_time);
    }
    
    text_layer_set_text(s_time_layer, buffer);
    
}

static void main_window_load(Window *window) {
    
    // Import fira sans
    s_fira_sans = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FIRA_SANS_EXTRALIGHT_48));
    
    // Create a time text layer
    s_time_layer = text_layer_create(GRect(0, 55, 144, 50));
    text_layer_set_text(s_time_layer, "00:00");

    // Design the text element
    text_layer_set_text_alignment(s_time_layer, GTextAlignmentCenter);
    text_layer_set_font(s_time_layer, s_fira_sans);
    text_layer_set_background_color(s_time_layer, GColorClear);
    text_layer_set_text_color(s_time_layer, GColorBlack);
    
    // Add it as a child layer to the Window's root layer
    layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_time_layer));

}

static void main_window_unload(Window *window) {
    // When we destroy our main window, we should destroy everything in it, too. 
    text_layer_destroy(s_time_layer);
    fonts_unload_custom_font(s_fira_sans);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
    update_time();
}

static void init() {
    // Create a root window
    s_main_window = window_create();
    
    // Create handlers to manage the elements inside the window
    window_set_window_handlers(s_main_window, (WindowHandlers) {
        .load = main_window_load,
        .unload = main_window_unload
    });
        
    // Show the (animated!) window on the watch
    window_stack_push(s_main_window, true);
    
    // Update the time initially
    update_time(); 
    
    // Time management. Not for you, for the watchface. You have to do your own. I can't help you there. 
    // I'm sorry. 
    tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
    
}

static void deinit() {
    // Destroy our main window
    window_destroy(s_main_window);
}


int main(void) {
    init();
    app_event_loop();
    deinit();
}