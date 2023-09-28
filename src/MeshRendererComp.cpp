#include<MeshRendererComp.h>
#include <Resources.h>
_VOXEL_BEGIN
std::string MeshRendererComp::name = "MeshRendererComp";
MeshRendererComp::MeshRendererComp()
{

}
MeshRendererComp::MeshRendererComp(std::shared_ptr<Material> mat, std::shared_ptr<Mesh> mesh)
{
	this->material = mat;
	this->mesh = mesh;
}
MeshRendererComp::MeshRendererComp(std::string&& mesh_path, std::string&& mat_path)
{
	this->mesh_path = mesh_path;
	this->mat_path = mat_path;
	this->material = Resources::get_material(mat_path);
	this->mesh = Resources::get_mesh(mesh_path);
}
MeshRendererComp::MeshRendererComp(std::string& mesh_path, std::string& mat_path)
{
	this->mesh_path = mesh_path;
	this->mat_path = mat_path;
	this->material = Resources::get_material(mat_path);
	this->mesh = Resources::get_mesh(mesh_path);
}
//void MeshRendererComp::init(std::string&& mesh_path, std::string&& mat_path) {
//	this->mesh_path = mesh_path;
//	this->mat_path = mat_path;
//};
//void MeshRendererComp::init(std::string& mesh_path, std::string& mat_path) {
//	this->mesh_path = mesh_path;
//	this->mat_path = mat_path;
//};
_VOXEL_END