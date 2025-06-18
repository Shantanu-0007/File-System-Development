from flask import Flask, request, jsonify, send_from_directory
import subprocess
import os

app = Flask(__name__)

# Path to your compiled C program (CLI for filesystem)
MYFS_BIN = "./myfs"

@app.route('/')
def index():
    return send_from_directory('.', 'index.html')

@app.route('/static/<path:path>')
def static_files(path):
    return send_from_directory('static', path)

# Create file
@app.route('/create_file', methods=['POST'])
def create_file():
    filename = request.json.get("filename")
    if not filename:
        return jsonify({"error": "Filename required"}), 400
    result = subprocess.run([MYFS_BIN, "create", filename], capture_output=True, text=True)
    return jsonify({"output": result.stdout.strip(), "error": result.stderr.strip()})

# Create directory
@app.route('/create_dir', methods=['POST'])
def create_dir():
    dirname = request.json.get("filename")
    result = subprocess.run([MYFS_BIN, "mkdir", dirname], capture_output=True, text=True)
    return jsonify({"output": result.stdout.strip(), "error": result.stderr.strip()})

# Delete entry
@app.route('/delete', methods=['POST'])
def delete():
    name = request.json.get("filename")
    result = subprocess.run([MYFS_BIN, "delete", name], capture_output=True, text=True)
    return jsonify({"output": result.stdout.strip(), "error": result.stderr.strip()})

# Read file
@app.route('/read', methods=['POST'])
def read():
    filename = request.json.get("filename")
    result = subprocess.run([MYFS_BIN, "cat", filename], capture_output=True, text=True)
    return jsonify({"content": result.stdout, "error": result.stderr.strip()})

# List files
@app.route('/list', methods=['GET'])
def list_files():
    result = subprocess.run([MYFS_BIN, "ls"], capture_output=True, text=True)
    files = result.stdout.strip().split("\n")
    return jsonify(files)

# Edit file
@app.route('/edit', methods=['POST'])
def edit_file():
    filename = request.json.get("filename")
    content = request.json.get("content")
    try:
        with open(filename, "w") as f:
            f.write(content)
        return jsonify({"message": f"File {filename} edited successfully."})
    except Exception as e:
        return jsonify({"error": str(e)}), 500

if __name__ == '__main__':
    app.run(debug=True)
