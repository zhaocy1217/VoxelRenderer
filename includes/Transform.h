#pragma once
#include <glm/gtx/quaternion.hpp>
#include <Defines.h>
#include<GObject.h>
_VOXEL_BEGIN
class Transform:public GComp
{
private:
    glm::mat4 matrix;
    glm::mat4 i_matrix;
    glm::vec3 pos;
    glm::quat quaternion = glm::identity<glm::quat>();
    glm::vec3 scale;
    void updateMatrix();
public:
    static std::string name;
    void setPRS(glm::vec3 pos, glm::quat quaternion, glm::vec3 scale);
    void updateUBO();
    glm::vec3 get_forward();
    Transform();
    glm::vec3 get_pos();
    void setPos(glm::vec3 p);

    glm::quat getQuat();
    void setQuat(glm::quat q);

    glm::vec3 getScale();
    void setScale(glm::vec3 s);

    glm::mat4  get_matrix();
    glm::mat4  get_I_Matrix();
    static glm::mat4 localToWorld(Transform& t)
    {
        glm::mat4 translation_m = glm::mat4(1.f, 0, 0, 0, 0, 1., 0, 0, 0, 0, 1, 0, t.pos.x, t.pos.y, t.pos.z, 1);
        glm::mat4 rotation_m = glm::mat4_cast(glm::normalize((t.quaternion)));
        glm::mat4 scale_m = glm::mat4(t.scale.x, 0, 0, 0, 0, t.scale.y, 0, 0, 0, 0, t.scale.z, 0, 0, 0, 0, 1);
        return translation_m * rotation_m * scale_m;
    }
};
_VOXEL_END