#include "Task.h"

$execute {
    (void)Mod::get()->hook(
        reinterpret_cast<void *>(addresser::getNonVirtual(&utils::file::pick)),
        &pickHook, "utils::file::pick"
    );
    (void)Mod::get()->hook(
        reinterpret_cast<void *>(addresser::getNonVirtual(&utils::file::pickMany)),
        &pickManyHook, "utils::file::pickMany"
    );
}

class $modify(PickerTestLayer, MenuLayer) {
    struct Fields {
        EventListener<PickTask> m_fileTaskEvent;
    };
    
    bool init() {
        // Initialize the menu layer
        if (!MenuLayer::init()) return false;

        auto myButton = CCMenuItemSpriteExtra::create(
            CCSprite::createWithSpriteFrameName("GJ_likeBtn_001.png"), 
            this, 
            menu_selector(PickerTestLayer::onButton)
        );

        auto menu = this->getChildByID("bottom-menu");
        menu->addChild(myButton);
        menu->updateLayout();

        return true;
    }

    void onButton(CCObject *) {
        m_fields->m_fileTaskEvent.setFilter(utils::file::pick(file::PickMode::OpenFile, {}));
        m_fields->m_fileTaskEvent.bind([](PickTask::Event * event) {
            if (event->isCancelled()) {
                log::info("shitass");
                queueInMainThread([] {
                    FLAlertLayer::create(
                        "Title", "cancelled", "OK"
                    )->show();
                });
            }
            if (auto * value = event->getValue()) {
                std::string str;
                if (value->isOk()) {
                    str = value->unwrap().generic_string();
                } else {
                    str = value->unwrapErr();
                }
                queueInMainThread([str] {
                    FLAlertLayer::create(
                        "Title", str, "OK"
                    )->show();
                });
            }
        });
    }
};
