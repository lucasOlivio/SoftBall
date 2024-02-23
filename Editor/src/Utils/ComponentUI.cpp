#include "pch.h"

#include "ComponentUI.h"

#include "Utils/UIWidgets.h"

#include "Engine/ECS/Components.h"
#include "Engine/ECS/SingletonComponents/Components.h"
#include "Engine/ECS/SingletonComponents/PhysicsLocator.h"

#include "Engine/Graphics/VAO/VAOManagerLocator.h"

namespace MyEngine
{
    std::map<ComponentId, ComponentUIHandler> ComponentUI::m_componentsUI = {};

    void ComponentUI::GenerateComponentsUI(Scene* pScene, Entity entityId)
    {
        EntityManager* pEntityManager = pScene->GetEntitymanager();

        if (pEntityManager->Size() == 1)
        {
            return;
        }

        // For all components that entity have get the UI generator from cache and draw UI
        EntityMask mask = pEntityManager->GetMask(entityId);
        for (int i = 0; i < MAX_COMPONENTS; i++)
        {
            if (!mask[i])
            {
                continue;
            }

            Separator();

            std::string removId = "Remove component##RemovCompEntt:" + std::to_string(entityId) + "Comp:" + std::to_string(i);
            if (ImGui::Button(removId.c_str()))
            {
                pScene->RemoveComponent(entityId, i);
            }

            ImGui::SameLine();

            ComponentUIHandler handler = GetComponentUI(i);
            handler(pScene, entityId);

            Separator();
        }

        if (entityId == 0)
        {
            // HACK: Singleton components come here for now
            ComponentType GridbroadPhaseType = pScene->GetComponentType<GridBroadphaseComponent>();

            Separator();

            ImGui::SameLine();

            ComponentUIHandler handler = GetComponentUI(GridbroadPhaseType);
            handler(pScene, entityId);

            Separator();
        }
    }

    void ComponentUI::MapComponentsUI(Scene* pScene)
    {
        ComponentType TagType = pScene->GetComponentType<TagComponent>();
        m_componentsUI[TagType] = m_TagUI;

        ComponentType CameraType = pScene->GetComponentType<CameraComponent>();
        m_componentsUI[CameraType] = m_CameraUI;

        ComponentType GridbroadPhaseType = pScene->GetComponentType<GridBroadphaseComponent>();
        m_componentsUI[GridbroadPhaseType] = m_GridbroadPhaseUI;

        ComponentType TransformType = pScene->GetComponentType<TransformComponent>();
        m_componentsUI[TransformType] = m_TransformUI;

        ComponentType MovementType = pScene->GetComponentType<MovementComponent>();
        m_componentsUI[MovementType] = m_MovementUI;

        ComponentType RotationType = pScene->GetComponentType<RotationComponent>();
        m_componentsUI[RotationType] = m_RotationUI;

        ComponentType ParentType = pScene->GetComponentType<ParentComponent>();
        m_componentsUI[ParentType] = m_ParentUI;

        ComponentType TextureType = pScene->GetComponentType<TextureComponent>();
        m_componentsUI[TextureType] = m_TextureUI;

        ComponentType MaterialType = pScene->GetComponentType<MaterialComponent>();
        m_componentsUI[MaterialType] = m_MaterialUI;

        ComponentType EmitterType = pScene->GetComponentType<EmitterComponent>();
        m_componentsUI[EmitterType] = m_EmitterUI;

        ComponentType LightType = pScene->GetComponentType<LightComponent>();
        m_componentsUI[LightType] = m_LightUI;

        ComponentType ModelType = pScene->GetComponentType<ModelComponent>();
        m_componentsUI[ModelType] = m_ModelUI;

        ComponentType FrameBufferType = pScene->GetComponentType<FrameBufferComponent>();
        m_componentsUI[FrameBufferType] = m_FrameBufferUI;

        ComponentType FrameBufferViewType = pScene->GetComponentType<FrameBufferViewComponent>();
        m_componentsUI[FrameBufferViewType] = m_FrameBufferViewUI;

        ComponentType TransformAnimationType = pScene->GetComponentType<TransformAnimationComponent>();
        m_componentsUI[TransformAnimationType] = m_TransformAnimationUI;

        ComponentType TilingType = pScene->GetComponentType<TilingComponent>();
        m_componentsUI[TilingType] = m_TilingUI;

        ComponentType GravityType = pScene->GetComponentType<GravityComponent>();
        m_componentsUI[GravityType] = m_GravityUI;

        ComponentType RigidBodyType = pScene->GetComponentType<RigidBodyComponent>();
        m_componentsUI[RigidBodyType] = m_RigidBodyUI;

        ComponentType MeshColliderType = pScene->GetComponentType<MeshColliderComponent>();
        m_componentsUI[MeshColliderType] = m_MeshColliderUI;

        ComponentType SphereColliderType = pScene->GetComponentType<SphereColliderComponent>();
        m_componentsUI[SphereColliderType] = m_SphereColliderUI;

        ComponentType AABBColliderType = pScene->GetComponentType<AABBColliderComponent>();
        m_componentsUI[AABBColliderType] = m_AABBColliderUI;

        ComponentType SoftBodyType = pScene->GetComponentType<SoftBodyComponent>();
        m_componentsUI[SoftBodyType] = m_SoftBodyUI;

        ComponentType PlayerType = pScene->GetComponentType<PlayerComponent>();
        m_componentsUI[PlayerType] = m_PlayerUI;

        ComponentType SteeringBehaviorType = pScene->GetComponentType<SteeringBehaviorComponent>();
        m_componentsUI[SteeringBehaviorType] = m_SteeringBehaviorUI;
    }

    ComponentUIHandler ComponentUI::GetComponentUI(ComponentType component)
    {
        std::map<ComponentId, ComponentUIHandler>::iterator it = m_componentsUI.find(component);
        if (it == m_componentsUI.end())
        {
            return m_EmptyUI;
        }

        return it->second;
    }

    void ComponentUI::m_EmptyUI(Scene* pScene, Entity entityId)
    {
    }

    void ComponentUI::m_TagUI(Scene* pScene, Entity entityId)
    {
        Title("Tag:");

        TagComponent* pTag = pScene->Get<TagComponent>(entityId);
        if (!pTag)
        {
            return;
        }

        ImGui::Text("Name");
        ImGui::InputText("##Name", &pTag->name);
    }

    void ComponentUI::m_CameraUI(Scene* pScene, Entity entityId)
    {
        Title("Camera:");

        CameraComponent* pCamera = pScene->Get<CameraComponent>(entityId);
        if (!pCamera)
        {
            return;
        }

        ImGui::Text("Up vector");
        ImGui::InputFloat3("##upVector", &pCamera->upVector.x);

        ImGui::Text("Distance");
        ImGui::InputFloat("##distance", &pCamera->distance);

        ImGui::Text("Height");
        ImGui::InputFloat("##height", &pCamera->height);

        ImGui::Text("Offset Target");
        ImGui::InputFloat("##offsetTarget", &pCamera->offsetTarget);

        ImGui::Text("fovy");
        ImGui::InputFloat("##fovy", &pCamera->fovy);

        ImGui::Text("zNear");
        ImGui::InputFloat("##zNear", &pCamera->zNear);

        ImGui::Text("zFar");
        ImGui::InputFloat("##zFar", &pCamera->zFar);
    }

    void ComponentUI::m_GridbroadPhaseUI(Scene* pScene, Entity entityId)
    {
        Title("Gridbroadphase:");

        GridBroadphaseComponent* pGridBroadphase = PhysicsLocator::GetGridBroadphase();
        if (!pGridBroadphase)
        {
            return;
        }

        ImGui::Text("Length per box");
        ImGui::InputFloat3("##Lengthbox", &pGridBroadphase->lengthPerBox.x);
    }

    void ComponentUI::m_TransformUI(Scene* pScene, Entity entityId)
	{
        Title("Transform:");

        TransformComponent* pTransform = pScene->Get<TransformComponent>(entityId);
        if (!pTransform)
        {
            return;
        }

        ImGui::Text("Position");
        ImGui::InputFloat("X##PositionX", &pTransform->position.x);
        ImGui::InputFloat("Y##PositionY", &pTransform->position.y);
        ImGui::InputFloat("Z##PositionZ", &pTransform->position.z);

        Separator();
        ImGui::Text("Rotation");
        glm::vec3 euler = glm::degrees(glm::eulerAngles(pTransform->orientation));
        if (ImGui::InputFloat("X##RotationX", &euler.x)) {
            pTransform->orientation = glm::quat(glm::radians(euler));
        }
        if (ImGui::InputFloat("Y##RotationY", &euler.y)) {
            pTransform->orientation = glm::quat(glm::radians(euler));
        }
        if (ImGui::InputFloat("Z##RotationZ", &euler.z)) {
            pTransform->orientation = glm::quat(glm::radians(euler));
        }

        Separator();
        ImGui::Text("Scale");
        ImGui::InputFloat("##Scale", &pTransform->scale);
	}

    void ComponentUI::m_MovementUI(Scene* pScene, Entity entityId)
    {
        Title("Movement:");

        MovementComponent* pMovement = pScene->Get<MovementComponent>(entityId);
        if (!pMovement)
        {
            return;
        }

        ImGui::Text("Velocity");
        ImGui::InputFloat("##VelocityX", &pMovement->velocity.x);
        ImGui::InputFloat("##VelocityY", &pMovement->velocity.y);
        ImGui::InputFloat("##VelocityZ", &pMovement->velocity.z);

        Separator();
        ImGui::Text("Acceleration");
        ImGui::InputFloat("##AccelerationX", &pMovement->acceleration.x);
        ImGui::InputFloat("##AccelerationY", &pMovement->acceleration.y);
        ImGui::InputFloat("##AccelerationZ", &pMovement->acceleration.z);

        Separator();
        ImGui::Text("Max speed");
        ImGui::InputFloat("##MovementMaxSpeed", &pMovement->maxSpeed);
        Separator();
        ImGui::Text("Drag");
        ImGui::InputFloat("##MovementDrag", &pMovement->drag);
    }

    void ComponentUI::m_RotationUI(Scene* pScene, Entity entityId)
    {
        Title("Rotation:");

        RotationComponent* pRotation = pScene->Get<RotationComponent>(entityId);
        if (!pRotation)
        {
            return;
        }

        ImGui::Text("Angular Velocity");
        ImGui::InputFloat("##AngularVelocityX", &pRotation->velocity.x);
        ImGui::InputFloat("##AngularVelocityY", &pRotation->velocity.y);
        ImGui::InputFloat("##AngularVelocityZ", &pRotation->velocity.z);

        Separator();
        ImGui::Text("Angular Acceleration");
        ImGui::InputFloat("##AngularAccelerationX", &pRotation->acceleration.x);
        ImGui::InputFloat("##AngularAccelerationY", &pRotation->acceleration.y);
        ImGui::InputFloat("##AngularAccelerationZ", &pRotation->acceleration.z);

        Separator();
        ImGui::InputFloat("##AngularMaxSpeed", &pRotation->maxSpeed);
        Separator();
        ImGui::InputFloat("##AngularDrag", &pRotation->drag);
    }

    void ComponentUI::m_ParentUI(Scene* pScene, Entity entityId)
    {
        Title("Parent:");

        ParentComponent* pParent = pScene->Get<ParentComponent>(entityId);
        if (!pParent)
        {
            return;
        }

        ImGui::Text("Parent Entity ID");
        ImGui::InputInt("##ParentEntityID", &pParent->parentId);
    }

    void ComponentUI::m_TextureUI(Scene* pScene, Entity entityId)
    {
        Title("Texture:");

        TextureComponent* pTexture = pScene->Get<TextureComponent>(entityId);
        if (!pTexture)
        {
            return;
        }

        ImGui::Text("File Name");
        ImGui::InputText("##FileName", &pTexture->fileName);

        // Way to set it with a list
        ImGui::Text("Texture Type");
        ImGui::InputInt("##TextureType", reinterpret_cast<int*>(&pTexture->textureType));

        ImGui::Text("Texture Sides");
        for (size_t i = 0; i < pTexture->vecTextures.size(); ++i)
        {
            ImGui::InputText(("##TextureSide" + std::to_string(i)).c_str(), &pTexture->vecTextures[i]);
        }
        if (ImGui::Button("Add texture side"))
        {
            pTexture->vecTextures.push_back("");
        }
    }

    void ComponentUI::m_MaterialUI(Scene* pScene, Entity entityId)
    {
        Title("Material:");

        MaterialComponent* pMaterial = pScene->Get<MaterialComponent>(entityId);
        if (!pMaterial)
        {
            return;
        }

        ImGui::Text("Name");
        ImGui::InputText("##MaterialName", &pMaterial->name);

        ImGui::Text("Offset");
        ImGui::InputFloat3("##Offset", &pMaterial->offset.x);

        ImGui::Text("Offset Move");
        ImGui::InputFloat3("##OffsetMove", &pMaterial->offsetMove.x);

        ImGui::Text("Offset Height Map");
        ImGui::InputFloat3("##OffsetHeightMap", &pMaterial->offsetHeightMap.x);

        ImGui::Text("Color Textures");
        for (size_t i = 0; i < pMaterial->colorTextures.size(); ++i)
        {
            ImGui::InputText(("##ColorTexture" + std::to_string(i)).c_str(), &pMaterial->colorTextures[i]);
        }

        if (ImGui::Button("Add color texture"))
        {
            pMaterial->colorTextures.push_back("");
        }

        ImGui::Text("Color Textures Ratios");
        ImGui::InputFloat3("##ColorTexturesRatios", &pMaterial->colorTexturesRatios.x);

        ImGui::Text("Use Height Map");
        ImGui::Checkbox("##UseHeightMap", &pMaterial->useHeightMap);

        ImGui::Text("Height Map Texture");
        ImGui::InputText("##HeightMapTexture", &pMaterial->heightMapTexture);

        ImGui::Text("Height Scale");
        ImGui::InputFloat("##HeightScale", &pMaterial->heightScale);

        ImGui::Text("Use Normal Texture");
        ImGui::Checkbox("##UseNormalTexture", &pMaterial->useNormalTexture);

        ImGui::Text("Normal Texture");
        ImGui::InputText("##NormalTexture", &pMaterial->normalTexture);

        ImGui::Text("Is Emissive");
        ImGui::Checkbox("##IsEmissive", &pMaterial->isEmissive);

        ImGui::Text("Use Alpha Texture");
        ImGui::Checkbox("##UseAlphaTexture", &pMaterial->useAlphaTexture);

        ImGui::Text("Alpha Texture");
        ImGui::InputText("##AlphaTexture", &pMaterial->alphaTexture);

        ImGui::Text("Use Cube Texture");
        ImGui::Checkbox("##UseCubeTexture", &pMaterial->useCubeTexture);

        ImGui::Text("Cube Texture");
        ImGui::InputText("##CubeTexture", &pMaterial->cubeTexture);
    }

    void ComponentUI::m_EmitterUI(Scene* pScene, Entity entityId)
    {
        Title("Emitter:");

        EmitterComponent* pEmitter = pScene->Get<EmitterComponent>(entityId);
        if (!pEmitter)
        {
            return;
        }

        ImGui::Text("Emit Rate:");
        ImGui::InputInt("Min##EmitRateMin", &pEmitter->emitRateMin);
        ImGui::InputInt("Max##EmitRateMax", &pEmitter->emitRateMax);

        ImGui::Text("Max Particles:");
        ImGui::InputInt("##MaxParticles", &pEmitter->maxParticles);

        ImGui::Text("Active:");
        ImGui::Checkbox("##IsActive", &pEmitter->isActive);

        ImGui::Text("Mesh Name:");
        ImGui::InputText("##MeshName", &(pEmitter->properties.meshName));

        ImGui::Text("Material:");
        ImGui::InputText("##Material", &(pEmitter->properties.material));

        ImGui::Text("Initial Color:");
        ImGui::InputFloat3("##ColorInitial", &(pEmitter->properties.colorInitial.x));

        ImGui::Text("Color Change:");
        ImGui::InputFloat3("##ColorChange", &(pEmitter->properties.colorChange.x));

        ImGui::Text("Constant Force:");
        ImGui::InputFloat3("##ConstForce", &(pEmitter->properties.constForce.x));

        ImGui::Text("Max Life Time:");
        ImGui::InputFloat("##MaxLifeTime", &(pEmitter->properties.maxLifeTime));

        ImGui::Text("Min Life Time:");
        ImGui::InputFloat("##MinLifeTime", &(pEmitter->properties.minLifeTime));

        ImGui::Text("Max Orientation (In quat):");
        ImGui::InputFloat4("##OriMax", &(pEmitter->properties.oriMax.w));

        ImGui::Text("Min Orientation (In quat):");
        ImGui::InputFloat4("##OriMin", &(pEmitter->properties.oriMin.w));

        ImGui::Text("Max Position:");
        ImGui::InputFloat3("##PosMax", &(pEmitter->properties.posMax.x));

        ImGui::Text("Min Position:");
        ImGui::InputFloat3("##PosMin", &(pEmitter->properties.posMin.x));

        ImGui::Text("Max Rotation (In degrees):");
        ImGui::InputFloat3("##RotMax", &(pEmitter->properties.rotMax.x));

        ImGui::Text("Min Rotation:");
        ImGui::InputFloat3("##RotMin", &(pEmitter->properties.rotMin.x));

        ImGui::Text("Max Scale:");
        ImGui::InputFloat("##ScaMax", &(pEmitter->properties.scaMax));

        ImGui::Text("Min Scale:");
        ImGui::InputFloat("##ScaMin", &(pEmitter->properties.scaMin));

        ImGui::Text("Max Velocity:");
        ImGui::InputFloat3("##VelMax", &(pEmitter->properties.velMax.x));

        ImGui::Text("Min Velocity:");
        ImGui::InputFloat3("##VelMin", &(pEmitter->properties.velMin.x));
    }

    void ComponentUI::m_LightUI(Scene* pScene, Entity entityId)
    {
        Title("Light:");

        LightComponent* pLight = pScene->Get<LightComponent>(entityId);
        if (!pLight)
        {
            return;
        }

        // Flickering control
        ImGui::Text("Flicker Offset");
        ImGui::InputFloat4("##FlickerOffset", &pLight->flickerOffset.x);

        // ulBasePath
        ImGui::Text("UL Base Path");
        ImGui::InputText("##ULBasePath", &pLight->ulBasePath);

        // Controls relative to transform component
        ImGui::Text("Position Offset");
        ImGui::InputFloat4("##PositionOffset", &pLight->positionOffset.x);

        ImGui::Text("Direction Offset");
        ImGui::InputFloat4("##DirectionOffset", &pLight->directionOffset.x);

        ImGui::Text("Distance Offset");
        ImGui::InputFloat("##DistanceOffset", &pLight->distanceOffset);

        // Light parameters
        ImGui::Text("Diffuse (R, G, B, A)");
        ImGui::InputFloat4("##Diffuse", &pLight->diffuse.x);

        ImGui::Text("Specular (R, G, B, Power)");
        ImGui::InputFloat4("##Specular", &pLight->specular.x);

        ImGui::Text("Attenuation (Constant, Linear, Quadratic, Distance CutOff)");
        ImGui::InputFloat4("##Attenuation", &pLight->atten.x);

        ImGui::Text("Direction");
        ImGui::InputFloat4("##Direction", &pLight->direction.x);

        ImGui::Text("Parameters (Type, Inner Angle, Outer Angle, Is Flickery)");
        ImGui::InputFloat4("##Params", &pLight->params.x);

        ImGui::Text("Status");
        ImGui::Checkbox("##Status", &pLight->status);
    }

    void ComponentUI::m_ModelUI(Scene* pScene, Entity entityId)
    {
        Title("Model:");

        iVAOManager* pVAOManager = VAOManagerLocator::Get();
        ModelComponent* pModel = pScene->Get<ModelComponent>(entityId);
        if (!pModel)
        {
            return;
        }

        // Model paths
        ImGui::Text("Models");
        for (size_t i = 0; i < pModel->models.size(); ++i)
        {
            if (ImGui::InputText(("##Model" + std::to_string(i)).c_str(), &pModel->models[i]))
            {
                sMesh* pMesh = pVAOManager->LoadModelIntoVAO(pModel->models[i], false);
                if (!pMesh)
                {
                    continue;
                }
                pModel->pMeshes[i] = pMesh;
            }
        }

        if (ImGui::Button("Add Mesh"))
        {
            pModel->models.push_back("cube.ply");

            sMesh* pMesh = pVAOManager->LoadModelIntoVAO(pModel->models.back(), false);
            pModel->pMeshes.push_back(pMesh);
        }

        ImGui::Text("(use material to add transparency texture)");
        ImGui::Checkbox("Use transparency", &pModel->useTransparency);

        // Color
        ImGui::Checkbox("Use default color", &pModel->useDefaultColor);
        ImGui::Text("Default color:");
        ImGui::InputFloat3("##ModelDefaultcolor", &pModel->defaultColor[0]);

        // Material
        ImGui::Text("Material");
        ImGui::InputText("##MaterialInput", &pModel->material);

        // Flags
        ImGui::Checkbox("Wireframe", &pModel->isWireframe);
        ImGui::Checkbox("Dynamic", &pModel->isDynamic);
        ImGui::Checkbox("Do Not Light", &pModel->doNotLight);
        ImGui::Checkbox("Use Color Texture", &pModel->useColorTexture);

        ImGui::Checkbox("Is Active", &pModel->isActive);

        // FBOIDs
        ImGui::Text("FBO IDs:");
        std::set<uint> fboIdsCopy = pModel->FBOIDs;
        for (uint id : fboIdsCopy)
        {
            ImGui::PushID(id);
            uint modifiedId = id; // Create a copy of the ID for modification
            ImGui::InputScalar(("##FBOID_" + std::to_string(id)).c_str(), ImGuiDataType_U32, &modifiedId);
            ImGui::SameLine();
            if (ImGui::Button(("Remove##FBOID_" + std::to_string(id)).c_str()))
            {
                pModel->FBOIDs.erase(id); // Remove the original ID
                break; // Exit loop to avoid modifying container while iterating
            }

            ImGui::PopID();

            if (modifiedId != id) {
                pModel->FBOIDs.erase(id); // Remove the original ID
                pModel->FBOIDs.insert(modifiedId); // Insert the modified ID
            }
        }

        if (ImGui::Button("Add FBO ID"))
        {
            int newIndex = *(pModel->FBOIDs.rbegin()) + 1; // Last index +1
            pModel->FBOIDs.insert(newIndex);
        }
    }

    void ComponentUI::m_FrameBufferUI(Scene* pScene, Entity entityId)
    {
        Title("Frame buffer:");

        iVAOManager* pVAOManager = VAOManagerLocator::Get();
        FrameBufferComponent* pFrameBuffer = pScene->Get<FrameBufferComponent>(entityId);
        if (!pFrameBuffer)
        {
            return;
        }

        ImGui::Text("Camera ID");
        ImGui::InputInt("##CameraId", &pFrameBuffer->cameraId);

        ImGui::Text("Width");
        ImGui::InputInt("##FBOWidth", &pFrameBuffer->width);

        ImGui::Text("Height");
        ImGui::InputInt("##FBOHeight", &pFrameBuffer->height);
    }

    void ComponentUI::m_FrameBufferViewUI(Scene* pScene, Entity entityId)
    {
        Title("Frame buffer view:");

        iVAOManager* pVAOManager = VAOManagerLocator::Get();
        FrameBufferViewComponent* pFrameBufferView = pScene->Get<FrameBufferViewComponent>(entityId);
        if (!pFrameBufferView)
        {
            return;
        }

        int FBOID = static_cast<int>(pFrameBufferView->FBOID);
        ImGui::Text("FBO ID");
        ImGui::InputInt("##FBOID", &FBOID);
        pFrameBufferView->FBOID = FBOID;

        int filter = static_cast<int>(pFrameBufferView->filter);
        ImGui::Text("Filter");
        ImGui::InputInt("##Filter", &filter);
        pFrameBufferView->filter = static_cast<eFBFilter>(filter);
    }

    void ComponentUI::m_TransformAnimationUI(Scene* pScene, Entity entityId)
    {
        Title("Animation Component:");

        TransformAnimationComponent* pTransformAnimation = pScene->Get<TransformAnimationComponent>(entityId);
        if (!pTransformAnimation)
        {
            return;
        }

        // Edit time
        ImGui::InputFloat("Time", &pTransformAnimation->time);
        ImGui::Checkbox("Is Active##AnimationIsActive", &pTransformAnimation->isActive);

        // Edit position keyframes
        ImGui::Text("Position Keyframes:");
        for (size_t i = 0; i < pTransformAnimation->positionKeyFrames.size(); ++i)
        {
            PositionKeyFrame& keyFrame = pTransformAnimation->positionKeyFrames[i];
            ImGui::InputFloat(("Time##AnimationPos" + std::to_string(i)).c_str(), &keyFrame.time);
            ImGui::InputFloat3(("Position##AnimationPos" + std::to_string(i)).c_str(), &keyFrame.value[0]);
            ImGui::InputInt(("EaseType##AnimationPos" + std::to_string(i)).c_str(), reinterpret_cast<int*>(&keyFrame.easeType));

            ImGui::Text("Is key event:");
            ImGui::Checkbox(("##AnimationPosKeyEvent" + std::to_string(i)).c_str(), &keyFrame.isKeyEvent);

            // Button to remove this keyframe
            if (ImGui::Button(("Remove##AnimationPos" + std::to_string(i)).c_str()))
            {
                pTransformAnimation->positionKeyFrames.erase(pTransformAnimation->positionKeyFrames.begin() + i);
                --i;
            }
        }

        if (ImGui::Button("Add Position Keyframe"))
        {
            PositionKeyFrame newKeyFrame;
            pTransformAnimation->positionKeyFrames.push_back(newKeyFrame);
        }

        Separator();

        // Edit scale keyframes
        ImGui::Text("Scale Keyframes:");
        for (size_t i = 0; i < pTransformAnimation->scaleKeyFrames.size(); ++i)
        {
            ScaleKeyFrame& keyFrame = pTransformAnimation->scaleKeyFrames[i];
            ImGui::InputFloat(("Time##AnimationScale" + std::to_string(i)).c_str(), &keyFrame.time);
            ImGui::InputFloat(("Scale##AnimationScale" + std::to_string(i)).c_str(), &keyFrame.value);
            ImGui::InputInt(("EaseType##AnimationScale" + std::to_string(i)).c_str(), reinterpret_cast<int*>(&keyFrame.easeType));

            ImGui::Text("Is key event:");
            ImGui::Checkbox(("##AnimationScaleKeyEvent" + std::to_string(i)).c_str(), &keyFrame.isKeyEvent);

            // Button to remove this keyframe
            if (ImGui::Button(("Remove##AnimationScale" + std::to_string(i)).c_str()))
            {
                pTransformAnimation->scaleKeyFrames.erase(pTransformAnimation->scaleKeyFrames.begin() + i);
                --i;
            }
        }

        if (ImGui::Button("Add Scale Keyframe"))
        {
            ScaleKeyFrame newKeyFrame;
            pTransformAnimation->scaleKeyFrames.push_back(newKeyFrame);
        }

        Separator();

        // Edit rotation keyframes
        ImGui::Text("Rotation Keyframes:");
        for (size_t i = 0; i < pTransformAnimation->rotationKeyFrames.size(); ++i)
        {
            RotationKeyFrame& keyFrame = pTransformAnimation->rotationKeyFrames[i];
            ImGui::InputFloat(("Time##AnimationRot" + std::to_string(i)).c_str(), &keyFrame.time);

            ImGui::Text("Rotation");
            glm::vec3 euler = glm::degrees(glm::eulerAngles(keyFrame.value));
            if (ImGui::InputFloat(("X##AnimationRotationX" + std::to_string(i)).c_str(), &euler.x)) {
                keyFrame.value = glm::quat(glm::radians(euler));
            }
            if (ImGui::InputFloat(("Y##AnimationRotationY" + std::to_string(i)).c_str(), &euler.y)) {
                keyFrame.value = glm::quat(glm::radians(euler));
            }
            if (ImGui::InputFloat(("Z##AnimationRotationZ" + std::to_string(i)).c_str(), &euler.z)) {
                keyFrame.value = glm::quat(glm::radians(euler));
            }

            ImGui::InputInt(("EaseType##AnimationRot" + std::to_string(i)).c_str(), reinterpret_cast<int*>(&keyFrame.easeType));

            ImGui::Text("Is key event:");
            ImGui::Checkbox(("##AnimationRotKeyEvent" + std::to_string(i)).c_str(), &keyFrame.isKeyEvent);

            // Button to remove this keyframe
            if (ImGui::Button(("Remove##AnimationRot" + std::to_string(i)).c_str()))
            {
                pTransformAnimation->rotationKeyFrames.erase(pTransformAnimation->rotationKeyFrames.begin() + i);
                --i;
            }
        }

        if (ImGui::Button("Add Rotation Keyframe"))
        {
            RotationKeyFrame newKeyFrame;
            pTransformAnimation->rotationKeyFrames.push_back(newKeyFrame);
        }

        Separator();
    }

    void ComponentUI::m_TilingUI(Scene* pScene, Entity entityId)
    {
        Title("Tiling:");

        TilingComponent* pTiling = pScene->Get<TilingComponent>(entityId);
        if (!pTiling)
        {
            return;
        }

        // Axis
        ImGui::Text("Axis");
        ImGui::InputFloat3("##TilingAxis", &pTiling->axis.x);

        // Offset
        ImGui::Text("Offset");
        ImGui::InputFloat3("##TilingOffset", &pTiling->offset.x);
    }

    void ComponentUI::m_GravityUI(Scene* pScene, Entity entityId)
    {
        Title("Gravity:");

        GravityComponent* pGravity = pScene->Get<GravityComponent>(entityId);
        if (!pGravity)
        {
            return;
        }

        // Acceleration
        ImGui::Text("Acceleration");
        ImGui::InputFloat3("##GravityAcceleration", &pGravity->acceleration.x);
    }

    void ComponentUI::m_RigidBodyUI(Scene* pScene, Entity entityId)
    {
        Title("Rigid Body:");

        RigidBodyComponent* pRigidBody = pScene->Get<RigidBodyComponent>(entityId);
        if (!pRigidBody)
        {
            return;
        }

        // Body Type
        ImGui::Text("Body Type");
        ImGui::InputInt("##BodyType", reinterpret_cast<int*>(&pRigidBody->bodyType));

        // Shape Type
        ImGui::Text("Shape Type");
        ImGui::InputInt("##ShapeType", reinterpret_cast<int*>(&pRigidBody->shapeType));
    }

    void ComponentUI::m_MeshColliderUI(Scene* pScene, Entity entityId)
    {
        Title("Mesh Collider:");

        MeshColliderComponent* pMeshCollider = pScene->Get<MeshColliderComponent>(entityId);
        if (!pMeshCollider)
        {
            return;
        }

        // Name
        ImGui::Text("Name");
        ImGui::InputText("##MeshColliderName", &pMeshCollider->name);
    }

    void ComponentUI::m_SphereColliderUI(Scene* pScene, Entity entityId)
    {
        Title("Sphere Collider:");

        SphereColliderComponent* pSphereCollider = pScene->Get<SphereColliderComponent>(entityId);
        if (!pSphereCollider)
        {
            return;
        }

        // Radius
        ImGui::Text("Radius");
        ImGui::InputFloat("##SphereColliderRadius", &pSphereCollider->radius);
    }

    void ComponentUI::m_AABBColliderUI(Scene* pScene, Entity entityId)
    {
        Title("AABB Collider:");

        AABBColliderComponent* pAABBCollider = pScene->Get<AABBColliderComponent>(entityId);
        if (!pAABBCollider)
        {
            return;
        }

        // Min
        ImGui::Text("Min");
        ImGui::InputFloat3("##AABBColliderMin", &pAABBCollider->min.x);

        // Max
        ImGui::Text("Max");
        ImGui::InputFloat3("##AABBColliderMax", &pAABBCollider->max.x);
    }

    void ComponentUI::m_SoftBodyUI(Scene* pScene, Entity entityId)
    {
        Title("SoftBody:");

        SoftBodyComponent* pSoftBody = pScene->Get<SoftBodyComponent>(entityId);
        if (!pSoftBody)
        {
            return;
        }

        ImGui::Text("Mesh name");
        ImGui::InputText("##SoftBodyMesh", &pSoftBody->meshName);

        ImGui::Text("Default spring strength");
        ImGui::InputFloat("##SoftBodyDefaultSpringStrength", &pSoftBody->defaultSpringStrength);

        ImGui::Text("Create wireframe?");
        ImGui::Checkbox("##SoftBodyCreateWireframe", &pSoftBody->createWireframe);
    }

    void ComponentUI::m_PlayerUI(Scene* pScene, Entity entityId)
    {
        Title("Player:");

        PlayerComponent* pPlayer = pScene->Get<PlayerComponent>(entityId);
        if (!pPlayer)
        {
            return;
        }

        ImGui::Text("Speed");
        ImGui::InputFloat("##PlayerSpeed", &pPlayer->speed);
    }

    void ComponentUI::m_SteeringBehaviorUI(Scene* pScene, Entity entityId)
    {
        Title("Steering Behavior:");

        SteeringBehaviorComponent* pSteeringBehavior = pScene->Get<SteeringBehaviorComponent>(entityId);
        if (!pSteeringBehavior)
        {
            return;
        }

        ImGui::Text("Steering type");
        ImGui::InputInt("##SteeringBehaviorType", reinterpret_cast<int*>(&pSteeringBehavior->steeringType));

        ImGui::Text("Target id");
        ImGui::InputInt("##SteeringBehaviorTargetId", &pSteeringBehavior->targetId);

        ImGui::Text("Speed");
        ImGui::InputFloat("##SteeringBehaviorSpeed", &pSteeringBehavior->speed);

        ImGui::Text("Max distance");
        ImGui::InputFloat("##SteeringBehaviorMaxDistance", &pSteeringBehavior->maxDistance);

        ImGui::Text("Future time");
        ImGui::InputFloat("##SteeringBehaviorFutureTime", &pSteeringBehavior->futureTime);
    }
}
