# Explicitly install dependencies as their own step so we can cache the results
$ErrorActionPreference = "Stop"
$VCPKG_DIR = "$HOME\vcpkg"
$VCPKG_DEST = "${HOME}/project/vcpkg_installed"
$DX9_SDK = "${HOME}/DirectXSDK"

# Only download and install packages if the vcpkg cache is not found
if (-not (Test-Path $VCPKG_DEST)) {
    Push-Location "$VCPKG_DIR"
    ./vcpkg.exe install --x-wait-for-lock --triplet "x86-windows" --vcpkg-root "$VCPKG_DIR" --x-manifest-root="${HOME}/project" --x-install-root="$VCPKG_DEST/x86-windows"
    Pop-Location
    if ($LASTEXITCODE -ne 0) {
        exit $LASTEXITCODE
    }
}

# Only download and install dx9 sdk if not cached
if (-not (Test-Path $DX9_SDK)) {
    & "C:\windows\system32\curl.exe" -L -o "${HOME}/7z.exe" "https://www.7-zip.org/a/7zr.exe"
    mkdir TEMP; cd TEMP
    & "C:\windows\system32\curl.exe" -L -o "_DX2010_.exe" "https://download.microsoft.com/download/A/E/7/AE743F1F-632B-4809-87A9-AA1BB3458E31/DXSDK_Jun10.exe"
    7z x _DX2010_.exe DXSDK/Include -o_DX2010_
    if ($LASTEXITCODE -ne 0) {
        exit $LASTEXITCODE
    }

    7z x _DX2010_.exe DXSDK/Lib/x86 -o_DX2010_
    mv _DX2010_/DXSDK $DX9_SDK
    cd ..; rm TEMP -Force -Recurse
    [System.Environment]::SetEnvironmentVariable('DX9_SDK', $DX9_SDK, 'User')
}