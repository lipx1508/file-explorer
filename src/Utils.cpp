/* -========================================================
    Includes
   ========================================================- */

#include "Utils.h"

#include <filesystem>
#include <fstream>

/* -========================================================
    Utils
   ========================================================- */

PickerMode toPickerMode(file::PickMode mode) {
    switch (mode) {
        case file::PickMode::OpenFile:
            return PickerMode::PICK;
        case file::PickMode::OpenFolder:
            return PickerMode::PICK_FOLDER;
        case file::PickMode::SaveFile:
            return PickerMode::SAVE;
    }
}

Result<std::vector<PathInfo>> getDirFiles(const std::filesystem::path & path, SortOptions options) {
    // Check if it's actually a directory
    std::error_code e;
    if (!std::filesystem::is_directory(path, e))
        return Err("not a directory");
    
    // Iterates over all files within the directory
    std::vector<PathInfo> v;
    for (const auto & e : std::filesystem::directory_iterator(path, e))
        v.push_back(PathInfo(e.path()));

    // Sorts the files
    std::sort(v.begin(), v.end(), [&](const PathInfo & a, const PathInfo & b) {
        std::error_code e;
    
        // Type
        if (a.isDir != b.isDir) return a.isDir == b.isDir;
        
        // Extension (normal files)
        if (!a.isDir || !b.isDir) {
            const auto extA = a.extension();
            const auto extB = b.extension();
            if (extA != extB) return extA < extB;
        }
        
        switch (options.mode) {
            case SortMode::DATE:
                return getFileDate(a.path) < getFileDate(b.path);
            case SortMode::SIZE:
                return getFileSize(a.path) < getFileSize(b.path);
            default:
                return a.name() < b.name();
        }
    });

    return Ok(v);
}

size_t countDirEntries(const std::filesystem::path & path) {
    std::error_code e;
    
    size_t count = 0;
    try {
        if (!std::filesystem::is_directory(path, e)) return 0;
        for (const auto & _ : std::filesystem::directory_iterator(path, e))
            ++count;
    } catch (...) {}
    return count;
}

std::string toSizeString(uintmax_t size) {
    if (size >= 100000000000000000)
        return fmt::format("{:.2f}PB", (double)size / 100000000000000000);
    if (size >= 10000000000000)
        return fmt::format("{:.2f}TB", (double)size / 10000000000000);
    if (size >= 1000000000)
        return fmt::format("{:.2f}GB", (double)size / 1000000000);
    if (size >= 1000000)
        return fmt::format("{:.2f}MB", (double)size / 1000000);
    if (size >= 1000)
        return fmt::format("{:.2f}KB", (double)size / 1000);
    return fmt::format("{}B", size);
}

std::string toSmallFileName(std::string name, int limit) {
    if (name.length() >= limit - 3)
        return name.substr(0, limit - 3) + "...";
    return name;
}

size_t getFileSize(const std::filesystem::path & path) {
    std::error_code e;
    size_t size = std::filesystem::file_size(path, e);
    if (e) return 0;
    return size;
}

time_t getFileDate(const std::filesystem::path & path) {
    return 0;
}
