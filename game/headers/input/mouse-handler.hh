#ifndef MOUSE_HANDLER_HH
#define MOUSE_HANDLER_HH

#include "game/headers/input/key.hh"

#include <unordered_map>
#include <vector>
#include <functional>

class MouseHandler {
public:
	using ButtonHandler = std::function<void(Input::Action, Input::Modifier)>;
	using CursorHandler = std::function<void(double, double)>;
	using ScrollHandler = std::function<void(double, double)>;

	MouseHandler() = default;
	~MouseHandler() = default;

	void registerButtonHandler(Input::MouseButton button, ButtonHandler handler);
	void registerCursorHandler(CursorHandler handler);
	void registerScrollHandler(ScrollHandler handler);

	void processButton(Input::MouseButton button, Input::Action action, Input::Modifier modifier) const;
	void processCursor(double x_position, double y_position) const;
	void processScroll(double x_offset, double y_offset) const;
private:
	std::unordered_map<Input::MouseButton, std::vector<ButtonHandler>> button_handler_map_;
	std::vector<CursorHandler> cursor_handler_vector_;
	std::vector<ScrollHandler> scroll_handler_vector_;
};

#endif // MOUSE_HANDLER_HH
