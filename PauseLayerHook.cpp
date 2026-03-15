#include <Geode/Geode.hpp>
#include <Geode/modify/PauseLayer.hpp>
#include "../ControlPanel.hpp"

using namespace geode::prelude;

// ─────────────────────────────────────────────────────────────────────────────
// PauseLayer hook — adds the "zzBot" button
//
// We always create our own CCMenu so the button's position is predictable.
// Trying to reuse the existing "main-menu" fails because that menu has its
// own non-zero position, making any absolute screen coords wrong when used
// as local child coordinates.
//
// Placement: right side of the pause panel, vertically centred.
// The pause panel background is roughly 240 px wide and centred on screen,
// so winSize/2 + 155 puts us just outside its right edge, clear of all
// existing buttons.
// ─────────────────────────────────────────────────────────────────────────────

class $modify(BotPauseLayer, PauseLayer) {

    void customSetup() {
        PauseLayer::customSetup();

        auto winSize = CCDirector::get()->getWinSize();

        auto* spr = ButtonSprite::create(
            "zzBot",
            "goldFont.fnt",
            "GJ_button_06.png",
            0.8f
        );

        auto* btn = CCMenuItemSpriteExtra::create(
            spr, this,
            menu_selector(BotPauseLayer::onOpenBotPanel)
        );

        // Menu sits at the screen centre; button offset places it to the right.
        auto* menu = CCMenu::create();
        menu->setPosition(winSize / 2);
        btn->setPosition({ 155.f, 0.f });
        menu->addChild(btn);

        this->addChild(menu, 10);
    }

    void onOpenBotPanel(CCObject*) {
        ControlPanel::create()->show();
    }
};