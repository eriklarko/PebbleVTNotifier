#include "string.h"

static const char* const DAYS[] = {
  "sön",
  "mån",
  "tis",
  "ons",
  "tors",
  "fre",
  "lör"
};

char* weekday_index_to_string(int index) {
	static char buffer[10];
	memset(buffer, 0, 10);
	strcat(buffer, DAYS[index]);
	return buffer;
}
