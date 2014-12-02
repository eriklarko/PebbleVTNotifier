#define BUFFER_SIZE 86
// a1a55b78-9e29-4a97-a9f9-e86b5c6580c5
#define MY_UUID { 0xA1, 0xA5, 0x5B, 0x78, 0x9E, 0x29, 0x4A, 0x97, 0xA9, 0xF9, 0xE8, 0x6B, 0x5C, 0x65, 0x80, 0xC5 }
PBL_APP_INFO(MY_UUID,
             "VTNotifier", "Epicness",
             0, 1, /* App version */
             DEFAULT_MENU_ICON,
             APP_INFO_WATCH_FACE);

void init_gui(void);
void init_bold_text(void);
void init_thin_text(void);
void init_tram_info(void);
void init_tram_time(void);
void init_text_layer(TextLayer*, GRect, GFont);

void update_tram(void);
void hide_tram_info_if_timedout(void);
void clear_tram(void);
char* build_tram_string(int);
char* build_tram_departure_time(int);
char* build_date_string(PblTm*);

void build_string_array(void);
void handle_minute_tick(AppContextRef, PebbleTickEvent*);
void update_time(PblTm*);

