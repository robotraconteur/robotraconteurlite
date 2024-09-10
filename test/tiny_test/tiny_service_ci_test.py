import subprocess
import sys
from pathlib import Path
import time
import signal
import argparse
import sys

parser = argparse.ArgumentParser(description='Run tiny client CI test')
parser.add_argument('--build-dir', type=str, help='Build directory', required=False, default=None)
args = parser.parse_args()

script_dir = Path(__file__).parent
repo_dir = script_dir.parent.parent.absolute()
examples_build_dir = args.build_dir
if examples_build_dir is None:
    examples_build_dir = (repo_dir / "build" / "examples").absolute()
examples_build_dir = str(examples_build_dir)
print(examples_build_dir)

py_dir = str((repo_dir / "examples" / "tiny_service").absolute())

try:

    service = subprocess.Popen([examples_build_dir + "/robotraconteurlite_tiny_service"],
                               cwd=examples_build_dir, stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    time.sleep(2)

    subprocess.check_call([sys.executable, py_dir + "/tiny_service_client.py"], cwd=py_dir)
finally:

    time.sleep(0.5)
    if sys.platform == "win32":
        service.kill()
    else:
        service.send_signal(signal.SIGINT)
        service.wait()

# Check return code
if service.returncode != 0:
    print(f"Service return code: {service.returncode}")
