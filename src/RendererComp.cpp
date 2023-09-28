#include<RendererComp.h>
_VOXEL_BEGIN
std::string RendererComp::name = "RendererComp";
RendererComp::RendererComp()
{
}
void RendererComp::init(std::string&& mat_path)
{
	this->mat_path = mat_path;
};
void RendererComp::init(std::string& mat_path) 
{
	this->mat_path = mat_path;
};
_VOXEL_END

