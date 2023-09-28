#include "util.h"
_VOXEL_BEGIN
int Util2::counter;
int Util2::getuid()
{
	return counter++;
}
int Util2::get_vec_len(std::string& type)
{
	int count = 1;
	if (type == "vec2")
	{
		count = 2;
	}
	else if (type == "vec3")
	{
		count = 3;
	}
	else if (type == "vec4")
	{
		count = 4;
	}
	return count;
}
float Util2::a2r(float a)
{
	return a / 360.f * (2 * 3.14);
}
std::size_t Util2::vec_char_hash(std::vector<char> const& vec)
{
	std::size_t seed = vec.size();
	for (auto& i : vec) {
		seed ^= i + 0x9e3779b9 + (seed << 6) + (seed >> 2);
	}
	seed ^= 0x9e3779b9 + (seed << 6) + (seed >> 2);
	return seed;
}
_VOXEL_END