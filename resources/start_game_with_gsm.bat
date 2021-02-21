rem Example script for starting GoldSrc Monitor and Half-Life just using one file
rem You can place this file in game directory and easy-n-fast start game and tool both
rem Assumed that GoldSrc Monitor located in directory C:\goldsrc-monitor
rem But you just can change it to yours in line number 6
@echo off
set DIR_GOLDSRC_MONITOR="C:\goldsrc-monitor"
start "" %DIR_GOLDSRC_MONITOR%\gsm-loader.exe +inject_delay 5000 +process_name hl.exe +library_name "%DIR_GOLDSRC_MONITOR%\gsm-library.dll"
start "" hl.exe -game ts
