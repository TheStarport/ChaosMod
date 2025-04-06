import click
import io
import os
import platform
import requests
import shutil
import subprocess
from zipfile import ZipFile
from get_release_notes import get_release_notes


def is_windows():
    return platform.system() == "Windows" or os.getenv("GITHUB_ACTIONS") is not None


def log(s: str):
    click.echo(">>> " + s)


def try_purge(directory: str, name: str, should_purge: bool):
    if os.path.isdir(directory):
        if should_purge:
            log(f"Found existing {name}, purging...")
            shutil.rmtree(directory)


def run(cmd: str, no_log: bool = False, allow_error: bool = False) -> int:

    log("Running Command: " + cmd)
    proc = subprocess.Popen(cmd, stdout=(subprocess.DEVNULL if no_log else subprocess.PIPE),
                            stderr=subprocess.STDOUT,  shell=True, text=True)

    if no_log:
        proc.wait()
    else:
        while proc.poll() is None:
            line = proc.stdout.readline()

            # if it ends with a new line character, remove it, so we don't print two lines
            if line.endswith('\n'):
                if line.endswith('\r\n'):
                    line = line[:-2]
                else:
                    line = line[:-1]

            print(line)

        print(proc.stdout.read())
        if not allow_error and proc.returncode != 0:
            raise ValueError(f"Execution of {cmd} failed")

    return proc.returncode


@click.group()
def cli():
    pass


@cli.command(short_help='Create dynamic patch notes from a provided date range. If running inside of GitHub actions,'
                        'it will write to the ENV variables: URL and BODY')
@click.option("-d", "--date", required=True, type=click.STRING,
              help="A date string in the format: YYYY/MM/DD")
@click.option("-t", "--token", type=click.STRING,
              help="A github token for querying the API. May be required depending on usage.")
def notes(date: str, token: str):
    url, body = get_release_notes(date, token)
    log("URL = " + url)
    log("BODY = \n" + body)


@cli.command(short_help='Downloads any external dependencies that conan cannot get')
@click.option("-p", "--purge", is_flag=True, help="Purge existing ")
def requirements(purge: bool):
    try_purge("vendor/discord_game_sdk", "Discord Game SDK", purge)
    try_purge("vendor/DXSDK", "DirectX9 SDK", purge)
    try_purge("vendor/ffmpeg", "FFMPEG", purge)

    log("Downloading Discord Game SDK")
    payload = requests.get('https://dl-game-sdk.discordapp.net/3.2.1/discord_game_sdk.zip')

    if os.path.isdir("vendor/discord_game_sdk"):
        log("Found existing discord_game_sdk, purging...")
        shutil.rmtree("vendor/discord_game_sdk")

    log("Unpacking Discord SDK")
    zip_file = ZipFile(io.BytesIO(payload.content))
    zip_file.extractall('vendor/discord_game_sdk')
    log("Finished extracting Discord Game SDK")

    log("Downloading DirectX 9 SDK")
    payload = requests.get('https://download.microsoft.com/download/A/E/7/AE743F1F-632B-4809-87A9-AA1BB3458E31'
                           '/DXSDK_Jun10.exe')

    with open("_DX2010_.exe", "wb") as dx:
        dx.write(payload.content)

    log("Extracting contents from _DX2010_.exe ...")
    if run("7z x _DX2010_.exe DXSDK/Include -ovendor -y", no_log=True) != 0 or run("7z x _DX2010_.exe DXSDK/Lib/x86 -ovendor -y", no_log=True) != 0:
        log("Unable to extract DXSDK! Ensure that 7z (7Zip command line) is available on the path.")
        raise EnvironmentError("Unable to extract DXSDK")

    log("Extracted contents successfully. Cleaning up...")
    os.remove("_DX2010_.exe")

    log("Extracting FFMPEG...")
    run("7z x vendor/ffmpeg.7z -ovendor/ffmpeg -y", no_log=True)

    log("Done")


@cli.command(short_help='Install and build dependencies via conan')
@click.option("-r", "--release", is_flag=True, help="Configure in release mode")
def install(release: bool):
    default_profile = run("conan profile path default", allow_error=True)
    if default_profile:
        run("conan profile detect")

    presets = "CMakePresets.json"
    if os.path.exists(presets):
        os.remove(presets)

    if is_windows():
        shutil.copy2("CMakePresetsWindows.json", presets)
        run(f"conan install . --build missing -pr:b=default -pr:h=./profiles/windows-{('dbg' if release else 'rel')}")
        run(f"conan install . --build missing -pr:b=default -pr:h=./profiles/windows-{('rel' if release else 'dbg')}")
    else:
        shutil.copy2("CMakePresetsLinux.json", presets)
        run(f"conan install . --build missing -pr:b=default -pr:h=./profiles/linux-dbg")
        run(f"conan install . --build missing -pr:b=default -pr:h=./profiles/linux-rel")


@cli.command(short_help='Runs a first-time build, downloading any needed dependencies, and generating preset files.')
@click.option("-r", "--release", is_flag=True, help="Build in release mode")
@click.pass_context
def build(ctx: click.Context, release: bool):
    preset = 'build' if is_windows() else ('release' if release else 'debug')

    # noinspection PyTypeChecker
    ctx.invoke(install, release=release)
    run(f'cmake --preset="{preset}"')
    run(f"cmake --build build/{('' if is_windows() else ('Release' if release else 'Debug'))}", allow_error=True)


if __name__ == '__main__':
    cli()
