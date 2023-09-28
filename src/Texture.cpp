#include "texture.h"
#include <fstream>
#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"
#include <Defines.h>
#include <VkHelper.h>

_VOXEL_BEGIN
GenRawDataTexGfxFuncPtr Texture::gen_gfx_func = nullptr;
DestroyGfxAssetFuncPtr Texture::destroy_raw_data_gfx_tex_func = nullptr;
std::shared_ptr<Texture> Texture::white = nullptr;
std::shared_ptr<Texture> Texture::black = nullptr;
Texture::Texture(const std::string& path) :AssetItem(path)
{
}
Texture::Texture() : AssetItem("UnknownTexture")
{
}
bool Texture::is_srgb_texture(std::string& name)
{
	return name == "_BaseMap" || name=="_EnvMap";
}
std::vector<IntRect> cubemapRects
{
	IntRect(2,1,1,1),IntRect(0,1,1,1),IntRect(1,0,1,1),IntRect(1,2,1,1),IntRect(1,1,1,1),IntRect(3,1,1,1)
};
void srgb2linear(int width, int height, stbi_uc* data)
{
	int pixelCount = width * height * 4;
	int curCount = 0;
	while (curCount < pixelCount)
	{
		if ((curCount + 1) % 4 == 0)
		{
			curCount++;
			continue;
		}
		auto v = *(data + curCount);
		*(data + curCount) = (stbi_uc)(pow(v / 255., 2.2f) * 255);
		curCount++;
	}
}

std::shared_ptr<Texture> Texture::load_from_uncompressed(std::shared_ptr<Texture> t, unsigned char* data, unsigned int w, unsigned int h, bool readable, bool srgb, bool cubemap)
{
	if (cubemap)
	{
		int w_cache = w;
		w /= 4;
		h /= 3;
		int facesize = w * h * 4;
		unsigned char* cubemap_data = new unsigned char[w * h * 4 * 6];
		for (size_t f = 0; f < cubemapRects.size(); f++)
		{
			auto rect = cubemapRects[f];
			rect.x = rect.x * w;
			rect.y = rect.y * h;
			rect.w = w;
			rect.h = h;
			int range = w * h * 4;
			int start = (rect.y * w_cache + rect.x) * 4;
			int newBlockStart = f * facesize;
			for (size_t i = 0; i < rect.h; i++)
			{
				for (size_t j = 0; j < rect.w; j++)
				{
					for (size_t k = 0; k < 4; k++)
					{
						auto offset = (i * w_cache + j) * 4 + k;
						auto offset2 = (i * rect.w + j) * 4 + k + newBlockStart;
						*(cubemap_data + offset2) = *(data + start + offset);
					}
				}
			}
		}
		delete data;
		data = cubemap_data;
	}
	t->base_info.cubemap = cubemap;
	t->base_info.size = glm::uvec2(w, h);
	t->base_info.channels = 4;// n;
	t->data = data;
	t->base_info.srgb = srgb;
	t->base_info.mipLevels = static_cast<unsigned int>(std::floor(std::log2(std::max(w, h)))) + 1;
	t->gen_gfx_handler();
	return t;
}
std::shared_ptr<Texture> Texture::load(const std::string& path, bool readable, bool srgb, bool cubemap)
{
	std::ifstream fin(path.c_str());
	if (!fin.fail())
	{
		fin.close();
	}
	else
	{
		return nullptr;
	}
	int width, height, n;
	stbi_set_flip_vertically_on_load(!cubemap);
	auto data = stbi_load(path.c_str(), &width, &height, &n, STBI_rgb_alpha);
	auto t = std::make_shared<Texture>(path);
	return load_from_uncompressed(t, data, width, height, readable, srgb, cubemap);
}

std::shared_ptr<Texture> Texture::load(const unsigned char* data, unsigned int length, bool readable, bool srgb, bool cubemap)
{
	int width, height, n;
	stbi_set_flip_vertically_on_load(!cubemap);
	auto unompressed_data = stbi_load_from_memory(data, length, &width, &height, &n, 4);
	auto t = std::make_shared<Texture>();
	return load_from_uncompressed(t, unompressed_data, width, height, readable, srgb, cubemap);
}
std::shared_ptr<Texture> Texture::gen(unsigned char* colors, unsigned int width, unsigned int height, bool srgb)
{
	auto t = std::make_shared<Texture>();
	return load_from_uncompressed(t, colors, width, height, true, srgb, false);
}

void* Texture::gen_gfx_handler()
{
	if (handler != nullptr)
	{
		return handler;
	}
	handler = gen_gfx_func(data, base_info);// new vk_texture(data, width, height, channels, mipLevels, cubemap, srgb);
	if (!readable)
	{
		delete[] this->data;
	}
	return handler;
}
Texture::~Texture()
{
	if (readable)
	{
		delete[] this->data;
	}
}
_VOXEL_END