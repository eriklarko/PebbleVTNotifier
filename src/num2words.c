#include "num2words.h"
#include "string.h"
#include "pebble_app.h"

static const char* const ONES[] = {
  "noll",
  "ett",
  "två",
  "tre",
  "fyra",
  "fem",
  "sex",
  "sju",
  "åtta",
  "nio"
};

static const char* const TEENS[] ={
  "",
  "elva",
  "tolv",
  "tretton",
  "fjorton",
  "femton",
  "sexton",
  "sjutton",
  "arton",
  "nitton"
};

static const char* const TENS[] = {
  "",
  "tio",
  "tjugo",
  "trettio",
  "fyrtio",
  "femtio",
  "sextio",
  "sjuttio",
  "åttio",
  "nittio"
};

static const char* STR_OH_CLOCK = "o'clock";
static const char* STR_NOON = "noon";
static const char* STR_MIDNIGHT = "midnight";
static const char* STR_QUARTER = "kvart";
static const char* STR_TO = "i";
static const char* STR_PAST = "över";
static const char* STR_HALF = "halv";
static const char* STR_AFTER = "after";

size_t append_number(char* words, int num) {
  int tens_val = num / 10 % 10;
  int ones_val = num % 10;

  size_t len = 0;

  if (tens_val > 0) {
    if (tens_val == 1 && num != 10) {
      strcat(words, TEENS[ones_val]);
      return strlen(TEENS[ones_val]);
    }
    strcat(words, TENS[tens_val]);
    len += strlen(TENS[tens_val]);
    if (ones_val > 0) {
      strcat(words, " ");
      len += 1;
    }
  }

  if (ones_val > 0 || num == 0) {
    strcat(words, ONES[ones_val]);
    len += strlen(ONES[ones_val]);
  }
  return len;
}

static size_t append_string(char* buffer, const size_t length, const char* str) {
  strcat(buffer, str);

  size_t written = strlen(str);
  return (length > written) ? written : length;
}


void fuzzy_time_to_words(PblTm* time, char words[3][86], size_t length) {
  int fuzzy_hours = time->tm_hour;
  int fuzzy_minutes = ((time->tm_min + 2) / 5) * 5;

  // Handle hour & minute roll-over.
  if (fuzzy_minutes >= 25) {
	if (fuzzy_minutes > 55) {
      fuzzy_minutes = 0;
    }
    
    fuzzy_hours += 1;
    if (fuzzy_hours > 23) {
      fuzzy_hours = 0;
    }
  }
  
  memset(*words, 0, length);
  memset(*(words+1), 0, length);
  memset(*(words+2), 0, length);
  
  if(time->tm_mon == 11 && time->tm_mday == 24 && fuzzy_hours == 15) {
	  append_string(*(words+2), 86, "kalle");
  } else if(time->tm_wday == 5 && fuzzy_hours == 15) {
	  append_string(*(words+2), 86, "öl");
  } else {
	  
    if(fuzzy_hours >= 13) {
	  fuzzy_hours -= 12;
    }  
	append_number(*(words+2), fuzzy_hours);
  }
  
  if(fuzzy_minutes > 0 && fuzzy_minutes <= 20) {
	if(fuzzy_minutes == 15) {
	  append_string(*words, 86, STR_QUARTER);
	} else {
	  append_number(*words, fuzzy_minutes);
	}
	append_string(*(words+1), 86, STR_PAST);
  } else if(fuzzy_minutes >= 40) {
	fuzzy_minutes = 60 - fuzzy_minutes;
	if(fuzzy_minutes == 15) {
	  append_string(*words, 86, STR_QUARTER);
	} else {
	  append_number(*words, fuzzy_minutes);
	}
	append_string(*(words+1), 86, STR_TO);
  } else if(fuzzy_minutes == 25) {
	append_number(*words, 5);
	append_string(*(words+1), 86, STR_TO);
	append_string(*(words+1), 86, " ");
	append_string(*(words+1), 86, STR_HALF);
  } else if(fuzzy_minutes == 30) {
	append_string(*(words+1), 86, STR_HALF);
  } else if(fuzzy_minutes == 35) {
	append_number(*words, 5);
	//append_string(*(words+1), 86, STR_PAST);
	append_string(*(words+1), 86, "öv ");
	append_string(*(words+1), 86, STR_HALF);
  }/* else {
	snprintf(*words, 86, "?? %d", fuzzy_minutes);
  }*/
}
