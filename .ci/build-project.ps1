$ErrorActionPreference = "Stop"

msbuild ${HOME}/project/ChaosMod/ChaosMod.sln /t:Build /p:Configuration=Release /p:Platform=x86