#ifndef OPENGL_DRAWABLE_H
#define OPENGL_DRAWABLE_H

#include <drawable.h>

#include <memory>

#include <model.h>
#include <mesh.h>
#include <shader.h>

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

	unsigned int vao_;
	unsigned int vbo_;

	std::vector<OpenGLTexture> opengl_textures_;

	void setupVertices();
	void setupTextures();
};

#endif // OPENGL_DRAWABLE_H
