from flask import Flask, send_from_directory

app = Flask(__name__)

# Serve index.html at root
@app.route('/')
def index():
    return send_from_directory('.', 'index.html')

# Existing API routes (example)
@app.route('/create', methods=['POST'])
def create_file():
    # your create file logic
    pass

# Add other routes...

if __name__ == '__main__':
    app.run(debug=True)
from flask import Flask, request, jsonify
import os

app = Flask(__name__)

MOUNT_DIR = "/path/to/mountdir"   # <-- update your real mountdir path

@app.route("/create_file", methods=["POST"])
def create_file():
    filename = request.json["filename"]
    path = os.path.join(MOUNT_DIR, filename)
    open(path, 'w').close()
    return jsonify({"message": "File created"})

@app.route("/create_dir", methods=["POST"])
def create_dir():
    dirname = request.json["filename"]
    path = os.path.join(MOUNT_DIR, dirname)
    os.mkdir(path)
    return jsonify({"message": "Directory created"})

@app.route("/delete", methods=["POST"])
def delete():
    filename = request.json["filename"]
    path = os.path.join(MOUNT_DIR, filename)
    if os.path.isdir(path):
        os.rmdir(path)
    else:
        os.unlink(path)
    return jsonify({"message": "Deleted"})

@app.route("/list", methods=["GET"])
def list_files():
    files = os.listdir(MOUNT_DIR)
    return jsonify(files)

@app.route("/read", methods=["POST"])
def read_file():
    filename = request.json["filename"]
    path = os.path.join(MOUNT_DIR, filename)
    with open(path, 'r') as f:
        content = f.read()
    return jsonify({"content": content})

if __name__ == "__main__":
    app.run(port=5000, debug=True)

from flask import Flask, request, jsonify, send_from_directory

app = Flask(__name__)

MOUNT_DIR = '/mnt/d/PBL/OS/os_project/mountdir'  # your mounted FUSE filesystem path (update this if different)

@app.route('/')
def index():
    return send_from_directory('.', 'index.html')

# Serve static files correctly
@app.route('/static/<path:path>')
def send_static(path):
    return send_from_directory('static', path)

# 📄 Create a file
@app.route('/create_file', methods=['POST'])
def create_file():
    data = request.get_json()
    filename = data.get('filename')
    content = data.get('content', '')
    if not filename:
        return jsonify({'error': 'Filename required'}), 400
    filepath = os.path.join(MOUNT_DIR, filename)
    with open(filepath, 'w') as f:
        f.write(content)
    return jsonify({'message': f'File {filename} created successfully!'})

# 📂 List files
@app.route('/list', methods=['GET'])
def list_files():
    files = []
    for entry in os.listdir(MOUNT_DIR):
        files.append(entry)
    return jsonify({'files': files})

if __name__ == '__main__':
    app.run(debug=True)
