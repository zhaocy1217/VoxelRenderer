#pragma once
#include<Defines.h>
#include<GComp.h>
#include<VkHelper.h>
_VOXEL_BEGIN
class Camera :public VoxelRendering::GComp
{
private:
	glm::vec3 lookatTarget;
	alignas(16) glm::mat4 view;
	alignas(16) glm::mat4 proj;
	void updateMat();
public:
	float fov, far, near;
	static std::string name;
	static Camera* main_camera;
	void init(float fov, float far, float near, glm::vec3 lookatTarget);
	void setFov(float fov);
	glm::mat4 get_view_mat();
	glm::mat4 get_proj_mat();
	void update(float dt) override;
};
_VOXEL_END