#pragma once

void* oUpdateTick;
void* __o__Attack;
void* __o__StartDestroyBlock;

void AttackDetour(GameMode* _this, Player* a1) {
    bool cancelled = false;
    AttackEvent event(a1);
    DispatchEvent(&event);
    event.cancelled = &cancelled;
    if (!cancelled)
        CallFunc<void*, GameMode*, Player*>(
            __o__Attack,
            _this,
            a1
        );
}

void startDestroyBlockDetour(void* a1, Vector3<int> const& a2, unsigned char a3, void* a4) {
    bool cancelled = false;
    StartDestroyBlockEvent event(a2, a3);
    event.cancelled = &cancelled;
    DispatchEvent(&event);
    if (!cancelled)
        CallFunc<void*, void*, Vector3<int>, unsigned char, void*>(
            __o__StartDestroyBlock,
            a1, a2, a3, a4
        );
}

void onUpdate(__int64* _this) {
    static bool hooked = false;
    if (!hooked) {
        auto player = Game::GetLocalPlayer();
        if (!!player)
        {
            GameMode* gm = player->getGameMode();

            if (!!gm) {
                auto Gamemode__vtable = *(uintptr_t**)gm;
                HookFunction(
                    (void*)Gamemode__vtable[1],
                    (void*)&startDestroyBlockDetour,
                    &__o__StartDestroyBlock,
                    "StartDestoryHook"
                );
                HookFunction(
                    (void*)Gamemode__vtable[14],
                    (void*)&AttackDetour,
                    &__o__Attack,
                    "Attack"
                );

                hooked = true;
            }
        }
    }
    bool cancelled = false;

    PreUpdateTickEvent preEvent{};
    preEvent.cancelled = &cancelled;
    DispatchEvent(&preEvent);

    CallFunc<void*, __int64*>(oUpdateTick, _this);

    UpdateTickEvent event{};
    event.cancelled = &cancelled;
    DispatchEvent(&event);
}

class onUpdateHook : public FuncHook
{
public:
    static onUpdateHook& Instance() {
        static onUpdateHook instance;
        return instance;
    }

    bool Initialize() override
    {
        void* updateTick = findSig(xorstr_("48 8b c4 48 89 58 ? 48 89 70 ? 48 89 78 ? 55 41 54 41 55 41 56 41 57 48 8d a8 ? ? ? ? 48 81 ec ? ? ? ? 0f 29 70 ? 48 8b 05 ? ? ? ? 48 33 c4 48 89 85 ? ? ? ? 4c 8b f9 45 33 ed"));
        return HookFunction(updateTick, (void*)&onUpdate, &oUpdateTick, xorstr_("Tick"));
    }
};