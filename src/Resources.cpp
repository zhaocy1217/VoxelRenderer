#include "resources.h"
_VOXEL_BEGIN
template<class T>
std::map<size_t, std::shared_ptr<T>>Resources::res_map = {};
#define MAX_SIZE 255
std::hash<std::string> Resources::strHash;
std::string Resources::exePath;
std::string Resources::basePath;
std::string Resources::shader_compiled_path;
 std::shared_ptr<Texture> Resources::get_texture(const std::string& path, bool srgb, bool cubemap)
{
	auto hashcode = strHash(path);
	auto npath = basePath + path;
	auto res = res_map<Texture>.find(hashcode);
	if (res != res_map<Texture>.end()) {

		return res->second;
	}
	else
	{
		auto t = Texture::load(npath, false, srgb, cubemap);
		if (t != nullptr)
		{
			res_map<Texture>[hashcode] = t;
		}
		return t;
	}
	return nullptr;
}
 std::shared_ptr<Texture> Resources::get_texture(std::string& path, const unsigned char* data, unsigned int length, bool readable, bool srgb, bool cubemap)
 {
	 auto hashcode = strHash(path);
	 auto npath = basePath + path;
	 auto res = res_map<Texture>.find(hashcode);
	 if (res != res_map<Texture>.end()) {

		 return res->second;
	 }
	 else
	 {
		 auto t = Texture::load(data, length, false, srgb, cubemap);
		 if (t != nullptr)
		 {
			 res_map<Texture>[hashcode] = t;
		 }
		 return t;
	 }
	 return nullptr;
 }
std::shared_ptr<Material> Resources::get_material(const std::string& path)
{
	auto hashcode = strHash(path);
	auto res = res_map<Material>.find(hashcode);
	if (res != res_map<Material>.end()) {

		return res->second;
	}
	else
	{
		auto npath = basePath + path;
		auto t = Material::load(npath);
		if (t != nullptr)
		{
			res_map<Material>[hashcode] = t;
		}
		return t;
	}
}
std::shared_ptr<Material> Resources::get_material(uint32_t id)
{
	auto res = res_map<Material>.find(id);
	if (res != res_map<Material>.end()) {

		return res->second;
	}
	else
	{
		auto t = std::make_shared<Material>();
		res_map<Material>[id] = t;
		return t;
	}
}
 char* Resources::read_text(const std::string& path)
{
	std::ifstream t;
	auto npath = basePath + path;
	t.open(npath, std::ios_base::binary);      // open input file  
	t.seekg(0, std::ios::end);    // go to the end  
	int length = t.tellg();           // report location (this is the length)  
	t.seekg(0, std::ios::beg);    // go back to the beginning  
	auto buffer = new char[length];    // allocate memory for a buffer of appropriate dimension  
	t.read(buffer, length);       // read the whole file into the buffer  
	t.close();
	return &(buffer[0]);
}

 void Resources::unload_everything()
 {
	 Mesh::quad = nullptr;
	 Material::emptyMat = nullptr;
	 Shader::blit = nullptr;
	 Texture::white = nullptr;
	 Texture::black = nullptr;
	 res_map<Material>.clear();
	 res_map<Texture>.clear();
	 res_map<Shader>.clear();
	 res_map<Mesh>.clear();
 }

std::shared_ptr<Mesh> Resources::get_mesh(const std::string& path)
{
	auto hashcode = strHash(path);
	auto res = res_map<Mesh>.find(hashcode);
	if (res != res_map<Mesh>.end()) {

		return res->second;
	}
	else
	{
		auto npath = basePath + path;
		auto t = Mesh::load(npath);
		if (t != nullptr)
		{
			res_map<Mesh>[hashcode] = t;
		}
		return t;
	}
	return nullptr;
}
std::shared_ptr<Mesh> Resources::get_mesh(const uint32_t id, const aiMesh* src_mesh)
{
	auto res = res_map<Mesh>.find(id);
	if (res != res_map<Mesh>.end()) {

		return res->second;
	}
	else
	{
		auto t = Mesh::load(src_mesh);
		if (t != nullptr)
		{
			res_map<Mesh>[id] = t;
		}
		return t;
	}
	return nullptr;
}
std::shared_ptr<Shader> Resources::get_shader (const std::string& path)
{
	auto hashcode = strHash(path);
	std::string directory;
	std::string filename;
	const size_t last_slash_idx = path.rfind('/');
	if (std::string::npos != last_slash_idx)
	{
		directory = path.substr(0, last_slash_idx);
		filename = path.substr(last_slash_idx+1, path.size());
	}
	auto npath = basePath + directory+"/compiled/"+filename;
	auto res = res_map<Shader>.find(hashcode);
	if (res != res_map<Shader>.end()) {

		return res->second;
	}
	else
	{
		std::shared_ptr<Shader> t = Shader::load(npath);
		if (t != nullptr)
		{
			res_map<Shader>[hashcode] = t;
		}
		return t;
	}
	return nullptr;
}
void Resources::init_default_resource()
{
	auto quad = std::make_shared<Mesh>();
	quad->vertex.push_back(glm::vec3(0,0,0));
	quad->vertex.push_back(glm::vec3(1, 0, 0));
	quad->vertex.push_back(glm::vec3(1, 1, 0));
	quad->vertex.push_back(glm::vec3(0, 1, 0));
	quad->uv.push_back(glm::vec2(0, 0));
	quad->uv.push_back(glm::vec2(1, 0));
	quad->uv.push_back(glm::vec2(1, 1));
	quad->uv.push_back(glm::vec2(0, 1));
	quad->triangle.push_back(0);
	quad->triangle.push_back(1);
	quad->triangle.push_back(2);
	quad->triangle.push_back(0);
	quad->triangle.push_back(2);
	quad->triangle.push_back(3);
	Mesh::quad = quad;
	Shader::blit = Resources::get_shader("assets/shaders/blit");
	Material::emptyMat = std::make_shared<Material>();
	Texture::white = std::make_shared<Texture>();
	auto colors = new unsigned char[4*4]{ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0 };
	Texture::black = Texture::gen(colors,2,2,true);
	auto white_colors = new unsigned char[4 * 4]{127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127 };
	Texture::white = Texture::gen(white_colors, 2, 2, true);

}
_VOXEL_END