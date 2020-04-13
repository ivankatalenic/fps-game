#include "game/headers/input/keyboard-handler.hh"

void KeyboardHandler::registerKeyHandler(Input::Key key, KeyHandler handler) {
	auto& handler_vector{handler_map_[key]};
	handler_vector.push_back(handler);
}

void KeyboardHandler::processKey(Input::Key key, Input::Action action, Input::Modifier modifier) const {
	auto search{handler_map_.find(key)};
	if (search == handler_map_.end()) {
		return;
	}
	const auto& handler_vector{search->second};
	for (auto handler : handler_vector) {
		handler(action, modifier);
	}
}
