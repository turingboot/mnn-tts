# mnn-tts

目前仅支持[Kokoro-82M](https://huggingface.co/hexgrad/Kokoro-82M)。


## 模型导出

```
cd mnn-tts
huggingface-cli download --resume-download onnx-community/Kokoro-82M-v1.0-ONNX --local-dir Kokoro-82M-v1.0-ONNX
python export.py ./Kokoro-82M-v1.0-ONNX
```

## 模型测试

```
mkdir build
cd build
cmake .. && make -j32
./tts_demo ../model/config.json 你好
open output.wav
```

## 参考项目
- [kokoro](https://github.com/hexgrad/kokoro)
- [misaki](https://pypi.org/project/misaki/)
- [cppjieba](https://github.com/yanyiwu/cppjieba)
- [cpp-pinyin](https://github.com/wolfgitpr/cpp-pinyin)