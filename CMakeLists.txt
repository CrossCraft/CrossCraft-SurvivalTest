cmake_minimum_required(VERSION 3.17)
project(CrossCraft-SurvivalTest)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_STANDARD 11)

if(WIN32)
    add_compile_definitions(BUILD_PLAT=0)
elseif(PSP)
    add_compile_definitions(BUILD_PLAT=2)
    add_compile_definitions(IOAPI_NO_64)

elseif(VITA)
    add_compile_definitions(BUILD_PLAT=3)
    add_compile_definitions(IOAPI_NO_64)
else()
    add_compile_definitions(BUILD_PLAT=1)
endif()

add_subdirectory(ext/Stardust-Celeste Stardust)

add_compile_definitions(YAML_CPP_STATIC_DEFINE)

if(NOT VITA)
    add_subdirectory(ext/zlib ZLib)
endif()

file(GLOB_RECURSE SRC_FILES src/*.cpp)
file(GLOB_RECURSE INC_FILES include/*.hpp)
set(MINIZIP_SRC ext/zlib/contrib/minizip/ioapi.c ext/zlib/contrib/minizip/mztools.c ext/zlib/contrib/minizip/unzip.c ext/zlib/contrib/minizip/zip.c)

if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    set(MINIZIP_SRC ext/zlib/contrib/minizip/iowin32.c ext/zlib/contrib/minizip/ioapi.c ext/zlib/contrib/minizip/mztools.c ext/zlib/contrib/minizip/unzip.c ext/zlib/contrib/minizip/zip.c)
endif()

add_executable(CrossCraft-SurvivalTest ${CCW_SRC} ${MINIZIP_SRC} ${SRC_FILES} ${INC_FILES})
target_include_directories(CrossCraft-SurvivalTest PUBLIC ext/Stardust-Celeste/ext/glm/glm/ ext/FNL/Cpp ext/zlib/)
target_link_directories(CrossCraft-SurvivalTest PUBLIC ext/CCW)

if(NOT VITA)
    target_link_libraries(CrossCraft-SurvivalTest SC-Entry Stardust-Celeste zlibstatic yaml-cpp)
else()
    target_link_libraries(CrossCraft-SurvivalTest SC-Entry Stardust-Celeste zlibstatic)
endif()

if(WIN32)
    target_link_libraries(CrossCraft-SurvivalTest ws2_32.lib)
else()
    target_compile_options(CrossCraft-SurvivalTest PRIVATE -g)
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

if(VITA)
    include(ext/Stardust-Celeste/setup_vita.cmake REQUIRED)
    setup_vita()

    include("$ENV{VITASDK}/share/vita.cmake" REQUIRED)

    set(VITA_APP_NAME "CrossCraft SurvivalTest")
    set(VITA_TITLEID "CCST00100")
    set(VITA_VERSION "01.00")

    target_include_directories(CrossCraft-SurvivalTest PUBLIC ext/Stardust-Celeste/include/)
    target_link_libraries(CrossCraft-SurvivalTest SceCtrl_stub vitaGL vitashark mathneon SceShaccCg_stub SceShaccCgExt SceIme_stub SceAppMgr_stub SceGxm_stub SceCommonDialog_stub SceProcessmgr_stub SceDisplay_stub SceKernelDmacMgr_stub ScePower_stub taihen_stub openal pthread SceAudioIn_stub SceAudio_stub vorbisfile vorbis ogg)

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
