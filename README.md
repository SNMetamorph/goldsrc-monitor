# GoldSrc Monitor
Simple utility for mapping/scripting or researching games based on GoldSrc engine<br>
Download this program and read description you can on [Gamebanana Page](https://gamebanana.com/gamefiles/8977)
<br>
![Gamebanana Page](https://gamebanana.com/gamefiles/embeddables/8977?type=large)

# Building
1) Clone this repository, following the instructions given above.
```
git clone --recursive https://github.com/SNMetamorph/goldsrc-monitor.git
cd goldsrc-monitor
git submodule update --init --recursive
```
2) Open CMakeLists.txt using Visual Studio
3) Select desired debug/release build type
4) Build entire project.
5) Result compiled files will be located in `build\bin\Debug` or `build\bin\Release`

# External libraries
This project uses [PolyHook 2.0](https://github.com/stevemk14ebr/PolyHook_2_0) library - Copyright (c) 2019, Stephen Eckels - MIT license
