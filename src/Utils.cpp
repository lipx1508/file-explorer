/* -========================================================
    Includes
   ========================================================- */

#include "Utils.h"

#include <filesystem>

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
    if (!std::filesystem::is_directory(path))
        return Err("not a directory");
    
    // Iterates over all files within the directory
    std::vector<PathInfo> v;
    for (const auto & e : std::filesystem::directory_iterator(path))
        v.push_back(PathInfo(e.path()));

    // Sorts the files
    std::sort(v.begin(), v.end(), [&](const PathInfo & a, const PathInfo & b) {
        constexpr auto rank = [](const PathInfo & f) {
            const auto status = std::filesystem::status(f.path);
            if (std::filesystem::is_directory(status))    return 0;
            if (std::filesystem::is_regular_file(status)) return 1;
            return 2;
        };
    
        // Type
        const int typeA = rank(a);
        const int typeB = rank(b);

        if (typeA != typeB) return typeA < typeB;
            
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
                return std::filesystem::file_size(a.path) < std::filesystem::file_size(b.path);
            default:
                return a.name() < b.name();
        }
    });

    return Ok(v);
}

size_t countDirEntries(const std::filesystem::path & path) {
    size_t count = 0;
    try {
        if (!std::filesystem::is_directory(path)) return 0;
        for (const auto & _ : std::filesystem::directory_iterator(path))
            ++count;
    } catch (...) {}
    return count;
}

std::string toSizeString(uintmax_t size) {
    if (size >= 100000000000000000)
        return std::format("{:.2f}PB", (double)size / 100000000000000000);
    if (size >= 10000000000000)
        return std::format("{:.2f}TB", (double)size / 10000000000000);
    if (size >= 1000000000)
        return std::format("{:.2f}GB", (double)size / 1000000000);
    if (size >= 1000000)
        return std::format("{:.2f}MB", (double)size / 1000000);
    if (size >= 1000)
        return std::format("{:.2f}KB", (double)size / 1000);
    return std::format("{}B", size);
}

std::string toSmallFileName(std::string name) {
    if (name.length() >= 13)
        return name.substr(0, 13) + "...";
    return name;
}

time_t getFileDate(const std::filesystem::path & path) {
    return 0;
}
