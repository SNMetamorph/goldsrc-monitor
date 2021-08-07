# GoldSrc Monitor
Simple utility for mapping/scripting or researching games based on GoldSrc/![Xash3D](https://github.com/FWGS/xash3d-fwgs) engine.<br>
Download this program and read description you can on [Gamebanana Page](https://gamebanana.com/gamefiles/8977)
<br>
![Gamebanana Page](https://gamebanana.com/mods/embeddables/39429?variant=large)

# Building
1) Clone this repository, following the instructions given above
```
git clone --recursive https://github.com/SNMetamorph/goldsrc-monitor.git
cd goldsrc-monitor
git submodule update --init --recursive
```
2) Open CMakeLists.txt using Visual Studio
3) Select desired debug/release build type
4) Build entire project
5) Result compiled files will be located in `build\bin\Debug` or `build\bin\Release`

# External libraries
[PolyHook 2.0](https://github.com/stevemk14ebr/PolyHook_2_0) - Copyright (c) 2019, Stephen Eckels - MIT license<br>
[Half Life 1 SDK](https://github.com/ValveSoftware/halflife) - Copyright (c) Valve Corp. - [License](https://github.com/ValveSoftware/halflife/blob/master/LICENSE)<br>
[RapidJSON](https://github.com/Tencent/rapidjson) - Copyright (c) 2015 THL A29 Limited, a Tencent company, and Milo Yip - [License](https://github.com/Tencent/rapidjson/blob/master/license.txt)
