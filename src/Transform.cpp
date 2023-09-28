#include "transform.h"

_VOXEL_BEGIN
std::string Transform::name = "Transform";
void Transform::updateMatrix()
{
	Transform::matrix = Transform::localToWorld(*this);
	Transform::i_matrix = glm::inverse(Transform::matrix);
}
void Transform::setPRS(glm::vec3 pos, glm::quat quaternion, glm::vec3 scale)
{
	Transform::pos = pos;
	Transform::quaternion = quaternion;
	Transform::scale = scale;
	updateMatrix();
}
void Transform::updateUBO()
{
}
glm::vec3 Transform::get_forward()
{
	auto rot_scale_mat = (glm::mat3)matrix;
	return rot_scale_mat * glm::vec3(0,0,1);
}
Transform::Transform()
{
	Transform::pos = glm::vec3(0,0,0);
	Transform::quaternion = glm::identity<glm::quat>();
	Transform::scale = glm::vec3(1, 1, 1);;
	matrix = Transform::localToWorld(*this);
	i_matrix = glm::inverse(matrix);
}

glm::vec3 Transform::get_pos()
{
	return Transform::pos;
}

void Transform::setPos(glm::vec3 p)
{
	Transform::pos = p;
	updateMatrix();
}

glm::quat Transform::getQuat()
{
	return Transform::quaternion;
}

void Transform::setQuat(glm::quat q)
{
	Transform::quaternion = q;
	updateMatrix();
}

glm::vec3 Transform::getScale()
{
	return Transform::scale;
}

void Transform::setScale(glm::vec3 s)
{
	Transform::scale =s; 
	updateMatrix();
}
glm::mat4 Transform::get_matrix()
{
	return Transform::matrix;
}
glm::mat4 Transform::get_I_Matrix()
{
	return Transform::i_matrix;
}
_VOXEL_END