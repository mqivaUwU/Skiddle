#pragma once

class AntiBot : public Module
{
public:
    AntiBot(int keybind = Keyboard::NONE, bool enabled = false) :
        Module("AntiBot", "Combat", "No more bots :)", keybind, enabled)
    {}
};
