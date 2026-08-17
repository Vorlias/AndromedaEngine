# For helper scripts

include(FetchContent)
set(LUTE_VERSION "v1.0.0")

string(TOLOWER "${CMAKE_SYSTEM_NAME}-${CMAKE_SYSTEM_PROCESSOR}" LUTE_TARGET)

FetchContent_Declare(download_lute
    URL "https://github.com/luau-lang/lute/releases/download/${LUTE_VERSION}/lute-${LUTE_TARGET}.zip"
    DOWNLOAD_NO_EXTRACT false
)

if(NOT download_lute_POPULATED)
    FetchContent_Populate(download_lute)
    if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
        if (EXISTS "${download_lute_SOURCE_DIR}/lute.exe")
            file(RENAME "${download_lute_SOURCE_DIR}/lute.exe" "../lute.exe")
        endif()
    else()
        if (EXISTS "${download_lute_SOURCE_DIR}/lute")
            file(RENAME "${download_lute_SOURCE_DIR}/lute" "../lute")
        endif() 
    endif()
endif()

