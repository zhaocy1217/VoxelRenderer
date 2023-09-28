#include<GfxRenderDataset.h>
#include <Resources.h>
_VOXEL_BEGIN
GfxRenderDataset::GfxRenderDataset()
{
};
GfxRenderDataset::GfxRenderDataset(std::shared_ptr<Mesh> ms, std::shared_ptr<Material> mat, RenderObjectData& rd)
{
	reset(ms, mat, rd);
};
GfxRenderDataset::~GfxRenderDataset()
{
}
void GfxRenderDataset::reset(std::shared_ptr<Mesh> ms, std::shared_ptr<Material> mat, RenderObjectData& rd)
{
	id = Util2::getuid();
	ms->gen_gfx_handler(mat->shader->vertex_inputs.inputs);
	mesh_gfx_handler = ms->handler;
	shader_gfx_handler = mat->shader->handler;
	ubo_buffer_data = &(mat->ubo_buffer_data);
	get_tex_gfx_func = std::bind(&(Material::get_gfx_tex_handler), mat, std::placeholders::_1);
	binding = &(mat->shader->resources_bindings);
	is_transparent = mat->render_type == RenderType::TRANSPARENT;
	rod = rd;
}
void GfxRenderDataset::reset(std::string& mesh_path, std::string& mat_path, RenderObjectData& rd)
{
	auto mat = Resources::get_material(mat_path);
	auto ms = Resources::get_mesh(mesh_path);
	this->reset(ms, mat, rd);
}
GfxRenderDataset::GfxRenderDataset(std::string&& mesh_path, std::string&& mat_path, RenderObjectData& rd)
{
	auto mat = Resources::get_material(mat_path);
	auto ms = Resources::get_mesh(mesh_path);
	this->reset(ms, mat, rd);
};
GfxRenderDataset::GfxRenderDataset(std::string& mesh_path, std::string& mat_path, RenderObjectData& rd)
{
	auto mat = Resources::get_material(mat_path);
	auto ms = Resources::get_mesh(mesh_path);
	this->reset(ms, mat, rd);
}

_VOXEL_END
