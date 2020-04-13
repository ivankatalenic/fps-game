#include "game/headers/input/mouse-handler.hh"

void MouseHandler::registerButtonHandler(Input::MouseButton button, ButtonHandler handler) {
	auto& handler_vector{button_handler_map_[button]};
	handler_vector.push_back(handler);
}

void MouseHandler::registerCursorHandler(CursorHandler handler) {
	cursor_handler_vector_.push_back(handler);
}

void MouseHandler::registerScrollHandler(ScrollHandler handler) {
	scroll_handler_vector_.push_back(handler);
}

void MouseHandler::processButton(Input::MouseButton button, Input::Action action, Input::Modifier modifier) const {
	auto search{button_handler_map_.find(button)};
	if (search == button_handler_map_.end()) {
		return;
	}
	const auto& handler_vector{search->second};
	for (auto handler : handler_vector) {
		handler(action, modifier);
	}
}

void MouseHandler::processCursor(double x_position, double y_position) const {
	for (auto handler : cursor_handler_vector_) {
		handler(x_position, y_position);
	}
}

void MouseHandler::processScroll(double x_offset, double y_offset) const {
	for (auto handler : scroll_handler_vector_) {
		handler(x_offset, y_offset);
	}
}
