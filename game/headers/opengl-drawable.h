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

class OpenGLDrawable : public Drawable {
public:
	OpenGLDrawable(std::shared_ptr<Mesh> mesh, std::shared_ptr<Shader> shader);

	void draw() const override;
private:
	std::shared_ptr<Mesh> _mesh;
	std::shared_ptr<Shader> _shader;

	unsigned int _vao;
	unsigned int _vbo;

	std::vector<OpenGLTexture> _opengl_textures;

	void setupVertices();
	void setupTextures();
public:
	static std::vector<OpenGLDrawable> createDrawablesFromModel(const Model& model, std::shared_ptr<Shader> shader);
};

#endif // OPENGL_DRAWABLE_H
