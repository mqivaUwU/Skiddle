#pragma once
#include "HUD.h"
class WatermarkModule : public Module
{
private:
    Vector2<float> logoPos = Vector2<float>(-20, -20); //lmao
    bool bigwatermark = true;
    float scale = 2.6f;

public:
    bool watermark = true;

    WatermarkModule(int keybind = Keyboard::NONE, bool enabled = true) :
        Module("Watermark", "Render", "Display watermark on the screen", keybind, enabled)
    {
        registerBoolSetting("Watermark", "", &watermark);
        registerBoolSetting("Big Watermark", "", &bigwatermark);
        registerFloatSetting("Scale", "", &scale, 0.5, 4);
    }

    void onEvent(ImGUIRenderEvent* event) override
    {
        Player* player = Game::GetLocalPlayer();

        if (!player)
            return;

        if (watermark && Game::Core::showMenu)
        {
            logoPos = logoPos.animate(Vector2<float>(6.f, 6.f), logoPos, RenderUtil::getDeltaTime() * 10.f);

            Vector2<float> pos2 = logoPos;
            int ind = 0;
            float fontSize = scale;
            for (char c : (std::string)Game::Skiddle::ClientName)
            {
                std::string string = combine(c, "");

                int colorIndex = ind * 25;

                float charWidth = ImRenderUtil::getTextWidth(&string, fontSize);
                float charHeight = ImRenderUtil::getTextHeight(fontSize);

                // Use the selected client color for the watermark
                UIColor watermarkColor = ColorUtil::getClientColor(2.1, 1, 1, colorIndex * 80);

                Vector4<float> rect(pos2.x, pos2.y, pos2.x + 20, pos2.y + 20);
                ImRenderUtil::drawShadowSquare(Vector2<float>(pos2.x + charWidth / 2, pos2.y + charHeight / 1.2), 15.f, watermarkColor, 1.f, 70.f, 0);

                if (bigwatermark)
                {
                    ImRenderUtil::drawText(Vector2<float>(pos2.x + 10, pos2.y + 10), &string, watermarkColor, fontSize, 1, true);
                    ImRenderUtil::drawText(Vector2<float>(pos2.x + 11.5, pos2.y + 10), &string, watermarkColor, fontSize, 1, true);
                }
                else
                {
                    ImRenderUtil::drawText(Vector2<float>(pos2.x + 10, pos2.y + 10), &string, watermarkColor, fontSize, 1, true);
                }

                pos2.x += charWidth;
                ++ind;
            }
        }
    }

    void onEnabled() {
        logoPos = Vector2<float>(-20, -20);
    }

    void onDisabled() {
        setEnabled(true);
    }
};
