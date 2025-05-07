#include <codecvt>
#include <locale>
#include <map>
#include <regex>
#include <set>
#include <string>
#include <unordered_set>

using namespace std;

// 编码转换工具
wstring_convert<codecvt_utf8_utf16<wchar_t>> converter;

// 韵母表
const unordered_set<wstring> _FINALS = {
    L"i",    L"u",    L"ü",    L"a",     L"ia",   L"ua",   L"o",     L"uo",
    L"e",    L"ie",   L"üe",   L"ai",    L"uai",  L"ei",   L"uei",   L"ao",
    L"iao",  L"ou",   L"iou",  L"an",    L"ian",  L"uan",  L"üan",   L"en",
    L"in",   L"uen",  L"ün",   L"ang",   L"iang", L"uang", L"eng",   L"ing",
    L"ueng", L"ong",  L"iong", L"er",    L"ê",
};

// u -> ü 映射
const map<wstring, wstring> UV_MAP = {
    {L"u", L"ü"}, {L"ū", L"ǖ"}, {L"ú", L"ǘ"}, {L"ǔ", L"ǚ"}, {L"ù", L"ǜ"}};
const set<wstring> U_TONES = {L"u", L"ū", L"ú", L"ǔ", L"ù"};
const set<wstring> I_TONES = {L"i", L"ī", L"í", L"ǐ", L"ì"};

// iu -> iou 映射
const map<wstring, wstring> IU_MAP = {
    {L"iu", L"iou"}, {L"iū", L"ioū"}, {L"iú", L"ioú"}, {L"iǔ", L"ioǔ"}, {L"iù", L"ioù"}};

// ui -> uei 映射
const map<wstring, wstring> UI_MAP = {
    {L"ui", L"uei"}, {L"uī", L"ueī"}, {L"uí", L"ueí"}, {L"uǐ", L"ueǐ"}, {L"uì", L"ueì"}};

// un -> uen 映射
const map<wstring, wstring> UN_MAP = {
    {L"un", L"uen"}, {L"ūn", L"ūen"}, {L"ún", L"úen"}, {L"ǔn", L"ǔen"}, {L"ùn", L"ùen"}};

inline wstring convert_zero_consonant(const wstring& pinyin) {
    wstring raw = pinyin;
    if (!pinyin.empty() && pinyin[0] == L'y') {
        wstring no_y = pinyin.substr(1);
        if (no_y.empty()) return raw;

        wstring first(1, no_y[0]);
        if (U_TONES.count(first)) {
            wstring replaced = UV_MAP.at(first) + no_y.substr(1);
            if (_FINALS.count(replaced)) return replaced;
        } else if (I_TONES.count(first)) {
            if (_FINALS.count(no_y)) return no_y;
        } else {
            wstring new_py = L"i" + no_y;
            if (_FINALS.count(new_py)) return new_py;
        }
        return raw;
    }

    if (!pinyin.empty() && pinyin[0] == L'w') {
        wstring no_w = pinyin.substr(1);
        if (no_w.empty()) return raw;

        wstring first(1, no_w[0]);
        if (U_TONES.count(first)) {
            if (_FINALS.count(no_w)) return no_w;
        } else {
            wstring new_py = L"u" + no_w;
            if (_FINALS.count(new_py)) return new_py;
        }
        return raw;
    }
    return pinyin;
}

inline wstring convert_uv(const wstring& pinyin) {
    if (pinyin.length() < 2) return pinyin;

    wchar_t first = pinyin[0];
    if (first != L'j' && first != L'q' && first != L'x') return pinyin;

    wstring second(1, pinyin[1]);
    if (UV_MAP.find(second) != UV_MAP.end()) {
        return wstring(1, first) + UV_MAP.at(second) + pinyin.substr(2);
    }
    return pinyin;
}

inline wstring convert_iou(const wstring& pinyin) {
    wregex re(LR"((\w+?)(i[ūúǔù]|iu)$)");
    wsmatch match;
    if (regex_match(pinyin, match, re) && match.size() == 3) {
        wstring key = match[2].str();
        if (IU_MAP.find(key) != IU_MAP.end()) {
            return match[1].str() + IU_MAP.at(key);
        }
    }
    return pinyin;
}

inline wstring convert_uei(const wstring& pinyin) {
    wregex re(LR"((\w+?)(u[īíǐì]|ui)$)");
    wsmatch match;
    if (regex_match(pinyin, match, re) && match.size() == 3) {
        wstring key = match[2].str();
        if (UI_MAP.find(key) != UI_MAP.end()) {
            return match[1].str() + UI_MAP.at(key);
        }
    }
    return pinyin;
}

inline wstring convert_uen(const wstring& pinyin) {
    wregex re(LR"(([a-z]+)(ǔn|un|ùn|ūn|ún)$)");
    wsmatch match;
    if (regex_match(pinyin, match, re) && match.size() == 3) {
        wstring key = match[2].str();
        if (UN_MAP.find(key) != UN_MAP.end()) {
            auto tmp = match[1].str() + UN_MAP.at(key);
            return match[1].str() + UN_MAP.at(key);
        }
    }
    return pinyin;
}

inline wstring convert_finals(const wstring& pinyin) {
    wstring result = convert_zero_consonant(pinyin);
    result = convert_uv(result);
    result = convert_iou(result);
    result = convert_uei(result);
    result = convert_uen(result);
    return result;
}

// 外部接口
inline string pinyin_to_finals(const string& pinyin) {
    wstring wide_pinyin = converter.from_bytes(pinyin);
    wstring result = convert_finals(wide_pinyin);
    return converter.to_bytes(result);
}