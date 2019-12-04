// <BEGIN LICENSE>

// <END LICENSE>

#ifndef LIBJIEBA_H
#define LIBJIEBA_H

#ifdef __cplusplus
#include <cstdlib>
#else
#include <stdlib.h>
#endif

#if defined(_WIN32)
#ifdef LIBBUILD
#define LIBJIEBA_API __declspec(dllexport)
#else
#define LIBJIEBA_API __declspec(dllimport)
#endif
#endif // 



extern "C" {

typedef void *jieba_context;

typedef struct jieba_word_t {
	char* word;
	unsigned int len;
} jieba_word;
	
typedef struct jieba_words_t {
    jieba_word *words;
    unsigned int count;
	int allocated;
} jieba_words;

LIBJIEBA_API jieba_context jieba_create_context(const char *dict_path, const char *hmm_path, const char *user_dict, const char *idf_path, const char *stop_words_path);

LIBJIEBA_API void jieba_destroy_context(jieba_context ctx);

/// sentence: UTF-8
LIBJIEBA_API int jieba_cut(jieba_context ctx, const char* sentence, jieba_words* words, int hmm);

LIBJIEBA_API int jieba_cut_full(jieba_context ctx, const char *sentence, unsigned int len, int allocate_words, jieba_words *words, int hmm);

LIBJIEBA_API int jieba_cut_all(jieba_context ctx, const char* sentence, jieba_words* words);

LIBJIEBA_API int jieba_cut_all_full(jieba_context ctx, const char* sentence, unsigned int len, int allocate_words, jieba_words* words);

LIBJIEBA_API int jieba_cut_for_search(jieba_context ctx, const char* sentence, jieba_words* words, int hmm);

LIBJIEBA_API int jieba_cut_for_search_full(jieba_context ctx, const char* sentence, unsigned int len, int allocate_words, jieba_words* words, int hmm);
	
LIBJIEBA_API void jieba_words_free(jieba_words *w);
	
}

#endif // LIBJIEBA_H
