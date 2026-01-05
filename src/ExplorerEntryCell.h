#pragma once

/* -========================================================
    Includes
   ========================================================- */

#include "Utils.h"

/* -========================================================
    File cell
   ========================================================- */

class ExplorerPopup;

class ExplorerEntryCell : public CCLayer {
public:
    static ExplorerEntryCell * create(PathInfo path, ExplorerPopup * parent);
    
    void toggle(bool toggle);
    
    PathInfo m_path;
protected:
    bool init();
    
    void onExpand(CCObject * o);
    void onSelect(CCMenuItemToggler * o);
    void onCopy(CCMenuItemToggler * o);
    void onDelete(CCMenuItemToggler * o);
private:
    ExplorerPopup * m_parent = nullptr;
    CCMenuItemToggler * m_toggle = nullptr;
};
