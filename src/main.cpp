
#include <Geode/Geode.hpp>
#include <Geode/modify/LevelEditorLayer.hpp>
#include "GeneratorCore.hpp"

#include <algorithm>
#include <string>
#include <utility>
#include <vector>

using namespace geode::prelude;

namespace {
    constexpr int MAX_OBJECTS = 40000;

    constexpr const char* DIFFICULTIES[] = {
        "Easy", "Normal", "Hard", "Harder", "Insane", "Demon",
        "Easy Demon", "Medium Demon", "Hard Demon",
        "Insane Demon", "Extreme Demon"
    };

    int g_bpm = 120;
    int g_length = 60;
    int g_difficulty = 0;

    float difficultyValue() {
        return static_cast<float>(g_difficulty) / 10.0f;
    }
}

class FGEditorOverlay : public CCLayer {
public:
    static FGEditorOverlay* create(LevelEditorLayer* editor) {
        auto ret = new FGEditorOverlay();

        if (ret && ret->init(editor)) {
            ret->autorelease();
            return ret;
        }

        CC_SAFE_DELETE(ret);
        return nullptr;
    }

    bool init(LevelEditorLayer* editor) {
        if (!CCLayer::init()) return false;

        m_editor = editor;

        auto size = CCDirector::sharedDirector()->getWinSize();
        auto menu = CCMenu::create();
        menu->setPosition({0, 0});

        auto sprite = ButtonSprite::create(
            "FG", 40, true, "bigFont.fnt",
            "GJ_button_01.png", 30.f, 1.f
        );

        auto button = CCMenuItemSpriteExtra::create(
            sprite, this,
            menu_selector(FGEditorOverlay::onOpen)
        );

        button->setPosition({
            size.width - 38.f,
            size.height - 45.f
        });

        menu->addChild(button);
        addChild(menu, 100);

        return true;
    }

    void onOpen(CCObject*) {
        if (getChildByID("fg-panel"_spr)) return;
        showPanel();
    }

    void onClose(CCObject*) {
        if (auto panel = getChildByID("fg-panel"_spr))
            panel->removeFromParentAndCleanup(true);
    }

    void onBPM(CCObject*) {
        g_bpm += 10;
        if (g_bpm > 240) g_bpm = 60;
        refreshPanel();
    }

    void onLength(CCObject*) {
        g_length += 15;
        if (g_length > 180) g_length = 30;
        refreshPanel();
    }

    void onDifficulty(CCObject*) {
        g_difficulty = (g_difficulty + 1) % 11;
        refreshPanel();
    }

    void onCreate(CCObject*) {
        if (m_generating || !m_editor) return;

        fg::GeneratorSettings settings;

        settings.length = static_cast<float>(g_length) * 100.f;
        settings.density = 0.55f;
        settings.difficulty = difficultyValue();
        settings.bpm = static_cast<std::uint32_t>(g_bpm);
        settings.maxObjects = MAX_OBJECTS;

        fg::GeneratorCore core;
        auto result = core.generate(settings);

        if (!result.validation.passed) {
            FLAlertLayer::create(
                "FlexibleGenerator",
                "Generation validation failed.",
                "OK"
            )->show();
            return;
        }

        m_objects = std::move(result.objects);
        m_nextObject = 0;
        m_generating = true;

        onClose(nullptr);

        auto size = CCDirector::sharedDirector()->getWinSize();

        auto loading = CCLayerColor::create({0, 0, 0, 225});
        loading->setContentSize(size);
        loading->setID("fg-loading"_spr);

        auto label = CCLabelBMFont::create(
            "Generating gameplay...", "bigFont.fnt"
        );

        label->setPosition({
            size.width / 2,
            size.height / 2
        });

        label->setScale(0.65f);
        loading->addChild(label);
        addChild(loading, 300);

        schedule(
            schedule_selector(FGEditorOverlay::placeObjects),
            0.01f
        );
    }

    void placeObjects(float) {
        if (!m_editor) {
            finishGeneration();
            return;
        }

        constexpr std::size_t BATCH_SIZE = 250;

        const auto end = std::min(
            m_nextObject + BATCH_SIZE,
            m_objects.size()
        );

        for (; m_nextObject < end; ++m_nextObject) {
            const auto& item = m_objects[m_nextObject];

            int objectID = 8;

            switch (item.kind) {
                case fg::ObjectKind::Spike:
                    objectID = 8;
                    break;

                case fg::ObjectKind::Block:
                    objectID = 1;
                    break;

                case fg::ObjectKind::SmallSpike:
                    objectID = 39;
                    break;

                case fg::ObjectKind::Saw:
                    // fallback sementara
                    objectID = 8;
                    break;
            }

            auto object = m_editor->createObject(
                objectID,
                {item.x, item.y},
                true
            );

            if (object)
                object->setScale(item.scale);
        }

        if (m_nextObject >= m_objects.size())
            finishGeneration();
    }

    void finishGeneration() {
        unschedule(
            schedule_selector(FGEditorOverlay::placeObjects)
        );

        m_generating = false;

        if (auto loading = getChildByID("fg-loading"_spr))
            loading->removeFromParentAndCleanup(true);

        auto message =
            "Generation finished. Objects placed: " +
            std::to_string(m_nextObject);

        FLAlertLayer::create(
            "FlexibleGenerator",
            message.c_str(),
            "OK"
        )->show();
    }

private:
    LevelEditorLayer* m_editor = nullptr;

    std::vector<fg::GeneratedObject> m_objects;

    std::size_t m_nextObject = 0;
    bool m_generating = false;

    void refreshPanel() {
        onClose(nullptr);
        showPanel();
    }

    void showPanel() {
        auto size = CCDirector::sharedDirector()->getWinSize();

        auto panel = CCLayerColor::create({12, 12, 12, 248});

        panel->setContentSize({360.f, 330.f});

        panel->setPosition({
            (size.width - 360.f) / 2.f,
            (size.height - 330.f) / 2.f
        });

        panel->setID("fg-panel"_spr);

        auto title = CCLabelBMFont::create(
            "FlexibleGenerator (BETA)",
            "bigFont.fnt"
        );

        title->setScale(0.55f);
        title->setPosition({180.f, 300.f});
        panel->addChild(title);

        auto maxLabel = CCLabelBMFont::create(
            "MAX OBJECTS: 40,000",
            "bigFont.fnt"
        );

        maxLabel->setScale(0.48f);
        maxLabel->setPosition({180.f, 263.f});
        panel->addChild(maxLabel);

        auto menu = CCMenu::create();
        menu->setPosition({0, 0});

        auto bpmSprite = ButtonSprite::create(
            ("BPM: " + std::to_string(g_bpm)).c_str()
        );

        auto bpmButton = CCMenuItemSpriteExtra::create(
            bpmSprite, this,
            menu_selector(FGEditorOverlay::onBPM)
        );

        bpmButton->setPosition({180.f, 220.f});
        menu->addChild(bpmButton);

        auto lengthSprite = ButtonSprite::create(
            ("Length: " + std::to_string(g_length) + "s").c_str()
        );

        auto lengthButton = CCMenuItemSpriteExtra::create(
            lengthSprite, this,
            menu_selector(FGEditorOverlay::onLength)
        );

        lengthButton->setPosition({180.f, 170.f});
        menu->addChild(lengthButton);

        auto diffSprite = ButtonSprite::create(
            (std::string("Difficulty: ") +
            DIFFICULTIES[g_difficulty]).c_str()
        );

        auto diffButton = CCMenuItemSpriteExtra::create(
            diffSprite, this,
            menu_selector(FGEditorOverlay::onDifficulty)
        );

        diffButton->setPosition({180.f, 120.f});
        menu->addChild(diffButton);

        auto createSprite = ButtonSprite::create(
            "Create Generator!"
        );

        auto createButton = CCMenuItemSpriteExtra::create(
            createSprite, this,
            menu_selector(FGEditorOverlay::onCreate)
        );

        createButton->setPosition({180.f, 65.f});
        menu->addChild(createButton);

        auto closeSprite = ButtonSprite::create("X");

        auto closeButton = CCMenuItemSpriteExtra::create(
            closeSprite, this,
            menu_selector(FGEditorOverlay::onClose)
        );

        closeButton->setPosition({330.f, 305.f});
        menu->addChild(closeButton);

        panel->addChild(menu);
        addChild(panel, 200);
    }
};

class $modify(FlexibleGeneratorEditor, LevelEditorLayer) {
    bool init(GJGameLevel* level, bool noUI) {
        if (!LevelEditorLayer::init(level, noUI))
            return false;

        if (auto overlay = FGEditorOverlay::create(this))
            this->addChild(overlay, 1000);

        return true;
    }
};
