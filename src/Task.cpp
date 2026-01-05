/* -========================================================
    Includes
   ========================================================- */

#include "Task.h"
#include "ExplorerPopup.h"

/* -========================================================
    Internal
   ========================================================- */

PickTask pickHook(file::PickMode mode, const file::FilePickOptions & options) {
    PickerMode popupMode = toPickerMode(mode);
    
    auto * popup = ExplorerPopup::create(popupMode, options);
    popup->show();
    return PickTask::runWithCallback([&](auto result, auto, auto cancelled) {
        auto * popup = ExplorerPopup::get();
        popup->m_state.file = result;
        popup->m_state.cancelled = cancelled;
    }, "File picker popup task");
}

PickManyTask pickManyHook(const file::FilePickOptions & options) {
    ExplorerPopup::create(PickerMode::PICK_MANY, options)->show();
    return PickManyTask::runWithCallback([&](auto result, auto, auto cancelled) {
        auto * popup = ExplorerPopup::get();
        if (!popup) return;
        popup->m_state.files = result;
        popup->m_state.cancelled = cancelled;
    }, "File picker (many) popup task");
}
