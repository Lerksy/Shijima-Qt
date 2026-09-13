#pragma once

// 
// libshimejifinder - library for finding and extracting shimeji from archives
// Copyright (C) 2025 pixelomer
// 
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.
// 

#if !SHIMEJIFINDER_NO_LIBARCHIVE

#include "../archive.hpp"
#include <archive.h>

struct archive_entry;
struct archive;
#endif

namespace shimejifinder {
namespace libarchive {

class archive : public shimejifinder::archive {
private:
    class nested_context {
    private:
        ::archive *parent;
        la_ssize_t offset;
        ::archive *ar;
        std::vector<uint8_t> buf;
        bool read(la_int64_t *size, const void **out_buf);
        static int close_callback(::archive *ar, void *data);
        static la_int64_t skip_callback(::archive *ar, void *data, la_int64_t skip);
        static la_ssize_t read_callback(::archive *ar, void *data, const void **buf);
        static int open_callback(::archive *ar, void *data);
    public:
        nested_context(::archive *parent);
        ::archive *archive();
    };

    static std::string get_error(::archive *ar);
    bool read_data(::archive *ar, std::function<bool (long, const void *, size_t)> cb);
    bool read_data(::archive *ar, std::ostream &out, size_t max_size = SIZE_MAX);
    bool try_recurse(int &idx, ::archive *, ::archive_entry *, std::string const& pathname,
        std::function<void (int, ::archive *, std::string const&)> &cb);
    void iterate_archive(std::function<void (int, ::archive *,
        std::string const&)> cb);
    void iterate_archive(::archive *ar, int &idx, std::string const& root,
        std::function<void (int, ::archive *, std::string const&)> &cb);
    int archive_open(::archive *ar);
protected:
    void fill_entries() override;
    void extract() override;
};

}
}
