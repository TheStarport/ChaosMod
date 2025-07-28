import os
import shutil
from glob import iglob
from sys import exit, argv

if len(argv) < 2:
    print("Error no directory provided")
    exit(1)

dest_dir = argv[1]
is_release = len(argv) > 2 and argv[2].lower() == "release"

# noinspection PyBroadException
print(f"Running post build, copying from {os.getcwd()} to {dest_dir}")
try:
    if not os.path.isdir('./build'):
        print('Build directory not present. Run the build first.')
        exit(0)

    src_dir = f"./build/{'Release' if is_release else 'Debug'}"
    exe_dir = os.path.join(dest_dir, 'EXE')

    files = iglob(os.path.join(src_dir, "*.dll"))
    for file in files:
        if os.path.isfile(file):
            shutil.copy2(file, exe_dir)

    shutil.copytree('./vendor/ffmpeg/bin', exe_dir, dirs_exist_ok=True)
    shutil.copytree('./Assets/', dest_dir, dirs_exist_ok=True)

except Exception as e:
    print(e)