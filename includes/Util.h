#pragma once
#include "Defines.h"
_VOXEL_BEGIN
class Util2
{
private:
	static int counter;
public:
	int static getuid();
	int static get_vec_len(std::string& type);
	float static a2r(float a);
	std::size_t static vec_char_hash(std::vector<char> const& vec);
};
_VOXEL_END