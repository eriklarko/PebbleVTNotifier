#include "pebble_os.h"
#include "pebble_app.h"
#include "pebble_fonts.h"
#include "PebbleVasttrafikNotifier.h"
#include "num2words.h"

static struct Vars {
  Window window;
  TextLayer watchBold;
  TextLayer watchThin;
  TextLayer tramInfo;
  TextLayer tramTime;
  TextLayer dateLayer;
  
  char buffer[3][BUFFER_SIZE];
  char concatBuf[BUFFER_SIZE];
  int tramNumber;
  int tramMinutes;
} s_vars;

void handle_init(AppContextRef ctx) {
  init_gui();	
  
  PblTm currentTime;
  get_time(&currentTime);
  update_time(&currentTime); 
  
  window_stack_push(&s_vars.window, true /* Animated */); 
}

////////////////////////////////////////////////////////////////////////
////////////////////////////// GUI INIT ////////////////////////////////
void init_gui() {
  s_vars.tramNumber = 11;
  s_vars.tramMinutes = 3;
	
  window_init(&s_vars.window, "VTNotifier");
  window_set_background_color(&s_vars.window, GColorBlack);
  
  init_bold_text();
  init_thin_text();
  init_tram_info();
  init_tram_time();
}

void init_bold_text() {
  text_layer_init(&s_vars.watchBold, GRect(12, 27, 144 /* width */, 168-30 /* height */));
  
  text_layer_set_text_color(&s_vars.watchBold, GColorWhite);
  text_layer_set_background_color(&s_vars.watchBold, GColorClear);
  text_layer_set_font(&s_vars.watchBold, fonts_get_system_font(FONT_KEY_BITHAM_42_BOLD));
  
  layer_add_child(&s_vars.window.layer, &s_vars.watchBold.layer);
}

void init_thin_text() {
  text_layer_init(&s_vars.watchThin, GRect(5, 7, 144 /* width */, 168 /* height */));
  
  text_layer_set_text_color(&s_vars.watchThin, GColorWhite);
  text_layer_set_background_color(&s_vars.watchThin, GColorClear);
  text_layer_set_font(&s_vars.watchThin, fonts_get_system_font(FONT_KEY_BITHAM_42_LIGHT));
  
  layer_add_child(&s_vars.window.layer, &s_vars.watchThin.layer);
}

void init_tram_info() {
  text_layer_init(&s_vars.tramInfo, GRect(5, 168-33, 144-40 /* width */, 168 /* height */));
  
  text_layer_set_text_color(&s_vars.tramInfo, GColorWhite);
  text_layer_set_background_color(&s_vars.tramInfo, GColorClear);
  text_layer_set_font(&s_vars.tramInfo, fonts_get_system_font(FONT_KEY_GOTHIC_24));
  
  layer_add_child(&s_vars.window.layer, &s_vars.tramInfo.layer);
}

void init_tram_time() {
  text_layer_init(&s_vars.tramTime, GRect(100, 168-35, 144 /* width */, 168 /* height */));
  
  text_layer_set_text_color(&s_vars.tramTime, GColorWhite);
  text_layer_set_background_color(&s_vars.tramTime, GColorClear);
  text_layer_set_font(&s_vars.tramTime, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));
  
  layer_add_child(&s_vars.window.layer, &s_vars.tramTime.layer);
}

void init_text_layer(TextLayer* text_layer, GRect pos_size, GFont font) {
  text_layer_init(text_layer, pos_size);
  
  text_layer_set_text_color(text_layer, GColorWhite);
  text_layer_set_background_color(text_layer, GColorClear);
  text_layer_set_font(text_layer, font);
  
  layer_add_child(&s_vars.window.layer, (*text_layer).layer);
}
////////////////////////////// GUI INIT ////////////////////////////////
////////////////////////////////////////////////////////////////////////

void handle_minute_tick(AppContextRef ctx, PebbleTickEvent *t) {
  update_time(t->tick_time);
}

void update_time(PblTm* time) {  
  fuzzy_time_to_words(time->tm_hour, time->tm_min, s_vars.buffer, BUFFER_SIZE);
  
  memset(s_vars.concatBuf, 0, sizeof s_vars.concatBuf);
  snprintf(s_vars.concatBuf, sizeof s_vars.concatBuf, "%s%s%s", *s_vars.buffer, " ", *(s_vars.buffer + 1));  
  char* boldText = *(s_vars.buffer+2);
  char* thinText = s_vars.concatBuf;
  
  char* tram_departure_time = build_tram_departure_time(s_vars.tramMinutes);
  
  text_layer_set_text(&s_vars.watchThin, thinText);
  text_layer_set_text(&s_vars.watchBold, boldText);
  text_layer_set_text(&s_vars.tramInfo, build_tram_string(s_vars.tramNumber));
  text_layer_set_text(&s_vars.tramTime, tram_departure_time);
  
  /*static char timeText[] = "00:00"; // Needs to be static because it's used by the system later.
  string_format_time(timeText, sizeof(timeText), "%H:%M", time);
  text_layer_set_text(&s_vars.tramInfo, timeText);*/
}

char* build_tram_string(int lineNumber) {
	static char* format_string = "%i:an går om";
	
	static char buffer[BUFFER_SIZE];
	snprintf(buffer, BUFFER_SIZE, format_string, lineNumber);
	return buffer;
}

char* build_tram_departure_time(int minutes) {
	static char buf[BUFFER_SIZE];
	memset(buf, 0, sizeof buf);
	append_number(buf, minutes);
	return buf;
}

void pbl_main(void *params) {
  PebbleAppHandlers handlers = {
	  
    .init_handler = &handle_init,
    
    .tick_info = {
		.tick_handler = &handle_minute_tick,
		.tick_units = MINUTE_UNIT
	}
  };
  app_event_loop(params, &handlers);
}
