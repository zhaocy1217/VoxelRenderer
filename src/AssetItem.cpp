
#include "AssetItem.h"
_VOXEL_BEGIN

AssetItem::AssetItem()
{
}
void AssetItem::init()
{
}
//std::vector<assetItem>* assetItem::subAssetItems = nullptr;
std::string AssetItem::getPath()
{
	return AssetItem::path;
}
_VOXEL_END