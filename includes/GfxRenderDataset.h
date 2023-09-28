#pragma once
#include<Defines.h>
#include <map>
#include <GfxTexture.h>
#include <Mesh.h>
#include <Material.h>
_VOXEL_BEGIN
class Material;
class Mesh;
class GfxRenderDataset
{

public:
	GfxRenderDataset(std::string&& mesh_path, std::string&& mat_path, RenderObjectData& rd);
	GfxRenderDataset(std::string& mesh_path, std::string& mat_path, RenderObjectData& rd);
	GfxRenderDataset(std::shared_ptr<Mesh> ms, std::shared_ptr<Material> mat, RenderObjectData& rd);
	GfxRenderDataset();
	virtual ~GfxRenderDataset();
	void reset(std::shared_ptr<Mesh> ms, std::shared_ptr<Material> mat, RenderObjectData& rd);
	void reset(std::string& mesh_path, std::string& mat_path, RenderObjectData& rd);
	void* mesh_gfx_handler;
	void* shader_gfx_handler;
	int id;
	std::function<GfxHandlerInterface* (std::string&)>get_tex_gfx_func;
	std::vector<ShaderBindingObject>* binding;
	std::map<std::string, std::vector<float>>* ubo_buffer_data;
	bool is_transparent;
	RenderObjectData rod;
};
_VOXEL_END