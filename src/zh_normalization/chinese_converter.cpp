
#include <zh_normalization/chinese_converter.h>
#include <codecvt>
#include <locale>
#include <string>

// 初始化静态成员变量
std::unordered_map<char32_t, char32_t> ChineseConverter::t2s_dict;
std::unordered_map<char32_t, char32_t> ChineseConverter::s2t_dict;
bool ChineseConverter::is_initialized = false;

void ChineseConverter::initializeDicts() {
    if (is_initialized) return;
    
    // 这里只添加一些示例映射，实际使用时需要添加完整的映射表
     s2t_dict= {
        {U'专', U'專'}, {U'业', U'業'}, {U'东', U'東'}, {U'丝', U'絲'},
     {U'丢', U'丟'}, {U'两', U'兩'}, {U'严', U'嚴'}, {U'丧', U'喪'},
     {U'个', U'個'}, {U'临', U'臨'}, {U'为', U'為'}, {U'举', U'舉'},
     {U'义', U'義'}, {U'乐', U'樂'}, {U'习', U'習'}, {U'乡', U'鄉'},
     {U'书', U'書'}, {U'买', U'買'}, {U'乱', U'亂'}, {U'争', U'爭'},
     {U'于', U'於'}, {U'亚', U'亞'}, {U'亲', U'親'}, {U'云', U'雲'},
     {U'产', U'產'}, {U'亩', U'畝'}, {U'亿', U'億'}, {U'仅', U'僅'},
     {U'从', U'從'}, {U'仓', U'倉'}, {U'仪', U'儀'}, {U'们', U'們'},
     {U'价', U'價'}, {U'众', U'眾'}, {U'优', U'優'}, {U'会', U'會'},
     {U'伛', U'傴'}, {U'伞', U'傘'}, {U'伟', U'偉'}, {U'传', U'傳'},
     {U'体', U'體'}, {U'余', U'餘'}, {U'佣', U'傭'}, {U'侠', U'俠'},
     {U'侣', U'侶'}, {U'侥', U'僥'}, {U'侦', U'偵'}, {U'侧', U'側'},
     {U'侨', U'僑'}, {U'侩', U'儈'}, {U'侪', U'儕'}, {U'债', U'債'},
     {U'倾', U'傾'}, {U'假', U'假'}, {U'偿', U'償'}, {U'储', U'儲'},
     {U'军', U'軍'}, {U'农', U'農'}, {U'冯', U'馮'}, {U'决', U'決'},
     {U'况', U'況'}, {U'冲', U'衝'}, {U'净', U'淨'}, {U'准', U'準'},
     {U'凤', U'鳳'}, {U'别', U'別'}, {U'刘', U'劉'}, {U'动', U'動'},
     {U'务', U'務'}, {U'医', U'醫'}, {U'华', U'華'}, {U'协', U'協'},
     {U'单', U'單'}, {U'卖', U'賣'}, {U'占', U'佔'}, {U'卫', U'衛'},
     {U'压', U'壓'}, {U'厅', U'廳'}, {U'历', U'歷'}, {U'县', U'縣'},
     {U'叠', U'疊'}, {U'发', U'發'}, {U'变', U'變'}, {U'台', U'臺'},
     {U'叶', U'葉'}, {U'号', U'號'}, {U'后', U'後'}, {U'向', U'向'},
     {U'吓', U'嚇'}, {U'吗', U'嗎'}, {U'听', U'聽'}, {U'启', U'啟'},
     {U'员', U'員'}, {U'周', U'週'}, {U'响', U'響'}, {U'国', U'國'}
    };

     t2s_dict = {
        {U'專', U'专'}, {U'業', U'业'}, {U'東', U'东'}, {U'絲', U'丝'},
          {U'丟', U'丢'}, {U'兩', U'两'}, {U'嚴', U'严'}, {U'喪', U'丧'},
          {U'個', U'个'}, {U'臨', U'临'}, {U'為', U'为'}, {U'舉', U'举'},
          {U'義', U'义'}, {U'樂', U'乐'}, {U'習', U'习'}, {U'鄉', U'乡'},
          {U'書', U'书'}, {U'買', U'买'}, {U'亂', U'乱'}, {U'爭', U'争'},
          {U'於', U'于'}, {U'亞', U'亚'}, {U'親', U'亲'}, {U'雲', U'云'},
          {U'產', U'产'}, {U'畝', U'亩'}, {U'億', U'亿'}, {U'僅', U'仅'},
          {U'從', U'从'}, {U'倉', U'仓'}, {U'儀', U'仪'}, {U'們', U'们'},
          {U'價', U'价'}, {U'眾', U'众'}, {U'優', U'优'}, {U'會', U'会'},
          {U'傴', U'伛'}, {U'傘', U'伞'}, {U'偉', U'伟'}, {U'傳', U'传'},
          {U'體', U'体'}, {U'餘', U'余'}, {U'傭', U'佣'}, {U'俠', U'侠'},
          {U'侶', U'侣'}, {U'僥', U'侥'}, {U'偵', U'侦'}, {U'側', U'侧'},
          {U'僑', U'侨'}, {U'儈', U'侩'}, {U'儕', U'侪'}, {U'債', U'债'},
          {U'傾', U'倾'}, {U'假', U'假'}, {U'償', U'偿'}, {U'儲', U'储'},
          {U'軍', U'军'}, {U'農', U'农'}, {U'馮', U'冯'}, {U'決', U'决'},
          {U'況', U'况'}, {U'衝', U'冲'}, {U'淨', U'净'}, {U'準', U'准'},
          {U'鳳', U'凤'}, {U'別', U'别'}, {U'劉', U'刘'}, {U'動', U'动'},
          {U'務', U'务'}, {U'醫', U'医'}, {U'華', U'华'}, {U'協', U'协'},
          {U'單', U'单'}, {U'賣', U'卖'}, {U'佔', U'占'}, {U'衛', U'卫'},
          {U'壓', U'压'}, {U'廳', U'厅'}, {U'歷', U'历'}, {U'縣', U'县'},
          {U'疊', U'叠'}, {U'發', U'发'}, {U'變', U'变'}, {U'臺', U'台'},
          {U'葉', U'叶'}, {U'號', U'号'}, {U'後', U'后'}, {U'向', U'向'},
          {U'嚇', U'吓'}, {U'嗎', U'吗'}, {U'聽', U'听'}, {U'啟', U'启'},
          {U'員', U'员'}, {U'週', U'周'}, {U'響', U'响'}, {U'國', U'国'}
    };
    
    is_initialized = true;
}

std::string ChineseConverter::traditionalToSimplified(const std::string& text) {
    initializeDicts();
    
    std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> converter;
    std::u32string u32str = converter.from_bytes(text);
    
    std::u32string result;
    for (char32_t c : u32str) {
        auto it = t2s_dict.find(c);
        if (it != t2s_dict.end()) {
            result += it->second;
        } else {
            result += c;
        }
    }
    
    return converter.to_bytes(result);
}

std::string ChineseConverter::simplifiedToTraditional(const std::string& text) {
    initializeDicts();
    
    std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> converter;
    std::u32string u32str = converter.from_bytes(text);
    
    std::u32string result;
    for (char32_t c : u32str) {
        auto it = s2t_dict.find(c);
        if (it != s2t_dict.end()) {
            result += it->second;
        } else {
            result += c;
        }
    }
    
    return converter.to_bytes(result);
} 