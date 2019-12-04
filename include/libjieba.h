// <BEGIN LICENSE>

// <END LICENSE>

#ifndef LIBJIEBA_H
#define LIBJIEBA_H

#ifdef __cplusplus
#include <cstdlib>
#else
#include <stdlib.h>
#include <stdbool.h>
#endif

#if defined(_WIN32)
#ifdef LIBBUILD
#define LIBJIEBA_API __declspec(dllexport)
#else
#define LIBJIEBA_API __declspec(dllimport)
#endif
#endif // 



extern "C" {

	typedef void* jieba_context;

	typedef struct jieba_words_t {
		char** words;
		unsigned int count;
	} jieba_words;

	typedef struct jieba_compact_word_t {
		const char* word;	// start
		unsigned int len;
	} jieba_compact_word;

	typedef struct jieba_compact_words {
		jieba_compact_word *words;
		unsigned int count;
	} jieba_compact_words;
	
	typedef struct jieba_tag_t {
		char* word;
		char* tag;
	} jieba_tag;

	typedef struct jieba_tags_t {
		jieba_tag *tags;
		unsigned int count;
		char* tag_list;
	} jieba_tags;
	
	LIBJIEBA_API jieba_context jieba_create_context(const char* dict_path, const char* hmm_path, const char* user_dict, const char* idf_path, const char* stop_words_path);
	LIBJIEBA_API void jieba_destroy_context(jieba_context ctx);
	LIBJIEBA_API void jieba_words_free(jieba_words* w);
	LIBJIEBA_API void jieba_compact_words_free(jieba_compact_words* w);
	LIBJIEBA_API void jieba_tags_free(jieba_tags* t);
	LIBJIEBA_API bool jieba_insert_user_word(jieba_context ctx, const char* word);
	LIBJIEBA_API bool jieba_insert_user_word_with_tag(jieba_context ctx, const char* word, const char* tag);


	LIBJIEBA_API int jieba_cut(jieba_context ctx, const char* sentence, jieba_words* words, bool hmm);
	LIBJIEBA_API int jieba_cut_full(jieba_context ctx, const char* sentence, unsigned int len, jieba_words* words, bool hmm);
	LIBJIEBA_API int jieba_cut_full_compact(jieba_context ctx, const char* sentence, unsigned int len, jieba_compact_words* words, bool hmm);
	LIBJIEBA_API int jieba_cut_all(jieba_context ctx, const char* sentence, jieba_words* words);
	LIBJIEBA_API int jieba_cut_all_full(jieba_context ctx, const char* sentence, unsigned int len, jieba_words* words);
	LIBJIEBA_API int jieba_cut_all_full_compact(jieba_context ctx, const char* sentence, unsigned int len, jieba_compact_words* words);
	LIBJIEBA_API int jieba_cut_for_search(jieba_context ctx, const char* sentence, jieba_words* words, bool hmm);
	LIBJIEBA_API int jieba_cut_for_search_full(jieba_context ctx, const char* sentence, unsigned int len, jieba_words* words, bool hmm);
	LIBJIEBA_API int jieba_cut_for_search_full_compact(jieba_context ctx, const char* sentence, unsigned int len, jieba_compact_words* words, bool hmm);

}

#endif // LIBJIEBA_H
