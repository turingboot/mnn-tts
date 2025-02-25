//
//  tts_demo.cpp
//
//  Created by MNN on 2024/2/20.
//  ZhaodeWang
//

#include "tts.hpp"

using namespace MNN::Transformer;

int main(int argc, const char* argv[]) {
    if (argc < 3) {
        std::cout << "Usage: " << argv[0] << " config.json text <file_name>" << std::endl;
        return 0;
    }

    std::string config_path = argv[1];
    std::unique_ptr<Tts> tts(Tts::createTTS(config_path));
    std::string text = argv[2];
    tts->load();
    auto wavform = tts->generate(text, 1.0);
    std::string filename = "output.wav";
    if (argc > 4) {
        filename = argv[3];
    }
    Tts::save(filename, wavform);
    return 0;
}