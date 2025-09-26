#include <Geode/Geode.hpp>
#include <Geode/ui/Popup.hpp>
#include <Geode/loader/Loader.hpp>
#include <Geode/ui/TextInput.hpp>

using namespace geode::prelude;

class ModsPopup : public Popup<> {
protected:
    ScrollLayer* m_scroll = nullptr;
    TextInput* m_searchBar = nullptr;
    std::vector<Mod*> m_mods;

    bool setup() override {
        this->setTitle("Installed Mods");

        // --- Search bar ---
        m_searchBar = TextInput::create(250.f, "Search mods...");
        m_searchBar->setCallback([this](std::string const& text) {
            this->populateMods(text);
        });
        m_mainLayer->addChildAtPosition(m_searchBar, Anchor::Top, {0, -40});

        // --- Scroll list ---
        m_scroll = ScrollLayer::create({300.f, 180.f});
        m_scroll->setAnchorPoint({0.5f, 0.5f});
        m_scroll->setPosition(m_mainLayer->getContentSize() / 2);
        m_mainLayer->addChild(m_scroll);

        // Populate initially
        this->populateMods("");

        return true;
    }

    void populateMods(std::string const& filter) {
        m_scroll->m_contentLayer->removeAllChildren();

        m_mods.clear();
        for (auto& mod : Loader::get()->getAllMods()) {
            if (filter.empty() || mod->getName().find(filter) != std::string::npos) {
                m_mods.push_back(mod);
            }
        }

        float y = 0.f;
        for (auto* mod : m_mods) {
            auto label = CCLabelBMFont::create(
                fmt::format("{} v{}", mod->getName(), mod->getVersion().toVString()).c_str(),
                "chatFont.fnt"
            );
            label->setAnchorPoint({0, 1});
            label->setPosition({5, -y});
            label->setScale(0.6f);
            m_scroll->m_contentLayer->addChild(label);

            y += 22.f;
        }

        m_scroll->m_contentLayer->setContentSize({300.f, y});
        m_scroll->moveToTop();
    }

public:
    static void showPopup() {
        ModsPopup::create(350.f, 300.f, "GJ_square01.png")->show();
    }
};