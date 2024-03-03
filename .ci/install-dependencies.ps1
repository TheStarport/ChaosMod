# Explicitly install dependencies as their own step so we can cache the results

$VCPKG_DIR = "$HOME\vcpkg"

Push-Location "$VCPKG_DIR"

./vcpkg.exe install --x-wait-for-lock --triplet "x86-windows" --vcpkg-root "$VCPKG_DIR" --x-manifest-root="${HOME}\ChaosMod" --x-install-root="${HOME}\ChaosMod\vcpkg_installed\x86-windows"

Pop-Location