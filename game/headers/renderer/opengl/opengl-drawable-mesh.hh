#ifndef OPENGL_DRAWABLE_HH
#define OPENGL_DRAWABLE_HH

#include "game/headers/renderer/drawable.hh"
#include "game/headers/model/model.hh"
#include "game/headers/model/mesh.hh"
#include "game/headers/renderer/opengl/shader.hh"

#include <memory>

struct OpenGLTexture {
	unsigned int id;
	Texture texture;
};

class OpenGLDrawableMesh : public Drawable {
public:
	OpenGLDrawableMesh(std::shared_ptr<Mesh> mesh, Shader& shader);

	void draw() const override;
private:
	std::shared_ptr<Mesh> mesh_;
	Shader& shader_;

	std::size_t vertex_count_;

	unsigned int vao_;
	unsigned int vbo_;

	std::vector<OpenGLTexture> opengl_textures_;

	void setupVertices();
	void setupTextures();
};

#endif // OPENGL_DRAWABLE_HH
