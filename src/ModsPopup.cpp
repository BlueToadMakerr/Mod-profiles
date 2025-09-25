#include <Geode/Geode.hpp>
#include <Geode/loader/Loader.hpp>
#include <Geode/loader/Mod.hpp>

using namespace geode::prelude;

class ModsPopup : public FLAlertLayer {
protected:
    CCScrollView* m_scroll = nullptr;
    CCNode* m_contentNode = nullptr;

    bool init() {
        if (!FLAlertLayer::init(0, "Installed Mods", "", "OK", nullptr, 360.f, false, 260.f, 1.f))
            return false;

        const float scrollW = m_buttonMenu->getContentSize().width - 40.f;
        const float scrollH = m_buttonMenu->getContentSize().height - 60.f;

        m_contentNode = CCNode::create();
        m_contentNode->setContentSize({scrollW, 0});

        m_scroll = CCScrollView::create({scrollW, scrollH}, m_contentNode);
        m_scroll->setDirection(kCCScrollViewDirectionVertical);
        m_scroll->setPosition({-scrollW / 2, -scrollH / 2});
        m_scroll->setBounceable(true);
        this->m_mainLayer->addChild(m_scroll);

        populateMods(scrollW);

        return true;
    }

    void populateMods(float width) {
        const float itemH = 50.f;
        const float padY = 8.f;
        float y = 0.f;

        for (auto* mod : Loader::get()->getAllMods()) {
            if (!mod) continue;

            auto itemBG = CCScale9Sprite::create("square01_001.png");
            itemBG->setContentSize({width, itemH});
            itemBG->setPosition({width / 2, -(y + itemH / 2)});
            m_contentNode->addChild(itemBG);

            auto lbl = CCLabelBMFont::create(mod->getName().c_str(), "goldFont.fnt");
            lbl->setAnchorPoint({0.f, 0.5f});
            lbl->setPosition({12.f, -(y + itemH / 2)});
            lbl->setScale(0.5f);
            m_contentNode->addChild(lbl);

            auto ver = CCLabelBMFont::create(mod->getVersion().toVString().c_str(), "chatFont.fnt");
            ver->setAnchorPoint({1.f, 0.5f});
            ver->setPosition({width - 12.f, -(y + itemH / 2)});
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
    static ModsPopup* create() {
        auto ret = new ModsPopup();
        if (ret && ret->init()) {
            ret->autorelease();
            return ret;
        }
        CC_SAFE_DELETE(ret);
        return nullptr;
    }

    static void showPopup() {
        auto popup = ModsPopup::create();
        if (popup) popup->show();
    }
};