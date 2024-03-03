$CHAOS_DIR = "$HOME\ChaosMod"

Push-Location "$CHAOS_DIR"

msbuild ./ChaosMod.sln /t:Build /p:Configuration=Release /p:Platform=x86

Pop-Location