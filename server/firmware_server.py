from flask import Flask, send_from_directory
import os

app = Flask(__name__)
# 펌웨어 파일이 들어갈 폴더
FIRMWARE_DIR = "./firmware_storage"

if not os.path.exists(FIRMWARE_DIR):
    os.makedirs(FIRMWARE_DIR)

@app.route('/download/<filename>')
def download_firmware(filename):
    # 지정된 폴더에서 파일을 찾아 전송
    return send_from_directory(FIRMWARE_DIR, filename)

if __name__ == '__main__':
    print(f"[File Server] Running on http://127.0.0.1:5000")
    app.run(host='0.0.0.0', port=5000)