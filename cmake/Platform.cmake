if(SHIJIMA_USE_STUB_PLATFORM)
    set(platform Stub)
elseif(WIN32)
    set(platform Windows)
elseif(APPLE)
    set(platform macOS)
elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    set(platform Linux)
else()
    message(FATAL_ERROR "Unsupported platform; use SHIJIMA_USE_STUB_PLATFORM=ON to disable desktop integration")
endif()

target_sources(shijima-qt PRIVATE "Platform/${platform}/ActiveWindowObserver.cc")
if(platform STREQUAL "macOS")
    enable_language(OBJCXX)
    target_sources(shijima-qt PRIVATE Platform/macOS/Platform.mm Platform/macOS/PrivateActiveWindowObserver.mm)
    target_compile_options(shijima-qt PRIVATE "$<$<COMPILE_LANGUAGE:OBJCXX>:-fobjc-arc>")
    set_property(TARGET shijima-qt PROPERTY OBJCXX_STANDARD 17)
    find_library(APPKIT_FRAMEWORK AppKit REQUIRED)
    find_library(APPLICATIONSERVICES_FRAMEWORK ApplicationServices REQUIRED)
    target_link_libraries(shijima-qt PRIVATE "${APPKIT_FRAMEWORK}" "${APPLICATIONSERVICES_FRAMEWORK}")
else()
    target_sources(shijima-qt PRIVATE "Platform/${platform}/Platform.cc")
    if(NOT platform STREQUAL "Stub")
        target_sources(shijima-qt PRIVATE "Platform/${platform}/PrivateActiveWindowObserver.cc")
    endif()
endif()

if(platform STREQUAL "Linux")
    find_package(Qt6 REQUIRED COMPONENTS DBus)
    find_package(X11 REQUIRED)
    target_link_libraries(shijima-qt PRIVATE Qt6::DBus X11::X11)
    target_sources(shijima-qt PRIVATE
        Platform/Linux/KWin.cc Platform/Linux/DBus.cc Platform/Linux/GNOME.cc
        Platform/Linux/GNOMEWindowObserverBackend.cc Platform/Linux/KDEWindowObserverBackend.cc
        Platform/Linux/ExtensionFile.cc
    )
    shijima_generate_resource("${SHIJIMA_GENERATED_DIR}/kwin_script.c" embed
        "${CMAKE_CURRENT_SOURCE_DIR}/Platform/Linux/kwin_script.js")
    shijima_generate_resource("${SHIJIMA_GENERATED_DIR}/gnome_script.c" gnome
        "${CMAKE_CURRENT_SOURCE_DIR}/Platform/Linux/gnome_script/extension.js"
        "${CMAKE_CURRENT_SOURCE_DIR}/Platform/Linux/gnome_script/metadata.json")
    # These are included by C++ sources, not compiled as independent C files.
    set_source_files_properties("${SHIJIMA_GENERATED_DIR}/kwin_script.c"
        "${SHIJIMA_GENERATED_DIR}/gnome_script.c" PROPERTIES HEADER_FILE_ONLY TRUE)
    target_sources(shijima-qt PRIVATE "${SHIJIMA_GENERATED_DIR}/kwin_script.c"
        "${SHIJIMA_GENERATED_DIR}/gnome_script.c")
endif()

if(WIN32)
    enable_language(RC)
    set_property(TARGET shijima-qt PROPERTY WIN32_EXECUTABLE TRUE)
    target_sources(shijima-qt PRIVATE resources.rc)
    target_link_libraries(shijima-qt PRIVATE user32)
elseif(APPLE)
    set_target_properties(shijima-qt PROPERTIES
        MACOSX_BUNDLE TRUE
        MACOSX_BUNDLE_INFO_PLIST "${CMAKE_CURRENT_SOURCE_DIR}/Shijima-Qt.app/Contents/Info.plist"
    )
    set(icon "Shijima-Qt.app/Contents/Resources/AppIcon.icns")
    set_source_files_properties("${icon}" PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
    target_sources(shijima-qt PRIVATE "${icon}")
endif()
