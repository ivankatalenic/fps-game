#ifndef SCENE_HH
#define SCENE_HH

#include "game/headers/gui/element.hh"

#include <memory>
#include <vector>

class Scene {
public:
	Scene() = default;
	~Scene() = default;

	void add(Element* element);
	void draw() const;
	void enable();
	void disable();

private:
	bool is_enabled{true};
	std::vector<Element*> elements;
};

#endif	// SCENE_HH
