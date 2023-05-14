#ifndef _UTF8_H
#define _UTF8_H

#include <stdbool.h>

typedef int utf8_t;

int utf8_len(utf8_t *str);
utf8_t *utf8_add_char(utf8_t *str, utf8_t c);
utf8_t *utf8_remove_last(utf8_t *str);
bool utf8_compare_subcstr(utf8_t *str, char *compare, int start);
char *utf8_to_cstr(utf8_t *str);

#endif
