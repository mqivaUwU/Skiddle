#pragma once

class Notifications : public Module
{
public:
	Notifications(int keybind = Keyboard::NONE, bool enabled = false) :
		Module("Notifications", "Render", "Display the notifications", keybind, enabled)
	{}

	void onEnabled() {
		Game::Lunacia::showNotification = true;
	}

	void onEvent(UpdateEvent* event) {
		Game::Lunacia::showNotification = true;
	}
	
	void onDisabled() {
		Game::Lunacia::showNotification = false;
	}
};