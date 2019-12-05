// <BEGIN LICENSE>

// <END LICENSE>


#if defined(__unix__)
#include <unistd.h>
#else
#include <cstring>
#endif

#if defined(_WIN32)
#define strdup _strdup
#endif

#include <vector>
#include <string>

#include "libjieba.h"
#include "cppjieba/Jieba.hpp"


// Local decl
static unsigned int tag_vector_to_jieba_tags(const std::vector<std::pair<std::string, std::string>>& tags, jieba_tags* result);
static unsigned int string_vector_to_jieba_words(const std::vector<std::string>& input, jieba_words* words);
static unsigned int string_vector_to_jieba_zerocopy_words(const std::vector<std::string>& input, jieba_zerocopy_words* words, const char* original);
static unsigned int weight_vector_to_jieba_weights(const std::vector<std::pair<std::string, double>>& input, jieba_weights* weights);
static cppjieba::Jieba* ctx2jieba(jieba_context ctx);

struct __jieba_extractor_context {
    bool created_from_jieba_context;
    cppjieba::KeywordExtractor& extractor;

    __jieba_extractor_context(bool fromJiebaCtx, cppjieba::KeywordExtractor& ex) :
        created_from_jieba_context(fromJiebaCtx), extractor(ex)
    {
    }
};

// Local functions
static unsigned int string_vector_to_jieba_words(const std::vector<std::string>& input, jieba_words* words)
{
    words->words = nullptr;
    words->count = 0;

    if (!input.empty()) {
        words->count = input.size();

        // 1. allocate jieba_word array
        words->words = (char**)malloc(sizeof(char*) * input.size());
        if (words->words == nullptr)
        {
            return 0;
        }

        // 2. calculate words array len and allocate
        std::size_t words_array_len_with_term = 0;
        for (const auto& w : input)
        {
            words_array_len_with_term += w.size() + 1;
        }

        char* words_array = (char*)malloc(words_array_len_with_term);
        if (words_array == nullptr)
        {
            free(words->words);
            return 0;
        }

        std::size_t offset = 0;

        // 3. populate jieba_word array
        for (std::size_t i = 0; i < input.size(); ++i) {
            words->words[i] = words_array + offset;
            const auto word_len = input[i].size();
            std::strncpy(words->words[i], input[i].data(), word_len)[word_len] = 0;

            offset += word_len + 1;  // with \0
        }
    }

    return input.size();
}

static unsigned int string_vector_to_jieba_zerocopy_words(const std::vector<std::string>& input, jieba_zerocopy_words* words, const char* original)
{
    words->words = nullptr;
    words->count = 0;

    if (!input.empty())
    {
        words->words = (jieba_zerocopy_word*)malloc(sizeof(jieba_zerocopy_word) * input.size());
        if (words->words == nullptr)
        {
            return 0;
        }

        for (std::size_t i = 0, offset = 0; i < input.size(); ++i)
        {
            jieba_zerocopy_word* w = words->words + i;
            w->word = original + offset;
            const auto len = input[i].size();
            w->len = len;
            offset += len;
        }
    }

    return input.size();
}

static unsigned int tag_vector_to_jieba_tags(const std::vector<std::pair<std::string, std::string>>& tags, jieba_tags* result)
{
    result->count = tags.size();

    if (tags.empty())
    {
        result->count = 0;
        result->tags = nullptr;
        result->words = nullptr;
        return 0;
    }

    std::size_t words_array_len = 0, tags_array_len = 0;

    for (const auto& pair : tags)
    {
        words_array_len += pair.first.size() + 1;
        tags_array_len += pair.second.size() + 1;
    }

    result->count = tags.size();
    result->words = (char**)malloc(sizeof(char*) * tags.size());
    if (!result->words)
    {
        goto err_alloc_tags_words_array;
    }

    result->tags = (char**)malloc(sizeof(char*) * tags.size());
    if (!result->tags)
    {
        goto err_alloc_tags_tags_array;
    }

    result->words[0] = (char*)malloc(words_array_len);
    if (!result->words[0])
    {
        goto err_alloc_tags_words;
    }

    result->tags[0] = (char*)malloc(tags_array_len);
    if (!result->tags[0])
    {
        goto err_alloc_tags_tags;
    }

    std::size_t words_offset = 0, tags_offset = 0;
    for (std::size_t i = 0; i < tags.size(); ++i)
    {
        char* word_ptr = result->words[0] + words_offset;
        char* tag_ptr = result->tags[0] + tags_offset;

        result->words[i] = word_ptr;
        result->tags[i] = tag_ptr;

        const auto& word = tags[i].first;
        const auto word_size = word.size();
        const auto& tag = tags[i].second;
        const auto tag_size = tag.size();
        std::strncpy(word_ptr, word.data(), word_size)[word_size] = 0;
        std::strncpy(tag_ptr, tag.data(), tag_size)[tag_size] = 0;

        words_offset += word_size + 1;
        tags_offset += tag_size + 1;
    }

    return tags.size();

err_alloc_tags_tags:
    free(result->words[0]);
    result->words[0] = nullptr;
err_alloc_tags_words:
    free(result->tags);
    result->tags = nullptr;
err_alloc_tags_tags_array:
    free(result->words);
    result->words = nullptr;
err_alloc_tags_words_array:
    return 0;
}

static unsigned int weight_vector_to_jieba_weights(const std::vector<std::pair<std::string, double>>& input, jieba_weights* weights)
{
    weights->count = input.size();
    weights->weights = nullptr;
    weights->words = nullptr;

    std::size_t word_array_len = 0;
    for (const auto& pair : input)
    {
        word_array_len += pair.first.size() + 1;
    }

    weights->words = (char**)malloc(sizeof(char*) * input.size());
    if (!weights->words)
    {
        goto err_alloc_wv2w_words_array;
    }

    weights->words[0] = (char*)malloc(word_array_len);
    if (!weights->words[0])
    {
        goto err_alloc_wv2w_words;
    }

    weights->weights = (double*)malloc(sizeof(double) * input.size());
    if (!weights->weights)
    {
        goto err_alloc_wv2w_weights;
    }

    std::size_t offset = 0;
    for (std::size_t i = 0; i < input.size(); ++i)
    {
        const auto& word = input[i].first;
        const std::size_t word_size = word.size();
        const double weight = input[i].second;

        char* word_ptr = weights->words[0] + offset;
        weights->words[i] = word_ptr;
        std::strncpy(word_ptr, word.data(), word_size)[word_size] = 0;
        offset += word_size + 1;

        weights->weights[i] = weight;
    }

    return input.size();

err_alloc_wv2w_weights:
    free(weights->words[0]);
    weights->words[0] = nullptr;

err_alloc_wv2w_words:
    free(weights->words);
    weights->words = nullptr;

err_alloc_wv2w_words_array:
    return 0;
}


static cppjieba::Jieba* ctx2jieba(jieba_context ctx)
{
    return static_cast<cppjieba::Jieba*>(ctx);
}

static cppjieba::KeywordExtractor& ctx2extractor(jieba_extractor_context ctx)
{
    auto exctx = static_cast<__jieba_extractor_context*>(ctx);
    return exctx->extractor;
}

// Jieba obj & utils

jieba_context jieba_create_context(const char* dict_path, const char* hmm_path, const char* user_dict, const char* idf_path, const char* stop_words_path)
{
    const jieba_context ctx = static_cast<jieba_context>(new cppjieba::Jieba(dict_path, hmm_path, user_dict, idf_path, stop_words_path));
    return ctx;
}

jieba_extractor_context jieba_extractor_create_context(const char* dict_path, const char* hmm_path,
    const char* user_dict, const char* idf_path, const char* stop_words_path)
{
    const auto extractor = new cppjieba::KeywordExtractor(dict_path,
        hmm_path,
        idf_path,
        stop_words_path,
        user_dict);

    auto* ctx = new __jieba_extractor_context(false, *extractor);
    return ctx;
}

jieba_extractor_context jieba_extractor_create_context_from_jieba_context(jieba_context ctx)
{
    auto jieba = ctx2jieba(ctx);
    auto* exctx = new __jieba_extractor_context(true, jieba->extractor);
    return exctx;
}

void jieba_destroy_context(jieba_context ctx)
{
    delete ctx2jieba(ctx);
}

void jieba_extractor_destroy_context(jieba_extractor_context ctx)
{
    auto exctx = static_cast<__jieba_extractor_context*>(ctx);
    if (exctx->created_from_jieba_context)
    {
        delete& exctx->extractor;
    }

    delete exctx;
}

void jieba_words_free(jieba_words* w)
{
    if (!w)
        return;

    if (w && w->words) {
        free(w->words[0]);
        free(w->words);
    }
}

void jieba_zerocopy_words_free(jieba_zerocopy_words* w)
{
    if (!w)
        return;

    if (w->words)
    {
        free(w->words);
    }
}

void jieba_tags_free(jieba_tags* t)
{
    if (!t) {
        return;
    }

    if (t->words)
    {
        free(t->words[0]);
        free(t->words);
    }

    if (t->tags)
    {
        free(t->tags[0]);
        free(t->tags);
    }
}

void jieba_weights_free(jieba_weights* t)
{
    if (!t)
        return;

    if (t->words)
    {
        free(t->words[0]);
        free(t->words);
    }

    if (t->weights)
    {
        free(t->weights);
    }
}

bool jieba_insert_user_word(jieba_context ctx, const char* word)
{
    return ctx2jieba(ctx)->InsertUserWord(word);
}

bool jieba_insert_user_word_with_tag(jieba_context ctx, const char* word, const char* tag)
{
    return ctx2jieba(ctx)->InsertUserWord(word, tag);
}

// Cut

#define CUTF_SIGNATURE jieba_context ctx, const std::string& s, std::vector<std::string>& result, bool hmm

template <typename cutF>
unsigned int generic_cut_jieba_words(jieba_context ctx, const char* sentence, unsigned int len, jieba_words* output, bool hmm, cutF fnCut)
{
    std::vector<std::string> result;
    const std::string s(sentence, len);

    fnCut(ctx, s, result, hmm);

    return string_vector_to_jieba_words(result, output);
}

template <typename cutF>
unsigned int generic_cut_jieba_zerocopy_words(jieba_context ctx, const char* sentence, unsigned int len, jieba_zerocopy_words* output, bool hmm, cutF fnCut)
{
    std::vector<std::string> result;
    const std::string s(sentence, len);

    fnCut(ctx, s, result, hmm);

    return string_vector_to_jieba_zerocopy_words(result, output, sentence);
}

unsigned int jieba_cut(jieba_context ctx, const char* sentence, jieba_words* words, bool hmm)
{
    return generic_cut_jieba_words(ctx, sentence, std::strlen(sentence), words, hmm,
        [](CUTF_SIGNATURE)
        {
            const auto jieba = ctx2jieba(ctx);
            jieba->Cut(s, result, hmm);
        });
}

unsigned int jieba_cut_full(jieba_context ctx, const char* sentence, unsigned int len, jieba_words* words, bool hmm)
{
    return generic_cut_jieba_words(ctx, sentence, len, words, hmm,
        [](CUTF_SIGNATURE)
        {
            const auto jieba = ctx2jieba(ctx);
            jieba->Cut(s, result, hmm);
        });
}

unsigned int jieba_cut_full_zerocopy(jieba_context ctx, const char* sentence, unsigned int len, jieba_zerocopy_words* words, bool hmm)
{
    return generic_cut_jieba_zerocopy_words(ctx, sentence, len, words, hmm,
        [](CUTF_SIGNATURE)
        {
            const auto jieba = ctx2jieba(ctx);
            jieba->Cut(s, result, hmm);
        });
}

unsigned int jieba_cut_all(jieba_context ctx, const char* sentence, jieba_words* words)
{
    return generic_cut_jieba_words(ctx, sentence, std::strlen(sentence), words, false,
        [](CUTF_SIGNATURE)
        {
            const auto jieba = ctx2jieba(ctx);
            jieba->CutAll(s, result);
        });
}

unsigned int jieba_cut_all_full(jieba_context ctx, const char* sentence, unsigned int len, jieba_words* words)
{
    return generic_cut_jieba_words(ctx, sentence, len, words, false,
        [](CUTF_SIGNATURE)
        {
            const auto jieba = ctx2jieba(ctx);
            jieba->CutAll(s, result);
        });
}

unsigned int jieba_cut_all_full_zerocopy(jieba_context ctx, const char* sentence, unsigned int len, jieba_zerocopy_words* words)
{
    return generic_cut_jieba_zerocopy_words(ctx, sentence, len, words, false,
        [](CUTF_SIGNATURE)
        {
            const auto jieba = ctx2jieba(ctx);
            jieba->CutAll(s, result);
        });
}


unsigned int jieba_cut_for_search(jieba_context ctx, const char* sentence, jieba_words* words, bool hmm)
{
    return generic_cut_jieba_words(ctx, sentence, std::strlen(sentence), words, hmm,
        [](CUTF_SIGNATURE)
        {
            const auto jieba = ctx2jieba(ctx);
            jieba->CutForSearch(s, result, hmm);
        });
}

unsigned int jieba_cut_for_search_full(jieba_context ctx, const char* sentence, unsigned int len, jieba_words* words, bool hmm)
{
    return generic_cut_jieba_words(ctx, sentence, len, words, hmm,
        [](CUTF_SIGNATURE)
        {
            const auto jieba = ctx2jieba(ctx);
            jieba->CutForSearch(s, result, hmm);
        });
}

unsigned int jieba_cut_for_search_full_zerocopy(jieba_context ctx, const char* sentence, unsigned int len, jieba_zerocopy_words* words, bool hmm)
{
    return generic_cut_jieba_zerocopy_words(ctx, sentence, len, words, hmm,
        [](CUTF_SIGNATURE)
        {
            const auto jieba = ctx2jieba(ctx);
            jieba->CutForSearch(s, result, hmm);
        });
}

// Cut & Tag

unsigned int jieba_cut_with_tag(jieba_context ctx, const char* sentence, jieba_tags* result)
{
    const auto jieba = ctx2jieba(ctx);
    std::vector<std::pair<std::string, std::string>> tags;
    jieba->Tag(sentence, tags);

    return tag_vector_to_jieba_tags(tags, result);
}

unsigned int jieba_extract(jieba_extractor_context ctx, const char* sentence, unsigned int top_n, jieba_weights* weights)
{
    return jieba_extract_full(ctx, sentence, std::strlen(sentence), top_n, weights);
}

unsigned int jieba_extract_full(jieba_extractor_context ctx, const char* sentence, int len, unsigned int top_n, jieba_weights* weights)
{
    auto& extractor = ctx2extractor(ctx);
    std::vector<std::pair<std::string, double>> result;
    extractor.Extract(std::string(sentence, len), result, top_n);

    return weight_vector_to_jieba_weights(result, weights);
}

