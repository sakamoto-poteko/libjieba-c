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


// Local decl
static int string_vector_to_jieba_words(const std::vector<std::string>& input, jieba_words* words);
static cppjieba::Jieba* ctx2jieba(jieba_context ctx);

// Local functions
static int string_vector_to_jieba_words(const std::vector<std::string>& input, jieba_words* words)
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

		// 2. populate jieba_word array
		for (unsigned i = 0; i < input.size(); ++i) {
			const auto& w = input[i];
			words->words[i] = strdup(w.c_str());	// Can be w.data() if on C++11/later
		}
	}

	return input.size();
}

static int string_vector_to_jieba_compact_words(const std::vector<std::string> &input, jieba_compact_words *words, const char *original)
{
	words->words = nullptr;
	words->count = 0;

	if (!input.empty())
	{
		words->words = (jieba_compact_word *)malloc(sizeof(jieba_compact_word) * input.size());
		if (words->words == nullptr)
		{
			return 0;
		}

		for (unsigned i = 0, offset = 0; i < input.size(); ++i)
		{
			jieba_compact_word* w = words->words + i;
			w->word = original + offset;
			const auto len = input[i].size();
			w->len = len;
			offset += len;
		}
	}

	return input.size();
}

static cppjieba::Jieba* ctx2jieba(jieba_context ctx)
{
	return static_cast<cppjieba::Jieba*>(ctx);
}

// Jieba obj & utils

jieba_context jieba_create_context(const char* dict_path, const char* hmm_path, const char* user_dict, const char* idf_path, const char* stop_words_path)
{
	const jieba_context ctx = static_cast<jieba_context>(new cppjieba::Jieba(dict_path, hmm_path, user_dict, idf_path, stop_words_path));
	return ctx;
}

void jieba_destroy_context(jieba_context ctx)
{
	delete ctx2jieba(ctx);
}

void jieba_words_free(jieba_words* w)
{
	if (w && w->words) {
		for (unsigned i = 0; i < w->count; ++i) {
			free(w->words[i]);
		}
		free(w->words);
	}
}

void jieba_compact_words_free(jieba_compact_words* w)
{
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

	if (t->tag_list)
	{
		free(t->tag_list);
	}

	if (t->tags)
	{
		for (unsigned i = 0; i < t->count; ++i)
		{
			free(t->tags[i].word);
		}

		free(t->tags);
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
int generic_cut_jieba_words(jieba_context ctx, const char* sentence, unsigned int len, jieba_words *output, bool hmm, cutF fnCut)
{
	std::vector<std::string> result;
	const std::string s(sentence, len);

	fnCut(ctx, s, result, hmm);

	return string_vector_to_jieba_words(result, output);
}

template <typename cutF>
int generic_cut_jieba_compact_words(jieba_context ctx, const char* sentence, unsigned int len, jieba_compact_words* output, bool hmm, cutF fnCut)
{
	std::vector<std::string> result;
	const std::string s(sentence, len);

	fnCut(ctx, s, result, hmm);

	return string_vector_to_jieba_compact_words(result, output, sentence);
}

int jieba_cut(jieba_context ctx, const char* sentence, jieba_words* words, bool hmm)
{
	return generic_cut_jieba_words(ctx, sentence, std::strlen(sentence), words, hmm,
		[](CUTF_SIGNATURE)
		{
			const auto jieba = ctx2jieba(ctx);
			jieba->Cut(s, result, hmm);
		});
}

int jieba_cut_full(jieba_context ctx, const char* sentence, unsigned int len, jieba_words* words, bool hmm)
{
	return generic_cut_jieba_words(ctx, sentence, len, words, hmm,
		[](CUTF_SIGNATURE)
		{
			const auto jieba = ctx2jieba(ctx);
			jieba->Cut(s, result, hmm);
		});
}

int jieba_cut_full_compact(jieba_context ctx, const char* sentence, unsigned int len, jieba_compact_words* words, bool hmm)
{
	return generic_cut_jieba_compact_words(ctx, sentence, len, words, hmm,
		[](CUTF_SIGNATURE)
		{
			const auto jieba = ctx2jieba(ctx);
			jieba->Cut(s, result, hmm);
		});
}

int jieba_cut_all(jieba_context ctx, const char* sentence, jieba_words* words)
{
	return generic_cut_jieba_words(ctx, sentence, std::strlen(sentence), words, false,
		[](CUTF_SIGNATURE)
		{
			const auto jieba = ctx2jieba(ctx);
			jieba->CutAll(s, result);
		});
}

int jieba_cut_all_full(jieba_context ctx, const char* sentence, unsigned int len, jieba_words* words)
{
	return generic_cut_jieba_words(ctx, sentence, len, words, false,
		[](CUTF_SIGNATURE)
		{
			const auto jieba = ctx2jieba(ctx);
			jieba->CutAll(s, result);
		});
}

int jieba_cut_all_full_compact(jieba_context ctx, const char* sentence, unsigned len, jieba_compact_words* words)
{
	return generic_cut_jieba_compact_words(ctx, sentence, len, words, false,
		[](CUTF_SIGNATURE)
		{
			const auto jieba = ctx2jieba(ctx);
			jieba->CutAll(s, result);
		});
}


int jieba_cut_for_search(jieba_context ctx, const char* sentence, jieba_words* words, bool hmm)
{
	return generic_cut_jieba_words(ctx, sentence, std::strlen(sentence), words, hmm,
		[](CUTF_SIGNATURE)
		{
			const auto jieba = ctx2jieba(ctx);
			jieba->CutForSearch(s, result, hmm);
		});
}

int jieba_cut_for_search_full(jieba_context ctx, const char* sentence, unsigned int len, jieba_words* words, bool hmm)
{
	return generic_cut_jieba_words(ctx, sentence, len, words, hmm,
		[](CUTF_SIGNATURE)
		{
			const auto jieba = ctx2jieba(ctx);
			jieba->CutForSearch(s, result, hmm);
		});
}

int jieba_cut_for_search_full_compact(jieba_context ctx, const char* sentence, unsigned len, jieba_compact_words* words,
	bool hmm)
{
	return generic_cut_jieba_compact_words(ctx, sentence, len, words, hmm,
		[](CUTF_SIGNATURE)
		{
			const auto jieba = ctx2jieba(ctx);
			jieba->CutForSearch(s, result, hmm);
		});
}

// Cut & Tag

//int jieba_cut_with_tag(jieba_context ctx, const char* sentence)
//{
//	const auto jieba = ctx2jieba(ctx);
//
//	std::vector<std::pair<std::string, std::string>> tags;
//	jieba->Tag(sentence, tags);
//
//	std::map<std::string, unsigned int> tag_list_map;
//
//}


