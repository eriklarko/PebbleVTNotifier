#include "pebble_os.h"
#include "pebble_app.h"
#include "pebble_fonts.h"
#include "PebbleVasttrafikNotifier.h"
#include "num2words.h"
#include "weekdays.h"

enum {
  TRAM_NUMBER = 0x0,
  TRAM_MINUTES = 0x1
};

static struct Vars {
  Window window;
  TextLayer watchBold;
  TextLayer watchThin;
  TextLayer tramInfo;
  TextLayer tramTime;
  TextLayer date;
  
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
  window_init(&s_vars.window, "VTNotifier");
  window_set_background_color(&s_vars.window, GColorBlack);
  
  init_bold_text();
  init_thin_text();
  init_tram_info();
  init_tram_time();
  init_text_layer(&s_vars.date, GRect(144-30, 52, 144 /* width */, 168-30 /* height */), fonts_get_system_font(FONT_KEY_GOTHIC_24));
}

void init_bold_text() {
  init_text_layer(&s_vars.watchBold, GRect(12, 27, 144 /* width */, 168-30 /* height */), fonts_get_system_font(FONT_KEY_BITHAM_42_BOLD));
}

void init_thin_text() {
  init_text_layer(&s_vars.watchThin, GRect(5, 7, 144 /* width */, 168 /* height */), fonts_get_system_font(FONT_KEY_BITHAM_42_LIGHT));
}

void init_tram_info() {
  init_text_layer(&s_vars.tramInfo, GRect(5, 168-33, 144-40 /* width */, 168 /* height */), fonts_get_system_font(FONT_KEY_GOTHIC_24));
}

void init_tram_time() {
  init_text_layer(&s_vars.tramTime, GRect(100, 168-35, 144 /* width */, 168 /* height */), fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));
}

void init_text_layer(TextLayer* text_layer, GRect pos_size, GFont font) {
  text_layer_init(text_layer, pos_size);
  
  text_layer_set_text_color(text_layer, GColorWhite);
  text_layer_set_background_color(text_layer, GColorClear);
  text_layer_set_font(text_layer, font);
  
  layer_add_child(&s_vars.window.layer, &text_layer->layer);
}
////////////////////////////// GUI INIT ////////////////////////////////
////////////////////////////////////////////////////////////////////////

void handle_minute_tick(AppContextRef ctx, PebbleTickEvent *t) {
  update_time(t->tick_time);
}

void update_time(PblTm* time) {  
  fuzzy_time_to_words(time, s_vars.buffer, BUFFER_SIZE);
  
  memset(s_vars.concatBuf, 0, sizeof s_vars.concatBuf);
  snprintf(s_vars.concatBuf, sizeof s_vars.concatBuf, "%s%s%s", *s_vars.buffer, " ", *(s_vars.buffer + 1));  
  char* boldText = *(s_vars.buffer+2);
  char* thinText = s_vars.concatBuf;
  
  text_layer_set_text(&s_vars.watchThin, thinText);
  text_layer_set_text(&s_vars.watchBold, boldText);
  text_layer_set_text(&s_vars.date, build_date_string(time));
  
  /*static char timeText[] = "00:00"; // Needs to be static because it's used by the system later.
  string_format_time(timeText, sizeof(timeText), "%H:%M", time);
  text_layer_set_text(&s_vars.tramInfo, timeText);*/
}

void update_tram() {
  text_layer_set_text(&s_vars.tramInfo, build_tram_string(s_vars.tramNumber));
  text_layer_set_text(&s_vars.tramTime, build_tram_departure_time(s_vars.tramMinutes));
}

void clear_tram() {
  text_layer_set_text(&s_vars.tramInfo, "");
  text_layer_set_text(&s_vars.tramTime, "");
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

char* build_date_string(PblTm* time) {
	char* weekday = weekday_index_to_string(time->tm_wday);
	static char* format_string = "%s\n%i";

	static char buffer[BUFFER_SIZE];
	snprintf(buffer, BUFFER_SIZE, format_string, weekday, time->tm_mday);
	return buffer;
}

////////////////////////////////////////////////////////////////////////
///////////////////////// COMMUNCATION /////////////////////////////////
void msgReceived(DictionaryIterator *received, void *context) {
  Tuple* tram = dict_find(received, TRAM_NUMBER);
  Tuple* mins = dict_find(received, TRAM_MINUTES);

  if (tram) {
	  s_vars.tramNumber = tram->value->int16;
  }

  if (mins) {
	  s_vars.tramMinutes = mins->value->int16;
  }

  if(s_vars.tramNumber < 0) {
    clear_tram();
  } else {
    update_tram();
  }
}
void msgDropped(void *context, AppMessageResult reason) {
  // incoming message dropped
}
///////////////////////// COMMUNCATION /////////////////////////////////
////////////////////////////////////////////////////////////////////////

void pbl_main(void *params) {
  s_vars.tramNumber = -1;

  PebbleAppHandlers handlers = {
	  
    .init_handler = &handle_init,
    
    .tick_info = {
		.tick_handler = &handle_minute_tick,
		.tick_units = MINUTE_UNIT
	},

	.messaging_info = {
      .buffer_sizes = {
        .inbound = 64, // inbound buffer size in bytes
        .outbound = 16, // outbound buffer size in bytes
      },
      .default_callbacks.callbacks = {
        .in_received = msgReceived,
        .in_dropped = msgDropped,
      },
    }
  };
  app_event_loop(params, &handlers);
}
