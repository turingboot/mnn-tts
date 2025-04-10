//
//  tts_demo.cpp
//
//  Created by MNN on 2024/2/20.
//  ZhaodeWang
//

#include "tts.hpp"
#include <zh_normalization/phonecode.h>
#include <zh_normalization/chinese_converter.h>
#include <zh_normalization/constants.h>
#include "zh_normalization/TextNormalization.h"
using namespace MNN::Transformer;

int main(int argc, const char* argv[]) {
//    if (argc < 3) {
//        std::cout << "Usage: " << argv[0] << " config.json text <file_name>" << std::endl;
//        return 0;
//    }

    // std::string config_path = "/home/tao/workspace/c++/mnn-tts/model/config.json";
    // std::unique_ptr<Tts> tts(Tts::createTTS(config_path));
    // std::string text = "你好";
    // tts->load();
    // auto wavform = tts->generate(text, 1.0);
    // std::string filename = "output.wav";
    // if (argc > 4) {
    //     filename = argv[3];
    // }
    // Tts::save(filename, wavform);

    TextNormalizer text_normalizer;
    const std::string text = "12月25日下午2:15-3:15，猫眼专业版数据显示";
    std::string result = text_normalizer.normalize_sentence(text);
    std::cout << result << std::endl;




    return 0;
}