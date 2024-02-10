#pragma once
void* __o__ChatLogFunc;

using namespace std;
void chatLogHookFunc(__int64 a1, TextHolder* msg, uint32_t a2) {
    auto message = *reinterpret_cast<TextHolder*>(reinterpret_cast<__int64>(msg) + 0x8);
    static auto player = Game::GetLocalPlayer();

    if (player == nullptr) 
        return;

    // Modules
    if (message.getText() != nullptr && player != nullptr) {
        string textStr = message.getText();
        string name = player->getNameTag()->getText();
        name = Util::sanitizeName(name);

        // Insults
        if (getModuleByName("insults")->isEnabled() && textStr.find(name) != string::npos && textStr.find("killed") != string::npos || textStr.find(name) != string::npos && textStr.find("slain") != string::npos || textStr.find("You") != string::npos && textStr.find("killed") != string::npos)
        {
            Game::killed = true;
        }

        // AutoQueue
        if (getModuleByName("autoqueue")->isEnabled() && textStr.find("§c§l» Game OVER!") != string::npos || textStr.find(name) != string::npos && textStr.find("did an oopsie!") != string::npos)
        {
            Game::sendQueue = true;
        }

        // AutoGG
        if (getModuleByName("autogg")->isEnabled() && textStr.find("§c§l» Game OVER!") != string::npos)
        {
            Game::sendGG = true;
        }

        // Kills
        if (textStr.find(name) != string::npos && textStr.find("killed") != string::npos || textStr.find(name) != string::npos && textStr.find("slain") != string::npos || textStr.find("You") != string::npos && textStr.find("killed") != string::npos)
        {
            Game::kills++;
        }
    }


    return CallFunc<void, __int64, TextHolder*, uint32_t>(
        __o__ChatLogFunc,
        a1,
        msg,
        a2
    );
}

class ChatLogHook : public FuncHook
{
public:
    static ChatLogHook& Instance()
    {
        static ChatLogHook instance;
        return instance;
    }

    bool Initialize() override
    {
        void* chatlogAddr = findSig(xorstr_("40 55 57 41 56 48 83 EC ? 48 8B 81"));
        return HookFunction(chatlogAddr, (void*)&chatLogHookFunc, &__o__ChatLogFunc, xorstr_("ChatLogFunc"));
    }
};