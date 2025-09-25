#include <Geode/Geode.hpp>
#include <Geode/loader/Loader.hpp>
#include <Geode/loader/Mod.hpp>

using namespace geode::prelude;

class ModsPopup : public CCNode {
protected:
    CCNode* m_bg = nullptr;
    CCScrollView* m_scroll = nullptr;
    CCNode* m_contentNode = nullptr;
    CCMenu* m_menu = nullptr;

    bool init() {
        log::info("ModsPopup: init called");

        // Fullscreen transparent background
        m_bg = CCLayerColor::create({0, 0, 0, 180});
        this->addChild(m_bg);

        // Popup background
        float popupWidth = 500.f;
        float popupHeight = 400.f;
        m_bg = CCScale9Sprite::create("square02_001.png");
        m_bg->setContentSize({popupWidth, popupHeight});
        m_bg->setPosition({0, 0}); // center
        this->addChild(m_bg, 1);

        // Scroll view for mods
        float scrollW = popupWidth - 40.f;
        float scrollH = popupHeight - 80.f;

        m_contentNode = CCNode::create();
        m_contentNode->setContentSize({scrollW, 0});

        m_scroll = CCScrollView::create({scrollW, scrollH}, m_contentNode);
        m_scroll->setDirection(kCCScrollViewDirectionVertical);
        m_scroll->setPosition({-scrollW / 2, -scrollH / 2}); // center in bg
        m_scroll->setBounceable(true);
        m_bg->addChild(m_scroll, 10);

        // Menu for Done button
        m_menu = CCMenu::create();
        m_menu->setPosition({0,0});
        m_bg->addChild(m_menu, 20);

        auto doneBtn = CCMenuItemLabel::create(
            CCLabelBMFont::create("Done", "goldFont.fnt"),
            this,
            menu_selector(ModsPopup::onDone)
        );
        doneBtn->setPosition({0, -popupHeight/2 + 30});
        m_menu->addChild(doneBtn);

        populateMods(scrollW, scrollH);

        return true;
    }

    void populateMods(float width, float scrollH) {
        log::info("ModsPopup: populateMods called");

        const float itemH = 50.f;
        const float padY = 8.f;

        auto mods = Loader::get()->getAllMods();
        log::info("ModsPopup: {} mods returned by Loader", mods.size());

        float contentHeight = mods.size() * (itemH + padY);
        if (contentHeight < scrollH) contentHeight = scrollH;

        m_contentNode->setContentSize({width, contentHeight});

        float y = contentHeight; // start at top
        for (auto* mod : mods) {
            if (!mod) continue;

            log::info("ModsPopup: adding mod '{}' version '{}'", mod->getName(), mod->getVersion().toVString());

            auto itemBG = CCScale9Sprite::create("square01_001.png");
            itemBG->setContentSize({width, itemH});
            itemBG->setPosition({width / 2, y - itemH / 2});
            m_contentNode->addChild(itemBG);

            auto lbl = CCLabelBMFont::create(mod->getName().c_str(), "goldFont.fnt");
            lbl->setAnchorPoint({0.f, 0.5f});
            lbl->setPosition({12.f, y - itemH / 2});
            lbl->setScale(0.5f);
            m_contentNode->addChild(lbl);

            auto ver = CCLabelBMFont::create(mod->getVersion().toVString().c_str(), "chatFont.fnt");
            ver->setAnchorPoint({1.f, 0.5f});
            ver->setPosition({width - 12.f, y - itemH / 2});
            ver->setScale(0.45f);
            m_contentNode->addChild(ver);

            y -= itemH + padY;
        }

        if (mods.empty()) {
            log::info("ModsPopup: no mods installed");
            auto lbl = CCLabelBMFont::create("No mods installed.", "chatFont.fnt");
            lbl->setPosition({width / 2, 20});
            m_contentNode->addChild(lbl);
            contentHeight = scrollH;
            m_contentNode->setContentSize({width, contentHeight});
        }

        m_scroll->setContentSize({width, contentHeight});
        m_scroll->setContentOffset({0, 0});
        log::info("ModsPopup: populateMods finished, content height {}", contentHeight);
    }

    void onDone(CCObject*) {
        log::info("ModsPopup: Done pressed, removing popup");
        this->removeFromParentAndCleanup(true);
    }

public:
    static ModsPopup* create() {
        auto ret = new ModsPopup();
        if (ret && ret->init()) {
            ret->autorelease();
            return ret;
        }
        CC_SAFE_DELETE(ret);
        return nullptr;
    }

    static void show() {
        auto popup = ModsPopup::create();
        if (!popup) return;

        auto scene = CCDirector::sharedDirector()->getRunningScene();
        if (scene) scene->addChild(popup, 9999); // high z-order
    }
};