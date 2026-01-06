/* -========================================================
    Includes
   ========================================================- */

#include "ExplorerPopup.h"
#include "ExplorerEntryCell.h"

/* -========================================================
    Constants
   ========================================================- */

const auto HOME_PATH = dirs::getGameDir();
const char * PATH_FILTER = "!\\\"#$%&\'()*+,-./:;<=>?@[]^_`{|}~AaBbCcDdEeFfGgHhIiJjKkLlMmNnOoPpQqRrSsTtUuVvWwXxYyZz0123456789";

/* -========================================================
    Internals
   ========================================================- */

// Reads a directory and returns an error if it doesn't exist
static std::vector<PathInfo> readDir(std::filesystem::path path, SortOptions options, bool ignoreDiag) {
    auto files = getDirFiles(path, options);
    if (!files) {
        if (ignoreDiag) return {};
        
        FLAlertLayer::create(
            files.unwrapErr().c_str(),
            "Please check the <cr>name spelling</c> and try again", "OK"
        )->show();
        return {};
    }
    return files.unwrap();
}

// Global popup
static ExplorerPopup * g_popup = nullptr;

/* -========================================================
    File popup
   ========================================================- */

bool ExplorerPopup::setup(std::string const & value) {
    m_buttonMenu->ignoreAnchorPointForPosition(false);
    
    m_currentPath = (m_options.defaultPath ? m_options.defaultPath.value().parent_path() : HOME_PATH);
    if (m_currentPath.empty()) m_currentPath = HOME_PATH;
    
    setupFrame();
    setupTop();
    setupCenter();
    setupBottom();
    
    // The file will be picked
    ExplorerEntryCell * interestCell = setupFileList(m_options.defaultPath ? m_options.defaultPath.value() : "");
    if (interestCell) {
        selectCell(interestCell);
    }
    
    if (m_fileName) {
        m_fileName->setString(m_options.defaultPath ? utils::string::pathToString(m_options.defaultPath.value().filename()) : "");
    }
    
    m_setup = true;
    
    this->setID("explorer-popup"_spr);
    
    return true;
}

void ExplorerPopup::setupFrame() {
    // Adds the refresh button
    auto refreshSprite = CCSprite::createWithSpriteFrameName("GJ_updateBtn_001.png");
    refreshSprite->setScale(0.8f);
    auto refreshButton = CCMenuItemSpriteExtra::create(
        refreshSprite, this, menu_selector(ExplorerPopup::onRefresh)
    );
    m_buttonMenu->addChildAtPosition(refreshButton, Anchor::TopRight, { -3.f, -3.f });
}

void ExplorerPopup::setupTop() {
    // Setup menu
    auto menu = CCMenu::create();
    menu->setContentSize({ 350.f, 30.f });
    menu->setAnchorPoint({ .5f, .5f });
    menu->setPosition({ 325.f, getContentHeight() / 2.f });
    menu->ignoreAnchorPointForPosition(false);
    menu->setLayout(
        RowLayout::create()
            ->setGap(10.f)
            ->setDefaultScaleLimits(.0001f, 1.f)
            ->setAxisReverse(true)
    );

    // Adds the path input
    m_pathInput = TextInput::create(300.f, "Insert directory...", "chatFont.fnt");
    m_pathInput->setFilter(PATH_FILTER);
    m_pathInput->setMaxCharCount(256); // the usual maximum at least for unix is 256 so we using that idc
    menu->addChildAtPosition(m_pathInput, Anchor::Center, { 0.f, -30.f });
    
    // Adds the search button
    auto searchSprite = CCSprite::createWithSpriteFrameName("gj_findBtn_001.png");
    searchSprite->setScale(1.f);
    auto searchButton = CCMenuItemSpriteExtra::create(
        searchSprite, this, menu_selector(ExplorerPopup::onSearch)
    );
    menu->addChildAtPosition(searchButton, Anchor::Right, { 0.f, -30.f });
    
    // Adds the parent button
    auto parentSprite = IconButtonSprite::create(
        "GJ_button_01.png", 
        CCSprite::createWithSpriteFrameName("edit_upBtn_001.png"), 
        "", "chatFont.fnt"
    );
    parentSprite->setContentSize({ 40.f, 40.f });
    parentSprite->setScale(.8f);
    auto parentButton = CCMenuItemSpriteExtra::create(
        parentSprite, this, menu_selector(ExplorerPopup::onParent)
    );
    parentButton->setContentSize({ 40.f, 40.f });
    parentButton->updateSprite();
    menu->addChildAtPosition(parentButton, Anchor::Left, { 0.f, -30.f });
    
    m_buttonMenu->addChildAtPosition(menu, Anchor::Top);
}

void ExplorerPopup::setupCenter() {
#ifdef DEBUG
    // Adds the filter button
    auto filterSprite = IconButtonSprite::create(
        "GJ_button_01.png", 
        CCSprite::createWithSpriteFrameName("GJ_filterIcon_001.png"), 
        "", "chatFont.fnt"
    );
    filterSprite->setContentSize({ 40.f, 40.f });
    filterSprite->setScale(.8f);
    auto filterButton = CCMenuItemSpriteExtra::create(
        filterSprite, this, menu_selector(ExplorerPopup::onParent)
    );
    filterButton->setContentSize({ 40.f, 40.f });
    filterButton->updateSprite();
    m_buttonMenu->addChildAtPosition(filterButton, Anchor::Left, { 30.f, 0.f });
#endif
}

void ExplorerPopup::setupBottom() {
    // Setup menu
    auto menu = CCMenu::create();
    menu->setContentSize({ 350.f, 30.f });
    menu->setAnchorPoint({ .5f, .5f });
    menu->setPosition({ 325.f, getContentHeight() / 2.f });
    menu->ignoreAnchorPointForPosition(false);
    menu->setLayout(
        RowLayout::create()
            ->setGap(10.f)
            ->setDefaultScaleLimits(.0001f, 1.f)
            ->setAxisReverse(true)
    );

    // Adds the file input
    if (m_mode != PickerMode::PICK_MANY) {
        m_fileName = TextInput::create(240.f, "File name", "chatFont.fnt");
        m_fileName->setFilter(PATH_FILTER);
        m_fileName->setMaxCharCount(256); // the usual maximum at least for unix is 256 so we using that idc
        menu->addChildAtPosition(m_fileName, Anchor::Center, { -30.f, 30.f });
    }
    
    // Confirm button
    auto okButtonSprite = ButtonSprite::create("OK");
    okButtonSprite->setScale(0.8f);
    m_okButton = CCMenuItemSpriteExtra::create(
        okButtonSprite, this, menu_selector(ExplorerPopup::onFinish)
    );
    if (m_mode != PickerMode::SAVE) {
        m_okButton->setEnabled(false);
        m_okButton->setColor({ 175, 175, 175 });
    }
    menu->addChildAtPosition(m_okButton, Anchor::Right, { -30.f, 30.f });
    
    m_buttonMenu->addChildAtPosition(menu, Anchor::Bottom);
}

ExplorerEntryCell * ExplorerPopup::setupFileList(std::filesystem::path possiblePath) {
    ExplorerEntryCell * ret = nullptr;
    
    m_pathInput->setString(utils::string::pathToString(m_currentPath));
    
    cue::resetNode(m_fileList);
    
    m_fileList = cue::ListNode::create({ 340.f, 180.f }, cue::DarkerBlue, cue::ListBorderStyle::CommentsBlue);
    m_fileList->setCellColors(ccColor4B(50, 50, 50, 255), ccColor4B(75, 75, 75, 255));
    m_fileList->setCellHeight(20.f);
    m_fileList->ignoreAnchorPointForPosition(false);
    
    auto files = readDir(m_currentPath, { .mode = m_sortMode }, !m_setup);
    for (size_t i = 0; i < files.size(); ++i) {
        auto cell = ExplorerEntryCell::create(files[i], this);
        m_fileList->addListCell(cue::ListCell::create(cell, m_fileList));
        
        if (!possiblePath.empty() && files[i].path == possiblePath)
            ret = cell;
    }
    
    m_fileList->setID("explorer-file-list"_spr);
    
    m_mainLayer->addChildAtPosition(m_fileList, geode::Anchor::Center);
    
    return ret;
}

void ExplorerPopup::changeDir(std::filesystem::path path, geode::utils::file::FilePickOptions options) {
    m_currentPath = path;
    m_options = options;
    refresh();
}

void ExplorerPopup::selectCell(ExplorerEntryCell * cell) {
    if (m_mode != PickerMode::PICK_MANY) {
        if (m_lastPick) {
            m_lastPick->toggle(false);
        }
        m_selection.clear();
    }
    
    if (m_fileName) {
        m_fileName->setString(utils::string::pathToString(cell->m_path.path.filename()));
    }
    m_selection.insert(cell);
    m_lastPick = cell;
    
    m_okButton->setEnabled(true);
    m_okButton->setColor({ 255, 255, 255 });
}

void ExplorerPopup::unselectCell(ExplorerEntryCell * cell) {
    m_lastPick = nullptr;
    m_selection.erase(cell);
    if (m_selection.empty()) {
        m_okButton->setEnabled(false);
        m_okButton->setColor({ 175, 175, 175 });
    }
}

Result<std::vector<std::filesystem::path>> ExplorerPopup::getFiles() {
    std::vector<std::filesystem::path> v;
    v.reserve(m_selection.size());
    for (const auto & p : m_selection) {
        try {
            v.push_back((m_currentPath / p->m_path.path).make_preferred());
        } catch (const std::exception & e) {
            return Err(e.what());
        }
    }
    return Ok(v);
}

Result<std::filesystem::path> ExplorerPopup::getFile() {
    std::filesystem::path v;
    if (m_fileName)
        v = (m_currentPath / std::filesystem::path(m_fileName->getString())).make_preferred();
    return Ok(v);
}

void ExplorerPopup::refresh() {
    m_lastPick = nullptr;
    m_selection.clear();
    setupFileList("");
}

void ExplorerPopup::onRefresh(CCObject *) {
    refresh();
}

void ExplorerPopup::onSearch(CCObject *) {
    std::error_code e;
    auto path = std::filesystem::weakly_canonical(m_pathInput->getString(), e);
    if (e) {
        Notification::create(e.message(), NotificationIcon::Error)->show();
        return;
    }
    changeDir(path, m_options);
}

void ExplorerPopup::onReset(CCObject *) {
    refresh();
}

void ExplorerPopup::onParent(CCObject *) {
    std::error_code e;
    auto path = std::filesystem::absolute(m_currentPath, e);
    if (e) return;
    changeDir(path.parent_path(), m_options);
}

void ExplorerPopup::onHome(CCObject *) {
    changeDir(HOME_PATH, m_options);
}

void ExplorerPopup::onUnselectAll(CCObject *) {
    m_selection.clear();
    refresh();
}

void ExplorerPopup::onFilter(CCObject *) {
    changeDir(HOME_PATH, m_options);
}

void ExplorerPopup::onClose(CCObject *) {
    m_state.cancelled();
    close();
}

void ExplorerPopup::keyBackClicked() {
    m_state.cancelled();
    close();
}

void ExplorerPopup::onFinish(CCObject *) {
    if (m_mode == PickerMode::SAVE) {
        auto v = getFile();
        if (v.isOk()) {
            auto path = v.unwrap();
            if (path.empty()) {
                Notification::create("Path is empty!", NotificationIcon::Error)->show();
                return;
            }
            
            std::error_code e;
            if (std::filesystem::exists(path, e)) {
                createQuickPopup(
                    "Confirmation", 
                    fmt::format("Are you sure you want to replace \"{}\"?", 
                        toSmallFileName(utils::string::pathToString(v.unwrap().filename()))
                    ), "No", "Yes", 
                    [](auto, bool btn2) {
                        if (!btn2) return;
                        auto * popup = ExplorerPopup::get();
                        popup->m_state.file(popup->getFile());
                        ExplorerPopup::close();
                    }
                );
                return;
            }
        }
    }
    if (m_mode == PickerMode::PICK_MANY) {
        m_state.files(getFiles());
    } else {
        m_state.file(getFile());
    }
    close();
}

void ExplorerPopup::close() {
    cue::resetNode(g_popup);
}

ExplorerPopup * ExplorerPopup::create(PickerMode mode, geode::utils::file::FilePickOptions options) {
    cue::resetNode(g_popup);

    auto ret = new ExplorerPopup();
    ret->m_mode = mode;
    ret->m_options = options;
    g_popup = ret;
    if (ret->initAnchored(440.f, 290.f, "", "GJ_square02.png")) {
        ret->autorelease();
        return ret;
    }

    delete ret;
    return nullptr;
}

ExplorerPopup * ExplorerPopup::get() {
    return g_popup;
}
