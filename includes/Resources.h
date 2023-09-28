#pragma once
#include <string>
#include <map>
#include <Texture.h>
#include "Defines.h"
#include <Material.h>
#include <Mesh.h>
#include <Shader.h>
#include <direct.h>
_VOXEL_BEGIN
class Resources
{
private:
	template<class T>
	static std::map<size_t, std::shared_ptr<T>> res_map;
public :
	static std::string exePath;
	static std::string basePath;
	static std::string shader_compiled_path;
	static std::hash<std::string> strHash;
	static std::shared_ptr<Texture> get_texture(const std::string& path, bool srgb, bool cubemap);
	static std::shared_ptr<Texture> get_texture(std::string& path, const unsigned char* data, unsigned int length, bool readable, bool srgb, bool cubemap);
	static std::shared_ptr<Material> get_material(const std::string& path);
	static std::shared_ptr<Material> get_material(uint32_t id);
	static std::shared_ptr<Mesh> get_mesh(const std::string& path);
	static std::shared_ptr<Mesh> get_mesh(const uint32_t id, const aiMesh* src_mesh);
	static std::shared_ptr<Shader> get_shader(const std::string& path);
	static void init_default_resource();
	static char* read_text(const std::string& path);
	static void unload_everything();
};
static std::vector<char> readTextFile(const std::string& path)
{
	std::ifstream file(path, std::ios::ate | std::ios::binary);
	if (!file.is_open())
	{
		throw std::runtime_error("failed to open file!");
	}
	size_t fileSize = (size_t)file.tellg();
	std::vector<char> buffer(fileSize);
	file.seekg(0);
	file.read(buffer.data(), fileSize);
	file.close();
	return buffer;
}

static std::string* readFileIntoString(const std::string& path) {
	std::ifstream input_file(path);
	if (!input_file.is_open()) {
		std::cerr << "Could not open the file - '"
			<< path << "'" << std::endl;
		exit(EXIT_FAILURE);
	}
	return new std::string((std::istreambuf_iterator<char>(input_file)), std::istreambuf_iterator<char>());
}
_VOXEL_END