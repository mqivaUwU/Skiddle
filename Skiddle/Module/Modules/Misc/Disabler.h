
#pragma once
#include "../../../Util/Time/TimeUtil.h"

class Disabler : public Module // this wont be fun to keep clean
{
public:
    Disabler(int keybind = Keyboard::NONE, bool enabled = false) :
        Module("Disabler", "Misc", "Basic version of an anticheat disabler", keybind, enabled)
    {
        registerEnumSetting("Mode", "The mode of which the disabler will work", { "OnGround", "Sentinel", "Flareon" }, &disablerMode);
        registerFloatSetting("CombatTick", "Attack speed", &ctm, 1, 50);
        IngameOnly();
    }

    int disablerMode = 0;
    float ctm = 1;

    void onEvent(UpdateEvent* event) {
        auto player = Game::GetLocalPlayer();
        if (player == nullptr) return;

        switch (disablerMode)
        {
        case 2: // Flareon
            GameMode * gm = player->getGameMode();

            if (TimeUtil::hasTimeElapsed("CTM", (1000 / ctm), true))
            {
                gm->attack(gm->LocalPlayer);
            }
            break;
        }
    }

    void onEvent(PacketSendingEvent* event) override
    {
        Packet* pkt = event->Packet;

        switch (disablerMode)
        {
        case 0: // OnGround
            if (pkt->instanceOf<MovePlayerPacket>())
            {
                MovePlayerPacket* packet = reinterpret_cast<MovePlayerPacket*>(pkt);
                packet->onGround = true;
            }
            if (pkt->instanceOf<PlayerAuthInputPacket>() && pkt->instanceOf<MovePlayerPacket>())
            {
                MovePlayerPacket* packet = reinterpret_cast<MovePlayerPacket*>(pkt);
                packet->onGround = true;
                packet->mode == 1;
                packet->tick == 1;
            }
            break;
        case 1: // Sentinel
            if (pkt->instanceOf<NetworkStackLatencyPacket>())
            {
                *event->cancelled = true;
            }
            if (pkt->instanceOf<PlayerAuthInputPacket>()) {
                MovePlayerPacket pkt(Game::GetLocalPlayer(), Game::GetLocalPlayer()->getPosition(), Game::GetLocalPlayer()->getMovementProxy()->getRotation(), Game::GetLocalPlayer()->getMovementProxy()->isOnGround());
                for (int i = 0; i < 4; i++) {
                    Game::GetInstance()->getPacketSender()->sendToServer(&pkt);
                    *event->cancelled = false;
                }
            }
            break;
        }
    }

    void onEvent(UpdateTickEvent* event) override
    {
        ClientInstance* instance = Game::GetInstance();
        LoopbackPacketSender* sender = instance->getPacketSender();

        if (!sender || !instance || !Game::GetLocalPlayer())
        {
            return;
        }


        auto player = Game::GetLocalPlayer();
        if (player == nullptr) return;


        switch (disablerMode)
        {
        case 1: // Sentinel
            if (TimeUtil::hasTimeElapsed("gmTick", 50, true))
            {
                /*
                MovePlayerPacket pkt(Game::GetLocalPlayer(), Game::GetLocalPlayer()->getPosition(), Game::GetLocalPlayer()->getMovementProxy()->getRotation(), Game::GetLocalPlayer()->getMovementProxy()->isOnGround());
                pkt.mode = TELEPORT;
                for (int i = 0; i < 50; i++)
                {
                    sender->sendToServer(&pkt);
                }*/
            }

            // Sleep and skips
            if (player->getMovementProxy()->isOnGround()) {
                Sleep(1.5);
            }
            else {
                static auto lastTime = std::chrono::high_resolution_clock::now();
                auto currentTime = std::chrono::high_resolution_clock::now();
                if (std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - lastTime).count() < 5.f) {
                    Game::Skiddle::skipTicks = true;
                }
                else {
                    Game::Skiddle::skipTicks = false;
                    lastTime = std::chrono::high_resolution_clock::now();
                }
            }
            break;
        case 2: // Flareon
            if (Game::GetInstance()->mcGame->canUseMoveKeys()) {
                MovePlayerPacket packet = MovePlayerPacket(Game::GetLocalPlayer(), Game::GetLocalPlayer()->getPosition(), Game::GetLocalPlayer()->getMovementProxy()->getRotation(), Game::GetLocalPlayer()->getMovementProxy()->isOnGround());
                packet.mode == 2;
                packet.onGround = true;
                packet.tick = rand() % 1000000 + 1000985;
                // packet.tick = rand() % 4000000 + 4000985;

                for (int i = 0; i < 2; i++)
                {
                    Game::GetInstance()->getPacketSender()->sendToServer(&packet);
                }
            }
            break;
        }
    }

    void onDisabled() {
        switch (disablerMode) {
        case 1: // Sentinel
            Game::Skiddle::skipTicks = false;
            break;
        }
    }
};