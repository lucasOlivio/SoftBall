#include "pch.h"

#include "GraphicsUtils.h"

#include "Engine/Graphics/Shaders/ShaderManager.h"

#include "Engine/Utils/Math.h"

namespace MyEngine
{
	void GraphicsUtils::DrawModel(const sRenderModelInfo& renderInfo)
	{
		iShaderProgram* pShader = ShaderManager::GetActiveShader();

		pShader->IsWireframe(renderInfo.isWireFrame);
		pShader->SetUniformFloat("doNotLight", renderInfo.doNotLight);
		pShader->SetUniformFloat("bUseColorTexture", renderInfo.useColorTexture);

		// Debug variables
		pShader->SetUniformFloat("bUseDefaultColor", renderInfo.useDefaultColor);
		pShader->SetUniformVec3("defaultColor", renderInfo.defaultColor);

		// Debug variables
		pShader->SetUniformFloat("bUseDebugColour", renderInfo.useDebugColor);
		pShader->SetUniformVec4("debugColourRGBA", renderInfo.debugColor);

		if (renderInfo.isDoubleSideFace)
		{
			glDisable(GL_CULL_FACE);
		}

		// Translates the matrix for every tile
		glm::mat4 matModel = renderInfo.matModel;
		for (int x = 0; x < renderInfo.tileAxis[0]; x++)
		{
			for (int y = 0; y < renderInfo.tileAxis[1]; y++)
			{
				for (int z = 0; z < renderInfo.tileAxis[2]; z++)
				{
					// Calculate the translation for the current tile
					glm::vec3 translation = glm::vec3(
						x * renderInfo.tileOffset.x,
						y * renderInfo.tileOffset.y,
						z * renderInfo.tileOffset.z
					);

					glm::mat4 tileModelMatrix = glm::translate(matModel, translation);

					pShader->SetUniformMatrix4f("matModel", tileModelMatrix);

					// Also calculate and pass the "inverse transpose" for the model matrix
					glm::mat4 matModelIT = glm::inverse(glm::transpose(tileModelMatrix));
					pShader->SetUniformMatrix4f("matModel_IT", matModelIT);

					glBindVertexArray(renderInfo.VAO_ID); //  enable VAO (and everything else)
					glDrawElements(GL_TRIANGLES,
									renderInfo.numberOfIndices,
									GL_UNSIGNED_INT,
									0);
					glBindVertexArray(0); 			  // disable VAO (and everything else)
				}
			}
		}

		if (renderInfo.isDoubleSideFace)
		{
			glEnable(GL_CULL_FACE);
		}
	}
}
