vcpkg_from_github(
    OUT_SOURCE_PATH SOURCE_PATH
    REPO alliedmodders/hlsdk
    REF a0edb7792a96998d349325bebab8ea41ec5cb239
    SHA512 d3bb02fbbc31ab5e397a663ca1567b649528eba1755029bf5364dce75b6b0fc9b52e45e32fa9708e614a3d9f0c1ebdc0a066c5ef2b5e256eabdf3a35ad3020df
    HEAD_REF master
)

file(INSTALL "${SOURCE_PATH}/LICENSE" DESTINATION "${CURRENT_PACKAGES_DIR}/share/${PORT}" RENAME copyright)
file(INSTALL "${SOURCE_PATH}/utils/vgui/include" DESTINATION "${CURRENT_PACKAGES_DIR}/include/${PORT}/utils/vgui/include")
file(INSTALL 
    "${SOURCE_PATH}/common" 
    "${SOURCE_PATH}/pm_shared"
    "${SOURCE_PATH}/engine"
    "${SOURCE_PATH}/game_shared"
    "${SOURCE_PATH}/public"
    "${SOURCE_PATH}/dlls"
    "${SOURCE_PATH}/cl_dll"
    DESTINATION "${CURRENT_PACKAGES_DIR}/include/${PORT}/"
    FILES_MATCHING PATTERN "*.h")

# remove empty & unused directories
file(REMOVE_RECURSE 
    "${CURRENT_PACKAGES_DIR}/include/${PORT}/cl_dll/hl" 
    "${CURRENT_PACKAGES_DIR}/include/${PORT}/cl_dll/msvc10" 
    "${CURRENT_PACKAGES_DIR}/include/${PORT}/cl_dll/msvc11"
    "${CURRENT_PACKAGES_DIR}/include/${PORT}/dlls/msvc10"
    "${CURRENT_PACKAGES_DIR}/include/${PORT}/dlls/msvc11"
    "${CURRENT_PACKAGES_DIR}/include/${PORT}/dlls/wpn_shared")
