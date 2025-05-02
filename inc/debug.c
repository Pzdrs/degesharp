#include <stdio.h>
#include <stdbool.h>

extern bool verbose_flex;

void _(const char* token) {
      if (verbose_flex) printf("%s ", token);
}