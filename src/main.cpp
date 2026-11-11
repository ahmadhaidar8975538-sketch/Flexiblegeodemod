
#include <Geode/Geode.hpp>
#include <Geode/modify/LevelEditorLayer.hpp>

#include "GeneratorCore.hpp"

#include <algorithm>
#include <cstdint>
#include <cstdlib>
#include <string>
#include <vector>

using namespace geode::prelude;

namespace {
    constexpr std::size_t MAX_OBJECTS = 40000;

    constexpr const char* DIFFICULTIES[] = {
        "Easy", "Normal", "Hard", "Harder", "Insane", "Demon",
        "Easy Demon", "Medium Demon", "Hard Demon",
        "Insane Demon", "Extreme Demon"
    };

    int g_difficulty = 0;
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
        if (!CCLayer::init())
            return false;

        m_editor = editor;

        auto size = CCDirector::sharedDirector()->getWinSize();

        auto menu = CCMenu::create();
        menu->setPosition({0.f, 0.f});

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
        if (getChildByID("fg-panel"_spr))
            return;

        showPanel();
    }

    void onClose(CCObject*) {
        if (auto panel = getChildByID("fg-panel"_spr))
            panel->removeFromParentAndCleanup(true);
    }

    void onDifficulty(CCObject*) {
        g_difficulty = (g_difficulty + 1) % 11;
        refreshPanel();
    }

    void onCreate(CCObject*) {
        if (m_generating || !m_editor)
            return;

        if (!m_bpmInput || !m_durationInput)
            return;

        const char* bpmText = m_bpmInput->getString();
        const char* durationText = m_durationInput->getString();

        if (!bpmText || !durationText ||
            bpmText[0] == '\0' || durationText[0] == '\0') {
            showError("BPM and duration cannot be empty.");
            return;
        }

        char* bpmEnd = nullptr;
        char* durationEnd = nullptr;

        unsigned long long bpm = std::strtoull(bpmText, &bpmEnd, 10);
        float duration = std::strtof(durationText, &durationEnd);

        if (bpmEnd == bpmText || *bpmEnd != '\0' ||
            durationEnd == durationText || *durationEnd != '\0' ||
            bpm == 0 || duration <= 0.f) {
            showError("Enter valid positive numbers.");
            return;
        }

        fg::GeneratorSettings settings;

        settings.length = duration * 100.f;
        settings.density = 0.55f;
        settings.difficulty =
            static_cast<float>(g_difficulty) / 10.f;
        settings.bpm = static_cast<std::uint32_t>(bpm);
        settings.maxObjects = MAX_OBJECTS;

        fg::GeneratorCore generator;
        auto result = generator.generate(settings);

        if (!result.validation.passed) {
            showError("Generation validation failed.");
            return;
        }

        m_objects = std::move(result.objects);
        m_nextObject = 0;
        m_generating = true;

        onClose(nullptr);
        showLoading();

        schedule(
            schedule_selector(FGEditorOverlay::placeObjects),
            0.01f
        );
    }

    void showLoading() {
        auto size = CCDirector::sharedDirector()->getWinSize();

        auto loading = CCLayerColor::create({0, 0, 0, 225});
        loading->setContentSize(size);
        loading->setID("fg-loading"_spr);

        auto label = CCLabelBMFont::create(
            "Generating gameplay...", "bigFont.fnt"
        );

        label->setPosition({
            size.width / 2.f,
            size.height / 2.f
        });

        label->setScale(0.65f);
        loading->addChild(label);

        addChild(loading, 300);
    }

    void placeObjects(float) {
        if (!m_editor) {
            finishGeneration();
            return;
        }

        constexpr std::size_t BATCH_SIZE = 250;

        const std::size_t end = std::min(
            m_nextObject + BATCH_SIZE,
            m_objects.size()
        );

        for (; m_nextObject < end; ++m_nextObject) {
            const auto& item = m_objects[m_nextObject];

            auto object = m_editor->createObject(
                item.objectID,
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

        std::string message =
            "Generation finished!\nObjects attempted: " +
            std::to_string(m_nextObject);

        FLAlertLayer::create(
            "FlexibleGenerator",
            message.c_str(),
            "OK"
        )->show();
    }

private:
    LevelEditorLayer* m_editor = nullptr;

    CCTextInputNode* m_bpmInput = nullptr;
    CCTextInputNode* m_durationInput = nullptr;

    std::vector<fg::GeneratedObject> m_objects;

    std::size_t m_nextObject = 0;
    bool m_generating = false;

    void showError(const char* message) {
        FLAlertLayer::create(
            "FlexibleGenerator",
            message,
            "OK"
        )->show();
    }

    void refreshPanel() {
        onClose(nullptr);
        showPanel();
    }

    void showPanel() {
        auto size = CCDirector::sharedDirector()->getWinSize();

        auto panel = CCLayerColor::create({12, 12, 12, 248});
        panel->setContentSize({380.f, 350.f});

        panel->setPosition({
            (size.width - 380.f) / 2.f,
            (size.height - 350.f) / 2.f
        });

        panel->setID("fg-panel"_spr);

        auto title = CCLabelBMFont::create(
            "FlexibleGenerator (BETA)", "bigFont.fnt"
        );

        title->setScale(0.55f);
        title->setPosition({190.f, 320.f});
        panel->addChild(title);

        auto maxLabel = CCLabelBMFont::create(
            "MAX OBJECTS: 40,000", "bigFont.fnt"
        );

        maxLabel->setScale(0.48f);
        maxLabel->setPosition({190.f, 285.f});
        panel->addChild(maxLabel);

        auto bpmLabel = CCLabelBMFont::create(
            "BPM", "bigFont.fnt"
        );
        bpmLabel->setScale(0.5f);
        bpmLabel->setPosition({90.f, 235.f});
        panel->addChild(bpmLabel);

        m_bpmInput = CCTextInputNode::create(
            140.f, 30.f, "Enter BPM", "bigFont.fnt", 16
        );
        m_bpmInput->setPosition({245.f, 235.f});
        m_bpmInput->setString("120");
        m_bpmInput->setAllowedChars("0123456789");
        panel->addChild(m_bpmInput);

        auto durationLabel = CCLabelBMFont::create(
            "DURATION (SEC)", "bigFont.fnt"
        );
        durationLabel->setScale(0.42f);
        durationLabel->setPosition({90.f, 185.f});
        panel->addChild(durationLabel);

        m_durationInput = CCTextInputNode::create(
            140.f, 30.f, "Enter seconds", "bigFont.fnt", 16
        );
        m_durationInput->setPosition({245.f, 185.f});
        m_durationInput->setString("60");
        m_durationInput->setAllowedChars("0123456789.");
        panel->addChild(m_durationInput);

        auto menu = CCMenu::create();
        menu->setPosition({0.f, 0.f});

        auto diffSprite = ButtonSprite::create(
            (std::string("Difficulty: ") +
            DIFFICULTIES[g_difficulty]).c_str()
        );

        auto diffButton = CCMenuItemSpriteExtra::create(
            diffSprite, this,
            menu_selector(FGEditorOverlay::onDifficulty)
        );

        diffButton->setPosition({190.f, 125.f});
        menu->addChild(diffButton);

        auto createSprite = ButtonSprite::create(
            "Create Generator!"
        );

        auto createButton = CCMenuItemSpriteExtra::create(
            createSprite, this,
            menu_selector(FGEditorOverlay::onCreate)
        );

        createButton->setPosition({190.f, 70.f});
        menu->addChild(createButton);

        auto closeSprite = ButtonSprite::create("X");

        auto closeButton = CCMenuItemSpriteExtra::create(
            closeSprite, this,
            menu_selector(FGEditorOverlay::onClose)
        );

        closeButton->setPosition({350.f, 325.f});
        menu->addChild(closeButton);

        panel->addChild(menu);
        addChild(panel, 200);
    }
};

class $modify(FlexibleGeneratorEditor, LevelEditorLayer) {
    bool init(GJGameLevel* level, bool noUI) {
        if (!LevelEditorLayer::init(level, noUI))
            return false;

        auto overlay = FGEditorOverlay::create(this);

        if (overlay)
            addChild(overlay, 1000);

        return true;
    }
};
