//
//  tts.cpp
//
//  Created by MNN on 2025/2/20.
//  ZhaodeWang
//

#include "tts.hpp"
#include "ttsconfig.hpp"
#include "tokenizer.hpp"
#include "zhg2p.hpp"
#include <audio/audio.hpp>

#include <cmath>
#include <fstream>
#include <iostream>
#include <sstream>
#include <memory>

using namespace MNN::Express;
namespace MNN {
namespace Transformer {

Tts *Tts::createTTS(const std::string &config_path) {
    std::shared_ptr<TtsConfig> config(new TtsConfig(config_path));
    return new Tts(config);
}

Tts::~Tts() {}

void Tts::load() {
    g2p_.reset(new Zhg2p());
    tokenizer_.reset(Tokenizer::createTokenizer(config_->tokenizer_file()));
    {
        ScheduleConfig config;
        BackendConfig cpuBackendConfig;
        config.type          = MNN_FORWARD_CPU;
        config.numThread     = 4;
        // cpuBackendConfig.power = BackendConfig::Power_Low;
        cpuBackendConfig.memory = BackendConfig::Memory_Low;
        // cpuBackendConfig.precision = BackendConfig::Precision_Low;
        config.backendConfig = &cpuBackendConfig;
        // ExecutorScope::Current()->setGlobalExecutorConfig(config.type, cpuBackendConfig, config.numThread);
        runtime_manager_.reset(Executor::RuntimeManager::createRuntimeManager(config));
    }
    Module::Config module_config;
    module_config.shapeMutable = true;
    module_config.rearrange = true;
    module_.reset(Module::load({"style", "input_ids", "speed"}, {"waveform"},
                                config_->tts_model().c_str(), runtime_manager_, &module_config));
    voices_ = Express::Variable::load(config_->voices().c_str());
}

template <typename T>
static inline VARP _var(std::vector<T> vec, const std::vector<int> &dims) {
    return _Const(vec.data(), dims, NCHW, halide_type_of<T>());
}

Express::VARP Tts::generate(const std::string &text, float speed) {
    std::string phonemes = g2p_->g2p(text);
    std::cout << phonemes << std::endl;
    auto ids = tokenizer_->encode(phonemes);
    int len = ids.size();
    ids.push_back(0);
    ids.insert(ids.begin(), 0);
    auto style = Express::_Squeeze(Express::_GatherV2(voices_[0], _var<int>({len}, {1}), _var<int>({0}, {1})), {0});
    auto input_ids = _var<int>(ids, {1, len + 2});
    auto speed_var = _var<float>({speed}, {1});
    auto ouputs = module_->onForward({style, input_ids, speed_var});
    auto wavform = ouputs[0];
    return wavform;
}

void Tts::save(const std::string &path, Express::VARP wavform) {
    AUDIO::save(path, wavform, 24000);
}

} // namespace Transformer
} // namespace MNN
