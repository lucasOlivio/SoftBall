#pragma once

#include "iMaterialManager.h"
#include <map>

namespace MyEngine
{
	class MaterialManager : public iMaterialManager
	{
	public:
		MaterialManager();
		virtual ~MaterialManager();

		virtual void BindMaterial(std::string materialName);
		virtual void UnbindMaterial();

		virtual void LoadMaterial(std::string materialName, const sMaterialInfo& materialIn);

		// Return default empty material if not found
		virtual sMaterialInfo& GetMaterialByName(std::string materialName);

		virtual std::map<std::string, sMaterialInfo>& GetMaterials();

		virtual void DeleteMaterial(std::string materialName);

	private:
		std::map<std::string, sMaterialInfo> m_materials;

		sMaterialInfo m_defaultMaterial;

		// Textures and variables in the uniforms and units at the moment
		std::string m_currMaterial;
	};
}