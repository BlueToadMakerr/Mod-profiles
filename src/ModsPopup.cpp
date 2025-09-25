#include <Geode/Geode.hpp>
#include <Geode/loader/Loader.hpp>
#include <Geode/loader/Mod.hpp>
#include <Geode/ui/TextArea.hpp>
#include <Geode/ui/ButtonSprite.hpp>
#include <Geode/ui/Anchor.hpp>

using namespace geode::prelude;

class ModsLayer : public CCLayer {
protected:
    CCNode* m_mainLayer = nullptr;
    CCScrollView* m_scroll = nullptr;
    CCNode* m_contentNode = nullptr;
    CCMenu* m_buttonMenu = nullptr;

    float popupWidth = 450.f;
    float popupHeight = 300.f;
    float padding = 10.f;
    float doneButtonHeight = 50.f;

    bool init() {
        auto director = CCDirector::sharedDirector();
        auto winSize = director->getWinSize();
        auto visibleOrigin = director->getVisibleOrigin();

        m_mainLayer = CCNode::create();
        this->addChild(m_mainLayer);

        // Popup background
        auto bg = CCScale9Sprite::create("square02_001.png");
        bg->setContentSize({popupWidth, popupHeight});
        bg->setPosition({visibleOrigin.x + winSize.width/2, visibleOrigin.y + winSize.height/2});
        m_mainLayer->addChild(bg);

        // Scrollable mods list
        float scrollW = popupWidth - 40.f;
        float scrollH = popupHeight - doneButtonHeight - 3*padding;

        m_contentNode = CCNode::create();
        m_contentNode->setContentSize({scrollW, 0});

        m_scroll = CCScrollView::create({scrollW, scrollH}, m_contentNode);
        m_scroll->setDirection(kCCScrollViewDirectionVertical);
        m_scroll->setBounceable(true);
        m_scroll->setPosition({
            bg->getPositionX(),
            bg->getPositionY() - popupHeight/2 + doneButtonHeight + 2*padding
        });
        m_mainLayer->addChild(m_scroll, 10);

        populateMods(scrollW, scrollH);

        // Button menu
        m_buttonMenu = CCMenu::create();
        m_buttonMenu->setPosition({0,0});
        m_mainLayer->addChild(m_buttonMenu, 20);

        auto okSpr = ButtonSprite::create("OK", "goldFont.fnt", "GJ_button_01.png", 0.8f);
        auto okBtn = CCMenuItemSpriteExtra::create(
            okSpr, this, menu_selector(ModsLayer::onDone)
        );

        okBtn->setPosition({
            bg->getPositionX(),                  // center horizontally
            bg->getPositionY() - popupHeight/2 + doneButtonHeight/2 + padding // bottom middle
        });

        m_buttonMenu->addChild(okBtn);

        // Enable touches to block background clicks
        setTouchEnabled(true);
        setTouchMode(kCCTouchesOneByOne);
        setTouchPriority(-128);

        return true;
    }

    void populateMods(float width, float scrollH) {
        const float itemH = 50.f;
        const float padY = 8.f;

        auto mods = Loader::get()->getAllMods();
        float contentHeight = mods.size() * (itemH + padY);
        if (contentHeight < scrollH) contentHeight = scrollH;

        m_contentNode->setContentSize({width, contentHeight});

        float y = contentHeight;
        for (auto* mod : mods) {
            if (!mod) continue;

            auto itemBG = CCScale9Sprite::create("square01_001.png");
            itemBG->setContentSize({width, itemH});
            itemBG->setPosition({width/2, y - itemH/2});
            m_contentNode->addChild(itemBG);

            auto lbl = CCLabelBMFont::create(mod->getName().c_str(), "goldFont.fnt");
            lbl->setAnchorPoint({0.f, 0.5f});
            lbl->setPosition({12.f, y - itemH/2});
            lbl->setScale(0.5f);
            m_contentNode->addChild(lbl);

            auto ver = CCLabelBMFont::create(mod->getVersion().toVString().c_str(), "chatFont.fnt");
            ver->setAnchorPoint({1.f, 0.5f});
            ver->setPosition({width - 12.f, y - itemH/2});
            ver->setScale(0.45f);
            m_contentNode->addChild(ver);

            y -= itemH + padY;
        }

        if (mods.empty()) {
            auto lbl = CCLabelBMFont::create("No mods installed.", "chatFont.fnt");
            lbl->setPosition({width/2, 20});
            m_contentNode->addChild(lbl);
        }

        m_scroll->setContentSize({width, contentHeight});
        m_scroll->setContentOffset({0,0});
    }

    void onDone(CCObject*) {
        this->removeFromParentAndCleanup(true);
    }

    virtual bool ccTouchBegan(CCTouch* touch, CCEvent* event) override {
        auto loc = touch->getLocation();
        auto bgPos = m_mainLayer->getPosition();
        auto bgSize = CCSize(popupWidth, popupHeight);

        // Block touches outside popup
        if (loc.x < bgPos.x - popupWidth/2 || loc.x > bgPos.x + popupWidth/2 ||
            loc.y < bgPos.y - popupHeight/2 || loc.y > bgPos.y + popupHeight/2) {
            return true;
        }
        return false;
    }

public:
    static ModsLayer* create() {
        auto ret = new ModsLayer();
        if (ret && ret->init()) {
            ret->autorelease();
            return ret;
        }
        CC_SAFE_DELETE(ret);
        return nullptr;
    }

    static void showPopup() {
        auto popup = ModsLayer::create();
        if (!popup) return;

        auto scene = CCDirector::sharedDirector()->getRunningScene();
        if (scene) scene->addChild(popup, 9999);
    }
};