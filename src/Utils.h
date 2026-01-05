#pragma once

/* -========================================================
    Includes
   ========================================================- */

#include <Geode/Geode.hpp>
#include <Geode/modify/MenuLayer.hpp>

using namespace geode::prelude;
using namespace cocos2d;

#include <string>
#include <filesystem>

/* -========================================================
    Utilities
   ========================================================- */

// namespace filepicker {
    // Sorting mode
    enum class SortMode {
        DATE = 0, 
        SIZE, 
        NAME, 
    };
    
    // Sorting options
    struct SortOptions {
        // The order that files can be sorted on
        SortMode mode = SortMode::DATE;
        
        // If the order is inverted or not
        bool inverted = false;
        
        // Shows hidden files (. prefix or OS-level)
        bool showHidden = false;
        
        // Removes any file that does not have said extensions
        std::string extFilter;
    };
    
    // The mode of the picker
    enum class PickerMode {
        PICK = 0,    // Allows you to pick only one file
        PICK_MANY,   // Allows you to pick more than one file
        PICK_FOLDER, // Allows you to pick a folder
        SAVE,        // Only saves files
    };
    
    // Picker state
    struct PickerState {
        PickerMode mode;
        SortOptions options;
    };
    
    // The type of file
    enum class FileTypeInfo {
        OTHER = 0, 
        TEXT, 
        IMAGE, 
        MUSIC, 
        ZIP, 
        GMD, 
        GEODE, 
    };
    
    enum class FolderTypeInfo {
        OTHER = 0, 
        SYMLINK, 
    };
    
    // Path information
    struct PathInfo {
        PathInfo() = default;
        PathInfo(std::filesystem::path m_path) : 
            path(m_path), isDir(std::filesystem::is_directory(m_path))
        {}
        
        constexpr std::string name() const {
            if (isDir) return path.filename().generic_string();
            return path.stem().generic_string();
        }
        
        constexpr std::string extension() const {
            if (isDir) return "";
            return path.extension().generic_string();
        }
        
        std::filesystem::path path;
        bool isDir = false;
    };
// }

/* -========================================================
    Utilities
   ========================================================- */

// namespace filepicker::utils {
    /* -========================================================
        Other utils
       ========================================================- */

    // Converts the pick or pickMany callback into a usable one
    PickerMode toPickerMode(file::PickMode mode);
    
    // Reads and sorts a directory
    Result<std::vector<PathInfo>> getDirFiles(const std::filesystem::path & path, SortOptions options);
    
    // Counts the amount of files inside a directory
    size_t countDirEntries(const std::filesystem::path & path);
    
    // Turns a size string from bytes to a human readable unit
    std::string toSizeString(uintmax_t size);
    
    // Makes a file name 16 characters max.
    std::string toSmallFileName(std::string name);
    
    // Gets the last date that a file was written to
    time_t getFileDate(const std::filesystem::path & path);
// }
