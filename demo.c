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

#include <stdio.h>
#include <stdlib.h>

#include "libjieba.h"

static void print_jieba_words(const char* sentence, jieba_words* words)
{
    printf("%s\n", sentence);

    for (unsigned i = 0; i < words->count; ++i)
    {
        printf("%s/", words->words[i]);
    }
    printf("\n\n");
}

static void print_jieba_tags(const char* sentence, jieba_tags* tags)
{
    printf("%s\n", sentence);

    for (unsigned i = 0; i < tags->count; ++i)
    {
        printf("%s:%s, ", tags->words[i], tags->tags[i]);
    }
    printf("\n\n");
}

static void print_jieba_weights(const char* sentence, jieba_weights* weights)
{
    printf("%s\n", sentence);

    for (unsigned i = 0; i < weights->count; ++i)
    {
        printf("%s: %f, ", weights->words[i], weights->weights[i]);
    }
    printf("\n\n");
}

int main(void)
{
#ifdef _WIN32
    system("chcp 65001");   // UTF-8
#endif 

    const char* DICT_PATH = "E:\\jiebadict\\dict\\jieba.dict.utf8";
    const char* HMM_PATH = "E:\\jiebadict\\dict\\hmm_model.utf8";
    const char* USER_DICT = "E:\\jiebadict\\dict\\user.dict.utf8";
    const char* IDF_PATH = "E:\\jiebadict\\dict\\idf.utf8";
    const char* STOP_WORDS_PATH = "E:\\jiebadict\\dict\\stop_words.utf8";

    const char* test_sentence1 = u8"他来到了网易杭研大厦";
    const char* test_sentence2 = u8"我来到北京清华大学";
    const char* test_sentence3 = u8"小明硕士毕业于中国科学院计算所，后在日本京都大学深造";
    const char* test_sentence4 = u8"我是拖拉机学院手扶拖拉机专业的。不用多久，我就会升职加薪，当上CEO，走上人生巅峰。";

    jieba_context ctx = jieba_create_context(DICT_PATH, HMM_PATH, USER_DICT, IDF_PATH, STOP_WORDS_PATH);

    jieba_words words_cut_hmm = { 0 };
    jieba_words words_cut_nohmm = { 0 };
    jieba_words words_cut_all = { 0 };
    jieba_words words_cut_for_search = { 0 };

    jieba_tags tags = { 0 };

    // Cut w/ HMM
    unsigned rc = jieba_cut(ctx, test_sentence1, &words_cut_hmm, true);
    print_jieba_words(test_sentence1, &words_cut_hmm);
    jieba_words_free(&words_cut_hmm);

    // Cut w/o HMM
    rc = jieba_cut(ctx, test_sentence1, &words_cut_nohmm, false);
    print_jieba_words(test_sentence1, &words_cut_nohmm);
    jieba_words_free(&words_cut_nohmm);

    // Cut All
    rc = jieba_cut_all(ctx, test_sentence2, &words_cut_all);
    print_jieba_words(test_sentence2, &words_cut_all);
    jieba_words_free(&words_cut_all);

    // Cut for Search
    rc = jieba_cut_for_search(ctx, test_sentence3, &words_cut_for_search, true);
    print_jieba_words(test_sentence3, &words_cut_for_search);
    jieba_words_free(&words_cut_for_search);

    // Cut w/ Tag
    rc = jieba_cut_with_tag(ctx, test_sentence4, &tags);
    print_jieba_tags(test_sentence4, &tags);
    jieba_tags_free(&tags);

    jieba_weights weights;

    // Get extractor from Jieba context
    jieba_extractor_context exctx = jieba_extractor_create_context_from_jieba_context(ctx);
    rc = jieba_extract(exctx, test_sentence4, 5, &weights);
    print_jieba_weights(test_sentence4, &weights);
    jieba_weights_free(&weights);

    // !!!!! Always destroy extractor context before destroying Jieba context !!!
    // If extractor ctx isn't created, destroy isn't needed
    jieba_extractor_destroy_context(exctx);

    jieba_destroy_context(ctx);

    // Build a new extractor
    exctx = jieba_extractor_create_context(DICT_PATH, HMM_PATH, USER_DICT, IDF_PATH, STOP_WORDS_PATH);
    rc = jieba_extract(exctx, test_sentence4, 5, &weights);
    print_jieba_weights(test_sentence4, &weights);
    jieba_weights_free(&weights);
    jieba_extractor_destroy_context(exctx);

    return 0;
}
