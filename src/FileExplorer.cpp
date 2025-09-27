#include <Geode/Geode.hpp>
#include <Geode/ui/GeodeUI.hpp>
#include <Geode/loader/Mod.hpp>

using namespace geode::prelude;

class FileExplorerPopup : public Popup<> {
protected:
    ScrollLayer* m_scrollLayer = nullptr;
    TextInput* m_fileNameInput = nullptr;
    std::string m_selectedFile;
    std::function<void(const std::string&)> m_callback;

    bool setup() override {
        setID("file-explorer"_spr);
        setTitle("Select Save File");

        auto [widthCS, heightCS] = m_mainLayer->getScaledContentSize();

        // File name input
        m_fileNameInput = TextInput::create(widthCS - 40.f, "New file name...");
        m_fileNameInput->setPosition({ widthCS / 2.f, heightCS - 50.f });
        m_mainLayer->addChild(m_fileNameInput);

        // Create button
        auto createBtnSpr = ButtonSprite::create("Create", "bigFont.fnt", "GJ_button_01.png", 0.5f);
        auto createBtn = CCMenuItemExt::createSpriteExtra(createBtnSpr, [this](CCObject*) {
            auto name = m_fileNameInput->getString();
            if (!name.empty()) {
                m_selectedFile = name;
                if (m_callback) m_callback(m_selectedFile);
                this->dismiss();
            }
        });
        auto menu = CCMenu::create();
        menu->setPosition({ widthCS / 2.f, heightCS - 90.f });
        menu->addChild(createBtn);
        m_mainLayer->addChild(menu);

        // Scroll area for existing save files
        auto scrollSize = CCSize{ widthCS - 17.5f, heightCS - 140.f };
        auto scrollBG = CCScale9Sprite::create("square02b_001.png");
        scrollBG->setContentSize(scrollSize);
        scrollBG->setAnchorPoint({ 0.5f, 0.5f });
        scrollBG->ignoreAnchorPointForPosition(false);
        scrollBG->setPosition({ widthCS / 2.f, (heightCS / 2.f) - 30.f });
        scrollBG->setColor({ 0, 0, 0 });
        scrollBG->setOpacity(100);
        m_mainLayer->addChild(scrollBG);

        m_scrollLayer = ScrollLayer::create({ scrollSize.width - 12.5f, scrollSize.height - 12.5f });
        m_scrollLayer->setAnchorPoint({ 0.5f, 0.5f });
        m_scrollLayer->ignoreAnchorPointForPosition(false);
        m_scrollLayer->setPosition(scrollBG->getPosition());
        m_mainLayer->addChild(m_scrollLayer);

        refreshFileList();
        return true;
    }

    void refreshFileList() {
        m_scrollLayer->m_contentLayer->removeAllChildren();

        // Load all save file names from the mod save data
        auto saveData = Mod::get()->save()->getValue("saves");
        if (!saveData.isObject()) saveData = geode::ObjectValue{};

        for (auto& [key, _] : saveData.getObject()) {
            auto item = CCNode::create();
            item->setContentSize({ m_scrollLayer->getScaledContentWidth(), 25.f });

            auto label = CCLabelBMFont::create(key.c_str(), "bigFont.fnt");
            label->setScale(0.5f);
            label->setAnchorPoint({ 0.f, 0.5f });
            label->setPosition({ 5.f, 12.5f });
            item->addChild(label);

            auto menu = CCMenu::create();
            menu->setPosition({ item->getContentSize().width - 40.f, item->getContentSize().height / 2 });

            auto selectBtnSpr = ButtonSprite::create("Select", "bigFont.fnt", "GJ_button_01.png", 0.5f);
            auto selectBtn = CCMenuItemExt::createSpriteExtra(selectBtnSpr, [this, key](CCObject*) {
                m_selectedFile = key;
                if (m_callback) m_callback(m_selectedFile);
                this->dismiss();
            });
            menu->addChild(selectBtn);
            item->addChild(menu);

            m_scrollLayer->m_contentLayer->addChild(item);
        }

        m_scrollLayer->m_contentLayer->updateLayout(true);
        m_scrollLayer->scrollToTop();
    }

public:
    static void show(std::function<void(const std::string&)> callback) {
        auto popup = new FileExplorerPopup();
        if (popup && popup->initAnchored(360.f, 300.f, "GJ_square01.png")) {
            popup->m_callback = callback;
            popup->autorelease();
            popup->show();
        } else {
            CC_SAFE_DELETE(popup);
        }
    }
};