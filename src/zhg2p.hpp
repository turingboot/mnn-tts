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
#include "zh_normalization/TextNormalizer.h"

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
    static std::string get_finals(const std::string &text);
    static std::string get_initials(const std::string &text);
    static std::string pinyin_to_phonemes(const std::string &pinyin_normal,  const std::string &pinyin_initial, const std::string &pinyin_final, int tone_nr);
    static std::string pinyin_to_ipa(const std::string &pinyin);
    std::string word2ipa(const std::string &word);
    std::string g2p(const std::string &text);

   static inline  std::map<std::string, std::vector<std::string>> INITIAL_MAPPING = {
        {"b", {"p"}}, {"c", {"ʦʰ"}}, {"ch", {"\uAB67ʰ"}}, {"d", {"t"}}, {"f", {"f"}},
        {"g", {"k"}}, {"h", {"x", "h"}}, {"j", {"ʨ"}}, {"k", {"kʰ"}}, {"l", {"l"}},
        {"m", {"m"}}, {"n", {"n"}}, {"p", {"pʰ"}}, {"q", {"ʨʰ"}}, {"r", {"ɻ", "ʐ"}},
        {"s", {"s"}}, {"sh", {"ʂ"}}, {"t", {"tʰ"}}, {"x", {"ɕ"}}, {"z", {"ʦ"}}, {"zh", {"\uAB67"}},};
   static inline std::map<std::string, std::vector<std::string>> FINAL_MAPPING = {
        {"a", {"a0"}}, {"ai", {"ai̯0"}}, {"an", {"a0", "n"}}, {"ang", {"a0", "ŋ"}}, {"ao", {"au̯0"}},
        {"e", {"ɤ0"}}, {"ei", {"ei̯0"}}, {"en", {"ə0", "n"}}, {"eng", {"ə0", "ŋ"}}, {"i", {"i0"}},
        {"ia", {"j", "a0"}}, {"ian", {"j", "ɛ0", "n"}}, {"iang", {"j", "a0", "ŋ"}}, {"iao", {"j", "au̯0"}},
        {"ie", {"j", "e0"}}, {"in", {"i0", "n"}}, {"iou", {"j", "ou̯0"}}, {"ing", {"i0", "ŋ"}},
        {"iong", {"j", "ʊ0", "ŋ"}}, {"ong", {"ʊ0", "ŋ"}}, {"ou", {"ou̯0"}}, {"u", {"u0"}}, {"uei", {"w", "ei̯0"}},
        {"ua", {"w", "a0"}}, {"uai", {"w", "ai̯0"}}, {"uan", {"w", "a0", "n"}}, {"uen", {"w", "ə0", "n"}},
        {"uang", {"w", "a0", "ŋ"}}, {"ueng", {"w", "ə0", "ŋ"}}, {"uo", {"w", "o0"}}, {"o", {"w", "o0"}},
        {"ü", {"y0"}}, {"üe", {"ɥ", "e0"}}, {"üan", {"ɥ", "ɛ0", "n"}}, {"ün", {"y0", "n"}}
    };

    static inline std::map<std::string, std::vector<std::string>> INTERJECTION_MAPPINGS = {
        {"io", {"j", "ɔ0"}},
        {"ê", {"ɛ0"}},
        {"er", {"ɚ0", "aɚ0"}},
        {"o", {"ɔ0"}},
    };

    static  inline std::map<std::string, std::vector<std::string>> SYLLABIC_CONSONANT_MAPPINGS = {
        {"hm", {"h", "ɔ0"}},
        {"hng", {"h", "ŋ0"}},
        {"m", {"m0"}},
        {"n", {"n0"}},
        {"ng", {"ŋ0"}},
    };

    static  inline std::vector<std::string> ZH_CH_SH_R = {"zh", "ch", "sh", "r"};
    static  inline std::vector<std::string> Z_C_S = {"z", "c", "s"};
    static  inline std::vector<std::string> FINAL_MAPPING_AFTER_ZH_CH_SH_R = {"ɻ̩0", "ʐ̩0"};
    static  inline std::vector<std::string> FINAL_MAPPING_AFTER_Z_C_S = {"ɹ̩0", "z̩0"};
};

} // namespace Transformer
} // namespace MNN