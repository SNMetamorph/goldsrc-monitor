# GoldSrc Monitor
There is small and simple instrument for researching or debugging  games, based on GoldSrc engine.<br>
[Gamebanana Page](https://gamebanana.com/gamefiles/8977)

# Cloning
```
git clone --recursive https://github.com/SNMetamorph/goldsrc-monitor.git
cd goldsrc-monitor
git submodule update --init --recursive
```

# Compiling
> Note: for compiling, you should set "HLSDK" environment variable to actual HLSDK path

1) Clone [PolyHook 2.0](https://github.com/stevemk14ebr/PolyHook_2_0) repository.
2) Build PolyHook as static library.
3) Clone this repository, following the instructions given above. 
4) Place resulting PolyHook_2.lib and capstone.lib to `msvc/build` directory.
5) Build solution using Visual Studio.

# About 
This project uses [PolyHook 2.0](https://github.com/stevemk14ebr/PolyHook_2_0) library - Copyright (c) 2019, Stephen Eckels - MIT license
