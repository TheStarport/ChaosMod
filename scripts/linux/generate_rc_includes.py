import re
import os
import shutil
from pathlib import Path

INCLUDE_REGEX = re.compile(r"^\s*#\s*include\s*[<\"](\S+)[>\"]$", re.MULTILINE)

def generate_rc_includes(msvc: Path, kit: Path, out_dir: Path, cmake_output: Path):
    msvc_ver = Path([f.name for f in os.scandir(str(msvc)) if f.is_dir()][0])
    sdk_ver = kit / 'Include'
    sdk_ver = Path([f.name for f in os.scandir(str(sdk_ver)) if f.is_dir()][0])
    clang_cl_path = shutil.which('clang-cl')
    if clang_cl_path is None:
        raise Exception('clang-cl not found in PATH')

    include_search_dirs: list[Path] = [
        # Got this by running "clang-cl -### <all other arguments to compile a .cpp>" and looking for -internal-isystem
        msvc / msvc_ver / 'include',
        msvc / msvc_ver / 'atlmfc/include',
        kit / 'Include' / sdk_ver / 'ucrt',
        kit / 'Include' / sdk_ver / 'shared',
        kit / 'Include' / sdk_ver / 'um',
        kit / 'Include' / sdk_ver / 'winrt',
        kit / 'Include' / sdk_ver / 'cppwinrt',
    ]

    # Store the paths and variables for CMake
    with open(cmake_output, "w") as f:
        f.write(f'set(CLANGCL_LINUX_MSVC_VERSION "{msvc_ver}")\n')
        f.write(f'set(CLANGCL_LINUX_SDK_VERSION "{sdk_ver}")\n')
        f.write(f'set(CLANGCL_LINUX_VFS "{os.path.join(os.getcwd(), 'profiles', 'vfs-overlay.yml')}")\n')
        f.write(f'set(CLANGCL_LINUX_INCLUDE_RC "{str(out_dir.absolute())}")\n')

    include_files_lower_case: dict[str, tuple[str, Path]] = {}

    for path in include_search_dirs:
        # Find all headers and save their paths
        for i in path.rglob('*', case_sensitive=False):
            if i.is_dir():
                continue

            include_name = i.relative_to(path).as_posix()
            include_name_lower = include_name.lower()

            if include_name_lower in include_files_lower_case:
                print(f'Duplicate "{include_name_lower}" = {include_files_lower_case[include_name_lower][1]}')
                print(f'Duplicate "{include_name_lower}" = {i}')
            else:
                include_files_lower_case[include_name_lower] = include_name, path

    include_files_with_case: set[str] = set()

    # Loop through all include files and find different case variations
    for file_name, base_path in include_files_lower_case.values():
        with open(base_path / file_name, 'r', errors='replace') as f:
            file_text = f.read()

        for match in re.findall(INCLUDE_REGEX, file_text):
            include_text: str = match.replace('\\', '/')
            include_text_lower = include_text.lower()

            if include_text_lower in include_files_lower_case:
                include_files_with_case.add(include_text)
            else:
                print(f'Not found: {include_text}')

    # Create symlinks to files in original case
    for file_name, base_path in include_files_lower_case.values():
        symlink_path = out_dir / file_name
        symlink_path.parent.mkdir(parents=True, exist_ok=True)

        print(f'[orig] {file_name} -> {base_path / file_name}')

        if not symlink_path.exists():
            os.symlink(base_path / file_name, symlink_path)

    # Create symlinks to fils in other cases
    for file_name_case in include_files_with_case:
        file_name, base_path = include_files_lower_case[file_name_case.lower()]

        if file_name_case == file_name:
            continue

        symlink_path = out_dir / file_name_case
        symlink_path.parent.mkdir(parents=True, exist_ok=True)

        print(f'[case] {file_name_case} -> {base_path / file_name}')

        if not symlink_path.exists():
            os.symlink(base_path / file_name, symlink_path)
