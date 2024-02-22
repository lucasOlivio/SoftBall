#pragma once

#include "Engine/ECS/Scene/Scene.h"
#include "Engine/Graphics/GraphicsProperties.h"
#include <string>

namespace MyEngine
{
	class iMaterialManager
	{
	public:
		iMaterialManager() {};
		virtual ~iMaterialManager() {};

		virtual void BindMaterial(std::string materialName) = 0;
		virtual void UnbindMaterial() = 0;

		virtual void LoadMaterial(std::string materialName, const sMaterialInfo& material) = 0;

		// Returns default empty material if not found
		virtual sMaterialInfo& GetMaterialByName(std::string materialName) = 0;

		virtual std::map<std::string, sMaterialInfo>& GetMaterials() = 0;

		virtual void DeleteMaterial(std::string materialName) = 0;
	};
}