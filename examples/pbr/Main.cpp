#include <iostream>
#include <fstream>
#include <GComp.h>
#include <MeshRendererComp.h>
#include <TestComp.h>
#include <Util.h>
#include <Camera.h>
#include <World.h>
#include <Resources.h>
#include <GUIControlComp.h>
#include <GLTFReader.h>
class MeshRendererComp;
class World;
int main(int argc, char* argv[])
{
	VoxelRendering::Resources::exePath = std::string(argv[0]);
	VoxelRendering::Resources::basePath = std::string(argv[0]) + "/../";
	VoxelRendering::World* world = VoxelRendering::World::create();
	auto cameraGo = std::make_shared<VoxelRendering::GObject>();
	auto cameraTrans = cameraGo->addComp<VoxelRendering::Transform>();
	cameraTrans->setPos(glm::vec3(.0f, 25.f, -50.0f));
	auto Camera = cameraGo->addComp<VoxelRendering::Camera>();
	world->push_object(std::move(cameraGo));
	VoxelRendering::BlurAndACESPostProcessing* pp = new VoxelRendering::BlurAndACESPostProcessing();
	world->post_processing = pp;
	Camera->init(50, 500, .2f, glm::vec3(0, 0, 0));
	{
		auto go = std::make_shared<VoxelRendering::GObject>();
		auto r = go->addComp<VoxelRendering::MeshRendererComp>("assets/models/sphere.obj", "assets/materials/testmat.yaml");
		auto trans = go->addComp<VoxelRendering::Transform>();
		trans->setPRS(glm::vec3(0, 0, 0), glm::qua(glm::vec3(VoxelRendering::Util2::a2r(0), VoxelRendering::Util2::a2r(0),
			VoxelRendering::Util2::a2r(0))), glm::vec3(1, 1, 1) * 10.f);
		go->addComp<TestComp>();
		world->push_object(std::move(go));
	}
	{
		auto go = std::make_shared<VoxelRendering::GObject>();
		auto r = go->addComp<VoxelRendering::MeshRendererComp>("assets/models/sphere.obj", "assets/materials/skybox.yaml");
		auto trans = go->addComp<VoxelRendering::Transform>();
		//go->addComp<testComp>();
		trans->setPRS(glm::vec3(0, 0, 0), glm::identity<glm::quat>(), glm::vec3(1, 1, 1) * 120.f);
		world->push_object(std::move(go));
	}
	{
		auto go = std::make_shared<VoxelRendering::GObject>();
		auto r = go->addComp<GUIControlComp>();
		r->pp = pp;
		world->gui_func = std::bind(&GUIControlComp::gui, r);
		world->push_object(std::move(go));
	}
	world->start();
	delete pp;
	return 1;
}
