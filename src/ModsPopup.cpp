#include <Geode/Geode.hpp>
#include <Geode/loader/Loader.hpp>
#include <Geode/loader/Mod.hpp>
#include <Geode/ui/GeodeUI.hpp> // for openInfoPopup
#include <Geode/utils/cocos/CCMenuItemExt.hpp> // for CCMenuItemExt

using namespace geode::prelude;

class ModsPopup : public Popup<> {
protected:
    ScrollLayer* m_scrollLayer = nullptr;
    TextInput* m_searchInput = nullptr;
    std::string m_searchQuery;

    bool setup() override {
        setID("mods-popup"_spr);
        setTitle("Installed Mods");

        auto [widthCS, heightCS] = m_mainLayer->getScaledContentSize();

        // search bar background
        auto searchBG = CCScale9Sprite::create("square02b_001.png");
        searchBG->setContentSize({ widthCS - 40.f, 30.f });
        searchBG->setColor({ 0, 0, 0 });
        searchBG->setOpacity(100);
        searchBG->setPosition({ widthCS / 2.f, heightCS - 50.f });
        m_mainLayer->addChild(searchBG);

        // search input
        m_searchInput = TextInput::create(widthCS - 60.f, "Search mods...");
        m_searchInput->setID("search-bar");
        m_searchInput->setPosition(searchBG->getPosition());
        m_searchInput->setCallback([this](std::string const& query) {
            m_searchQuery = query;
            refreshModList();
        });
        m_mainLayer->addChild(m_searchInput);

        // scroll area background
        auto scrollSize = CCSize{ widthCS - 17.5f, heightCS - 100.f };
        auto scrollBG = CCScale9Sprite::create("square02b_001.png");
        scrollBG->setContentSize(scrollSize);
        scrollBG->setAnchorPoint({ 0.5f, 0.5f });
        scrollBG->ignoreAnchorPointForPosition(false);
        scrollBG->setPosition({ widthCS / 2.f, (heightCS / 2.f) - 30.f });
        scrollBG->setColor({ 0, 0, 0 });
        scrollBG->setOpacity(100);
        m_mainLayer->addChild(scrollBG);

        // scroll layer
        auto scrollLayerLayout = ColumnLayout::create()
            ->setAxisAlignment(AxisAlignment::Start)
            ->setAutoGrowAxis(scrollSize.height - 12.5f)
            ->setGrowCrossAxis(false)
            ->setGap(5.f);

        m_scrollLayer = ScrollLayer::create({ scrollSize.width - 12.5f, scrollSize.height - 12.5f });
        m_scrollLayer->setID("mod-list");
        m_scrollLayer->setAnchorPoint({ 0.5f, 0.5f });
        m_scrollLayer->ignoreAnchorPointForPosition(false);
        m_scrollLayer->setPosition(scrollBG->getPosition());

        m_scrollLayer->m_contentLayer->setLayout(scrollLayerLayout);
        m_mainLayer->addChild(m_scrollLayer);

        refreshModList();
        return true;
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
                if (lowerName.find(lowerQuery) == std::string::npos) {
                    continue; // skip mods that don’t match search
                }
            }

            auto item = CCNode::create();
            item->setContentSize({ m_scrollLayer->getScaledContentWidth(), 25.f });

            // mod name label
            auto label = CCLabelBMFont::create(mod->getName().c_str(), "bigFont.fnt");
            label->setScale(0.5f);
            label->setAnchorPoint({ 0.f, 0.5f });
            label->setPosition({ 5.f, item->getContentSize().height / 2 });
            item->addChild(label);

            // button menu
            auto menu = CCMenu::create();
            menu->setPosition({ item->getContentSize().width - 40.f, item->getContentSize().height / 2 });

            auto viewBtnSpr = ButtonSprite::create("View", "bigFont.fnt", "GJ_button_01.png", 0.5f);
            auto viewBtn = CCMenuItemExt::createSpriteExtra(
                viewBtnSpr,
                [mod](CCObject*) {
                    geode::ui::openInfoPopup(mod->getID());
                }
            );
            menu->addChild(viewBtn);
            item->addChild(menu);

            m_scrollLayer->m_contentLayer->addChild(item);
        }

        m_scrollLayer->m_contentLayer->updateLayout(true);
        m_scrollLayer->scrollToTop();
    }

public:
    static void showPopup() {
        auto popup = ModsPopup::create(360.f, 300.f, "GJ_square01.png");
        if (popup) {
            popup->show();
        }
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