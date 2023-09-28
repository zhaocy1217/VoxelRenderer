#pragma once
#include <string>
#include "Defines.h"
#include <vector>
_VOXEL_BEGIN
class AssetItem
{
private:
	std::string path;
public:
	AssetItem(const std::string& path) :path(path) {};
	AssetItem();
	virtual void  init();
	std::string getPath();
	//virtual bool isLoadFinished();
};
_VOXEL_END