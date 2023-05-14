#ifndef _UTF8_H
#define _UTF8_H

typedef int utf8_t;

int utf8_len(utf8_t *str);
utf8_t *utf8_add_char(utf8_t *str, utf8_t c);
utf8_t *utf8_remove_last(utf8_t *str);
char *utf8_to_cstr(utf8_t *str);

#endif
