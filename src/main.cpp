#include <Geode/Geode.hpp>
#include <Geode/modify/MenuLayer.hpp>
#include "ModsPopup.cpp"

using namespace geode::prelude;

// --- MenuLayer modification ---
class $modify(MyMenuLayer, MenuLayer) {
    bool init() {
        log::info("MyMenuLayer: init called");

        if (!MenuLayer::init()) {
            log::info("MyMenuLayer: MenuLayer::init failed");
            return false;
        }

        log::info("MyMenuLayer: creating mods button");

        // Create mods button
        auto myButton = CCMenuItemSpriteExtra::create(
            CCSprite::createWithSpriteFrameName("GJ_optionsBtn02_001.png"),
            this,
            menu_selector(MyMenuLayer::onMyButton)
        );
        myButton->setID("mods-button"_spr);

        // Find bottom menu
        auto menu = this->getChildByID("bottom-menu");
        if (!menu) {
            log::info("MyMenuLayer: bottom-menu node not found");
        } else {
            log::info("MyMenuLayer: bottom-menu found, adding button");
            menu->addChild(myButton);
            menu->updateLayout();
            log::info("MyMenuLayer: button added and layout updated");
        }

        return true;
    }

    void onMyButton(CCObject*) {
        log::info("MyMenuLayer: mods button pressed");
        ModsPopup::showPopup();
    }
};

// --- ModsLayer modification ---
class $modify(MyModsLayer, ModsLayer) {
    bool init(ModListType type) {
        log::info("MyModsLayer: init called");

        if (!ModsLayer::init(type)) {
            log::info("MyModsLayer: ModsLayer::init failed");
            return false;
        }

        log::info("MyModsLayer: creating mods button for action-menu");

        // Create mods button
        auto myButton = CCMenuItemSpriteExtra::create(
            CCSprite::createWithSpriteFrameName("GJ_optionsBtn02_001.png"),
            this,
            menu_selector(MyModsLayer::onMyButton)
        );
        myButton->setID("mods-button"_spr);

        // Find the action-menu in ModsLayer
        auto actionMenu = this->getChildByID("action-menu");
        if (!actionMenu) {
            log::info("MyModsLayer: action-menu node not found");
        } else {
            log::info("MyModsLayer: action-menu found, adding button");
            actionMenu->addChild(myButton);
            actionMenu->updateLayout();
            log::info("MyModsLayer: button added and layout updated");
        }

        return true;
    }

    void onMyButton(CCObject*) {
        log::info("MyModsLayer: mods button pressed");
        ModsPopup::showPopup();
    }
};