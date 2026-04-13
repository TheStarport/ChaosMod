import json
import os
from pathlib import Path

import click

from .linux.generate_rc_includes import generate_rc_includes
from .linux.generate_vfs import build_vfs
from .utils import cli, run, is_windows

@cli.command(short_help='Install and build dependencies via conan')
def configure():
    msvc_dir = os.environ.get('MSVC', None)
    if not is_windows():
        if msvc_dir is None:
            raise Exception("ENV MSVC is not set or points to a directory that does not exist")

        if not os.path.exists("profiles/rc_includes"):
            vc_dir = Path(os.path.join(msvc_dir, 'VC', 'Tools', 'MSVC'))
            kit_dir = Path(os.path.join(msvc_dir, 'Windows Kits', '10'))
            generate_rc_includes(vc_dir, kit_dir, out_dir=Path('profiles/rc_includes'),
                                 cmake_output=Path('profiles/clang.cmake'))
            build_vfs(str(vc_dir), str(kit_dir), out_file='profiles/vfs-overlay.yml')

    default_profile = run("conan profile path default", allow_error=True)
    if default_profile:
        run("conan profile detect")

    host = 'windows' if is_windows() else 'linux'
    args = [
        ".",
        '--build missing',
        '-pr:b=default',
        f'-pr:h=./profiles/{host}',
    ]

    if os.path.exists('ConanPresets.json'):
        os.remove('ConanPresets.json')

    run("conan remote add fluf scripts", allow_error=True)
    no_debug = os.environ.get('DISABLE_CONAN_DEBUG', False)
    debug = not no_debug or no_debug != 'TRUE'
    if debug:
        run(f"conan install {' '.join(args)} -s build_type=Debug")

    run(f"conan install {' '.join(args)} -s build_type=RelWithDebInfo")

    if not debug:
        # If we do not have a debug preset, we need to manufacture one to make cmake happy
        with open("ConanPresets.json", "r") as jsonFile:
            data = json.load(jsonFile)

        data["configurePresets"] = [
            {
                "name": "conan-debug",
                "inherits": "conan-relwithdebinfo",
            }
        ]

        data["buildPresets"] = [
            {
                "name": "conan-debug",
                "inherits": "conan-relwithdebinfo"
            }
        ]

        with open("ConanPresets.json", "w") as jsonFile:
            json.dump(data, jsonFile, indent=4)


# noinspection PyShadowingBuiltins
@cli.command(short_help='Runs a first-time build, downloading any needed dependencies, and generating preset files.')
@click.option("-r", "--release", is_flag=True, help="Build in release mode")
@click.option("--no-post-build", is_flag=True, help="Configure in release mode")
@click.option("--no-hooks", is_flag=True, help="Disable FLUF core hooks")
@click.pass_context
def build(ctx: click.Context, release: bool, no_post_build: bool, no_hooks: bool):
    preset = 'release' if release else 'debug'

    ctx.invoke(configure)  # noinspection PyTypeChecker

    run(f"cmake --preset={preset} {'-DNO_POST_BUILD=TRUE' if no_post_build else ''} {'-DNO_HOOKS=TRUE' if no_hooks else ''}")
    run(f"cmake --build build/{'RelWithDebInfo' if release else 'Debug'}", allow_error=True)
