//
// Created by tao on 25-4-9.
//

#ifndef TEXTNORMALIZATION_H
#define TEXTNORMALIZATION_H


#include <string>
#include <vector>
#include <regex>
#include <map>
#include <memory>

class TextNormalizer {
public:
    TextNormalizer();
    std::vector<std::string> normalize(const std::string& text);
    static std::string normalize_sentence(const std::string& sentence);

private:
    std::regex SENTENCE_SPLITOR;
    std::vector<std::string> _split(const std::string& text, const std::string& lang = "zh");
    std::string _post_replace(const std::string& sentence);
};

#endif //TEXTNORMALIZATION_H
