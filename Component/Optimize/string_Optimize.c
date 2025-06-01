#include "string_Optimize.h"


static void preprocess_bad_char(bm_matcher_t *matcher) {
    for (uint8_t i = 0; i < 64; i++) {
        matcher->bad_char_shift[i] = matcher->pat_len;
    }
    for (uint8_t i = 0; i < matcher->pat_len - 1; i++) {
        matcher->bad_char_shift[(uint8_t)matcher->pattern[i]] = matcher->pat_len - 1 - i;
    }
}

static void preprocess_good_suffix(bm_matcher_t *matcher) {
    uint8_t *f = (uint8_t *)malloc((matcher->pat_len + 1) * sizeof(uint8_t));
    uint16_t i = matcher->pat_len, j = matcher->pat_len + 1;
    f[i] = j;
    
    while (i > 0) {
        while (j <= matcher->pat_len && matcher->pattern[i-1] != matcher->pattern[j-1]) {
            if (matcher->good_suffix_shift[j] == 0) {
                matcher->good_suffix_shift[j] = j - i;
            }
            j = f[j];
        }
        i--; j--;
        f[i] = j;
    }
    
    j = f[0];
    for (i = 0; i <= matcher->pat_len; i++) {
        if (matcher->good_suffix_shift[i] == 0) {
            matcher->good_suffix_shift[i] = j;
        }
        if (i == j) {
            j = f[j];
        }
    }
    free(f);
}

void bm_init(bm_matcher_t *matcher, const char *pattern) {
    matcher->pattern = pattern;
    matcher->pat_len = strlen(pattern);
    matcher->good_suffix_shift = (uint8_t *)calloc(matcher->pat_len + 1, sizeof(uint8_t));
    preprocess_bad_char(matcher);
    preprocess_good_suffix(matcher);
}

const char *bm_search(const bm_matcher_t *matcher, const char *text, int text_len) {
    int i, j;
    int shift;
    
    for (i = 0; i <= text_len - matcher->pat_len; ) {
        for (j = matcher->pat_len - 1; j >= 0 && matcher->pattern[j] == text[i+j]; j--);
        
        if (j < 0) {
            return text + i; // 匹配成功
        } else {
            shift = matcher->good_suffix_shift[j+1];
            if (shift < matcher->bad_char_shift[(uint8_t)text[i+j]] - (matcher->pat_len - 1 - j)) {
                shift = matcher->bad_char_shift[(uint8_t)text[i+j]] - (matcher->pat_len - 1 - j);
            }
            i += shift;
        }
    }
    return NULL;
}

void bm_release(bm_matcher_t *matcher) {
    free(matcher->good_suffix_shift);
}
