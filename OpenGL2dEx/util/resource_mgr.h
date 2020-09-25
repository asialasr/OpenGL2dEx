#ifndef RESOURCE_MGR_H
#define RESOURCE_MGR_H

#include "shader.h"
#include "texture_2d.h"
#include "text_renderer.h"
#include <map>

namespace util {

class ResourceManager {
public:
	using ShaderId = unsigned int;
	using Texture2DId = unsigned int;
	using FontId = unsigned int;

	// load/get shaders
	static ShaderId load_shader(const char * vertex_path, const char * fragment_path, const Optional<const char *> geometry_path);
	static const Shader   &get_shader(ShaderId shader_id);
	
	// 2D Textures
	static Texture2DId load_texture(const char *file, bool alpha);
	static const Texture2D   &get_texture(Texture2DId texture_id);

	// Fonts
	static FontId load_font(const char *font_path, 
							ShaderId               shader_id,
						    TextRenderer::FontSize font_size, 
							TextRenderer::Dimension width, 
							TextRenderer::Dimension height);
	static const TextRenderer &get_font(FontId font_id);

	// de-allocate all resources
	static void clear();
private:
	// singleton
	ResourceManager()
	{}

	static std::map<ShaderId, Shader> shaders_;
	static ShaderId                   next_shader_id_;

	static std::map<Texture2DId, Texture2D> textures_;
	static Texture2DId                      next_texture_id_;

	static std::map<FontId, TextRenderer> fonts_;
	static FontId                         next_font_id_;
};

} // namespace util

#endif // !RESOURCE_MGR_H
