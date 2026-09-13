add_library(unarr STATIC
    _7z/_7z.c
    common/conv.c
    common/crc32.c
    common/stream.c
    common/unarr.c
    lzmasdk/7zArcIn.c
    lzmasdk/7zBuf.c
    lzmasdk/7zDec.c
    lzmasdk/7zStream.c
    lzmasdk/Bcj2.c
    lzmasdk/Bra.c
    lzmasdk/Bra86.c
    lzmasdk/CpuArch.c
    lzmasdk/Delta.c
    lzmasdk/Lzma2Dec.c
    lzmasdk/LzmaDec.c
    lzmasdk/Ppmd7.c
    lzmasdk/Ppmd7Dec.c
    lzmasdk/Ppmd7aDec.c
    lzmasdk/Ppmd8.c
    lzmasdk/Ppmd8Dec.c
    rar/filter-rar.c
    rar/huffman-rar.c
    rar/parse-rar.c
    rar/rar.c
    rar/rarvm.c
    rar/uncompress-rar.c
    tar/parse-tar.c
    tar/tar.c
    zip/inflate.c
    zip/parse-zip.c
    zip/uncompress-zip.c
    zip/zip.c
)

target_include_directories(unarr PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
target_link_libraries(unarr PRIVATE
    BZip2::BZip2
    LibLZMA::LibLZMA
    ZLIB::ZLIB
)
target_compile_definitions(unarr PRIVATE
    HAVE_7Z
    HAVE_BZIP2
    HAVE_LIBLZMA
    HAVE_ZLIB
    Z7_PPMD_SUPPORT
    _FILE_OFFSET_BITS=64
)
set_target_properties(unarr PROPERTIES
    C_STANDARD 99
    C_STANDARD_REQUIRED ON
    C_EXTENSIONS OFF
)

if(MSVC)
    target_compile_options(unarr PRIVATE /W3)
    target_compile_definitions(unarr PRIVATE _CRT_SECURE_NO_WARNINGS)
else()
    target_compile_options(unarr PRIVATE
        -Wall
        -Wextra
        -Wpedantic
        -Wstrict-prototypes
        -Wmissing-prototypes
        -Werror=implicit-function-declaration
    )
endif()
