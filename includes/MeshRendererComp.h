#pragma once
#include<RendererComp.h>
_VOXEL_BEGIN
class MeshRendererComp:public RendererComp
{
public:
	static std::string name;
	std::string mesh_path;
	std::shared_ptr<Material> material;
	std::shared_ptr<Mesh> mesh;
	MeshRendererComp(std::shared_ptr<Material> mat, std::shared_ptr<Mesh> mesh);
	MeshRendererComp();
	MeshRendererComp(std::string& mesh_path, std::string& mat_path);
	MeshRendererComp(std::string&& mesh_path, std::string&& mat_path);
	//void init(std::string&& mesh_path, std::string&& mat_path);
	//void init(std::string& mesh_path, std::string& mat_path);
};
_VOXEL_END