#pragma once
#include "../../../Util/Time/TimeUtil.h"

class Disabler : public Module
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

    void onEvent(UpdateEvent* event) override
    {
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
            // No need for case 1 (Sentinel) anymore
        }
    }

    void onEvent(UpdateTickEvent* event) override
    {
        auto player = Game::GetLocalPlayer();
        if (player == nullptr) return;

        switch (disablerMode)
        {
            // No need for case 1 (Sentinel) anymore
        case 2: // Flareon
            if (Game::GetInstance()->mcGame->canUseMoveKeys()) {
                MovePlayerPacket packet = MovePlayerPacket(Game::GetLocalPlayer(), Game::GetLocalPlayer()->getPosition(), Game::GetLocalPlayer()->getMovementProxy()->getRotation(), Game::GetLocalPlayer()->getMovementProxy()->isOnGround());
                packet.mode == 2;
                packet.onGround = true;
                packet.tick = rand() % 1000000 + 1000985;

                for (int i = 0; i < 2; i++)
                {
                    Game::GetInstance()->getPacketSender()->sendToServer(&packet);
                }
            }
            break;
        }
    }

    void onDisabled() override
    {
        // No need for case 1 (Sentinel) anymore
    }
};
