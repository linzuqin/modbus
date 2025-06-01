#ifndef __STRING_OPTIMIZE_H__
#define __STRING_OPTIMIZE_H__
#include "stm32f10x.h"                  // Device header

#include <stdint.h>
#include <string.h>
#include <limits.h>
#include <stdlib.h>
#include <stdio.h>

typedef struct {
    const char *pattern;
    uint16_t pat_len;
    uint8_t bad_char_shift[64];
    uint8_t *good_suffix_shift;
} bm_matcher_t;

void bm_init(bm_matcher_t *matcher, const char *pattern);
const char *bm_search(const bm_matcher_t *matcher, const char *text, int text_len);
void bm_release(bm_matcher_t *matcher);

#endif
