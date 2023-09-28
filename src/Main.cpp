#include <resources.h>
#include <iostream>
#include <fstream>
#include<shader.h>
#include <glm/ext/vector_float3.hpp>
#include <mesh.h>
#include <texture.h>
#include "window.h"
#include "yaml-cpp/yaml.h"
#include <material.h>
#define MYTEST
#include <g_comp.h>
#include <renderer.h>
#include <render_group.h>
#include <testComp.h>
#include <util.h>
#include <camera.h>
class vk_renderer;
int main(int argc, char* argv[])
{
	VoxelRendering::resources::exePath = std::string(argv[0]);
	VoxelRendering::resources::basePath = std::string(argv[0])+"/../";
	VoxelRendering::window* w = new VoxelRendering::window();
	auto instance = VoxelRendering::vk_helper::vk_instance;
	auto cameraGo = std::make_shared<VoxelRendering::g_object>();
	auto cameraTrans = cameraGo->addComp<VoxelRendering::transform>();
	cameraTrans->setPos(glm::vec3(.0f, 25.f, -50.0f));
	auto camera = cameraGo->addComp<VoxelRendering::camera>();
	VoxelRendering::vk_helper::vk_instance->allObjs.push_back(cameraGo);
	camera->init(50,500,.2f, glm::vec3(0,0,0));
	{
		auto go = std::make_shared<VoxelRendering::g_object>();
		auto r = go->addComp<VoxelRendering::renderer_comp>("assets/models/sphere.obj", "assets/materials/testmat.yaml");
		auto trans = go->addComp<VoxelRendering::transform>();
		trans->setPRS(glm::vec3(0,0,0), glm::qua(glm::vec3(VoxelRendering::util2::a2r(0), VoxelRendering::util2::a2r(0),
			VoxelRendering::util2::a2r(0))), glm::vec3(1, 1, 1) * 10.f);
		go->addComp<testComp>();
		VoxelRendering::vk_helper::vk_instance->allObjs.push_back(go);
	}
	{
		auto go = std::make_shared<VoxelRendering::g_object>();
		auto r = go->addComp<VoxelRendering::renderer_comp>("assets/models/sphere.obj", "assets/materials/skybox.yaml");
		auto trans = go->addComp<VoxelRendering::transform>();
		//go->addComp<testComp>();
		trans->setPRS(glm::vec3(0, 0, 0), glm::identity<glm::quat>(), glm::vec3(1, 1, 1) * 120.f);
		VoxelRendering::vk_helper::vk_instance->allObjs.push_back(go);
	}
	w->run();
	return 1;
}
