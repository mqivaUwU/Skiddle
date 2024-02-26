#pragma once
#include "../../../Util/Time/TimeUtil.h"

class Disabler : public Module
{
public:
    Disabler(int keybind = Keyboard::NONE, bool enabled = false) :
        Module("Disabler", "Misc", "Basic version of an anticheat disabler", keybind, enabled)
    {
        registerEnumSetting("Mode", "The mode of which the disabler will work", { "GroundSpoof", "Sentinel" }, &disablerMode);
        registerFloatSetting("CombatTick", "Attack speed", &ctm, 0, 50);
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
        case 1: // Sentinel
            if (Game::GetInstance()->mcGame->canUseMoveKeys()) {
                // Disable player movement
                player->getMovementProxy()->setSpeed(0.0f);
                player->getMovementProxy()->setJumping(false);

                // Continuously send fake movement packets to confuse the server
                for (int i = 0; i < 5; i++) {
                    MovePlayerPacket packet = MovePlayerPacket(player, player->getPosition(), player->getMovementProxy()->getRotation(), player->getMovementProxy()->isOnGround());
                    packet.mode == 2;
                    packet.onGround = true;
                    packet.tick = rand() % 1000000 + 1000985;
                    Game::GetInstance()->getPacketSender()->sendToServer(&packet);
                }
            }
            break;
        default:
            break;
        }
    }

    void onEvent(PacketSendingEvent* event) override
    {
        Packet* pkt = event->Packet;

        switch (disablerMode)
        {
        case 0: // GroundSpoof
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
        default:
            break;
        }
    }

    void onEvent(UpdateTickEvent* event) override
    {
        auto player = Game::GetLocalPlayer();
        if (player == nullptr) return;

        switch (disablerMode)
        {
        default:
            break;
        }
    }

    void onDisabled() override
    {
        // Cleanup or additional actions when the module is disabled
    }
};
