#pragma once
#include "Defines.h"
#include<memory>
#include "GComp.h"
#include<map>
_VOXEL_BEGIN
class GComp;
class GObject
{
	private:
		std::map<std::string, std::shared_ptr<VoxelRendering::GComp>> comps;
	public:
		void update(float dt);
		template <typename T>
		std::shared_ptr<T> get_comp()
		{
			auto rst = comps.find(T::name);
			if (rst != comps.end())
			{
				std::shared_ptr<VoxelRendering::GComp> comp = rst->second;
				return std::static_pointer_cast<T>(comp);
			}
			return nullptr;
		}

		template <class T, class... ARGS>
		std::shared_ptr<T> addComp(ARGS... args)
		{
			std::shared_ptr<T> comp = std::make_shared<T>(args...);
			std::shared_ptr<VoxelRendering::GComp> baseCls = std::static_pointer_cast<VoxelRendering::GComp>(comp);
			baseCls->go = this;
			comps[T::name] = (baseCls);
			return comp;
		}
		virtual ~GObject();
};
_VOXEL_END