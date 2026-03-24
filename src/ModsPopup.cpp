#include <Geode/Geode.hpp>
#include <Geode/loader/Loader.hpp>
#include <Geode/loader/Mod.hpp>
#include <Geode/ui/GeodeUI.hpp>
#include <Geode/ui/Popup.hpp>
#include <Geode/ui/TextInput.hpp>
#include <Geode/ui/ScrollLayer.hpp>
#include "FileExplorer.cpp"

using namespace geode::prelude;

class ModsPopup : public Popup {
protected:
    ScrollLayer* m_scrollLayer = nullptr;
    TextInput* m_searchInput = nullptr;
    std::string m_searchQuery;
    std::string m_currentFile;
    std::map<std::string, bool> m_modStates;

    bool init() {
        if (!Popup::init(400.f, 300.f))
            return false;

        const float widthCS = 400.f;
        const float heightCS = 300.f;

        // Title
        auto title = CCLabelBMFont::create("Installed Mods", "bigFont.fnt");
        title->setScale(0.6f);
        title->setPosition({ widthCS / 2.f, heightCS - 20.f });
        m_mainLayer->addChild(title);

        // Search background
        auto searchBG = CCScale9Sprite::create("square02b_001.png");
        searchBG->setContentSize({ widthCS - 40.f, 30.f });
        searchBG->setColor({ 0, 0, 0 });
        searchBG->setOpacity(100);
        searchBG->setPosition({ widthCS / 2.f, heightCS - 50.f });
        searchBG->setVisible(false);
        m_mainLayer->addChild(searchBG);

        // Search input
        m_searchInput = TextInput::create(widthCS - 240.f, "Search mods...");
        m_searchInput->setPosition({ searchBG->getPositionX() - 80.f, searchBG->getPositionY() });
        m_searchInput->setCallback([this](const std::string& query) {
            m_searchQuery = query;
            refreshModList();
        });
        m_mainLayer->addChild(m_searchInput);

        // Load/Save/Toggle buttons
        auto loadBtnSpr = ButtonSprite::create("Load", "bigFont.fnt", "GJ_button_01.png", 0.3f);
        auto loadBtn = CCMenuItemExt::createSpriteExtra(loadBtnSpr, [this](CCObject*) {
            FileExplorerPopup::open([this](const std::string& file) {
                m_currentFile = file;
                loadFile(file);
                refreshModList();
            });
        });

        auto saveBtnSpr = ButtonSprite::create("Save", "bigFont.fnt", "GJ_button_01.png", 0.3f);
        auto saveBtn = CCMenuItemExt::createSpriteExtra(saveBtnSpr, [this](CCObject*) {
            FileExplorerPopup::open([this](const std::string& file) {
                m_currentFile = file;
                saveFile(file);
            });
        });

        auto toggleAllBtnSpr = ButtonSprite::create("Toggle All", "bigFont.fnt", "GJ_button_01.png", 0.3f);
        auto toggleAllBtn = CCMenuItemExt::createSpriteExtra(toggleAllBtnSpr, [this](CCObject*) {
            toggleAllMods();
        });

        auto menu = CCMenu::create();
        menu->setPosition({ widthCS - 80.f, heightCS - 50.f });
        menu->addChild(loadBtn);
        loadBtn->setPositionX(-85.f);
        menu->addChild(saveBtn);
        saveBtn->setPositionX(-40.f);
        menu->addChild(toggleAllBtn);
        toggleAllBtn->setPositionX(20.f);
        m_mainLayer->addChild(menu);

        // Scroll layer background — leave ~50px at the bottom for the Apply & Restart button
        auto scrollSize = CCSize{ widthCS - 17.5f, heightCS - 120.f };
        auto scrollBG = CCScale9Sprite::create("square02b_001.png");
        scrollBG->setContentSize(scrollSize);
        scrollBG->setAnchorPoint({ 0.5f, 0.5f });
        scrollBG->ignoreAnchorPointForPosition(false);
        scrollBG->setPosition({ widthCS / 2.f, (heightCS / 2.f) - 15.f });
        scrollBG->setColor({ 0, 0, 0 });
        scrollBG->setOpacity(100);
        m_mainLayer->addChild(scrollBG);

        auto scrollLayerLayout = ColumnLayout::create()
            ->setAxisAlignment(AxisAlignment::Start)
            ->setAutoGrowAxis(scrollSize.height - 12.5f)
            ->setGrowCrossAxis(false)
            ->setGap(5.f);

        auto scrollLayerSize = CCSize{ scrollSize.width - 12.5f, scrollSize.height - 12.5f };
        m_scrollLayer = ScrollLayer::create(scrollLayerSize);
        // ScrollLayer uses BL origin — derive its position from the scrollBG center.
        m_scrollLayer->setAnchorPoint({ 0.f, 0.f });
        m_scrollLayer->ignoreAnchorPointForPosition(true);
        m_scrollLayer->setPosition({
            scrollBG->getPositionX() - scrollLayerSize.width  / 2.f,
            scrollBG->getPositionY() - scrollLayerSize.height / 2.f
        });
        m_scrollLayer->m_contentLayer->setLayout(scrollLayerLayout);
        m_mainLayer->addChild(m_scrollLayer);

        refreshModList();

        // Apply button
        auto applyBtnSpr = ButtonSprite::create("Apply & Restart", "bigFont.fnt", "GJ_button_01.png", 0.5f);
        auto applyBtn = CCMenuItemExt::createSpriteExtra(applyBtnSpr, [this](CCObject*) {
            applyModsAndRestart();
        });
        auto applyMenu = CCMenu::createWithItem(applyBtn);
        applyMenu->setPosition({ widthCS / 2.f, 20.f });
        m_mainLayer->addChild(applyMenu);

        return true;
    }

    void refreshModList() {
        m_scrollLayer->m_contentLayer->removeAllChildren();

        auto allMods = Loader::get()->getAllMods();
        for (Mod* mod : allMods) {
            if (!m_searchQuery.empty()) {
                std::string lowerName(mod->getName());
                std::string lowerQuery = m_searchQuery;
                std::transform(lowerName.begin(), lowerName.end(), lowerName.begin(), ::tolower);
                std::transform(lowerQuery.begin(), lowerQuery.end(), lowerQuery.begin(), ::tolower);
                if (lowerName.find(lowerQuery) == std::string::npos) continue;
            }

            auto item = CCNode::create();
            item->setContentSize({ m_scrollLayer->getScaledContentWidth(), 40.f });

            auto label = CCLabelBMFont::create(mod->getName().c_str(), "bigFont.fnt");
            label->setScale(0.5f);
            label->setAnchorPoint({ 0.f, 0.5f });
            label->setPosition({ 5.f, item->getContentSize().height / 2 });
            item->addChild(label);

            auto itemMenu = CCMenu::create();
            itemMenu->setPosition({ item->getContentSize().width - 35.f, item->getContentSize().height / 2 });

            std::string modID = mod->getID();
            bool checked = m_modStates.count(modID) ? m_modStates[modID] : mod->isOrWillBeEnabled();

            if (modID == "bluetoadmaker.modprofiles") {
                bool disableSelf = mod->getSettingValue<bool>("disable-self");
                if (!disableSelf) {
                    checked = true;
                    m_modStates[modID] = true;
                }
            }

            auto toggleBtnSpr = ButtonSprite::create(checked ? "Enabled" : "Disabled", "bigFont.fnt", checked ? "GJ_button_01.png" : "GJ_button_06.png", 0.4f);
            auto toggleBtn = CCMenuItemExt::createSpriteExtra(toggleBtnSpr, [this, mod, modID, toggleBtnSpr](CCObject*) {
                bool disableSelf = mod->getSettingValue<bool>("disable-self");

                if (modID == "bluetoadmaker.modprofiles" && !disableSelf) {
                    m_modStates[modID] = true;
                    toggleBtnSpr->setString("Enabled");
                    toggleBtnSpr->updateBGImage("GJ_button_01.png");
                    return;
                }

                m_modStates[modID] = !m_modStates[modID];
                bool nowEnabled = m_modStates[modID];
                toggleBtnSpr->setString(nowEnabled ? "Enabled" : "Disabled");
                toggleBtnSpr->updateBGImage(nowEnabled ? "GJ_button_01.png" : "GJ_button_06.png");

                if (modID == "bluetoadmaker.modprofiles" && disableSelf) {
                    if (nowEnabled)
                        (void)mod->enable();
                    else
                        (void)mod->disable();
                }
            });
            toggleBtn->setPosition({ -65.f, 0.f });
            itemMenu->addChild(toggleBtn);

            auto viewBtnSpr = ButtonSprite::create("View", "bigFont.fnt", "GJ_button_01.png", 0.4f);
            auto viewBtn = CCMenuItemExt::createSpriteExtra(viewBtnSpr, [mod](CCObject*) {
                (void)geode::openInfoPopup(mod->getID());
            });
            viewBtn->setPosition({ 0.f, 0.f });
            itemMenu->addChild(viewBtn);

            item->addChild(itemMenu);
            m_scrollLayer->m_contentLayer->addChild(item);
        }

        m_scrollLayer->m_contentLayer->updateLayout(true);
        m_scrollLayer->scrollToTop();
    }

    void loadFile(const std::string& file) {
        m_modStates.clear();
        auto allMods = Loader::get()->getAllMods();
        for (Mod* mod : allMods) {
            bool val = Mod::get()->getSavedValue<int>("save_" + file + "_" + mod->getID(), 0) != 0;
            m_modStates[mod->getID()] = val;
        }
    }

    void saveFile(const std::string& file) {
        auto allMods = Loader::get()->getAllMods();
        for (Mod* mod : allMods) {
            if (mod->isInternal()) continue;
            std::string key = "save_" + file + "_" + mod->getID();
            bool checked = m_modStates.count(mod->getID()) ? m_modStates[mod->getID()] : mod->isOrWillBeEnabled();
            Mod::get()->setSavedValue(key, checked ? 1 : 0);
        }
    }

    void toggleAllMods() {
        static bool s_allModsEnabled = true;
        s_allModsEnabled = !s_allModsEnabled;

        auto allMods = Loader::get()->getAllMods();
        for (Mod* mod : allMods) {
            if (mod->isInternal()) continue;
            std::string id = mod->getID();
            bool disableSelf = mod->getSettingValue<bool>("disable-self");

            if (id == "bluetoadmaker.modprofiles" && !disableSelf) {
                m_modStates[id] = true;
                continue;
            }

            m_modStates[id] = s_allModsEnabled;
            if (id == "bluetoadmaker.modprofiles" && disableSelf) {
                if (s_allModsEnabled) (void)mod->enable();
                else (void)mod->disable();
            }
        }

        refreshModList();
    }

    void applyModsAndRestart() {
        auto allMods = Loader::get()->getAllMods();
        for (Mod* mod : allMods) {
            if (mod->isInternal()) continue;

            bool enabled = m_modStates.count(mod->getID()) ? m_modStates[mod->getID()] : mod->isOrWillBeEnabled();

            if (mod->getID() == "bluetoadmaker.modprofiles") {
                bool disableSelf = mod->getSettingValue<bool>("disable-self");
                if (!disableSelf) enabled = true;
            }

            if (enabled)
                (void)mod->enable();
            else
                (void)mod->disable();
        }

        geode::utils::game::restart(true);
    }

public:
    static void showPopup() {
        auto popup = new ModsPopup();
        if (popup && popup->init()) {
            popup->autorelease();
            popup->show();
        } else {
            CC_SAFE_DELETE(popup);
        }
    }
};