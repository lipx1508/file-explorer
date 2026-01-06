/* -========================================================
    Includes
   ========================================================- */

#include "ExplorerEntryCell.h"
#include "ExplorerPopup.h"

#include <filesystem>
#include <format>
#include <fmt/chrono.h>

/* -========================================================
    File cell
   ========================================================- */

ExplorerEntryCell * ExplorerEntryCell::create(PathInfo path, ExplorerPopup * parent) {
    auto * ret = new ExplorerEntryCell();
    ret->m_parent = parent;
    ret->m_path   = path;

    if (ret->init()) {
        ret->autorelease();
        return ret;
    }
    delete ret;
    return nullptr;
}

bool ExplorerEntryCell::init() {
    const float TEXT_OFFX = (m_path.isDir ? 20.f : 0.f);
    const float SIZE_NORMAL = .6f;
    const float SIZE_SMALL = .4f;
    const float SIZE_TINY = .3f;
    
    setContentSize({ 340.f, 20.f });
    
    const float cellPosY = getContentHeight() / 2.f + 1.f;
    const auto  filename = toSmallFileName(m_path.name(), 32);
    
    auto nameText = CCLabelBMFont::create(filename.c_str(), "bigFont.fnt");
    nameText->setAnchorPoint({ .0f, .5f });
    nameText->setPosition({ TEXT_OFFX + 8.f, cellPosY });
    nameText->setScale(SIZE_SMALL);
    
    float nameX = TEXT_OFFX + nameText->getScaledContentWidth();
    
    const auto ext = m_path.extension();
    auto extText = CCLabelBMFont::create(ext.c_str(), "bigFont.fnt");
    extText->setAnchorPoint({ .0f, .5f });
    extText->setPosition({ nameX + 8.f, cellPosY - 1.f });
    extText->setScale(SIZE_TINY);
    
    nameX += extText->getScaledContentWidth();
    
    std::string infoString = "";
    
    if (m_path.isDir) {
        infoString += fmt::format("{} files", countDirEntries(m_path.path));
    } else {
        infoString += toSizeString(getFileSize(m_path.path));
    }
    
    auto infoText = CCLabelBMFont::create(infoString.c_str(), "chatFont.fnt");
    infoText->setAnchorPoint({ .0f, .5f });
    infoText->setPosition({ nameX + 11.f, cellPosY });
    infoText->setScale(SIZE_NORMAL);
    infoText->setColor(ccColor3B(190, 190, 190));

    auto menu = CCMenu::create();
    menu->setContentSize({ 340.f, 320.f });
    menu->setAnchorPoint({ .5f, .5f });
    menu->ignoreAnchorPointForPosition(false);
    menu->setLayout(
        RowLayout::create()
            ->setGap(10.f)
            ->setDefaultScaleLimits(.0001f, 1.f)
            ->setAxisReverse(true)
    );

    if (m_path.isDir) {
        // Expand directory sprite
        auto expandSprite = CCSprite::createWithSpriteFrameName("GJ_arrow_01_001.png");
        expandSprite->setScale(SIZE_SMALL);
        expandSprite->setFlipX(true);
        auto expandButton = CCMenuItemSpriteExtra::create(
            expandSprite, this, menu_selector(ExplorerEntryCell::onExpand)
        );
        menu->addChild(expandButton);
    
        // Folder icon sprite
        auto folderSprite = CCSprite::createWithSpriteFrameName("gj_folderBtn_001.png");
        folderSprite->setScale(SIZE_SMALL);
        addChildAtPosition(folderSprite, Anchor::Left, { 16.f, 0.f });
    }
    if ((m_parent->m_mode == PickerMode::PICK_FOLDER && m_path.isDir) || (m_parent->m_mode != PickerMode::PICK_FOLDER && !m_path.isDir)) {
        // Selection
        m_toggle = CCMenuItemExt::createTogglerWithStandardSprites(.4f, [&](CCMenuItemToggler * o) {
            if (!o || !m_parent) return;
            
            // For some reason this beautiful and totally awesome function returns THE OPPOSITE OF THE DAMN TOGGLE
            if (o->isToggled())
                m_parent->unselectCell(this);
            else
                m_parent->selectCell(this);
        });
        menu->addChild(m_toggle);
    }
    
    menu->updateLayout();

    addChildAtPosition(menu, Anchor::Right, { -16.f, 0.f });
    addChild(nameText);
    addChild(extText);
    if (infoText) addChild(infoText);
    
    return true;
}

void ExplorerEntryCell::onExpand(CCObject * o) {
    m_parent->changeDir(m_path.path);
}

void ExplorerEntryCell::toggle(bool toggle) {
    if (!m_toggle) return;
    m_toggle->toggle(toggle);
}

void ExplorerEntryCell::onCopy(CCMenuItemToggler * o) {

}

void ExplorerEntryCell::onDelete(CCMenuItemToggler * o) {

}
