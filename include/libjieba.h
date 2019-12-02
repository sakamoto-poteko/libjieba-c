// <BEGIN LICENSE>

// <END LICENSE>

#ifndef LIBJIEBA_H
#define LIBJIEBA_H

#if defined(__unix__)
#include <unistd.h>
#else
#include <cstring>
#endif

extern "C" {

typedef void *jieba_context;

jieba_context jieba_create_context(const char *dict_path, const char *hmm_path, const char *user_dict, const char *idf_path, const char *stop_words_path);

void jieba_destroy_context(jieba_context ctx);
}

#endif // LIBJIEBA_H
