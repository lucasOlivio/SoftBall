#include "pch.h"

#include "GraphicsLocator.h"

namespace MyEngine
{
    TransparentEntitiesComponent* GraphicsLocator::m_pTransparentEntities = new TransparentEntitiesComponent();
    WindowComponent* GraphicsLocator::m_pWindow = new WindowComponent();
    AnimationControllerComponent* GraphicsLocator::m_pAnimationController = new AnimationControllerComponent();

    TransparentEntitiesComponent* GraphicsLocator::GetTransparentEntities()
    {
        return m_pTransparentEntities;
    }

    WindowComponent* GraphicsLocator::GetWindow()
    {
        return m_pWindow;
    }

    AnimationControllerComponent* GraphicsLocator::GetAnimationController()
    {
        return m_pAnimationController;
    }

    void GraphicsLocator::SetTransparentEntities(TransparentEntitiesComponent* pTransparentEntities)
    {
        delete m_pTransparentEntities;
        m_pTransparentEntities = pTransparentEntities;
    }

    void GraphicsLocator::SetWindow(WindowComponent* pWindow)
    {
        delete m_pWindow;
        m_pWindow = pWindow;
    }

    void GraphicsLocator::SetAnimationController(AnimationControllerComponent* pAnimationController)
    {
        delete m_pAnimationController;
        m_pAnimationController = pAnimationController;
    }

    void GraphicsLocator::Clear()
    {
        delete m_pTransparentEntities;
        delete m_pWindow;
        delete m_pAnimationController;
    }
}
