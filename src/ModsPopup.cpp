#include <Geode/Geode.hpp>
#include <Geode/loader/Loader.hpp>
#include <Geode/loader/Mod.hpp>

using namespace geode::prelude;

class ModsPopup : public FLAlertLayer {
protected:
    CCScrollView* m_scroll = nullptr;
    CCNode* m_contentNode = nullptr;

    bool init() {  
        log::info("ModsPopup: init called");

        if (!FLAlertLayer::init(0, "Installed Mods", "", "OK", nullptr, 360.f, false, 260.f, 1.f)) {
            log::info("ModsPopup: FLAlertLayer::init failed");
            return false;  
        }

        log::info("ModsPopup: creating scroll view");

        const float scrollW = m_buttonMenu->getContentSize().width - 40.f;  
        const float scrollH = m_buttonMenu->getContentSize().height - 60.f;  

        m_contentNode = CCNode::create();  
        m_contentNode->setContentSize({scrollW, 0});  

        m_scroll = CCScrollView::create({scrollW, scrollH}, m_contentNode);  
        m_scroll->setDirection(kCCScrollViewDirectionVertical);  
        m_scroll->setPosition({-scrollW / 2, -scrollH / 2});  
        m_scroll->setBounceable(true);  
        this->m_mainLayer->addChild(m_scroll);  

        log::info("ModsPopup: calling populateMods");
        populateMods(scrollW);  

        return true;  
    }  

    void populateMods(float width) {  
        log::info("ModsPopup: populateMods called");

        const float itemH = 50.f;  
        const float padY = 8.f;  
        float y = 0.f;  

        auto mods = Loader::get()->getAllMods();
        log::info("ModsPopup: {} mods returned by Loader", mods.size());

        for (auto* mod : mods) {  
            if (!mod) continue;  

            log::info("ModsPopup: adding mod '{}' version '{}'", mod->getName(), mod->getVersion().toVString());

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
            log::info("ModsPopup: no mods installed");  
            auto lbl = CCLabelBMFont::create("No mods installed.", "chatFont.fnt");  
            lbl->setPosition({width / 2, 20});  
            m_contentNode->addChild(lbl);  
            y = 40.f;  
        }  

        m_contentNode->setContentSize({width, y});  
        m_scroll->setContentSize({width, y});  
        m_scroll->setContentOffset({0, m_scroll->getViewSize().height - y});  

        log::info("ModsPopup: populateMods finished, content height {}", y);
    }

public:
    static ModsPopup* create() {
        log::info("ModsPopup: create called");
        auto ret = new ModsPopup();
        if (ret && ret->init()) {
            ret->autorelease();
            log::info("ModsPopup: create success");
            return ret;
        }
        CC_SAFE_DELETE(ret);
        log::info("ModsPopup: create failed");
        return nullptr;
    }

    static void showPopup() {  
        log::info("ModsPopup: showPopup called");
        auto popup = ModsPopup::create();  
        if (popup) log::info("ModsPopup: popup created, showing");  
        else log::info("ModsPopup: popup creation failed");
        if (popup) popup->show();  
    }
};