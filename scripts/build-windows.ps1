
$qtPath = "$Env:GITHUB_WORKSPACE\qt_build"
$tc2artnetPath = "$Env:GITHUB_WORKSPACE\tc2artnet"

cd "$tc2artnetPath"

& "$qtPath\bin\qmake.exe" tc2artnet.pro "CONFIG+=release"
mingw32-make -j4

cp "$tc2artnetPath\release\tc2artnet.exe" "$Env:GITHUB_WORKSPACE"

dir
