// Single-file ModItem implementation (favorites removed, view/details kept)
// Includes limited to Geode and fmt only (no local project includes)

#include <fmt/core.h>

#include <Geode/Geode.hpp>
#include <Geode/ui/GeodeUI.hpp>
#include <Geode/utils/ColorProvider.hpp>

using namespace geode::prelude;

class ModItem : public CCNode {
protected:
    Mod* m_mod = nullptr;
    bool m_geodeTheme = false;
    // heartIcons/favorite removed — kept param for compatibility but unused
    CCScale9Sprite* m_backgroundSprite = nullptr;

public:
    ModItem() = default;
    virtual ~ModItem() = default;

    // Initialize the ModItem node (kept signature for compatibility)
    bool init(
        Mod* mod,
        CCSize const& size,
        bool geodeTheme,
        bool /*heartIcons - ignored*/
    ) {
        m_mod = mod;
        m_geodeTheme = geodeTheme;

        if (!CCNode::init()) {
            return false;
        }

        auto colProvider = ColorProvider::get();

        // node setup
        auto modID = mod->getID();
        setID(modID);
        setAnchorPoint({ 0, 1 });
        setContentSize(size);

        // Background for mod item
        m_backgroundSprite = CCScale9Sprite::create("square02b_001.png");
        m_backgroundSprite->setID("background");
        m_backgroundSprite->setScale(0.5f);
        m_backgroundSprite->setContentSize({ size.width * 2.f, size.height * 2.f });
        m_backgroundSprite->setAnchorPoint({ 0.5, 0.5 });
        m_backgroundSprite->ignoreAnchorPointForPosition(false);
        m_backgroundSprite->setPosition({ getScaledContentWidth() / 2.f, getScaledContentHeight() / 2.f });
        auto bgColor = colProvider->color("geode.loader/mod-developer-item-bg");
        m_backgroundSprite->setColor(to3B(bgColor));
        m_backgroundSprite->setOpacity(bgColor.a);
        addChild(m_backgroundSprite);

        // scaled content size for internal layout
        auto [widthCS, heightCS] = getScaledContentSize();

        // Create a simple mod "logo" node: small square with initial
        auto createModLogo = [&](Mod* m) -> CCNode* {
            // small square background
            auto logoBG = CCScale9Sprite::create("square01_001.png");
            logoBG->setContentSize({ 40.f, 40.f });
            logoBG->ignoreAnchorPointForPosition(false);
            logoBG->setAnchorPoint({ 0.5f, 0.5f });

            // initial label (first character of name)
            std::string name = m->getName();
            std::string initial = name.empty() ? "?" : std::string(1, std::toupper(name[0]));
            auto lbl = CCLabelBMFont::create(initial.c_str(), "bigFont.fnt");
            lbl->setScale(0.6f);
            lbl->setPosition({ logoBG->getContentSize().width / 2.f, logoBG->getContentSize().height / 2.f });
            logoBG->addChild(lbl);

            return logoBG;
        };

        // Mod icon sprite / logo node
        auto modIcon = createModLogo(m_mod);
        modIcon->setID("mod-icon");
        modIcon->setScale(0.5f);
        modIcon->setPosition({ 20.f, heightCS / 2.f });
        modIcon->ignoreAnchorPointForPosition(false);
        modIcon->setAnchorPoint({ 0.5f, 0.5f });
        addChild(modIcon);

        // Mod name label
        auto nameLabel = CCLabelBMFont::create(m_mod->getName().c_str(), "bigFont.fnt");
        nameLabel->setID("mod-name");
        nameLabel->setScale(0.4f);
        nameLabel->setPosition({ 37.5f, (heightCS / 2.f) + 5.f });
        nameLabel->setAlignment(CCTextAlignment::kCCTextAlignmentLeft);
        nameLabel->setAnchorPoint({ 0, 0.5f });
        addChild(nameLabel);

        // View button (opens mod info)
        auto viewBtnSprite = ButtonSprite::create(
            "View",
            "bigFont.fnt",
            m_geodeTheme ? "geode.loader/GE_button_05.png" : "GJ_button_01.png",
            0.75f
        );
        viewBtnSprite->setScale(0.75f);

        auto viewBtn = CCMenuItemSpriteExtra::create(
            viewBtnSprite,
            this,
            menu_selector(ModItem::onViewMod)
        );
        viewBtn->setID("view-button");

        // Layout for right-side menu
        auto btnMenuLayout = RowLayout::create()
            ->setDefaultScaleLimits(0.625f, 0.875f)
            ->setAxisAlignment(AxisAlignment::End)
            ->setCrossAxisAlignment(AxisAlignment::Center)
            ->setCrossAxisLineAlignment(AxisAlignment::Center)
            ->setGrowCrossAxis(false)
            ->setAutoGrowAxis(0.f)
            ->setAxisReverse(true)
            ->setGap(5.f);

        auto btnMenu = CCMenu::create();
        btnMenu->setID("button-menu");
        btnMenu->setAnchorPoint({ 1, 0.5f });
        btnMenu->setPosition({ widthCS - 10.f, heightCS / 2.f });
        btnMenu->setScaledContentSize({ widthCS * 0.375f, heightCS - 10.f });
        btnMenu->setLayout(btnMenuLayout);

        btnMenu->addChild(viewBtn);

        // If not in "minimal" mode (respecting user setting), show developer/version/extra buttons
        if (!Mod::get()->getSettingValue<bool>("minimal")) {
            // Developers label
            auto devs = m_mod->getDevelopers();
            std::string devLabelText = devs.empty() ? "Unknown" : devs[0];
            int andMore = static_cast<int>(devs.size()) - 1;
            if (andMore > 0) devLabelText += " & " + std::to_string(andMore) + " more";

            auto devLabel = CCLabelBMFont::create(devLabelText.c_str(), "goldFont.fnt");
            devLabel->setID("mod-developers");
            devLabel->setPosition({ nameLabel->getScaledContentWidth() + 40.f, nameLabel->getPositionY() });
            devLabel->setScale(0.25f);
            devLabel->setAnchorPoint({ 0, 0.5f });
            devLabel->setAlignment(CCTextAlignment::kCCTextAlignmentLeft);
            addChild(devLabel);

            // Version label
            auto versionLabel = CCLabelBMFont::create(m_mod->getVersion().toVString().c_str(), "goldFont.fnt");
            versionLabel->setID("mod-version");
            versionLabel->setPosition({ 37.5f, (heightCS / 2.f) - 5.f });
            versionLabel->setScale(0.3f);
            versionLabel->setAnchorPoint({ 0, 0.5f });
            versionLabel->setAlignment(CCTextAlignment::kCCTextAlignmentLeft);
            addChild(versionLabel);

            // Description button (small circle)
            auto descBtnSprite = CircleButtonSprite::createWithSpriteFrameName(
                "geode.loader/message.png",
                0.875f,
                m_geodeTheme ? CircleBaseColor::DarkPurple : CircleBaseColor::Green,
                CircleBaseSize::Small
            );
            descBtnSprite->setScale(0.375f);

            auto descBtn = CCMenuItemSpriteExtra::create(
                descBtnSprite,
                this,
                menu_selector(ModItem::onModDesc)
            );
            descBtn->setID("short-description-button");
            btnMenu->addChild(descBtn);

            // Issue reports button
            auto issueBtnSprite = CCSprite::createWithSpriteFrameName("geode.loader/info-warning.png");
            issueBtnSprite->setScale(0.5f);

            auto issueBtn = CCMenuItemSpriteExtra::create(
                issueBtnSprite,
                this,
                menu_selector(ModItem::onModIssues)
            );
            issueBtn->setID("issue-reports-button");
            btnMenu->addChild(issueBtn);
        }

        addChild(btnMenu);

        // ID label (small, dim)
        auto idLabelOffset = 0.f;
        // we already added versionLabel etc above so we could compute offset; for simplicity set as-is
        auto idLabel = CCLabelBMFont::create(modID.c_str(), "bigFont.fnt");
        idLabel->setID("mod-id");
        idLabel->setPosition({ 37.5f + idLabelOffset, (heightCS / 2.f) - 5.f });
        idLabel->setScale(0.25f);
        idLabel->setAnchorPoint({ 0, 0.5f });
        idLabel->setOpacity(125);
        idLabel->setAlignment(CCTextAlignment::kCCTextAlignmentLeft);
        addChild(idLabel);

        btnMenu->updateLayout(true);

        // Show outdated / update / disabled indicators if relevant
        auto loadProblem = m_mod->targetsOutdatedVersion();
        if (loadProblem.has_value()) {
            if (loadProblem->isOutdated() && Mod::get()->getSettingValue<bool>("indicate-outdated")) {
                auto gdVer = m_mod->getMetadata().getGameVersion();
                auto reason = fmt::format("Outdated ({})", gdVer.value_or("Any"));

                auto modOutdated = CCLabelBMFont::create(reason.c_str(), "bigFont.fnt");
                modOutdated->setID("outdated-indicator");
                modOutdated->setScale(0.2f);
                modOutdated->setOpacity(200);
                modOutdated->setAnchorPoint({ 0, 0.5f });
                modOutdated->setAlignment(CCTextAlignment::kCCTextAlignmentLeft);
                modOutdated->setPosition({ 37.5f, (heightCS / 2.f) - 12.5f });
                modOutdated->setColor(colProvider->color3b("geode.loader/mod-list-outdated-label"));

                nameLabel->setOpacity(200);
                addChild(modOutdated);
            }
        } else if (m_mod->isEnabled() && Mod::get()->getSettingValue<bool>("indicate-update")) {
            // Check updates (best-effort; wrapped)
            try {
                auto update = m_mod->checkUpdates().getFinishedValue();
                if (update->unwrapOrDefault().has_value()) {
                    auto pendingUpdate = CCSprite::createWithSpriteFrameName("geode.loader/updates-available.png");
                    pendingUpdate->setID("pending-update-indicator");
                    pendingUpdate->setScale(0.375f);
                    pendingUpdate->setPosition({ btnMenu->getPositionX(), btnMenu->getPositionY() + (btnMenu->getScaledContentHeight() / 2.f) });
                    addChild(pendingUpdate);
                }
            } catch (...) {
                // ignore update-check errors
            }
        } else if (!m_mod->isEnabled() && Mod::get()->getSettingValue<bool>("indicate-disabled")) {
            auto modDisabled = CCLabelBMFont::create("Disabled", "bigFont.fnt");
            modDisabled->setID("disabled-indicator");
            modDisabled->setScale(0.2f);
            modDisabled->setOpacity(200);
            modDisabled->setAnchorPoint({ 0, 0.5f });
            modDisabled->setAlignment(CCTextAlignment::kCCTextAlignmentLeft);
            modDisabled->setPosition({ 37.5f, (heightCS / 2.f) - 12.5f });
            modDisabled->setColor({ 255, 65, 65 });
            nameLabel->setOpacity(200);
            addChild(modDisabled);
        }

        return true;
    }

    // Handler: show mod info popup (reuses Geode's ModInfoPopup)
    void onViewMod(CCObject*) {
        if (!m_mod) return;
        // Use ModInfoPopup to show details (part of Geode UI)
        // If ModInfoPopup isn't available in your SDK, you can replace this with your own info popup.
        if (auto popup = ModInfoPopup::create(m_mod)) {
            popup->show();
        } else {
            // fallback: show a simple alert with mod name + description
            auto desc = m_mod->getDescription().value_or("<cr>No description available.</c>");
            FLAlertLayer::create(m_mod->getName().c_str(), desc, "OK")->show();
        }
    }

    // Handler: short description alert
    void onModDesc(CCObject*) {
        if (!m_mod) return;
        auto desc = m_mod->getDescription().value_or("<cr>No description available.</c>");
        if (auto alert = FLAlertLayer::create(m_mod->getName().c_str(), desc, "OK")) {
            alert->show();
        }
    }

    // Handler: issue reports (simple fallback)
    void onModIssues(CCObject*) {
        if (!m_mod) return;
        // Try to show an issue-reporting UI; fallback to simple alert
        auto body = std::string("To report issues, visit the mod's repository or contact the author.");
        // If metadata contains a URL, show it
        if (auto md = m_mod->getMetadata().getHomepage(); md.has_value()) {
            body += "\n\nHomepage: " + md.value();
        }
        FLAlertLayer::create(("Report - " + m_mod->getName()).c_str(), body.c_str(), "OK")->show();
    }

    // Factory create method (keeps compatibility with original signature)
    static ModItem* create(
        Mod* mod,
        CCSize const& size,
        bool geodeTheme,
        bool heartIcons /* still accepted but unused */
    ) {
        auto ret = new ModItem();
        if (ret && ret->init(mod, size, geodeTheme, heartIcons)) {
            ret->autorelease();
            return ret;
        }
        CC_SAFE_DELETE(ret);
        return nullptr;
    }
};

// No other file dependencies, favorites removed.
// Drop this file into your project and use ModItem::create(...) where needed.
