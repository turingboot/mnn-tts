import os
import sys
import json
import glob
import base64
import argparse
import numpy as np
import MNN.expr as expr
from MNN.tools import mnnconvert

RESET = "\033[0m"
GREEN = "\033[32;1m"
YELLOW = "\033[33;4m"

class Kokoro:

    def __init__(self, args):
        self.model_path = args.path
        self.dst_path = args.dst_path
        if not os.path.exists(self.dst_path):
            os.makedirs(self.dst_path)
        if os.path.exists(args.mnnconvert):
            self.mnnconvert = args.mnnconvert
        else:
            self.mnnconvert = None

    def convert(self, onnx_path, mnn_path):
        convert_args = [
            '',
            '-f',
            'ONNX',
            '--modelFile',
            str(onnx_path),
            '--MNNModel',
            str(mnn_path),
            '--weightQuantBits',
            '8',
            #'--weightQuantBlock',
            #'128'
        ]
        sfd = os.dup(1)
        log_fp = open('./.export.log', "a")
        log_fd = log_fp.fileno()
        # mnnconvert ... > .export.log
        os.dup2(log_fd, 1)
        try:
            sys.argv = convert_args
            sys.argc = len(convert_args)
            if self.mnnconvert is None:
                mnnconvert.main()
            else:
                convert_args[0] = self.mnnconvert
                cmd = ' '.join(convert_args)
                message = os.popen(cmd).read()
                print(message)
            sys.argv = []
        finally:
            os.dup2(sfd, 1)
            os.close(log_fd)

    def export_model(self):
        onnx_file = os.path.join(self.model_path, "onnx", "model.onnx")
        self.convert(onnx_file, f'{self.dst_path}/tts.mnn')

    def export_voice(self):
        voices = []
        self.styles = []
        for voice_file in glob.glob(os.path.join(self.model_path, "voices", "*.bin")):
            style = os.path.basename(voice_file).split('.')[0]
            if style != 'zf_xiaoxiao': continue
            voice = np.fromfile(voice_file, dtype=np.float32).reshape(-1, 1, 256)
            voices.append(expr.const(voice, voice.shape, expr.NCHW, expr.float))
        expr.save(voices, f'{self.dst_path}/voices.mnn')

    def export_config(self):
        tts_config = {}
        tts_config['styles'] = self.styles
        with open(f'{self.dst_path}/tts_config.json', 'w', encoding='utf-8') as f:
            json.dump(tts_config, f, ensure_ascii=False, indent=4)
        with open(f'{self.dst_path}/config.json', 'w', encoding='utf-8') as f:
            config = {
                "tts_model": "tts.mnn",
                "voices": "voices.mnn",
                "tokenizer_file": "tokenizer.txt",
                "backend_type": "cpu",
                "thread_num": 4,
                "precision": "low",
                "memory": "low",
            }
            json.dump(config, f, ensure_ascii=False, indent=4)

    def export_tokenizer(self):
        # TOKENIZER MAGIC NUMBER
        MAGIC_NUMBER = 430
        # TOKENIZER TYPE
        SENTENCEPIECE = 0; TIKTOIKEN = 1; BERT = 2; HUGGINGFACE = 3
        def write_line(fp, *args):
            for arg in args:
                for token in arg:
                    fp.write(str(token) + ' ')
            fp.write('\n')
        def write_header(fp, type, stop_ids, speicals, prefix = []):
            fp.write(f'{MAGIC_NUMBER} {type}\n')
            fp.write(f'{len(speicals)} {len(stop_ids)} {len(prefix)}\n')
            write_line(fp, speicals, stop_ids, prefix)

        tokenizer_file = os.path.join(self.model_path, "tokenizer.json")
        with open(tokenizer_file, "r", encoding="utf-8") as f:
            vocab_dict = json.load(f)['model']['vocab']
            vocab_size = 0
            for k, v in vocab_dict.items():
                vocab_size = max(vocab_size, v)
            vocab_list = ['' for i in range(vocab_size + 1)]
            for k, v in vocab_dict.items():
                vocab_list[v] = k
        file_path = os.path.join(self.dst_path, "tokenizer.txt")
        with open(file_path, "w", encoding="utf8") as fp:
            write_header(fp, TIKTOIKEN, [], [], [])
            fp.write(f'{len(vocab_list)}\n')
            for v in vocab_list:
                line = base64.b64encode(v.encode('utf-8')).decode("utf8") + "\n"
                fp.write(line)

    def export(self):
        self.export_model()
        self.export_voice()
        self.export_tokenizer()
        self.export_config()
        print(f'{GREEN}[SUCCESS]{RESET} export model to {YELLOW}{self.dst_path}{RESET}')


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='tts_exporter', formatter_class=argparse.RawTextHelpFormatter)
    parser.add_argument('--path', type=str, required=True, help='path of model.')
    parser.add_argument('--dst_path', type=str, default='./model', help='export onnx/mnn model to path, defaut is `./model`.')
    parser.add_argument('--mnnconvert', type=str, default='../../../build/MNNConvert', help='local mnnconvert path, if invalid, using pymnn.')
    args = parser.parse_args()
    kokoro = Kokoro(args)
    kokoro.export()