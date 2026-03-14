#include <Geode/Geode.hpp>
#include <Geode/modify/PauseLayer.hpp>
#include "../ControlPanel.hpp"

using namespace geode::prelude;

// ─────────────────────────────────────────────────────────────────────────────
// PauseLayer hook — adds the "Bot Toolkit" button
// ─────────────────────────────────────────────────────────────────────────────

class $modify(BotPauseLayer, PauseLayer) {

    void customSetup() {
        PauseLayer::customSetup();
        addBotButton();
    }

private:

    void addBotButton() {
        // Build a compact sprite button using a standard GD button frame.
        auto* spr = ButtonSprite::create(
            "Bot Toolkit",
            "goldFont.fnt",
            "GJ_button_06.png",
            0.7f
        );

        auto* btn = CCMenuItemSpriteExtra::create(
            spr, this,
            menu_selector(BotPauseLayer::onOpenBotPanel)
        );

        // Position it below the standard pause menu buttons on the right side.
        // The pause menu's main layer is 240 units wide; we anchor to the
        // right side to avoid overlapping existing buttons.
        auto winSize = CCDirector::get()->getWinSize();

        btn->setPosition({
            winSize.width * 0.5f + 140.f,
            winSize.height * 0.5f - 20.f
        });

        // The pause layer's menu is named "main-menu" in Geode.
        if (auto* menu = this->getChildByID("main-menu"))
            menu->addChild(btn);
        else {
            // Fallback: add to the layer directly with our own menu.
            auto* fallbackMenu = CCMenu::create();
            fallbackMenu->setPosition(CCPointZero);
            fallbackMenu->addChild(btn);
            this->addChild(fallbackMenu, 10);
        }
    }

    void onOpenBotPanel(CCObject*) {
        ControlPanel::create()->show();
    }
};
