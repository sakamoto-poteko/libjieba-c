// <BEGIN LICENSE>

// <END LICENSE>

#include "libjieba.h"

#include "cppjieba/Jieba.hpp"

jieba_context jieba_create_context(const char *dict_path, const char *hmm_path, const char *user_dict, const char *idf_path, const char *stop_words_path)
{
    jieba_context ctx = static_cast<jieba_context>(new cppjieba::Jieba(dict_path, hmm_path, user_dict, idf_path, stop_words_path));
    return ctx;
}

void jieba_destroy_context(jieba_context ctx)
{
    delete static_cast<cppjieba::Jieba *>(ctx);
}

int jieba_cut(jieba_context ctx, const char *sentence, std::size_t len, jieba_words *words)
{
    auto jieba = static_cast<cppjieba::Jieba *>(ctx);

    std::vector<std::string> cutted;

    std::string s(sentence, len);

    jieba->Cut(s, cutted);

    words->count = cutted.size();

    if (cutted.size() > 0) {
        words->words = (char **)malloc(sizeof(char *) * cutted.size());

        for (std::size_t i = 0; i < cutted.size(); ++i) {
            words->words[i] = strdup(cutted[i].c_str());
        }
    }

    return cutted.size();
}
