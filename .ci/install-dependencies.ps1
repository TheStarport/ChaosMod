# Explicitly install dependencies as their own step so we can cache the results
$ErrorActionPreference = "Stop"
$VCPKG_DIR = "$HOME\vcpkg"
$DEST = "${HOME}/project/vcpkg_installed"

# Only download and install packages if the vcpkg cache is not found
if (-not (Test-Path $DEST)) {
    Push-Location "$VCPKG_DIR"
    ./vcpkg.exe install --x-wait-for-lock --triplet "x86-windows" --vcpkg-root "$VCPKG_DIR" --x-manifest-root="${HOME}/project" --x-install-root="$DEST/x86-windows" 2>&1
    Pop-Location
}
