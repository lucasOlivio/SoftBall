#include "pch.h"

#include "MaterialManager.h"
#include "Engine/Graphics/Shaders/ShaderManager.h"
#include "Engine/Graphics/Textures/TextureManagerLocator.h"
#include "Engine/ECS/Scene/SceneView.hpp"

namespace MyEngine
{
	using itMaterials = std::map<std::string, sMaterialInfo>::iterator;

	MaterialManager::MaterialManager()
	{
		m_defaultMaterial = sMaterialInfo();
		LoadMaterial("", m_defaultMaterial);
	}

	MaterialManager::~MaterialManager()
	{
	}

	void MaterialManager::BindMaterial(std::string materialName)
	{
		if (materialName == "")
		{
			UnbindMaterial();
			return;
		}

		sMaterialInfo& material = GetMaterialByName(materialName);

		// Only change material if not already binded
		if (material.name == m_currMaterial)
		{
			return;
		}
		UnbindMaterial();

		iShaderProgram* pShader = ShaderManager::GetActiveShader();
		iTextureManager* pTextureManager = TextureManagerLocator::Get();

		// Update offset, alpha and if is emissive material
		// TODO: Regulate the intensity of the emissiviness
		pShader->SetUniformInt("isEmissive", material.isEmissive);

		glm::vec3 finaloffset = material.currOffset + material.offset;
		pShader->SetUniformVec2("UVOffset", finaloffset);

		pShader->SetUniformVec2("HeightMapOffset", material.currOffsetHeightMap);

		// Bind color textures
		for (int i = 0; i < material.colorTextures.size(); i++)
		{
			pTextureManager->BindTexture(material.colorTextures[i],
										 eTextureType::COLOR,
										 material.colorTexturesRatios[i]);
		}

		// TODO: Remove repetition, all could be in a vector or map
		// Bind heightmap textures
		if (material.useHeightMap)
		{
			// Height scale can be passed as the "ratio parameter", will be placed on the heightscale uniform
			pTextureManager->BindTexture(material.heightMapTexture,
				eTextureType::HEIGHTMAP,
				material.heightScale);
		}

		// Bind normal textures
		if (material.useNormalTexture)
		{
			pTextureManager->BindTexture(material.normalTexture,
				eTextureType::NORMAL,
				0);
		}

		// Bind specular textures
		if (material.useSpecularTexture)
		{
			pTextureManager->BindTexture(material.specularTexture,
				eTextureType::SPECULAR,
				0);
		}

		// Bind discard textures
		if (material.useDiscardTexture)
		{
			pTextureManager->BindTexture(material.discardTexture,
				eTextureType::DISCARD,
				0);
		}

		// Bind alpha textures
		if (material.useAlphaTexture)
		{
			pTextureManager->BindTexture(material.alphaTexture,
				eTextureType::TRANSPARENCY,
				0);
		}

		// Bind cube textures
		if (material.useCubeTexture)
		{
			pTextureManager->BindTexture(material.cubeTexture,
										   eTextureType::CUBE,
										   0);
		}

		m_currMaterial = material.name;
	}

	void MaterialManager::UnbindMaterial()
	{
		iShaderProgram* pShader = ShaderManager::GetActiveShader();
		iTextureManager* pTextureManager = TextureManagerLocator::Get();

		pTextureManager->ResetSamplers();
		pShader->SetUniformVec2("UVOffset", glm::vec2(0.0, 0.0));
		pShader->SetUniformVec2("HeightMapOffset", glm::vec2(0.0, 0.0));
		pShader->SetUniformInt("isEmissive", false);

		m_currMaterial = "";
	}

	void MaterialManager::LoadMaterial(std::string materialName, const sMaterialInfo& materialIn)
	{
		m_materials[materialName] = materialIn;
	}

	sMaterialInfo& MaterialManager::GetMaterialByName(std::string materialName)
	{
		itMaterials it = m_materials.find(materialName);
		if (it != m_materials.end())
		{
			return it->second;
		}

		std::string warning = "Material '" + materialName + "' not found!";
		LOG_WARNING(warning);

		return m_defaultMaterial;
	}

	std::map<std::string, sMaterialInfo>& MaterialManager::GetMaterials()
	{
		return m_materials;
	}

	void MaterialManager::DeleteMaterial(std::string materialName)
	{
		m_materials.erase(materialName);
	}
}
