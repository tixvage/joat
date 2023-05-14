#include "utf8.h"

#include <stdlib.h>
#include <string.h>

int utf8_len(utf8_t *str) {
    if (str == NULL) return 0;
    int count = 0;
    while(*str != 0)
    {
        count++;
        str++;
    }
    return count;
}

utf8_t *utf8_add_char(utf8_t *str, utf8_t c) {
    if (str == NULL) {
        str = calloc(1 + 1, sizeof(utf8_t));
        str[0] = c;
    } else {
        int len = utf8_len(str);
        str = realloc(str, (len + 2) * sizeof(utf8_t));
        str[len] = c;
        str[len + 1] = 0;
    }

    return str;
}

utf8_t *utf8_remove_last(utf8_t *str) {
    if (str == NULL) return str;
    int len = utf8_len(str);
    if (len <= 0) return 0;
    str = realloc(str, len * sizeof(utf8_t));
    str[len - 1] = 0;

    return str;
}

bool utf8_compare_subcstr(utf8_t *str, char *compare, int start) {
    int len_str = utf8_len(str);
    int len_compare = strlen(compare);

    if (len_str < start + len_compare) {
        return false;
    }

    for (int i = start; i < len_compare; i++) {
        if (str[i] != compare[i - start]) {
            return false;
        }
    }

    return true;
}


char *utf8_to_cstr(utf8_t *str) {
    int len = utf8_len(str);
    int loc = 0;
    char* cstr = calloc(loc + 1, sizeof(char));
    for (int i = 0; i < len; i++) {
        wchar_t ch = str[i];
        if (ch < 0x80) {
            cstr[loc] = ch;
            loc++;
            cstr = realloc(cstr, (loc + 1) * sizeof(char));
        } else if (ch < 0x800) {
            cstr[loc] = 0xc0 | (ch >> 6);
            loc++;
            cstr = realloc(cstr, (loc + 1) * sizeof(char));
            cstr[loc] = 0x80 | (ch & 0x003f);
            loc++;
            cstr = realloc(cstr, (loc + 1) * sizeof(char));
        } else {
            cstr[loc] = 0xe0 | (ch >> 12);
            loc++;
            cstr = realloc(cstr, (loc + 1) * sizeof(char));
            cstr[loc] = 0x80 | ((ch >> 6) & 0x003f);
            loc++;
            cstr = realloc(cstr, (loc + 1) * sizeof(char));
            cstr[loc] = 0x80 | (ch & 0x003f);
            loc++;
            cstr = realloc(cstr, (loc + 1) * sizeof(char));
        }
    }

    cstr[loc] = 0;
    return cstr;
}
