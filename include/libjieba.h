// <BEGIN LICENSE>

// <END LICENSE>

#ifndef LIBJIEBA_H
#define LIBJIEBA_H

#if defined(__unix__)
#include <unistd.h>
#else
#include <cstring>
#endif

#ifdef __cplusplus
#include <cstdlib>
#else
#include <stdlib.h>
#endif

extern "C" {

typedef void *jieba_context;

typedef struct jieba_words_t {
    char **words;
    int count;
} jieba_words;

jieba_context
jieba_create_context(const char *dict_path, const char *hmm_path, const char *user_dict, const char *idf_path, const char *stop_words_path);

void jieba_destroy_context(jieba_context ctx);

int jieba_cut(jieba_context ctx, const char *sentence, size_t len, jieba_words *words);

void jieba_words_free(jieba_words *w);
}

#endif // LIBJIEBA_H
