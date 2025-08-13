#include <Geode/Geode.hpp>
#include <Geode/loader/Loader.hpp>
#include <Geode/ui/Popup.hpp>

using namespace geode::prelude;

class ModManagerPopup : public Popup<> {
protected:
    bool setup() override {
        this->setTitle("Denabler - Mod Manager");
        
        
        auto disableBtn = CCMenuItemSpriteExtra::create(
            ButtonSprite::create("Disable All", "goldFont.fnt", "GJ_button_01.png", 0.8f),
            this,
            menu_selector(ModManagerPopup::onDisableAll)
        );
        
        auto enableBtn = CCMenuItemSpriteExtra::create(
            ButtonSprite::create("Enable All", "goldFont.fnt", "GJ_button_01.png", 0.8f),
            this,
            menu_selector(ModManagerPopup::onEnableAll)
        );
        
        auto menu = CCMenu::create();
        menu->addChild(disableBtn);
        menu->addChild(enableBtn);
        menu->setLayout(RowLayout::create()->setGap(20));
        m_mainLayer->addChildAtPosition(menu, Anchor::Center, ccp(0, -20));
        
        return true;
    }
    
    void onDisableAll(CCObject*) {
        this->onClose(nullptr);
        
        auto allMods = Loader::get()->getAllMods();
        int disabledCount = 0;
        int failedCount = 0;
        
        bool disableSelf = Mod::get()->getSettingValue<bool>("disable-self");
        
        for (auto mod : allMods) {
            if (mod->isInternal() || !mod->isOrWillBeEnabled()) {
                continue;
            }
            
            if (mod->getID() == "dulak.all-mods" && !disableSelf) {
                continue;
            }
            
            auto result = mod->disable();
            if (result.isOk()) {
                disabledCount++;
                log::info("Disabled mod: {}", mod->getID());
            } else {
                failedCount++;
                log::warn("Failed to disable mod {}: {}", mod->getID(), result.unwrapErr());
            }
        }
        
        std::string message;
        if (failedCount == 0) {
            message = fmt::format("Successfully disabled {} mod(s)!\nRestart required for changes to take effect.", disabledCount);
        } else {
            message = fmt::format("Disabled {} mod(s), {} failed.\nRestart required for changes to take effect.", disabledCount, failedCount);
        }
        
        FLAlertLayer::create("Disable All Mods", message, "OK")->show();
    }
    
    void onEnableAll(CCObject*) {
        this->onClose(nullptr);
        
        auto allMods = Loader::get()->getAllMods();
        int enabledCount = 0;
        int failedCount = 0;
        
        for (auto mod : allMods) {
            if (mod->isInternal() || mod->isOrWillBeEnabled()) {
                continue;
            }
            
            auto result = mod->enable();
            if (result.isOk()) {
                enabledCount++;
                log::info("Enabled mod: {}", mod->getID());
            } else {
                failedCount++;
                log::warn("Failed to enable mod {}: {}", mod->getID(), result.unwrapErr());
            }
        }
        
        std::string message;
        if (failedCount == 0) {
            message = fmt::format("Successfully enabled {} mod(s)!\nRestart required for changes to take effect.", enabledCount);
        } else {
            message = fmt::format("Enabled {} mod(s), {} failed.\nRestart required for changes to take effect.", enabledCount, failedCount);
        }
        
        FLAlertLayer::create("Enable All Mods", message, "OK")->show();
    }

public:
    static ModManagerPopup* create() {
        auto ret = new ModManagerPopup();
        if (ret->initAnchored(320.f, 160.f)) {
            ret->autorelease();
            return ret;
        }
        delete ret;
        return nullptr;
    }
};

#include <Geode/modify/MenuLayer.hpp>
class $modify(MyMenuLayer, MenuLayer) {
    bool init() {
        if (!MenuLayer::init()) {
            return false;
        }

        log::debug("All-Mods: MenuLayer initialized with {} children.", this->getChildrenCount());

        auto modButton = CCMenuItemSpriteExtra::create(
            CCSprite::createWithSpriteFrameName("GJ_optionsBtn02_001.png"),
            this,
            menu_selector(MyMenuLayer::onModManagerButton)
        );
        modButton->setScale(1.2f);

        auto bottomMenu = this->getChildByID("bottom-menu");
        if (bottomMenu) {
            bottomMenu->addChild(modButton);
            modButton->setID("mod-manager-button"_spr);
            bottomMenu->updateLayout();
        }

        return true;
    }

    void onModManagerButton(CCObject*) {
        ModManagerPopup::create()->show();
    }

    void enableAllMods(CCObject*) {
        auto allMods = Loader::get()->getAllMods();
        int enabledCount = 0;
        int failedCount = 0;
        
        for (auto mod : allMods) {
            if (mod->isInternal() || mod->isOrWillBeEnabled()) {
                continue;
            }
            
            auto result = mod->enable();
            if (result.isOk()) {
                enabledCount++;
                log::info("Enabled mod: {}", mod->getID());
            } else {
                failedCount++;
                log::warn("Failed to enable mod {}: {}", mod->getID(), result.unwrapErr());
            }
        }
        
        std::string message;
        if (failedCount == 0) {
            message = fmt::format("Successfully enabled {} mod(s)!\nRestart required for changes to take effect.", enabledCount);
        } else {
            message = fmt::format("Enabled {} mod(s), {} failed.\nRestart required for changes to take effect.", enabledCount, failedCount);
        }
        
        FLAlertLayer::create("Enable All Mods", message, "OK")->show();
    }
    
    void disableAllMods(CCObject*) {
        auto allMods = Loader::get()->getAllMods();
        int disabledCount = 0;
        int failedCount = 0;
        
        bool disableSelf = Mod::get()->getSettingValue<bool>("disable-self");
        
        for (auto mod : allMods) {
            if (mod->isInternal() || !mod->isOrWillBeEnabled()) {
                continue;
            }
            
            if (mod->getID() == "dulak.all-mods" && !disableSelf) {
                continue;
            }
            
            auto result = mod->disable();
            if (result.isOk()) {
                disabledCount++;
                log::info("Disabled mod: {}", mod->getID());
            } else {
                failedCount++;
                log::warn("Failed to disable mod {}: {}", mod->getID(), result.unwrapErr());
            }
        }
        
        std::string message;
        if (failedCount == 0) {
            message = fmt::format("Successfully disabled {} mod(s)!\nRestart required for changes to take effect.", disabledCount);
        } else {
            message = fmt::format("Disabled {} mod(s), {} failed.\nRestart required for changes to take effect.", disabledCount, failedCount);
        }
        
        FLAlertLayer::create("Disable All Mods", message, "OK")->show();
    }
};

$on_mod(Loaded) {
    log::info("Denabler loaded! Look for the mod manager button in the bottom menu. Check settings to configure self-disable behavior.");
};