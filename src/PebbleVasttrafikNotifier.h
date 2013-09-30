#define BUFFER_SIZE 86
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

char* build_tram_string(int);
char* build_tram_departure_time(int);

void build_string_array(void);
void handle_minute_tick(AppContextRef, PebbleTickEvent*);
void update_time(PblTm*);
