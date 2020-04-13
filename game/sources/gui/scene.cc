#include "game/headers/gui/scene.hh"

void Scene::add(Element* element) {
	elements.push_back(element);
}

void Scene::draw() const {
	if (!is_enabled) {
		return;
	}

	for (auto element : elements) {
		element->draw();
	}
}

void Scene::enable() {
	is_enabled = true;
}

void Scene::disable() {
	is_enabled = false;
}
