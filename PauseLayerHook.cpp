#include <Geode/Geode.hpp>
#include <Geode/modify/PauseLayer.hpp>
#include "../ControlPanel.hpp"

using namespace geode::prelude;

class $modify(BotPauseLayer, PauseLayer) {

    // Hook create() instead of customSetup() — more reliable in GD 2.2081.
    // create() is always called and always returns a fully built PauseLayer,
    // so we can safely add children to it before it's shown.
    static PauseLayer* create(bool p0) {
        auto* ret = PauseLayer::create(p0);
        if (!ret) return ret;

        auto winSize = CCDirector::get()->getWinSize();

        auto* spr = ButtonSprite::create(
            "zzBot",
            "goldFont.fnt",
            "GJ_button_06.png",
            0.8f
        );

        auto* btn = CCMenuItemSpriteExtra::create(
            spr,
            ret,
            menu_selector(BotPauseLayer::onOpenBotPanel)
        );

        auto* menu = CCMenu::create();
        menu->setPosition(winSize / 2);
        btn->setPosition({ 155.f, 0.f });
        menu->addChild(btn);
        menu->setZOrder(10);

        ret->addChild(menu);

        return ret;
    }

    void onOpenBotPanel(CCObject*) {
        ControlPanel::create()->show();
    }
};