# The pinned upstream builds use GCC-only flags and generate files with Bash
# inside their source trees. Define their targets here so the submodules stay
# unmodified and all generators/toolchains use the same build graph.
set(finder_root "${PROJECT_SOURCE_DIR}/libshimejifinder")

# Keep unarr static: no platform-specific DLL naming/copying or runtime RPATH
# is needed for this bundled dependency. Its own install rules are excluded.
set(BUILD_SHARED_LIBS OFF)
set(CMAKE_POLICY_DEFAULT_CMP0077 NEW)
set(CMAKE_POLICY_VERSION_MINIMUM 3.5)
set(BUILD_INTEGRATION_TESTS OFF)
set(BUILD_UNIT_TESTS OFF)
set(BUILD_FUZZER OFF)
# The upstream build checks the legacy Find-module variable names. Conan's
# config packages provide imported targets instead; both packages are required
# by the root project so these compatibility flags are always valid here.
set(BZIP2_FOUND TRUE)
set(LIBLZMA_FOUND TRUE)

# Static libarchive also defines Ppmd8_* symbols. Prefix unarr's internal
# implementation, including its callers, so both codecs can coexist.
foreach(symbol IN ITEMS
        Ppmd8_Construct Ppmd8_Alloc Ppmd8_Free Ppmd8_Init
        Ppmd8_Update1 Ppmd8_Update1_0 Ppmd8_Update2 Ppmd8_UpdateModel
        Ppmd8_MakeEscFreq Ppmd8_Init_RangeDec Ppmd8_DecodeSymbol)
    target_compile_definitions(unarr PRIVATE "${symbol}=shijima_unarr_${symbol}")
endforeach()

shijima_generate_resource("${SHIJIMA_GENERATED_DIR}/default_actions.cc" embed "${finder_root}/default_actions.xml")
shijima_generate_resource("${SHIJIMA_GENERATED_DIR}/default_behaviors.cc" embed "${finder_root}/default_behaviors.xml")
set_source_files_properties("${SHIJIMA_GENERATED_DIR}/default_actions.cc"
    "${SHIJIMA_GENERATED_DIR}/default_behaviors.cc" PROPERTIES HEADER_FILE_ONLY TRUE)

# archive.cc includes an unused POSIX header and relies on a transitive fstream
# include. Adapt only the build copy, preserving the pinned submodule checkout.
configure_file("${finder_root}/shimejifinder/archive.cc" "${CMAKE_CURRENT_BINARY_DIR}/archive.cc.in" COPYONLY)
file(READ "${CMAKE_CURRENT_BINARY_DIR}/archive.cc.in" archive_source)
string(REPLACE "#include <unistd.h>" "#include <fstream>" archive_source "${archive_source}")
file(CONFIGURE OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/archive.cc" CONTENT "${archive_source}" @ONLY)

set(finder_archive_source "${finder_root}/shimejifinder/libarchive/archive.cc")
if(MSVC)
    # MSVC has no POSIX mode_t; use the actual type returned by libarchive.
    file(CONFIGURE OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/libarchive-compat.cc" CONTENT
        "#include <archive_entry.h>\nusing mode_t = decltype(archive_entry_filetype(nullptr));\n#include \"${finder_archive_source}\"\n" @ONLY)
    set(finder_archive_source "${CMAKE_CURRENT_BINARY_DIR}/libarchive-compat.cc")
endif()


target_include_directories(shimejifinder PUBLIC "${finder_root}"
    PRIVATE "${finder_root}/shimejifinder" "${SHIJIMA_GENERATED_DIR}")

if(MSVC)
    foreach(target IN ITEMS shijima shimejifinder)
        target_compile_options(${target} PRIVATE /utf-8)
        target_compile_definitions(${target} PRIVATE _CRT_SECURE_NO_WARNINGS NOMINMAX)
    endforeach()
endif()
