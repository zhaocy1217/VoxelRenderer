#include <iostream>
#include <fstream>
#include <GComp.h>
#include <MeshRendererComp.h>
#include <TestComp.h>
#include <Util.h>
#include <Camera.h>
#include <World.h>
#include <Resources.h>
#include <GLTFReader.h>
#include <BlurAndACESPostProcessing.h>
class MeshRendererComp;
class World;
int main(int argc, char* argv[])
{
	VoxelRendering::Resources::exePath = std::string(argv[0]);
	VoxelRendering::Resources::basePath = std::string(argv[0]) + "/../";
	VoxelRendering::World* world = VoxelRendering::World::create();
	auto cameraGo = std::make_shared<VoxelRendering::GObject>();
	auto cameraTrans = cameraGo->addComp<VoxelRendering::Transform>();
	cameraTrans->setPos(glm::vec3(.0f, 2, -2));
	cameraTrans->setQuat(glm::quatLookAtLH(glm::normalize(glm::vec3(0, 0, 1)), glm::vec3(0, 1, 0)));
	auto Camera = cameraGo->addComp<VoxelRendering::Camera>();
	Camera->init(50, 500, .01f, glm::vec3(0, 0, 0));
	world->push_object(std::move(cameraGo));
	VoxelRendering::GLTFReader::Read(world, "assets/models/ToyCar.glb");
	{
		auto go = std::make_shared<VoxelRendering::GObject>();
		auto r = go->addComp<VoxelRendering::MeshRendererComp>("assets/models/sphere.obj", "assets/materials/skybox.yaml");
		auto trans = go->addComp<VoxelRendering::Transform>();
		trans->setPRS(glm::vec3(0, 0, 0), glm::identity<glm::quat>(), glm::vec3(1, 1, 1) * 120.f);
		world->push_object(std::move(go));
	}
	world->start();
	return 1;
}
