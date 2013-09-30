#pragma once

#include "string.h"

void fuzzy_time_to_words(int hours, int minutes, char words[3][86], size_t length);
size_t append_number(char*, int);
