# GoldSrc Monitor
Simple utility for mapping/scripting or researching games based on GoldSrc engine<br>
Download this program and read description you can on [Gamebanana Page](https://gamebanana.com/gamefiles/8977)
<br>
![Gamebanana Page](https://gamebanana.com/gamefiles/embeddables/8977?type=large)
# Cloning
```
git clone --recursive https://github.com/SNMetamorph/goldsrc-monitor.git
cd goldsrc-monitor
git submodule update --init --recursive
```

# Building
> Note: for compiling, you should set "HLSDK" environment variable to actual HLSDK path

1) Clone this repository, following the instructions given above.
2) Go to `external/polyhook` and build PolyHook as static library.
3) Create `build` directory in repository root directory.
4) Place resulting PolyHook_2.lib and capstone.lib to `build` directory.
5) Build solution using Visual Studio.

# External libraries
This project uses [PolyHook 2.0](https://github.com/stevemk14ebr/PolyHook_2_0) library - Copyright (c) 2019, Stephen Eckels - MIT license
