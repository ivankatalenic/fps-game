#ifndef KEYBOARD_HANDLER_HH
#define KEYBOARD_HANDLER_HH

#include "game/headers/input/key.hh"

#include <unordered_map>
#include <vector>
#include <functional>

class KeyboardHandler {
public:
	using KeyHandler = std::function<void(Input::Action, Input::Modifier)>;

	KeyboardHandler() = default;
	~KeyboardHandler() = default;

	void registerKeyHandler(Input::Key key, KeyHandler func);
	void processKey(Input::Key key, Input::Action action, Input::Modifier modifier) const;
private:
	std::unordered_map<Input::Key, std::vector<KeyHandler>> handler_map_;
};

#endif // KEYBOARD_HANDLER_HH
