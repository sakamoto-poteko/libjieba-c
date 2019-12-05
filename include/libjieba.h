// <BEGIN LICENSE>
/*
 * Copyright 2019 Poteko Sakamoto

 * Permission is hereby granted, free of charge, to any person obtaining a copy of this
 * software and associated documentation files (the "Software"), to deal in the Software
 * without restriction, including without limitation the rights to use, copy, modify,
 * merge, publish, distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to the
 * following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies
 * or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
 * PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
 * OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */
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


#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

    typedef void* jieba_context;
    typedef void* jieba_extractor_context;

    typedef struct jieba_words_t {
        char** words;
        unsigned int count;
    } jieba_words;

    typedef struct jieba_zerocopy_word_t {
        const char* word;	// start
        unsigned int len;
    } jieba_zerocopy_word;

    typedef struct jieba_zerocopy_words {
        jieba_zerocopy_word* words;
        unsigned int count;
    } jieba_zerocopy_words;

    typedef struct jieba_tags_t {
        char** tags;
        char** words;
        unsigned int count;
    } jieba_tags;

    typedef struct jieba_weights_t {
        char** words;
        double* weights;
        unsigned int count;
    } jieba_weights;

    LIBJIEBA_API jieba_context jieba_create_context(const char* dict_path, const char* hmm_path, const char* user_dict, const char* idf_path, const char* stop_words_path);
    LIBJIEBA_API jieba_extractor_context jieba_extractor_create_context(const char* dict_path, const char* hmm_path, const char* user_dict, const char* idf_path, const char* stop_words_path);
    LIBJIEBA_API jieba_extractor_context jieba_extractor_create_context_from_jieba_context(jieba_context ctx);
    LIBJIEBA_API void jieba_destroy_context(jieba_context ctx);
    LIBJIEBA_API void jieba_extractor_destroy_context(jieba_extractor_context ctx);

    LIBJIEBA_API void jieba_words_free(jieba_words* w);
    LIBJIEBA_API void jieba_zerocopy_words_free(jieba_zerocopy_words* w);
    LIBJIEBA_API void jieba_tags_free(jieba_tags* t);
    LIBJIEBA_API void jieba_weights_free(jieba_weights* t);

    LIBJIEBA_API bool jieba_insert_user_word(jieba_context ctx, const char* word);
    LIBJIEBA_API bool jieba_insert_user_word_with_tag(jieba_context ctx, const char* word, const char* tag);


    LIBJIEBA_API unsigned int jieba_cut(jieba_context ctx, const char* sentence, jieba_words* words, bool hmm);
    LIBJIEBA_API unsigned int jieba_cut_full(jieba_context ctx, const char* sentence, unsigned int len, jieba_words* words, bool hmm);
    LIBJIEBA_API unsigned int jieba_cut_full_zerocopy(jieba_context ctx, const char* sentence, unsigned int len, jieba_zerocopy_words* words, bool hmm);
    LIBJIEBA_API unsigned int jieba_cut_all(jieba_context ctx, const char* sentence, jieba_words* words);
    LIBJIEBA_API unsigned int jieba_cut_all_full(jieba_context ctx, const char* sentence, unsigned int len, jieba_words* words);
    LIBJIEBA_API unsigned int jieba_cut_all_full_zerocopy(jieba_context ctx, const char* sentence, unsigned int len, jieba_zerocopy_words* words);
    LIBJIEBA_API unsigned int jieba_cut_for_search(jieba_context ctx, const char* sentence, jieba_words* words, bool hmm);
    LIBJIEBA_API unsigned int jieba_cut_for_search_full(jieba_context ctx, const char* sentence, unsigned int len, jieba_words* words, bool hmm);
    LIBJIEBA_API unsigned int jieba_cut_for_search_full_zerocopy(jieba_context ctx, const char* sentence, unsigned int len, jieba_zerocopy_words* words, bool hmm);

    LIBJIEBA_API unsigned int jieba_cut_with_tag(jieba_context ctx, const char* sentence, jieba_tags* result);

    LIBJIEBA_API unsigned int jieba_extract(jieba_extractor_context ctx, const char* sentence, unsigned int top_n, jieba_weights* weights);
    LIBJIEBA_API unsigned int jieba_extract_full(jieba_extractor_context ctx, const char* sentence, int len, unsigned int top_n, jieba_weights* weights);


#ifdef __cplusplus
}
#endif

#endif // LIBJIEBA_H
