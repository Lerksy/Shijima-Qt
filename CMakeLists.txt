cmake_minimum_required(VERSION 3.24)
project(ShijimaQt VERSION 0.1.0 LANGUAGES C CXX)

set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/conandata.yml")

if(CMAKE_SOURCE_DIR STREQUAL CMAKE_BINARY_DIR)
    message(FATAL_ERROR "Use an out-of-source build: cmake -S . -B build")
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(NOT CMAKE_CONFIGURATION_TYPES AND NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build configuration" FORCE)
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS Debug Release RelWithDebInfo MinSizeRel)
endif()

option(SHIJIMA_USE_QTMULTIMEDIA "Enable mascot sound effects (requires Qt Multimedia)" ON)
option(SHIJIMA_USE_STUB_PLATFORM "Build without native desktop integration" OFF)
option(SHIJIMA_DEPLOY_QT "Bundle Qt runtime libraries when installing" OFF)

set(qt_components Core Gui Widgets Concurrent)
if(SHIJIMA_USE_QTMULTIMEDIA)
    list(APPEND qt_components Multimedia)
endif()
if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    list(APPEND qt_components DBus)
endif()

find_package(Python3 3.8 REQUIRED COMPONENTS Interpreter)
find_package(Qt6 REQUIRED COMPONENTS ${qt_components})
find_package(LibArchive CONFIG REQUIRED)
find_package(httplib CONFIG REQUIRED)
find_package(BZip2 CONFIG REQUIRED)
find_package(LibLZMA CONFIG REQUIRED)
find_package(ZLIB CONFIG REQUIRED)
find_package(pugixml CONFIG REQUIRED)
find_package(Threads REQUIRED)

include(GNUInstallDirs)
include(cmake/Resources.cmake)
add_subdirectory(libshijima)
add_subdirectory(libshimejifinder)

qt_add_executable(shijima-qt
    main.cc
    Asset.cc
    AssetLoader.cc
    cli.cc
    ForcedProgressDialog.cc
    MascotData.cc
    ShijimaContextMenu.cc
    ShijimaHttpApi.cc
    ShijimaLicensesDialog.cc
    ShijimaManager.cc
    ShijimaWidget.cc
    ShimejiInspectorDialog.cc
    SoundEffectManager.cc
)
target_include_directories(shijima-qt PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}" "${SHIJIMA_GENERATED_DIR}")
target_link_libraries(shijima-qt PRIVATE
    Qt6::Core Qt6::Gui Qt6::Widgets Qt6::Concurrent
    shijima shimejifinder httplib::httplib Threads::Threads
)
target_compile_definitions(shijima-qt PRIVATE SHIJIMA_USE_QTMULTIMEDIA=$<BOOL:${SHIJIMA_USE_QTMULTIMEDIA}>)
if(SHIJIMA_USE_QTMULTIMEDIA)
    target_link_libraries(shijima-qt PRIVATE Qt6::Multimedia)
endif()
if(MSVC)
    target_compile_options(shijima-qt PRIVATE /W3 /utf-8 /permissive-)
    target_compile_definitions(shijima-qt PRIVATE _CRT_SECURE_NO_WARNINGS NOMINMAX)
else()
    target_compile_options(shijima-qt PRIVATE -Wall)
endif()

set(mascot_files)
foreach(number RANGE 1 46)
    list(APPEND mascot_files "${CMAKE_CURRENT_SOURCE_DIR}/DefaultMascot/img/shime${number}.png")
endforeach()
list(APPEND mascot_files "${CMAKE_CURRENT_SOURCE_DIR}/DefaultMascot/behaviors.xml"
    "${CMAKE_CURRENT_SOURCE_DIR}/DefaultMascot/actions.xml")
shijima_generate_resource("${SHIJIMA_GENERATED_DIR}/DefaultMascot.cc" mascot ${mascot_files})

set(license_files)
foreach(name IN ITEMS Shijima-Qt.LICENSE.txt duktape.LICENSE.txt duktape.AUTHORS.rst
        libarchive.LICENSE.txt libshijima.LICENSE.txt libshimejifinder.LICENSE.txt
        unarr.LICENSE.txt unarr.AUTHORS.txt Qt.LICENSE.txt rapidxml.LICENSE.txt cpp-httplib.LICENSE.txt)
    list(APPEND license_files "${CMAKE_CURRENT_SOURCE_DIR}/licenses/${name}")
endforeach()
shijima_generate_resource("${SHIJIMA_GENERATED_DIR}/licenses_generated.hpp" licenses ${license_files})
target_sources(shijima-qt PRIVATE "${SHIJIMA_GENERATED_DIR}/DefaultMascot.cc"
    "${SHIJIMA_GENERATED_DIR}/licenses_generated.hpp")

include(cmake/Platform.cmake)

install(TARGETS shijima-qt
    RUNTIME_DEPENDENCY_SET shijima_runtime
    BUNDLE DESTINATION .
    RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
)
if(WIN32 AND NOT CMAKE_CROSSCOMPILING)
    # Conan dependencies default to static libraries. For shared overrides,
    # activate Conan's run environment before installing to locate their DLLs.
    install(RUNTIME_DEPENDENCY_SET shijima_runtime
        DIRECTORIES "$<TARGET_FILE_DIR:shijima-qt>" "$<TARGET_FILE_DIR:Qt6::Core>"
        PRE_EXCLUDE_REGEXES "api-ms-" "ext-ms-"
        POST_EXCLUDE_REGEXES ".*[Ww][Ii][Nn][Dd][Oo][Ww][Ss][/\\\\][Ss][Yy][Ss][Tt][Ee][Mm]32[/\\\\].*"
        RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
    )
endif()
install(FILES ${license_files} DESTINATION "${CMAKE_INSTALL_DATADIR}/licenses/shijima-qt")
if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    install(FILES com.pixelomer.ShijimaQt.desktop DESTINATION "${CMAKE_INSTALL_DATADIR}/applications")
    install(FILES com.pixelomer.ShijimaQt.metainfo.xml DESTINATION "${CMAKE_INSTALL_DATADIR}/metainfo")
    install(FILES com.pixelomer.ShijimaQt.png DESTINATION "${CMAKE_INSTALL_DATADIR}/icons/hicolor/512x512/apps")
endif()
if(SHIJIMA_DEPLOY_QT)
    if(Qt6_VERSION VERSION_LESS 6.5)
        message(FATAL_ERROR "SHIJIMA_DEPLOY_QT requires Qt 6.5 or newer")
    endif()
    qt_generate_deploy_app_script(TARGET shijima-qt OUTPUT_SCRIPT deploy_script)
    install(SCRIPT "${deploy_script}")
endif()
