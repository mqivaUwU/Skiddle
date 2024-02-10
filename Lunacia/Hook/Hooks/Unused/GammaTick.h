#pragma once

void* __o__GammaTick;

float gammaTickDetour(__int64 _this)
{
    if (Game::Core::gamma == -1)
        return CallFunc<float, __int64>(__o__GammaTick, _this);
    else
        return Game::Core::gamma;
}

class GammaTickHook : public FuncHook
{
public:
    static GammaTickHook& Instance()
    {
        static GammaTickHook instance;
        return instance;
    }

    bool Initialize() override
    {
        void* gammaTickAddr = findSig(xorstr_("48 83 EC ? 80 B9 ? ? ? ? ? 48 8D 54 ? ? 48 8B 01 48 8B 40 ? 74 38 41 B8 16 ? ? ?"));
        return HookFunction(gammaTickAddr, (void*)&gammaTickDetour, &__o__GammaTick, xorstr_("GammaTick"));
    }
};