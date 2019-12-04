// <BEGIN LICENSE>

// <END LICENSE>


#if defined(__unix__)
#include <unistd.h>
#else
#include <cstring>

#if defined(_WIN32)
#define strdup _strdup
#endif

#endif

#include "libjieba.h"
#include "cppjieba/Jieba.hpp"


static int string_vector_to_jieba_words(const std::vector<std::string>& input, jieba_words* words);

jieba_context jieba_create_context(const char* dict_path, const char* hmm_path, const char* user_dict, const char* idf_path, const char* stop_words_path)
{
	jieba_context ctx = static_cast<jieba_context>(new cppjieba::Jieba(dict_path, hmm_path, user_dict, idf_path, stop_words_path));
	return ctx;
}

void jieba_destroy_context(jieba_context ctx)
{
	delete static_cast<cppjieba::Jieba*>(ctx);
}

static int string_vector_to_jieba_words(const std::vector<std::string>& input, jieba_words* words)
{
	words->words = nullptr;
	words->allocated = false;
	words->count = 0;

	if (!input.empty()) {
		words->count = input.size();

		// 1. allocate jieba_word array
		words->words = (jieba_word*)malloc(sizeof(jieba_word) * input.size());
		if (words->words == nullptr)
		{
			return 0;
		}

		// 2. mark words as allocated
		words->allocated = 1;

		// 3. populate jieba_word array
		for (unsigned i = 0; i < input.size(); ++i) {
			const auto& w = input[i];
			const auto word = words->words + i;
			word->word = strdup(w.c_str());	// Can be w.data() if on C++11/later
			word->len = w.size();
		}
	}

	return input.size();
}

int jieba_cut(jieba_context ctx, const char* sentence, jieba_words* words, int hmm)
{
	return jieba_cut_full(ctx, sentence, std::strlen(sentence), true, words, hmm);
}

int jieba_cut_full(jieba_context ctx, const char* sentence, unsigned int len, int allocate_words, jieba_words* words, int hmm)
{
	const auto jieba = static_cast<cppjieba::Jieba*>(ctx);

	std::vector<std::string> result;
	const std::string s(sentence, len);

	jieba->Cut(s, result, hmm);

	return string_vector_to_jieba_words(result, words);
}

void jieba_words_free(jieba_words* w)
{
	if (w && w->allocated && w->words) {
		for (unsigned i = 0; i < w->count; ++i) {
			free(w->words[i].word);
		}
		free(w->words);
	}
}

int jieba_cut_all(jieba_context ctx, const char* sentence, jieba_words* words)
{
	return jieba_cut_all_full(ctx, sentence, std::strlen(sentence), true, words);
}

int jieba_cut_all_full(jieba_context ctx, const char* sentence, unsigned  int len, int allocate_words,
	jieba_words* words)
{
	const auto jieba = static_cast<cppjieba::Jieba*>(ctx);

	std::vector<std::string> result;
	const std::string s(sentence, len);

	jieba->CutAll(s, result);

	return string_vector_to_jieba_words(result, words);
}

int jieba_cut_for_search(jieba_context ctx, const char* sentence, jieba_words* words, int hmm)
{
	return jieba_cut_for_search_full(ctx, sentence, std::strlen(sentence), true, words, hmm);
}

int jieba_cut_for_search_full(jieba_context ctx, const char* sentence, unsigned int len, int allocate_words,
	jieba_words* words, int hmm)
{
	const auto jieba = static_cast<cppjieba::Jieba*>(ctx);

	std::vector<std::string> result;
	const std::string s(sentence, len);

	jieba->CutForSearch(s, result, hmm);

	return string_vector_to_jieba_words(result, words);
}
