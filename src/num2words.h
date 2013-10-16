#pragma once

#include "string.h"
#include "pebble_app.h"

void fuzzy_time_to_words(PblTm*, char words[3][86], size_t length);
size_t append_number(char*, int);
