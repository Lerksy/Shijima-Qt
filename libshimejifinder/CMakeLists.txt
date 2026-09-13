add_subdirectory(unarr EXCLUDE_FROM_ALL)

# Libarchive and unarr both contain the PPMd implementation from the LZMA SDK.
# Rename unarr's copy so the static libraries can be linked together.
foreach(symbol IN ITEMS
        Ppmd8_Construct Ppmd8_Alloc Ppmd8_Free Ppmd8_Init
        Ppmd8_Update1 Ppmd8_Update1_0 Ppmd8_Update2 Ppmd8_UpdateModel
        Ppmd8_MakeEscFreq Ppmd8_Init_RangeDec Ppmd8_DecodeSymbol)
    target_compile_definitions(unarr PRIVATE "${symbol}=shijima_unarr_${symbol}")
endforeach()

add_library(shimejifinder STATIC
    shimejifinder/libarchive/archive.cc
    shimejifinder/libunarr/unarr_FILE.c
    shimejifinder/libunarr/archive.cc
    shimejifinder/analyze.cc
    shimejifinder/archive_folder.cc
    shimejifinder/archive.cc
    shimejifinder/archive_entry.cc
    shimejifinder/extract_target.cc
    shimejifinder/extractor.cc
    shimejifinder/fs_extractor.cc
    shimejifinder/memory_extractor.cc
    shimejifinder/utf8_convert/jni.cc
    shimejifinder/utf8_convert/icu.cc
    shimejifinder/utf8_convert/iconv.cc
    shimejifinder/utils.cc
)

if(SHIMEJIFINDER_USE_CUSTOM_UTF8_CONVERT)
    # consumer of libshimejifinder must provide shimejifinder::is_valid_utf8
    # and shimejifinder::shift_jis_to_utf8
    target_compile_definitions(shimejifinder PUBLIC SHIMEJIFINDER_USE_CUSTOM_UTF8_CONVERT=1
        SHIMEJIFINDER_HAS_UTF8_CONVERT=1)
elseif(SHIMEJIFINDER_USE_ICU)
    find_package(ICU COMPONENTS uc REQUIRED)
    target_link_libraries(shimejifinder ICU::uc)
    target_compile_definitions(shimejifinder PUBLIC SHIMEJIFINDER_USE_ICU=1
        SHIMEJIFINDER_HAS_UTF8_CONVERT=1)
elseif(SHIMEJIFINDER_USE_ICONV)
    find_package(Iconv REQUIRED)
    target_link_libraries(shimejifinder Iconv::Iconv)
    target_compile_definitions(shimejifinder PUBLIC SHIMEJIFINDER_USE_ICONV=1
        SHIMEJIFINDER_HAS_UTF8_CONVERT=1)
elseif(SHIMEJIFINDER_USE_JNI)
    find_package(JNI REQUIRED)
    target_link_libraries(shimejifinder JNI::JNI JNI::JVM)
    target_include_directories(shimejifinder PUBLIC ${JNI_INCLUDE_DIRS})
    target_compile_definitions(shimejifinder PUBLIC SHIMEJIFINDER_USE_JNI=1
        SHIMEJIFINDER_HAS_UTF8_CONVERT=1)
elseif(ANDROID AND (SHIMEJIFINDER_USE_JNI OR NOT DEFINED SHIMEJIFINDER_USE_JNI))
    target_link_libraries(shimejifinder android)
    target_compile_definitions(shimejifinder PUBLIC SHIMEJIFINDER_USE_JNI=1
        SHIMEJIFINDER_HAS_UTF8_CONVERT=1)
else()
    target_compile_definitions(shimejifinder PUBLIC SHIMEJIFINDER_HAS_UTF8_CONVERT=0)
endif()

target_include_directories(shimejifinder PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
target_link_libraries(shimejifinder PUBLIC
    LibArchive::LibArchive
    pugixml::pugixml
    unarr
)
if(MSVC)
    target_compile_options(shimejifinder PRIVATE /utf-8)
    target_compile_definitions(shimejifinder PRIVATE _CRT_SECURE_NO_WARNINGS NOMINMAX)
else()
    target_compile_options(shimejifinder PRIVATE -Wall -Wextra -Werror -Wpedantic)
endif()
