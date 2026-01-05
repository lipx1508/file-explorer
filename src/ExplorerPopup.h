#pragma once

/* -========================================================
    Includes
   ========================================================- */

#include <cue/ListNode.hpp>

#include "Utils.h"
#include "Task.h"

/* -========================================================
    State popup
   ========================================================- */

struct State {
    std::function<void(PickTaskRet)> file;
    std::function<void(PickManyTaskRet)> files;
    std::function<bool()> cancelled;
};

/* -========================================================
    File popup
   ========================================================- */

class ExplorerEntryCell;

// The popup popup
class ExplorerPopup : public geode::Popup<std::string const &> {
public:
    // Creates the file popup as a global instance and cancels the previous in case it was open
    static ExplorerPopup * create(PickerMode mode, geode::utils::file::FilePickOptions options);
    
    // Gets the global instance of this popup
    static ExplorerPopup * get();
    
    // Closes the global popup
    static void close();
    
    // Changes the current directory
    void changeDir(std::filesystem::path path, geode::utils::file::FilePickOptions options = {});
    
    // Marks a file cell as selected
    void selectCell(ExplorerEntryCell * cell);
    
    // Unmarks a file cell as unselected
    void unselectCell(ExplorerEntryCell * cell);
    
    // Returns a normalized list with the paths that were picked
    Result<std::vector<std::filesystem::path>> getFiles();
    
    // Returns a normalized list with the path that was picked
    Result<std::filesystem::path> getFile();
    
    // The current state of the dialog
    State m_state;
    
    // The popup mode, translation from the geode alt.
    PickerMode m_mode;
protected:
    bool setup(std::string const & value) override;
private:
    void setupFrame();
    void setupTop();
    void setupCenter();
    void setupBottom();
    ExplorerEntryCell * setupFileList(std::filesystem::path possiblePath);

    void refresh();
    
    // Refreshes the file list
    void onRefresh(CCObject *);
    
    // Searches for 
    void onSearch(CCObject *);
    void onReset(CCObject *);
    void onParent(CCObject *);
    void onHome(CCObject *);
    void onUnselectAll(CCObject *);
    void onFilter(CCObject *);
    
    void onClose(CCObject *) override;
    void keyBackClicked() override;
    void onFinish(CCObject *);
    
    // Current path the popup is in
    std::filesystem::path m_currentPath;
    
    // File picker Options
    file::FilePickOptions m_options;
    
    // Input to change the current path
    TextInput * m_pathInput = nullptr;
    
    // Input to select a file name
    TextInput * m_fileName = nullptr;
    
    // Changes every time a new file is picked
    // TODO: remove this and use m_selection with a single item instead
    ExplorerEntryCell * m_lastPick = nullptr;
    
    // Current files selected
    std::set<ExplorerEntryCell *> m_selection;
    
    // The file list
    cue::ListNode * m_fileList = nullptr;
    
    // The OK button
    CCMenuItemSpriteExtra * m_okButton;
    
    // The file sorting mode
    SortMode m_sortMode = SortMode::NAME;
    
    // If the setup has finished at least once
    bool m_setup = false;
};
