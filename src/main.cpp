#include <Geode/Geode.hpp>
#include <Geode/modify/MenuLayer.hpp>
#include "ModsPopup.cpp"

using namespace geode::prelude;

class $modify(MyMenuLayer, MenuLayer) {
    bool init() {
        log::info("MyMenuLayer: init called");

        if (!MenuLayer::init()) {
            log::info("MyMenuLayer: MenuLayer::init failed");
            return false;
        }

        log::info("MyMenuLayer: creating mods button");

        auto circle = CircleButtonSprite::create(
            CCSprite::create("textures/button.png"),
            CircleBaseColor::Green,
            CircleBaseSize::MediumAlt
        );

        if (auto inner = circle->getChildByType<CCSprite>(0)) {
            inner->setScale(0.85f);
        }

        auto myButton = CCMenuItemSpriteExtra::create(
            circle,
            this,
            menu_selector(MyMenuLayer::onMyButton)
        );

        auto menu = this->getChildByID("bottom-menu");
        if (!menu)
            log::info("MyMenuLayer: bottom-menu node not found");
        else
            log::info("MyMenuLayer: bottom-menu found, adding button");

        menu->addChild(myButton);
        myButton->setID("mods-button"_spr);

        menu->updateLayout();
        log::info("MyMenuLayer: button added and layout updated");

        return true;
    }

    void onMyButton(CCObject*) {
        log::info("MyMenuLayer: mods button pressed");
        ModsPopup::showPopup();
    }
};