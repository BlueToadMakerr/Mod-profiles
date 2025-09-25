#include <Geode/Geode.hpp>
#include <Geode/modify/MenuLayer.hpp>
#include "ModsPopup.cpp"

using namespace geode::prelude;

class $modify(MyMenuLayer, MenuLayer) {
    bool init() {
        if (!MenuLayer::init()) return false;

        // Create our button using the desired sprite
        auto myButton = CCMenuItemSpriteExtra::create(
            CCSprite::createWithSpriteFrameName("GJ_optionsBtn02_001.png"),
            this,
            menu_selector(MyMenuLayer::onMyButton)
        );

        // Access the existing bottom-menu node by its ID
        auto menu = this->getChildByID("bottom-menu");
        menu->addChild(myButton);

        // Give our button a unique ID (namespaced with your mod id)
        myButton->setID("mods-button"_spr);

        // Recalculate menu layout so our button sits correctly at the bottom
        menu->updateLayout();

        return true;
    }

    void onMyButton(CCObject*) {
        // Show the mods popup we made earlier
        ModsPopup::showPopup();
    }
};