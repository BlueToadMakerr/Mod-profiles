#include <Geode/Geode.hpp>
#include <Geode/loader/Loader.hpp>
#include <Geode/loader/Mod.hpp>
#include <Geode/ui/GeodeUI.hpp>

using namespace geode::prelude;

// Popup that lists all installed mods in a scrollable view
class ModsPopup : public FLAlertLayer {
protected:
    CCScrollView* m_scroll = nullptr;
    CCNode* m_contentNode = nullptr;

    bool setup() override {
        auto winSize = CCDirector::sharedDirector()->getWinSize();

        // Panel size
        const float w = winSize.width * 0.78f;
        const float h = winSize.height * 0.72f;

        // Title
        auto title = CCLabelBMFont::create("Installed Mods", "goldFont.fnt");
        title->setPosition({0, h / 2 - 24});
        m_buttonMenu->addChild(title);

        // Scrollable area
        const float scrollW = w - 40.f;
        const float scrollH = h - 80.f;

        m_contentNode = CCNode::create();
        m_contentNode->setContentSize({scrollW, 0});

        m_scroll = CCScrollView::create({scrollW, scrollH}, m_contentNode);
        m_scroll->setDirection(kCCScrollViewDirectionVertical);
        m_scroll->setPosition({-scrollW / 2, -scrollH / 2});
        m_scroll->setBounceable(true);
        this->m_mainLayer->addChild(m_scroll);

        // Populate with Geode mods
        populateMods(scrollW);

        return true;
    }

    void populateMods(float width) {
        const float itemH = 50.f;
        const float padY = 8.f;
        float y = 0.f;

        for (auto* mod : Loader::get()->getAllMods()) {
            if (!mod) continue;

            // Background
            auto itemBG = CCScale9Sprite::create("square01_001.png");
            itemBG->setContentSize({width, itemH});
            itemBG->setPosition({width / 2, - (y + itemH / 2)});
            m_contentNode->addChild(itemBG);

            // Name
            auto lbl = CCLabelBMFont::create(mod->getName().c_str(), "goldFont.fnt");
            lbl->setAnchorPoint({0.f, 0.5f});
            lbl->setPosition({12.f, - (y + itemH / 2)});
            lbl->setScale(0.5f);
            m_contentNode->addChild(lbl);

            // Version
            auto ver = CCLabelBMFont::create(mod->getVersion().toString().c_str(), "chatFont.fnt");
            ver->setAnchorPoint({1.f, 0.5f});
            ver->setPosition({width - 12.f, - (y + itemH / 2)});
            ver->setScale(0.45f);
            m_contentNode->addChild(ver);

            y += itemH + padY;
        }

        if (y == 0.f) {
            auto lbl = CCLabelBMFont::create("No mods installed.", "chatFont.fnt");
            lbl->setPosition({width / 2, 20});
            m_contentNode->addChild(lbl);
            y = 40.f;
        }

        m_contentNode->setContentSize({width, y});
        m_scroll->setContentSize({width, y});
        m_scroll->setContentOffset({0, m_scroll->getViewSize().height - y});
    }

public:
    static void show() {
        auto popup = ModsPopup::create();
        popup->show();
    }

    static ModsPopup* create() {
        auto ret = new ModsPopup();
        if (ret && ret->init(360.f, 260.f, "GJ_square01.png", "OK")) {
            ret->autorelease();
            return ret;
        }
        CC_SAFE_DELETE(ret);
        return nullptr;
    }
};