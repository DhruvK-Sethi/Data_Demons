from flask import Flask, jsonify
import subprocess

app = Flask(__name__)

@app.route("/test/<int:i>/<int:j>")
def test(i,j):
    # Compile the C++ code
    subprocess.run(["g++", "test.cpp", "-o", "test.exe"])
    
    # Execute the compiled C++ code and capture the output
    result = subprocess.run(["./test.exe"], input=str(i)+" "+str(j), capture_output=True, text=True)

    # Return the output as a response
    return jsonify({"output": result.stdout})

@app.route("/astar")
def astar():
    # subprocess.run(["g++","test3.cpp","-o", "test3.exe", "-lsqlite3"])

    result = subprocess.run(["./test3.exe"], capture_output=True, text=True)

    return jsonify({"output": result.stdout})


if __name__ == "__main__":
    app.run(debug=True)
