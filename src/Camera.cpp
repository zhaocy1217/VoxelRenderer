#include "Camera.h"
_VOXEL_BEGIN
std::string Camera::name = "Camera";
Camera* Camera::main_camera = nullptr;
void Camera::init(float fov, float far, float near, glm::vec3 lookatTarget)
{
	this->fov = fov;
	this->far = far;
	this->near = near;
	this->lookatTarget = lookatTarget;
	main_camera = this;
	updateMat();
}
void Camera::updateMat()
{
	uint32_t w, h;
	VoxelRendering::VkHelper::vk_instance->get_window_size(&w, &h);
	auto trans = go->get_comp<VoxelRendering::Transform>();
	auto target = trans->get_forward();
	view = glm::lookAt(trans->get_pos(), target, glm::vec3(0.0f, 1.0f, .0f));
	//view[0][0] *= -1;
	proj = glm::perspective(glm::radians(fov), w / (float)h, near, far);
	proj[1][1] *= -1;
}
void Camera::setFov(float fov)
{
	this->fov = fov;
	updateMat();
}

glm::mat4 Camera::get_view_mat()
{
	return view;
}

glm::mat4 Camera::get_proj_mat()
{
	return proj;
}
void Camera::update(float dt)
{
	updateMat();
}
_VOXEL_END

