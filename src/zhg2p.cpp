//
//  zhg2p.cpp
//
//  Created by MNN on 2025/2/24.
//  ZhaodeWang
//

#include "zhg2p.hpp"
#include <filesystem>

namespace MNN {
namespace Transformer {

Zhg2p::Zhg2p() {
    const std::filesystem::path jieba_dir = "../resource/jieba";
    jieba_.reset(new cppjieba::Jieba(jieba_dir / "jieba.dict.utf8",
        jieba_dir / "hmm_model.utf8",
        jieba_dir / "user.dict.utf8",
        jieba_dir / "idf.utf8",
        jieba_dir / "stop_words.utf8")
    );
    const auto pinyin_dir = "../resource/pinyin";
    Pinyin::setDictionaryPath(pinyin_dir);
    pinyin_.reset(new Pinyin::Pinyin());
}

std::string Zhg2p::retone(const std::string &p) {
    std::string result = p;
    result = std::regex_replace(result, std::regex("˧˩˧"), "↓");
    result = std::regex_replace(result, std::regex("˧˥"), "↗");
    result = std::regex_replace(result, std::regex("˥˩"), "↘");
    result = std::regex_replace(result, std::regex("˥"), "→");
    return result;
}

std::string Zhg2p::pinyinToIPA(const std::string &py) {
    return py;
}

std::string Zhg2p::py2ipa(const std::string &py) {
    std::string ipa = pinyinToIPA(py);
    return retone(ipa);
}

std::vector<std::string> Zhg2p::obtainPinyins(const std::string &word) {
    std::vector<std::string> pinyins;
    return pinyins;
}

static bool startsWith(const std::string& str, const std::string& prefix) {
    return str.size() >= prefix.size() && str.substr(0, prefix.size()) == prefix;
}

static bool contains(const std::vector<std::string>& vec, const std::string& value) {
    return std::find(vec.begin(), vec.end(), value) != vec.end();
}

static std::string get_initials(const std::string &pinyin) {
    const std::string INITIALS[] = {
        "b", "p", "m", "f", "d", "t", "n", "l", "g", "k", "h",
        "j", "q", "x", "zh", "ch", "sh", "r", "z", "c", "s"
    };
    for (const auto &i : INITIALS) {
        if (startsWith(pinyin, i)) {
            return i;
        }
    }
    return "";
}

std::string replace_all(std::string str, const std::string& from, const std::string& to) {
    size_t start_pos = 0;
    while((start_pos = str.find(from, start_pos)) != std::string::npos) {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length();
    }
    return str;
}

static std::string apply_tone(std::string variant, int tone) {
    std::vector<std::string> TONE_MAPPING = {
        "", "→", "↗", "↓", "↘", ""
    };
    auto tone_ipa = TONE_MAPPING[tone];
    auto result = replace_all(variant, "0", tone_ipa);
    result = replace_all(result, "\u027b\u0329", "ɨ");
    result = replace_all(result, "\u027d\u0329", "ɨ");
    return result;
}


static std::string pinyin_to_phonemes(const std::string &pinyin_initial, const std::string &pinyin_final, int tone_nr) {
    std::map<std::string, std::vector<std::string>> INITIAL_MAPPING = {
        {"b", {"p"}}, {"c", {"ʦʰ"}}, {"ch", {"\uAB67ʰ"}}, {"d", {"t"}}, {"f", {"f"}},
        {"g", {"k"}}, {"h", {"x", "h"}}, {"j", {"ʨ"}}, {"k", {"kʰ"}}, {"l", {"l"}},
        {"m", {"m"}}, {"n", {"n"}}, {"p", {"pʰ"}}, {"q", {"ʨʰ"}}, {"r", {"ɻ", "ʐ"}},
        {"s", {"s"}}, {"sh", {"ʂ"}}, {"t", {"tʰ"}}, {"x", {"ɕ"}}, {"z", {"ʦ"}}, {"zh", {"\uAB67"}}
    };
    std::map<std::string, std::vector<std::string>> FINAL_MAPPING = {
        {"a", {"a0"}}, {"ai", {"ai̯0"}}, {"an", {"a0", "n"}}, {"ang", {"a0", "ŋ"}}, {"ao", {"au̯0"}},
        {"e", {"ɤ0"}}, {"ei", {"ei̯0"}}, {"en", {"ə0", "n"}}, {"eng", {"ə0", "ŋ"}}, {"i", {"i0"}},
        {"ia", {"j", "a0"}}, {"ian", {"j", "ɛ0", "n"}}, {"iang", {"j", "a0", "ŋ"}}, {"iao", {"j", "au̯0"}},
        {"ie", {"j", "e0"}}, {"in", {"i0", "n"}}, {"iou", {"j", "ou̯0"}}, {"ing", {"i0", "ŋ"}},
        {"iong", {"j", "ʊ0", "ŋ"}}, {"ong", {"ʊ0", "ŋ"}}, {"ou", {"ou̯0"}}, {"u", {"u0"}}, {"uei", {"w", "ei̯0"}},
        {"ua", {"w", "a0"}}, {"uai", {"w", "ai̯0"}}, {"uan", {"w", "a0", "n"}}, {"uen", {"w", "ə0", "n"}},
        {"uang", {"w", "a0", "ŋ"}}, {"ueng", {"w", "ə0", "ŋ"}}, {"uo", {"w", "o0"}}, {"o", {"w", "o0"}},
        {"ü", {"y0"}}, {"üe", {"ɥ", "e0"}}, {"üan", {"ɥ", "ɛ0", "n"}}, {"ün", {"y0", "n"}}
    };
    std::vector<std::string> ZH_CH_SH_R = {"zh", "ch", "sh", "r"};
    std::vector<std::string> Z_C_S = {"z", "c", "s"};
    // std::vector<std::string> FINAL_MAPPING_AFTER_ZH_CH_SH_R = {"ɻ̩0", "ʐ̩0"};
    // std::vector<std::string> FINAL_MAPPING_AFTER_Z_C_S = {"ɹ̩0", "z̩0"};
    std::vector<std::string> FINAL_MAPPING_AFTER_ZH_CH_SH_R = {"ɻ̩0"};
    std::vector<std::string> FINAL_MAPPING_AFTER_Z_C_S = {"ɹ̩0"};
    std::vector<std::string> phonemes;
    std::vector<std::string> initial_phonemes, final_phonemes;
    if (pinyin_initial != "") {
        initial_phonemes = INITIAL_MAPPING[pinyin_initial];
    }
    if (contains(ZH_CH_SH_R, pinyin_initial) && pinyin_final == "i") {
        final_phonemes = FINAL_MAPPING_AFTER_ZH_CH_SH_R;
    } else if (contains(Z_C_S, pinyin_initial) && pinyin_final == "i") {
        final_phonemes = FINAL_MAPPING_AFTER_Z_C_S;
    } else {
        final_phonemes = FINAL_MAPPING[pinyin_final];
    }
#if 0
    // dump initial_phonemes, final_phonemes
    for (const auto &initial_phoneme : initial_phonemes) {
        std::cout << initial_phoneme << std::endl;
    }
    std::cout << "###" << std::endl;
    for (const auto &final_phoneme : final_phonemes) {
        std::cout << apply_tone(final_phoneme, tone_nr) << std::endl;
    }
#endif
    std::string result = initial_phonemes[0];
    for (const auto &final_phoneme : final_phonemes) {
        result += apply_tone(final_phoneme, tone_nr);
    }
    return result;
}

static std::string convert_final(std::string final) {
    // TODO: convert final
    // convert_zero_consonant
    // convert_uv
    // convert_iou
    // convert_uei
    // convert_uen
    return final;
}

std::string Zhg2p::pinyin_to_ipa(const std::string &pinyin) {
    // get_tone
    int tone_nr = pinyin.at(pinyin.size() - 1) - '0';
    std::string py = pinyin.substr(0, pinyin.size() - 1);
    // get_initials
    std::string pinyin_initial = get_initials(py);
    std::string pinyin_final = convert_final(py.substr(pinyin_initial.size()));
    // std::cout << pinyin << " # " << pinyin_initial << " # " << pinyin_final << " # " << tone_nr << std::endl;
    auto phonemes = pinyin_to_phonemes(pinyin_initial, pinyin_final, tone_nr);
    return phonemes;
}

std::string Zhg2p::word2ipa(const std::string &word) {
    auto pinyins = pinyin_->hanziToPinyin(word, Pinyin::ManTone::Style::TONE3, Pinyin::Error::Default, false, false, true);
    std::string result = "";
    for (const auto py : pinyins) {
        result += pinyin_to_ipa(py.pinyin);
    }
    return result;
}

std::string Zhg2p::preprocess(const std::string &text) {
    std::string result = text;
    result = std::regex_replace(result, std::regex("、"), ", ");
    result = std::regex_replace(result, std::regex("，"), ", ");
    result = std::regex_replace(result, std::regex("。"), ". ");
    result = std::regex_replace(result, std::regex("«"), " “");
    result = std::regex_replace(result, std::regex("»"), "” ");
    result = std::regex_replace(result, std::regex("《"), " “");
    result = std::regex_replace(result, std::regex("》"), "” ");
    result = std::regex_replace(result, std::regex("（"), " (");
    result = std::regex_replace(result, std::regex("）"), ") ");
    result = std::regex_replace(result, std::regex("！"), "! ");
    result = std::regex_replace(result, std::regex("："), ": ");
    result = std::regex_replace(result, std::regex("；"), "; ");
    result = std::regex_replace(result, std::regex("？"), "? ");
    return std::regex_replace(result, std::regex("\\s+"), " ");
}

std::string Zhg2p::g2p(const std::string &text) {
    if (text.empty()) {
        return "";
    }
    std::string preprocessedText = preprocess(text);

    std::regex reg(u8"([\u4E00-\u9FFF]+|[^\u4E00-\u9FFF]+)");
    std::regex zh_regex(u8"[\u4E00-\u9FFF]+");
    std::vector<std::pair<std::string, bool>> segments;

    auto begin = std::sregex_iterator(preprocessedText.begin(), preprocessedText.end(), reg);
    auto end = std::sregex_iterator();

    for (auto it = begin; it != end; ++it) {
        auto segment = it->str();
        bool is_zh = std::regex_search(segment, zh_regex);
        segments.emplace_back(segment, is_zh);
    }

    std::string result;
    for (const auto& segment : segments) {
        if (segment.second) {
            std::vector<std::string> words;
            jieba_->Cut(segment.first, words);
            for (int i = 0; i < words.size(); i++) {
                result += word2ipa(words[i]);
                if (i < words.size() - 1) {
                    result += " ";
                }
            }
        } else {
            result += segment.first;
        }
    }
    result = replace_all(result, "\xCC\xAF", "");
    // std::cout << "#" << result << "#" << std::endl;
    return result;
}

}
}