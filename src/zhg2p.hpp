//
//  zhg2p.hpp
//
//  Created by MNN on 2025/2/24.
//  ZhaodeWang
//

#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <regex>
#include <cppjieba/Jieba.hpp>
#include <cpp-pinyin/Pinyin.h>
#include <cpp-pinyin/G2pglobal.h>

namespace MNN {
namespace Transformer {

class Zhg2p {
private:
    std::unique_ptr<cppjieba::Jieba> jieba_;
    std::unique_ptr<Pinyin::Pinyin> pinyin_;
public:
    Zhg2p();
    static std::string pinyinToIPA(const std::string &py);
    static std::vector<std::string> obtainPinyins(const std::string &word);
    static std::string retone(const std::string &p);
    static std::string py2ipa(const std::string &py);
    static std::string preprocess(const std::string &text);
    static std::string pinyin_to_ipa(const std::string &pinyin);
    std::string word2ipa(const std::string &word);
    std::string g2p(const std::string &text);
};

} // namespace Transformer
} // namespace MNN