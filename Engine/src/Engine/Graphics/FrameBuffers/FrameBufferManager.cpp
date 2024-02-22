#include "pch.h"

#include "FrameBufferManager.h"

#include "Engine/ECS/Components.h"
#include "Engine/ECS/SingletonComponents/GraphicsLocator.h"

#include "Engine/Graphics/Shaders/ShaderManager.h"
#include "Engine/Graphics/Renderer/RendererManagerLocator.h"

#include "Engine/Utils/CameraUtils.h"

namespace MyEngine
{
	using itFBOs = std::unordered_map<uint, FrameBufferObject>::iterator;

	FrameBufferManager::FrameBufferManager()
	{
		// Create main buffer with default parameters
		FrameBufferObject FBO = FrameBufferObject();

		m_mapFBOs = { {FBO.ID, FBO} };
		m_currFBOID = FBO.ID;

		// Add FBO to renderer
		iRendererManager* pRendererManager = RendererManagerLocator::Get();
		pRendererManager->AddFBO(FBO.ID);
	}

	FrameBufferManager::~FrameBufferManager()
	{
		DeleteAllFBOs();
	}

	bool FrameBufferManager::CreateFBO(Entity cameraId, int width, int height, uint& newFBOID)
	{
		FrameBufferObject FBO = FrameBufferObject();
		FBO.width = width;
		FBO.height = height;
		FBO.cameraId = cameraId;

		glCreateFramebuffers(1, &(FBO.ID));
		glBindFramebuffer(GL_FRAMEBUFFER, FBO.ID);

		// Create the colour buffer (texture)
		glGenTextures(1, &(FBO.colorTextureId));
		glBindTexture(GL_TEXTURE_2D, FBO.colorTextureId);

		glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGB8,
						FBO.width,
						FBO.height);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		// Create the depth buffer (texture)
		glGenTextures(1, &(FBO.depthTextureId));
		glBindTexture(GL_TEXTURE_2D, FBO.depthTextureId);

		glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH24_STENCIL8,
						FBO.width,
						FBO.height);

		// Attach textures to buffer
		glFramebufferTexture(GL_FRAMEBUFFER,
							 GL_COLOR_ATTACHMENT0,
							 FBO.colorTextureId, 0);

		glFramebufferTexture(GL_FRAMEBUFFER,
							 GL_DEPTH_STENCIL_ATTACHMENT,
							 FBO.depthTextureId, 0);

		static const GLenum draw_bufers[] =
		{
			GL_COLOR_ATTACHMENT0
		};
		glDrawBuffers(1, draw_bufers);

		bool isComplete = true;
		std::string error = "";

		GLenum statusCode = glCheckFramebufferStatus(GL_FRAMEBUFFER);
		switch (statusCode)
		{
		case GL_FRAMEBUFFER_COMPLETE:
			isComplete = true;
			break;
		case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
			error = "GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT";
			isComplete = false;
			break;
		case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
			error = "GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT";
		case GL_FRAMEBUFFER_UNSUPPORTED:
			error = "GL_FRAMEBUFFER_UNSUPPORTED";
		default:
			error = "Unknown error: " + std::to_string(statusCode);
			isComplete = false;
			break;
		}

		if (isComplete)
		{
			m_mapFBOs[FBO.ID] = FBO;

			// Add FBO to renderer
			iRendererManager* pRendererManager = RendererManagerLocator::Get();
			pRendererManager->AddFBO(FBO.ID);
		}
		else
		{
			LOG_ERROR(error);
		}

		// Point back to default frame buffer
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		return isComplete;
	}

	void FrameBufferManager::DeleteFBO(uint FBOID)
	{
		itFBOs it = m_mapFBOs.find(FBOID);
		FrameBufferObject& FBO = it->second;

		glDeleteTextures(1, &(FBO.colorTextureId));
		glDeleteTextures(1, &(FBO.depthTextureId));

		glDeleteFramebuffers(1, &(FBO.ID));

		m_mapFBOs.erase(it);

		return;
	}

	void FrameBufferManager::DeleteAllFBOs()
	{
		itFBOs it = m_mapFBOs.begin();
		while (it != m_mapFBOs.end())
		{
			uint FBOID = it->second.ID;

			if (FBOID == 0)
			{
				// Dont delete main FBO!
				it++;
				continue;
			}

			DeleteFBO(it->second.ID);
			it = m_mapFBOs.begin();
		}
	}

	bool FrameBufferManager::ResetFBO(uint FBOID, uint& newFBOID)
	{
		FrameBufferObject& FBO = GetFBO(FBOID);

		DeleteFBO(FBOID);

		bool isLoaded = CreateFBO(FBO.cameraId, FBO.width, FBO.height, newFBOID);

		return isLoaded;
	}

	FrameBufferObject& FrameBufferManager::GetFBO(uint FBOID)
	{
		itFBOs it = m_mapFBOs.find(FBOID);

		if (it == m_mapFBOs.end())
		{
			it = m_mapFBOs.find(0);
		}

		return it->second;
	}

	void FrameBufferManager::BindFBO(Scene* pScene, uint FBOID)
	{
		iShaderProgram* pShader = ShaderManager::GetActiveShader();

		FrameBufferObject& FBO = GetFBO(FBOID);
		CameraComponent* pCamera = pScene->Get<CameraComponent>(FBO.cameraId);
		TransformComponent* pTransformCamera = pScene->Get<TransformComponent>(FBO.cameraId);

		// Main buffer we need to update the FBO size accordingly to the window size
		if (FBOID == 0)
		{
			WindowComponent* pWindow = GraphicsLocator::GetWindow();

			glfwGetFramebufferSize(pWindow->pGLFWWindow, &FBO.width, &FBO.height);
		}

		if (m_currFBOID != FBOID)
		{
			glBindFramebuffer(GL_FRAMEBUFFER, FBO.ID);
			glViewport(0, 0, FBO.width, FBO.height);
		}

		// Update projection and view matrix
		glm::mat4 matProjection = CameraUtils::ProjectionMat(pCamera->fovy, pCamera->zNear,
			pCamera->zFar, FBO.width, FBO.height);
		pShader->SetUniformMatrix4f("matProjection", matProjection);

		glm::mat4 matView = CameraUtils::ViewMat(pTransformCamera->position, pTransformCamera->orientation,
			pCamera->distance, pCamera->height, pCamera->offsetTarget);
		pShader->SetUniformMatrix4f("matView", matView);

		m_currFBOID = FBOID;
	}

	void FrameBufferManager::BindFBOText(uint FBOID)
	{
		iShaderProgram* pShader = ShaderManager::GetActiveShader();

		FrameBufferObject& FBO = GetFBO(FBOID);

		GLint textUnitId = 50;

		glActiveTexture(GL_TEXTURE0 + textUnitId);
		// Get texture number from the FBO colour texture
		glBindTexture(GL_TEXTURE_2D, FBO.colorTextureId);

		pShader->SetUniformInt("isFBOView", true);
		pShader->SetUniformInt("FBOViewTexture", textUnitId);
		pShader->SetUniformVec2("screenWidthAndHeight", glm::vec2(FBO.width, FBO.height));
	}

	void FrameBufferManager::ClearFBO(uint FBOID)
	{
		FrameBufferObject& FBO = GetFBO(FBOID);


		GLfloat	zero = 0.0f;
		GLfloat one = 1.0f;
		
		glClearBufferfv(GL_COLOR, 0, &zero);
		glClearBufferfv(GL_DEPTH, 0, &one);

		glStencilMask(0xFF);

		glClearBufferfi(GL_DEPTH_STENCIL, 0, 1.0f, 0);
	}
}
