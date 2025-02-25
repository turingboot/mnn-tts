//
//  tts.hpp
//
//  Created by MNN on 2025/2/20.
//  ZhaodeWang
//

#ifndef TTS_hpp
#define TTS_hpp

#include <vector>
#include <memory>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <streambuf>
#include <functional>
#include <unordered_map>

#include <MNN/expr/Expr.hpp>
#include <MNN/expr/Module.hpp>
#include <MNN/expr/MathOp.hpp>
#include <MNN/expr/NeuralNetWorkOp.hpp>

namespace MNN {
namespace Transformer {

class TtsConfig;
class Tokenizer;
class Zhg2p;

class MNN_PUBLIC Tts {
public:
    static Tts* createTTS(const std::string& config_path);
    static void save(const std::string& file, Express::VARP wavform);
    Tts(std::shared_ptr<TtsConfig> config) : config_(config) {}
    virtual ~Tts();
    void load();
    Express::VARP generate(const std::string& text, float speed = 1.0f);
private:
    std::shared_ptr<TtsConfig> config_;
    std::shared_ptr<Tokenizer> tokenizer_;
    std::shared_ptr<Express::Executor::RuntimeManager> runtime_manager_;
    std::shared_ptr<Express::Module> module_;
    std::vector<Express::VARP> voices_;
    std::shared_ptr<Zhg2p> g2p_;
};

}
}

#endif // TTS_hpp
