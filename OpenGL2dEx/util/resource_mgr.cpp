#include "resource_mgr.h"
#include "shader.h"
#include "logging.h"

#include <stb_image.h>

namespace util {

	std::map<ResourceManager::ShaderId, Shader> ResourceManager::shaders_{};
	ResourceManager::ShaderId					ResourceManager::next_shader_id_{ 0 };

	std::map<ResourceManager::Texture2DId, Texture2D> ResourceManager::textures_{};
	ResourceManager::Texture2DId					  ResourceManager::next_texture_id_{0};

	std::map<ResourceManager::FontId, TextRenderer> ResourceManager::fonts_{};
	ResourceManager::FontId							ResourceManager::next_font_id_{ 0 };

	ResourceManager::ShaderId ResourceManager::load_shader(const char * vertex_path, const char * fragment_path, const Optional<const char *> geometry_path)
	{
		const ShaderId shader_id = next_shader_id_;
		shaders_.insert(std::make_pair(next_shader_id_++, Shader{ vertex_path, fragment_path, geometry_path }));
		return shader_id;
	}

	const Shader &ResourceManager::get_shader(const ShaderId shader_id)
	{
		ASSERT(shaders_.find(shader_id) != shaders_.end(), "ResourceManager: shader not found");
		return shaders_.at(shader_id);
	}

namespace {
	Texture2D load_texture_from_file(const char *file, bool alpha)
	{
		Texture2D texture;
		if (alpha)
		{
			texture.set_internal_format(GL_RGBA);
			texture.set_image_format(GL_RGBA);
		}

		int width, height, num_channels;
		unsigned char* data = stbi_load(file, &width, &height, &num_channels, 0);
		ASSERT(data, "No data read in from image file: " + std::string{ file });

		texture.generate(width, height, data);

		stbi_image_free(data);

		return texture;
	}
}

	ResourceManager::Texture2DId ResourceManager::load_texture(const char *file, bool alpha)
	{
		const auto texture_id = next_texture_id_;
		textures_[next_texture_id_++] = load_texture_from_file(file, alpha);
		return texture_id;
	}

	const Texture2D &ResourceManager::get_texture(Texture2DId texture_id)
	{
		ASSERT(textures_.find(texture_id) != textures_.end(), "Texture ID not found");
		return textures_[texture_id];
	}

	ResourceManager::FontId ResourceManager::load_font(const char *font_path,
													   const ShaderId               shader_id,
													   const TextRenderer::FontSize font_size, 
													   const TextRenderer::Dimension width, 
													   const TextRenderer::Dimension height)
	{
		const auto font_id = next_font_id_++;
		fonts_[font_id] = TextRenderer(get_shader(shader_id), width, height);
		fonts_[font_id].load(font_path, font_size);
		return font_id;
	}

	const TextRenderer &ResourceManager::get_font(const FontId font_id)
	{
		ASSERT(fonts_.find(font_id) != fonts_.end(), "Font not found");
		return fonts_[font_id];
	}

	void ResourceManager::clear()
	{
		next_shader_id_ = 0;
		next_texture_id_ = 0;

		shaders_.clear();
		textures_.clear();
	}

} // namespace util