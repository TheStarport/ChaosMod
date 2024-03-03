# Explicitly install dependencies as their own step so we can cache the results
$ErrorActionPreference = "Stop"
$VCPKG_DIR = "$HOME\vcpkg"

Push-Location "$VCPKG_DIR"

./vcpkg.exe install --x-wait-for-lock --triplet "x86-windows" --vcpkg-root "$VCPKG_DIR" --x-manifest-root="${HOME}" --x-install-root="${HOME}\vcpkg_installed\x86-windows"

Pop-Location