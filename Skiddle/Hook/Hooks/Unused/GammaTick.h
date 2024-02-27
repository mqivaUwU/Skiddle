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
        void* gammaTickAddr = findSig(xorstr_("8B 42 18 48 83 C4 28 C3 E8 ? ? ? ? CC CC CC CC CC 48 83 EC 28 80")); //Updated to 1.20.51
        return HookFunction(gammaTickAddr, (void*)&gammaTickDetour, &__o__GammaTick, xorstr_("GammaTick"));
    }
};