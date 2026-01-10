#include <cube_texture.h>

#include <format>

#include <stb_image.h>

#include <logger.h>

namespace vox {

CubeTexture::CubeTexture(const std::string &cubemap_path) {
	const std::string filename_list[6]{ "px.png", "nx.png", "py.png", "ny.png", "pz.png", "nz.png" };

	glCreateTextures(GL_TEXTURE_CUBE_MAP, 1, &_texture);
	glTextureStorage2D(_texture, 1, GL_RGBA8, 1024, 1024);  // HACK, hard code size for now
	glTextureParameteri(_texture, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTextureParameteri(_texture, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTextureParameteri(_texture, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTextureParameteri(_texture, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTextureParameteri(_texture, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	for (int i = 0; i < 6; i++) {
		const std::string texture_path = std::format("{}/{}", cubemap_path, filename_list[i]);

		int tex_width = 0;
		int tex_height = 0;
		int tex_n_channels = 0;
		auto *tex_src = stbi_load(texture_path.c_str(), &tex_width, &tex_height, &tex_n_channels, 4);
		if (tex_src == nullptr) {
			SPDLOG_WARN("texture \"{}\" does not exist!", texture_path);
			return;
		}

		glTextureSubImage3D(
		    _texture,
		    0,
		    0, 0, i,
		    1024, 1024, 1,
		    GL_RGBA,
		    GL_UNSIGNED_BYTE,
		    tex_src);

		stbi_image_free(tex_src);
	}
}

}  // namespace vox
