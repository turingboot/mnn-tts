//
//  zhg2p.cpp
//
//  Created by MNN on 2025/2/24.
//  ZhaodeWang
//

#include "zhg2p.hpp"
#include <filesystem>
#include <cpp-pinyin/ToFinal.hpp>
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

std::string Zhg2p::get_initials(const std::string &pinyin) {
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

static std::string replaceSpecialChars(const std::string& utf8_str) {
    // 创建 UTF-8 到 wstring 的转换器
    std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;

    // 将 UTF-8 string 转换为 wstring
    std::wstring p = converter.from_bytes(utf8_str);

    // 替换 chr(635) + chr(809) → "ɨ"
    size_t pos = p.find(L"\u027B\u0329");
    if (pos != std::wstring::npos) {
        p.replace(pos, 2, L"ɨ");
    }

    // 替换 chr(633) + chr(809) → "ɨ"
    pos = p.find(L"\u0279\u0329");
    if (pos != std::wstring::npos) {
        p.replace(pos, 2, L"ɨ");
    }

    // 将 wstring 转换回 UTF-8 string
    return converter.to_bytes(p);
}

static std::string apply_tone(std::string variant, int tone) {
    std::vector<std::string> TONE_MAPPING = {
        "", "→", "↗", "↓", "↘", ""
    };
    auto tone_ipa = TONE_MAPPING[tone];
    auto result = replace_all(variant, "0", tone_ipa);
    result =  replaceSpecialChars(result);
    return result;
}


std::string Zhg2p::pinyin_to_phonemes(const std::string &pinyin_normal,  const std::string &pinyin_initial, const std::string &pinyin_final, int tone_nr) {
    std::vector<std::vector<std::string>> part;
    std::vector<std::string> initial_phonemes, final_phonemes;

    if (INTERJECTION_MAPPINGS.find(pinyin_normal) != INTERJECTION_MAPPINGS.end()) {
        // Key 存在
       auto interjection_ipa_mapping = INTERJECTION_MAPPINGS[pinyin_normal];
         auto interjection_ipa = apply_tone(interjection_ipa_mapping.front(), tone_nr);
          return interjection_ipa;
    }

    if (SYLLABIC_CONSONANT_MAPPINGS.find(pinyin_normal) != SYLLABIC_CONSONANT_MAPPINGS.end()) {
        // Key 存在
        auto syllabic_consonant_ipa_mapping = SYLLABIC_CONSONANT_MAPPINGS[pinyin_normal];
        auto syllabic_consonant_ipa = apply_tone(syllabic_consonant_ipa_mapping.front(), tone_nr);
        return syllabic_consonant_ipa;
    }

    if (!pinyin_initial.empty()) {
        initial_phonemes = INITIAL_MAPPING[pinyin_initial];
        // part.emplace_back(initial_phonemes);
    }

     bool is_special = false;
    if (contains(ZH_CH_SH_R, pinyin_initial) && pinyin_final == "i") {
        final_phonemes = FINAL_MAPPING_AFTER_ZH_CH_SH_R;
        is_special = true;
    } else if (contains(Z_C_S, pinyin_initial) && pinyin_final == "i") {
        final_phonemes = FINAL_MAPPING_AFTER_Z_C_S;
        is_special = true;
    } else {
        final_phonemes = FINAL_MAPPING[pinyin_final];
    }

    std::string result = initial_phonemes.empty() ? "" : initial_phonemes[0];
    if (is_special) {
        std::vector<std::string> final_phonemes_t;
        final_phonemes_t.reserve(final_phonemes.size());
        for (const auto &final_phoneme : final_phonemes) {
            final_phonemes_t.emplace_back(apply_tone(final_phoneme, tone_nr));
        }
        result += final_phonemes_t.front();
    }
    else {
        for (const auto &final_phoneme : final_phonemes) {
            result += apply_tone(final_phoneme, tone_nr);
        }
    }


    return result;
}

std::string Zhg2p::get_finals(const std::string& pinyin_normal) {
    // return pinyin_to_finals(pinyin_normal);

    return  pinyin_normal.substr(0, pinyin_normal.size() - 1);
}

std::string Zhg2p::pinyin_to_ipa(const std::string &pinyin) {
    // get_tone
    int tone_nr = pinyin.at(pinyin.size() - 1) - '0';
    std::string py = pinyin.substr(0, pinyin.size() - 1);
    py = pinyin_to_finals(py);
    // get_initials
    std::string pinyin_initial = get_initials(py);
    std::string pinyin_final = py.substr(pinyin_initial.size());
    // std::cout << pinyin << " # " << pinyin_initial << " # " << pinyin_final << " # " << tone_nr << std::endl;
    auto phonemes = pinyin_to_phonemes(py, pinyin_initial, pinyin_final, tone_nr);
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
    auto normalized_text = TextNormalizer::normalize_sentence(text);
    std::string preprocessedText = preprocess(normalized_text);

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