import subprocess
import sys
import json

print("Running build via Python...")
with open("config.json", "r") as f:
    config = json.load(f)
defaultb_mode = config.get("build_mode", "debug")
runtime_mode = config.get("runtime_mode", "debug")
if len(sys.argv) > 1:
    build_mode = sys.argv[1]
else:
    build_mode = defaultb_mode
    print("No build mode specified, using default from config.json")

if len(sys.argv) > 1:
    runtime_mode = sys.argv[1]
else:
    print("No runtime mode specified, using default from config.json")


verbose_logging = config.get("verbose_logging", "True")


#execute build command
result = subprocess.run(["make", f"MODE={build_mode}"], check=True)
if result.returncode == 0:
    print("Build successful!")
else:
    print("Build failed!")
#execute runtime command
runtime_args = [f"--mode={runtime_mode}"]
#if verbose_logging:
 #   runtime_args.append("--verbose")
run = subprocess.run(
    ["./build/main"] + runtime_args
)
if run.returncode == 0:
    print("Runtime successful!")
else:
    print("Runtime failed!")