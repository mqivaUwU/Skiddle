#pragma once

class Aura : public Module
{
public:
    Aura(int keybind = Keyboard::NONE, bool enabled = false) :
        Module("Aura", "Combat", "Hit entities around you", keybind, enabled)
    {
        registerEnumSetting("Mode", "How many entities should be attacked", { "Single", "Multi" }, &switchMode);
        registerFloatSetting("Range", "The distance of attacking", &range, 3, 10);
        registerBoolSetting("Rotations", "Rotatew Serversidedly", &rotations);

        registerFloatSetting("Min APS", "How many times you attack in a second", &minAPS, 1, 30);
        registerFloatSetting("Max APS", "How many times you attack in a second", &maxAPS, 1, 30);

        ingameOnly = true;
        IngameOnly();
    }

    struct sortEntities
    {
        bool operator()(Player* lhs, Player* rhs)
        {
            return (lhs->GetPosition().distance(Game::GetLocalPlayer()->GetPosition()) < (rhs->GetPosition().distance(Game::GetLocalPlayer()->GetPosition())));
        }
    };

    void findEntity(Player* act)
    {
        if (!act) return;
        if (act == Game::GetLocalPlayer()) return;
        if (getModuleByName("AntiBot")->isEnabled() && act->isBot()) return;
        if (act->IsBadPtr()) return;

        float distance = act->GetPosition().distance(Game::GetLocalPlayer()->GetPosition());
        if (distance > range) return;
        if (distance <= range)
        {
            Game::TargetLists::auraList.push_back(act);
            sort(Game::TargetLists::auraList.begin(), Game::TargetLists::auraList.end(), sortEntities());
        }
    }


    // Slots
    bool switchSlot1() {
        auto supplies = Game::GetLocalPlayer()->getSupplies();
        auto inv = supplies->inventory;
        auto prevSlot = supplies->hotbarSlot;

        for (int n = 0; n < 9; n++) {
            ItemStack* stack = inv->getItem(n);
            Item* it = stack->item;
            if (stack->item) {
                if (prevSlot != n) {
                    supplies->hotbarSlot = n;
                };
                return true;
            };
        };

        return false;
    }

    int switchMode = 0;

    float range = 4;
    float minAPS = 10;
    float maxAPS = 10;
    bool rotations = true;

    inline float RandomFloat(float a, float b) {
        float random = ((float)rand()) / (float)RAND_MAX;
        float diff = b - a;
        float r = random * diff;
        return a + r;
    }

    void onEvent(UpdateEvent* event) override
    {
        auto player = Game::GetLocalPlayer();
        if (!player)
        {
            return;
        }

        Game::TargetLists::auraList.clear();
        auto callback = [this](Player* act) { findEntity(act); };
        getEntities(callback);

        GameMode* gm = player->getGameMode();

        if (!gm)
            return;


        if (!Game::TargetLists::auraList.empty())
        {
            Game::Skiddle::ShouldBlock = true;


            switchSlot1();

            if (TimeUtil::hasTimeElapsed("kaTimer", 1000 / RandomFloat(minAPS, maxAPS), true))
            {
                switch (switchMode)
                {
                case 0: // Single
                    gm->attack(Game::TargetLists::auraList[0]);
                    break;
                case 1: // Multi
                    for (auto e : Game::TargetLists::auraList)
                    {
                        gm->attack(e);
                    }
                    break;
                }
                Game::cpsCount++;
                player->swing();
            }
        }
        else
        {
            Game::Skiddle::ShouldBlock = false;
        }
    }

    void onEvent(PacketSendingEvent* event) override
    {
        auto player = Game::GetLocalPlayer();
        if (!player)
        {
            return;
        }

        if (!Game::GetLocalPlayer() || !Game::GetInstance()->getPacketSender() || !rotations || Game::TargetLists::auraList.empty())
        {
            return;
        }

        if (event->Packet->instanceOf<PlayerAuthInputPacket>())
        {
            PlayerAuthInputPacket* pkt = reinterpret_cast<PlayerAuthInputPacket*>(event->Packet);
            Vector2 angle = CalcAngle(Game::GetLocalPlayer()->GetPosition(), Game::TargetLists::auraList[0]->GetPosition());

            pkt->yaw = angle.y;
            pkt->yawUnused = angle.y;
            pkt->pitch = angle.x;
        }

        if (event->Packet->instanceOf<MovePlayerPacket>())
        {
            MovePlayerPacket* pkt = reinterpret_cast<MovePlayerPacket*>(event->Packet);
            Vector2 angle = CalcAngle(Game::GetLocalPlayer()->GetPosition(), Game::TargetLists::auraList[0]->GetPosition());

            pkt->yaw = angle.y;
            pkt->headYaw = angle.y; // i removed it
            pkt->pitch = angle.x;
        }
    }

    void onEnabled()
    {
        Game::TargetLists::auraList.clear();
    }

    void onDisabled()
    {

        Game::Skiddle::ShouldBlock = false;
        Game::TargetLists::auraList.clear();
    }

    const static Vector2<float> CalcAngle(Vector3<float> ths, Vector3<float> dst)
    {
        Vector3 diff = dst.submissive(ths);

        diff.y = diff.y / diff.magnitude();
        Vector2<float> angles;
        angles.x = asinf(diff.y) * -DEG_RAD;
        angles.y = (float)-atan2f(diff.x, diff.z) * DEG_RAD;

        return angles;
    }
};