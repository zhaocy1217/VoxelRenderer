#include "TestComp.h"
#include <VkHelper.h>
#include <chrono>
#include <Camera.h>
#include <Transform.h>
float dp = 0;
glm::quat* quatOri;
std::string TestComp::name = "TestComp";
void TestComp::update(float dt)
{
    
    uint32_t w, h;
    VoxelRendering::VkHelper::vk_instance->get_window_size(&w, &h);
    //static auto startTime = std::chrono::high_resolution_clock::now();
    //auto currentTime = std::chrono::high_resolution_clock::now();
    //float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();
    auto trans = go->get_comp<VoxelRendering::Transform>();
    trans->setPos(glm::vec3(0, 0, 0) + offset);
    if (quatOri == nullptr)
    {
        quatOri = new glm::quat();
        *quatOri = trans->getQuat();
    }
    dp += dt;
    trans->setQuat(glm::rotate(glm::identity<glm::quat>(), dp, glm::vec3(0, 1, 0)) * (*quatOri));
    auto cameratrans = VoxelRendering::Camera::main_camera->go->get_comp<VoxelRendering::Transform>();
    auto posi = cameratrans->get_pos();
    auto dir = glm::normalize(posi); 
    auto dist = glm::distance(posi, glm::vec3(0,0,0));
    auto ndir = glm::angleAxis(glm::radians(dt*30), glm::vec3(0, 1, 0)) * dir;
    auto nposi = ndir * dist;
    //posi.x += dt / 100;
    //cameratrans->setPos(nposi);
}
