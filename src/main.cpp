#include <Geode/Geode.hpp>
#include <Geode/modify/MenuLayer.hpp>

using namespace geode::prelude;

class $modify(FlexibleGeneratorMenu, MenuLayer) {
    bool init() {
        if (!MenuLayer::init()) {
            return false;
        }

        auto menu = this->getChildByID("bottom-menu");
        if (!menu) {
            log::warn("Flexible Generator: bottom-menu was not found.");
            return true;
        }

        auto sprite = ButtonSprite::create("Generator");
        auto button = CCMenuItemSpriteExtra::create(
            sprite,
            this,
            menu_selector(FlexibleGeneratorMenu::onGeneratorButton)
        );
        button->setID("flexible-generator-button"_spr);
        menu->addChild(button);
        menu->updateLayout();
        return true;
    }

    void onGeneratorButton(CCObject*) {
        FLAlertLayer::create(
            "Flexible Generator",
            "Prototype loaded. The generation core is ready for the next "
            "integration stage; editor placement and playability testing "
            "are not implemented yet.",
            "OK"
        )->show();
    }
};
