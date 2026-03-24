#include <Geode/Geode.hpp>
#include <Geode/ui/GeodeUI.hpp>

using namespace geode::prelude;

// Delegate for FLAlertLayer confirmation
class ConfirmDeleteDelegate : public FLAlertLayerProtocol {
public:
    std::function<void(bool)> m_callback;

    ConfirmDeleteDelegate(std::function<void(bool)> cb) : m_callback(cb) {}

    void FLAlert_Clicked(FLAlertLayer* layer, bool btn1) override {
        if (m_callback) m_callback(btn1);
        layer->removeFromParent();
    }
};

class FileExplorerPopup : public Popup {
protected:
    ScrollLayer* m_scrollLayer = nullptr;
    TextInput* m_fileNameInput = nullptr;
    std::string m_selectedFile;
    std::function<void(const std::string&)> m_callback;

    bool init(std::function<void(const std::string&)> callback) {
        if (!Popup::init(360.f, 300.f))
            return false;

        m_callback = callback;

        this->setID("file-explorer"_spr);
        this->setTitle("Select Save File");

        // m_mainLayer is the correct content node in v5 — it is a child of the popup
        // background, sized to the popup dimensions, with its origin at the popup's
        // bottom-left corner. All child positions are relative to it.
        const float widthCS = 360.f;
        const float heightCS = 300.f;

        // Create button sits to the right of the input on the same row.
        // Reserve ~70px on the right for it, so shrink the input accordingly.
        const float btnWidth = 70.f;
        const float gap = 8.f;
        const float inputWidth = widthCS - 40.f - btnWidth - gap;
        const float rowY = heightCS - 50.f;

        m_fileNameInput = TextInput::create(inputWidth, "New file name...");
        // Anchor input to the left side of the row
        m_fileNameInput->setPosition({ 20.f + inputWidth / 2.f, rowY });
        m_mainLayer->addChild(m_fileNameInput);

        // Create button — placed to the right of the input
        auto createBtnSpr = ButtonSprite::create("Create", "bigFont.fnt", "GJ_button_01.png", 0.5f);
        auto createBtn = CCMenuItemExt::createSpriteExtra(createBtnSpr, [this](CCObject*) {
            auto name = m_fileNameInput->getString();
            if (!name.empty()) {
                addSaveFile(name);
                m_selectedFile = name;
                if (m_callback) m_callback(m_selectedFile);
                this->removeFromParent();
            }
        });
        auto menu = CCMenu::create();
        menu->setPosition({ 20.f + inputWidth + gap + btnWidth / 2.f, rowY });
        menu->addChild(createBtn);
        m_mainLayer->addChild(menu);

        // Scroll area — starts below the single input+button row
        auto scrollSize = CCSize{ widthCS - 17.5f, heightCS - 90.f };
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

        refreshFileList();
        return true;
    }

    void refreshFileList() {
        m_scrollLayer->m_contentLayer->removeAllChildren();

        std::string filesStr = Mod::get()->getSavedValue<std::string>("save_files", "");
        std::vector<std::string> files;
        size_t pos = 0;
        while (true) {
            size_t next = filesStr.find(';', pos);
            if (next == std::string::npos) {
                if (pos < filesStr.size()) files.push_back(filesStr.substr(pos));
                break;
            }
            files.push_back(filesStr.substr(pos, next - pos));
            pos = next + 1;
        }

        for (auto& file : files) {
            auto item = CCNode::create();
            item->setContentSize({ m_scrollLayer->getScaledContentWidth(), 30.f });

            auto label = CCLabelBMFont::create(file.c_str(), "bigFont.fnt");
            label->setScale(0.5f);
            label->setAnchorPoint({ 0.f, 0.5f });
            label->setPosition({ 5.f, item->getContentSize().height / 2 });
            item->addChild(label);

            auto selectMenu = CCMenu::create();
            selectMenu->setPosition({ item->getContentSize().width - 90.f, item->getContentSize().height / 2 });
            auto selectBtnSpr = ButtonSprite::create("Select", "bigFont.fnt", "GJ_button_01.png", 0.5f);
            selectBtnSpr->setScale(0.7f);
            auto selectBtn = CCMenuItemExt::createSpriteExtra(selectBtnSpr, [this, file](CCObject*) {
                m_selectedFile = file;
                if (m_callback) m_callback(m_selectedFile);
                this->removeFromParent();
            });
            selectMenu->addChild(selectBtn);
            item->addChild(selectMenu);

            auto deleteMenu = CCMenu::create();
            deleteMenu->setPosition({ item->getContentSize().width - 40.f, item->getContentSize().height / 2 });
            auto trashSprite = CCSprite::createWithSpriteFrameName("GJ_trashBtn_001.png");
            trashSprite->setScale(0.8f);
            auto deleteBtn = CCMenuItemExt::createSpriteExtra(trashSprite, [this, file](CCObject*) {
                auto delegate = new ConfirmDeleteDelegate([this, file](bool yes) {
                    if (yes) {
                        removeSaveFile(file);
                        refreshFileList();
                    }
                });
                FLAlertLayer::create(
                    delegate,
                    "Confirm Delete",
                    ("Are you sure you want to delete \"" + file + "\"?").c_str(),
                    "No",
                    "Yes"
                )->show();
            });
            deleteMenu->addChild(deleteBtn);
            item->addChild(deleteMenu);

            m_scrollLayer->m_contentLayer->addChild(item);
        }

        m_scrollLayer->m_contentLayer->updateLayout(true);
        m_scrollLayer->scrollToTop();
    }

    void addSaveFile(const std::string& name) {
        std::string filesStr = Mod::get()->getSavedValue<std::string>("save_files", "");
        if (!filesStr.empty()) filesStr += ";";
        filesStr += name;
        Mod::get()->setSavedValue("save_files", filesStr);
    }

    void removeSaveFile(const std::string& name) {
        std::string filesStr = Mod::get()->getSavedValue<std::string>("save_files", "");
        std::vector<std::string> files;
        size_t pos = 0;
        while (true) {
            size_t next = filesStr.find(';', pos);
            if (next == std::string::npos) {
                if (pos < filesStr.size()) files.push_back(filesStr.substr(pos));
                break;
            }
            files.push_back(filesStr.substr(pos, next - pos));
            pos = next + 1;
        }
        files.erase(std::remove(files.begin(), files.end(), name), files.end());
        std::string newFilesStr;
        for (size_t i = 0; i < files.size(); ++i) {
            if (i != 0) newFilesStr += ";";
            newFilesStr += files[i];
        }
        Mod::get()->setSavedValue("save_files", newFilesStr);
    }

public:
    static void open(std::function<void(const std::string&)> callback) {
        auto popup = new FileExplorerPopup();
        if (popup && popup->init(callback)) {
            popup->autorelease();
            popup->show();
        } else {
            CC_SAFE_DELETE(popup);
        }
    }
};