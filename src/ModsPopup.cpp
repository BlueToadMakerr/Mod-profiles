#include <Geode/Geode.hpp>
#include <Geode/loader/Loader.hpp>
#include <Geode/loader/Mod.hpp>
#include <Geode/ui/GeodeUI.hpp> // for openInfoPopup
#include "FileExplorer.cpp"

using namespace geode::prelude;

class ModsPopup : public Popup<> {
protected:
    ScrollLayer* m_scrollLayer = nullptr;
    TextInput* m_searchInput = nullptr;
    CCLabelBMFont* m_currentFileLabel = nullptr;

    std::string m_searchQuery;
    std::string m_currentFile;
    std::map<std::string, bool> m_modStates; // modID -> checked state

    bool setup() override {
        auto [widthCS, heightCS] = m_mainLayer->getScaledContentSize();

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
        m_mainLayer->addChild(searchBG);

        // Search input
        m_searchInput = TextInput::create(widthCS - 120.f, "Search mods...");
        m_searchInput->setPosition({ searchBG->getPositionX() - 40.f, searchBG->getPositionY() });
        m_searchInput->setCallback([this](const std::string& query) {
            m_searchQuery = query;
            refreshModList();
        });
        m_mainLayer->addChild(m_searchInput);

        // Buttons: Load, Save, Toggle All
        auto loadBtnSpr = ButtonSprite::create("Load", "bigFont.fnt", "GJ_button_01.png", 0.3f);
        auto loadBtn = CCMenuItemExt::createSpriteExtra(loadBtnSpr, [this](CCObject*) {
            FileExplorerPopup::show([this](const std::string& file) {
                m_currentFile = file;
                loadFile(file);
                updateCurrentFileLabel();
                refreshModList();
            });
        });

        auto saveBtnSpr = ButtonSprite::create("Save", "bigFont.fnt", "GJ_button_01.png", 0.3f);
        auto saveBtn = CCMenuItemExt::createSpriteExtra(saveBtnSpr, [this](CCObject*) {
            FileExplorerPopup::show([this](const std::string& file) {
                m_currentFile = file;
                saveFile(file);
                updateCurrentFileLabel();
            });
        });

        auto toggleAllSpr = ButtonSprite::create("Toggle All", "bigFont.fnt", "GJ_button_01.png", 0.3f);
        auto toggleAllBtn = CCMenuItemExt::createSpriteExtra(toggleAllSpr, [this](CCObject*) {
            toggleAllMods();
        });

        auto menu = CCMenu::create();
        menu->setPosition({ widthCS - 50.f, heightCS - 50.f });
        loadBtn->setPositionX(-80.f);
        saveBtn->setPositionX(-30.f);
        toggleAllBtn->setPositionX(10.f);
        menu->addChild(loadBtn);
        menu->addChild(saveBtn);
        menu->addChild(toggleAllBtn);
        m_mainLayer->addChild(menu);

        // Current file label
        m_currentFileLabel = CCLabelBMFont::create("No file selected", "bigFont.fnt");
        m_currentFileLabel->setScale(0.5f);
        m_currentFileLabel->setPosition({ widthCS / 2.f, heightCS - 90.f });
        m_mainLayer->addChild(m_currentFileLabel);

        // Scroll layer background
        auto scrollSize = CCSize{ widthCS - 17.5f, heightCS - 140.f };
        auto scrollBG = CCScale9Sprite::create("square02b_001.png");
        scrollBG->setContentSize(scrollSize);
        scrollBG->setAnchorPoint({ 0.5f, 0.5f });
        scrollBG->ignoreAnchorPointForPosition(false);
        scrollBG->setPosition({ widthCS / 2.f, (heightCS / 2.f) - 30.f });
        scrollBG->setColor({ 0, 0, 0 });
        scrollBG->setOpacity(100);
        m_mainLayer->addChild(scrollBG);

        auto scrollLayerLayout = ColumnLayout::create()
            ->setAxisAlignment(AxisAlignment::Start)
            ->setAutoGrowAxis(scrollSize.height - 12.5f)
            ->setGrowCrossAxis(false)
            ->setGap(5.f);

        m_scrollLayer = ScrollLayer::create({ scrollSize.width - 12.5f, scrollSize.height - 12.5f });
        m_scrollLayer->setAnchorPoint({ 0.5f, 0.5f });
        m_scrollLayer->ignoreAnchorPointForPosition(false);
        m_scrollLayer->setPosition(scrollBG->getPosition());
        m_scrollLayer->m_contentLayer->setLayout(scrollLayerLayout);
        m_mainLayer->addChild(m_scrollLayer);

        refreshModList();

        // Apply button (bottom center)
        auto applyBtnSpr = ButtonSprite::create("Apply & Restart", "bigFont.fnt", "GJ_button_01.png", 0.5f);
        auto applyBtn = CCMenuItemExt::createSpriteExtra(applyBtnSpr, [this](CCObject*) {
            applyModsAndRestart();
        });
        auto applyMenu = CCMenu::create(applyBtn, nullptr);
        applyMenu->setPosition({ widthCS / 2.f, 20.f });
        m_mainLayer->addChild(applyMenu);

        return true;
    }

    void toggleAllMods() {
        auto allMods = Loader::get()->getAllMods();
        int enabledCount = 0;
        for (Mod* mod : allMods) {
            if (mod->isInternal()) continue;
            if (m_modStates.count(mod->getID()) ? m_modStates[mod->getID()] : mod->isOrWillBeEnabled())
                enabledCount++;
        }

        bool turnOn = enabledCount < (int)allMods.size() / 2; // majority check
        for (Mod* mod : allMods) {
            if (mod->isInternal()) continue;
            m_modStates[mod->getID()] = turnOn;
        }

        refreshModList(); // update button text
    }

    void updateCurrentFileLabel() {
        std::string text = m_currentFile.empty() ? "No file selected" : ("File: " + m_currentFile);
        m_currentFileLabel->setString(text.c_str());
    }

    void refreshModList() {
        m_scrollLayer->m_contentLayer->removeAllChildren();

        auto allMods = Loader::get()->getAllMods();
        for (Mod* mod : allMods) {
            if (!m_searchQuery.empty()) {
                auto name = mod->getName();
                auto lowerName = name;
                auto lowerQuery = m_searchQuery;
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

            auto menu = CCMenu::create();
            menu->setPosition({ item->getContentSize().width - 35.f, item->getContentSize().height / 2 });

            std::string modID = mod->getID();
            bool checked = m_modStates.count(modID) ? m_modStates[modID] : mod->isOrWillBeEnabled();
            auto toggleBtnSpr = ButtonSprite::create(checked ? "Enabled" : "Disabled", "bigFont.fnt", "GJ_button_01.png", 0.4f);
            auto toggleBtn = CCMenuItemExt::createSpriteExtra(toggleBtnSpr, [this, modID, toggleBtnSpr](CCObject*) {
                m_modStates[modID] = !m_modStates[modID];
                toggleBtnSpr->setString(m_modStates[modID] ? "Enabled" : "Disabled");
            });
            toggleBtn->setPosition({ -65.f, 0.f });
            menu->addChild(toggleBtn);

            auto viewBtnSpr = ButtonSprite::create("View", "bigFont.fnt", "GJ_button_01.png", 0.4f);
            auto viewBtn = CCMenuItemExt::createSpriteExtra(viewBtnSpr, [mod](CCObject*) {
                (void)geode::openInfoPopup(mod->getID());
            });
            viewBtn->setPosition({ 0.f, 0.f });
            menu->addChild(viewBtn);

            item->addChild(menu);
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

    void applyModsAndRestart() {
        auto allMods = Loader::get()->getAllMods();
        for (Mod* mod : allMods) {
            if (mod->isInternal()) continue;
            bool enabled = m_modStates.count(mod->getID()) ? m_modStates[mod->getID()] : mod->isOrWillBeEnabled();
            if (enabled) (void)mod->enable();
            else (void)mod->disable();
        }
        geode::utils::game::restart(); // restart the game after applying changes
    }

public:
    static void showPopup() {
        auto popup = ModsPopup::create(400.f, 300.f, "GJ_square01.png");
        if (popup) popup->show();
    }

    static ModsPopup* create(float width, float height, const char* spr) {
        auto ret = new ModsPopup();
        if (ret && ret->initAnchored(width, height, spr)) {
            ret->autorelease();
            return ret;
        }
        CC_SAFE_DELETE(ret);
        return nullptr;
    }
};