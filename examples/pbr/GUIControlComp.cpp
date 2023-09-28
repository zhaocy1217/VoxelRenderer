#include<GUIControlComp.h>
#include<Camera.h>
#include<Transform.h>
#include "../../third/imgui/imgui.h"
std::string GUIControlComp::name = "GUIControlComp";
void GUIControlComp::gui()
{
	ImGui::Begin("PostProcess:");
	ImGui::SetWindowSize({150,100});
	ImGui::Checkbox("Blur", &show_blur);
	ImGui::Checkbox("ACES", &show_aces);
	pp->blur = show_blur;
	pp->aces = show_aces;
	ImGui::End();
	auto mainCamera = VoxelRendering::Camera::main_camera;
	auto trans = mainCamera->go->get_comp<VoxelRendering::Transform>();
	ImGui::Begin("Camera:");
	//ImGui::SetWindowSize({ 150,100 });
	auto cur_posi = trans->get_pos();
	posi[0] = cur_posi.x; posi[1] = cur_posi.y; posi[2] = cur_posi.z;
	ImGui::InputFloat3("Position", posi);
	trans->setPos(glm::vec3(posi[0], posi[1], posi[2]));

	auto cur_quat = trans->getQuat();
	auto euler = glm::eulerAngles(cur_quat);
	rot[0] = euler.x; rot[1] = euler.y; rot[2] = euler.z;
	ImGui::InputFloat3("Rot", rot);
	trans->setQuat(glm::quat(glm::vec3(rot[0], rot[1], rot[2])));
	ImGui::End();
}