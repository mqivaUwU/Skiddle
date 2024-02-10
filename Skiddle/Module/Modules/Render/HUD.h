#pragma once

class HUD : public Module
{
private:
    Vector2<float> logoPos = Vector2<float>(-20, -20); //lmao

    int delay0;
    int delay = 400;

    int onece1 = 0;

public:
    int colortype = 1;
    bool bigwatermark = true;
    bool watermark = true;
    float scale = 2.6f;

    HUD(int keybind = Keyboard::NONE, bool enabled = true) :
        Module("HUD", "Render", "Display info on the screen", keybind, enabled)
    {
        registerEnumSetting("Mode", "The type of mode", { "Rainbow", "Wave", "Astolfo" }, &colortype);
        registerFloatSetting("R", "", &r, 0, 255);
        registerFloatSetting("G", "", &g, 0, 255);
        registerFloatSetting("B", "", &b, 0, 255);

        registerFloatSetting("R2", "", &r2, 0, 255);
        registerFloatSetting("G2", "", &g2, 0, 255);
        registerFloatSetting("B2", "", &b2, 0, 255);
        registerBoolSetting("Big Watermark", "", &bigwatermark);
        registerBoolSetting("Watermark", "", &watermark);
        registerFloatSetting("Scale", "", &scale, 0.5, 4);
    }

    float r = 120, g = 35, b = 255;
    float r2 = 0, g2 = 0, b2 = 0;
    /*
    float r = 255, g = 123, b = 0;
    float r2 = 255, g2 = 255, b2 = 255;*/


    void onEvent(UpdateTickEvent* event) 
    {
        switch (colortype) {
        case 0: // rainbow
            ColorUtil::setClientColorType(0);
            ColorUtil::setClientCustomColor(255, 255, 255);
            ColorUtil::setClientCustomColor2(255, 255, 255);
            break;
        case 1: // wave
            ColorUtil::setClientColorType(1);
            ColorUtil::setClientCustomWaveColor(r, g, b);
            ColorUtil::setClientCustomWaveColor2(r2, g2, b2);
            break;
        case 2: // astolfo
            ColorUtil::setClientColorType(2);
            break;
        }
    }


    void onEvent(ImGUIRenderEvent* event) override
    {
        Player* player = Game::GetLocalPlayer();

        if (!player)
            return;

        std::ostringstream oss;
        oss.precision(2);
        oss << std::fixed << player->getHorizontalSpeed();

        if (Game::Core::showMenu) {
            drawTile(combine("FPS: ", Game::gameFps), 1);
            drawTile(combine("BPS: ", oss.str().c_str()), 0);
        }
    

        /// Watermark
        if (watermark && Game::Core::showMenu) {
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
                Vector4<float> rect(pos2.x, pos2.y, pos2.x + 20, pos2.y + 20);
                ImRenderUtil::drawShadowSquare(Vector2<float>(pos2.x + charWidth / 2, pos2.y + charHeight / 1.2), 15.f, ColorUtil::getClientColor(2.1, 1, 1, colorIndex * 80), 1.f, 70.f, 0);
                // ImRenderUtil::drawShadowSquare(Vector2<float>(pos2.x + charWidth / 2, pos2.y + charHeight / 1.2), 15.f, ColorUtil::getClientColor(3, 1, 1, -colorIndex * 85), 1.f, 70.f, 0);
                if (bigwatermark) {
                    ImRenderUtil::drawText(Vector2<float>(pos2.x + 10, pos2.y + 10), &string, ColorUtil::getClientColor(2.1, 1, 1, colorIndex * 80), fontSize, 1, true);
                    ImRenderUtil::drawText(Vector2<float>(pos2.x + 11.5, pos2.y + 10), &string, ColorUtil::getClientColor(2.1, 1, 1, colorIndex * 80), fontSize, 1, true);
                }
                else {
                    ImRenderUtil::drawText(Vector2<float>(pos2.x + 10, pos2.y + 10), &string, ColorUtil::getClientColor(2.1, 1, 1, colorIndex * 80), fontSize, 1, true);
                }
              
                pos2.x += charWidth;
                ++ind;
            }
        }



        // Anti Piracy Screen lol
        if (Game::detectedleaks) {
            Util::downloadFile("Error.wav", "", "Assets\\Sounds\\Client\\");
            Util::downloadFile("Error0.wav", "", "Assets\\Sounds\\Client\\");
            Util::downloadFile("AntiPiracyScreen.png", "", "Assets\\Images\\");
            Util::downloadFile("AntiPiracyScreen2.png", "", "Assets\\Images\\");

            if (delay0 > delay) {
                if (onece1 == 0) {
                    Util::systemPlay("Sounds\\Client\\Error.wav");
                    Game::GetInstance()->releaseCursor();
                    onece1++;
                }

                Game::Core::showMenuBackground = false;
                Game::Skiddle::showNotification = false;
                Game::Core::showAntiPiracyScreen1 = true;
                Game::Core::showMenu = false;
            }
            else {
                delay0++;
            }
        }
    }

    void drawTile(std::string tileDescr, int index = 2)
    {
        Vector2<float> tilePos = Vector2<float>(5, (Game::Core::ClientInstance->getGuiData()->WindowResolution2.y - 25) - (index * 16.f));
        Vector4<float> rectPos = Vector4<float>(5, (Game::Core::ClientInstance->getGuiData()->WindowResolution2.y - 15) - (index * 16.f), 60, (Game::Core::ClientInstance->getGuiData()->WindowResolution2.y - 15) - (index * 16.f));
        ImRenderUtil::fillShadowRectangle(rectPos, UIColor(0, 0, 0), 0.8f, 50.f, 0);
        ImRenderUtil::drawText(tilePos, &tileDescr, UIColor(255, 255, 255), 1.2f, 1, true);

        float width = ImRenderUtil::getTextWidth(&tileDescr, 1);

        if (size.x < width)
            size.x = width;

        location = tilePos;
        size.y = (index + 1) * 10;
    }

    void onEnabled() {
        logoPos = Vector2<float>(-20, -20);
    }

    void onDisabled()
    {
        setEnabled(true);
    }
};