cmake_minimum_required(VERSION 3.17)
project(CrossCraft-SurvivalTest)

add_compile_options(-march=native -flto -O2 -Wno-deprecated-non-prototype)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_STANDARD 11)

# Set compile definitions
if(WIN32)
    add_compile_definitions(BUILD_PLAT=0)
elseif(PSP)
    add_compile_definitions(BUILD_PLAT=2)
    add_compile_definitions(IOAPI_NO_64)
elseif(3DS)
    add_compile_definitions(BUILD_PLAT=7)
    add_compile_definitions(IOAPI_NO_64)
elseif(VITA)
    add_compile_definitions(BUILD_PLAT=3)
    add_compile_definitions(IOAPI_NO_64)
else()
    add_compile_definitions(BUILD_PLAT=1)
endif()

# Add Stardust & Yaml-CPP
add_subdirectory(ext/Stardust-Celeste Stardust)
add_compile_definitions(YAML_CPP_STATIC_DEFINE)

file(GLOB_RECURSE SRC_FILES src/*.cpp)
file(GLOB_RECURSE INC_FILES include/*.hpp)
set(MINIZIP_SRC ext/zlib/contrib/minizip/ioapi.c ext/zlib/contrib/minizip/mztools.c ext/zlib/contrib/minizip/unzip.c ext/zlib/contrib/minizip/zip.c)
set(LUA_SRC ext/lua/onelua.c)

if(NOT VITA)
    set(ZLIB_SRCS ext/zlib/adler32.c ext/zlib/compress.c ext/zlib/crc32.c ext/zlib/deflate.c ext/zlib/gzclose.c ext/zlib/gzlib.c ext/zlib/gzread.c ext/zlib/gzwrite.c ext/zlib/inflate.c ext/zlib/infback.c ext/zlib/inftrees.c ext/zlib/inffast.c ext/zlib/trees.c ext/zlib/uncompr.c ext/zlib/zutil.c)
endif()

if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    set(MINIZIP_SRC ext/zlib/contrib/minizip/iowin32.c ext/zlib/contrib/minizip/ioapi.c ext/zlib/contrib/minizip/mztools.c ext/zlib/contrib/minizip/unzip.c ext/zlib/contrib/minizip/zip.c)
endif()

add_executable(CrossCraft-SurvivalTest ${ZLIB_SRCS} ${LUA_SRC} ${MINIZIP_SRC} ${SRC_FILES} ${INC_FILES})
target_compile_definitions(CrossCraft-SurvivalTest PUBLIC MAKE_LIB)
target_include_directories(CrossCraft-SurvivalTest PUBLIC ext/Stardust-Celeste/ext/yaml/include ext/Stardust-Celeste/ext/glm/glm/ ext/FNL/Cpp ext/zlib/ ext/lua/ ext/Stardust-Celeste/ext)
target_link_libraries(CrossCraft-SurvivalTest Stardust-Celeste yaml-cpp)

if(WIN32)
    add_library(ZLib ${ZLIB_SRCS})
    target_link_libraries(CrossCraft-SurvivalTest ws2_32 stdc++ ZLib)
    target_compile_options(CrossCraft-SurvivalTest PUBLIC -Wall -Wextra)
endif()

if(PSP)
    create_pbp_file(
        TARGET CrossCraft-SurvivalTest
        TITLE CrossCraft-SurvivalTest
        ICON_PATH ../src/assets/psp/ICON0.png
        BACKGROUND_PATH ../src/assets/psp/PIC1.png
        BUILD_PRX 1
    )
endif()

if(3DS)
    include(ext/Stardust-Celeste/Tools3DS.cmake)

    add_3dsx_target(
        CrossCraft-SurvivalTest
        APP_TITLE CrossCraft-SurvivalTest
        APP_DESCRIPTION "CrossCraft Survival Test 3DS"
        APP_AUTHOR "Iridescence and CypressImplex"
    )
endif()

if(VITA)
    include(ext/Stardust-Celeste/setup_vita.cmake REQUIRED)
    setup_vita()

    include("$ENV{VITASDK}/share/vita.cmake" REQUIRED)

    set(VITA_APP_NAME "CrossCraft SurvivalTest")
    set(VITA_TITLEID "CCST00100")
    set(VITA_VERSION "01.00")

    target_include_directories(CrossCraft-SurvivalTest PUBLIC ext/Stardust-Celeste/include/)
    target_link_libraries(CrossCraft-SurvivalTest Stardust-Celeste z)

    vita_create_self(${PROJECT_NAME}.self ${PROJECT_NAME})

    vita_create_vpk(${PROJECT_NAME}.vpk ${VITA_TITLEID} ${PROJECT_NAME}.self
        VERSION ${VITA_VERSION}
        NAME ${VITA_APP_NAME}
        FILE src/assets/vita/icon0.png sce_sys/icon0.png
        FILE src/assets/vita/livearea/contents/bg.png sce_sys/livearea/contents/bg.png
        FILE src/assets/vita/livearea/contents/startup.png sce_sys/livearea/contents/startup.png
        FILE src/assets/vita/livearea/contents/template.xml sce_sys/livearea/contents/template.xml
        FILE resourcepacks/default.zip default.zip
    )
endif()
