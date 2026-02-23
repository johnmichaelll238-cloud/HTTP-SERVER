import subprocess
import json

print("Running build via Python...")
with open("config.json", "r") as f:
    config = json.load(f)
build_mode = config.get("build_mode", "debug")
runtime_mode = config.get("runtime_mode", "debug")
verbose_logging = config.get("verbose_logging", "True")

#validate config values
if build_mode not in ["debug", "release"]:
    print(f"invalid build_mode : {build_mode}")
if runtime_mode not in ["debug", "release"]:
    print(f"invalid runtime_mode : {runtime_mode}")


#execute build command
subprocess.run(["make", f"MODE={build_mode}"])
#if result.returncode == 0:
   # print("Build successful!")
#else:
   # print("Build failed!")
#execute runtime command
runtime_args = [f"--mode={runtime_mode}"]
if verbose_logging:
    runtime_args.append("--verbose")
subprocess.run(
    ["./build/main"] + runtime_args
    )