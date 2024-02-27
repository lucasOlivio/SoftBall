#include "pch.h"

#include "SceneSerializerJSON.h"

#include "Engine/Utils/ParserJSON.h"

#include "Engine/ECS/SingletonComponents/GraphicsLocator.h"
#include "Engine/ECS/SingletonComponents/CoreLocator.h"
#include "Engine/ECS/SingletonComponents/PhysicsLocator.h"

#include "Engine/Graphics/FrameBuffers/FrameBuffersProperties.h"

#include <rapidjson/filereadstream.h>
#include <rapidjson/filewritestream.h>
#include <rapidjson/prettywriter.h>
#include <rapidjson/stringbuffer.h>
#include <fstream>

namespace MyEngine
{
    SceneSerializerJSON::SceneSerializerJSON()
    {
        m_doc.SetObject();
    }

    SceneSerializerJSON::~SceneSerializerJSON()
    {
    }

    bool SceneSerializerJSON::DeserializeScene(const std::string& filePath, Scene& sceneOut)
    {
        bool isLoaded = m_LoadDoc(filePath);
        if (!isLoaded)
        {
            return false;
        }

        // Parse scene json and load into object
        bool isParsed = m_ParseDocToScene(sceneOut);
        if (!isParsed)
        {
            std::string errorMsg = "Error parsing file '" + filePath + "'";
            LOG_ERROR(errorMsg.c_str());
            return false;
        }

        m_doc.Clear();

        return true;
    }

    bool SceneSerializerJSON::SerializeScene(const std::string& filePath, Scene& sceneIn)
    {
        // Convert the scene object to a JSON document
        bool isParsed = m_ParseSceneToDoc(sceneIn);
        if (!isParsed)
        {
            std::string errorMsg = "Error parsing doc to file '" + filePath + "'";
            LOG_ERROR(errorMsg.c_str());
            return false;
        }

        bool isSaved = m_SaveDoc(filePath);

        if (!isSaved) {
            std::string errorMsg = "Error saving json to file '" + filePath + "'";
            LOG_ERROR(errorMsg.c_str());
            return false;
        }

        m_doc.Clear();

        return true;
    }

    bool SceneSerializerJSON::m_LoadDoc(const std::string& filePath)
    {
        using namespace rapidjson;

        // Load file stream into document object
        FILE* fp = 0;
        fopen_s(&fp, filePath.c_str(), "rb");

        if (!fp)
        {
            std::string errorMsg = "File '" + filePath + "' not found!";
            LOG_ERROR(errorMsg);
            return false;
        }

        char readBuffer[MAX_LINE_LENGTH];
        FileReadStream fs(fp, readBuffer, sizeof(readBuffer));

        m_doc.ParseStream(fs);

        fclose(fp);

        std::string infoMsg = "File '" + filePath + "' loaded!";
        LOG_INFO(infoMsg);

        return true;
    }

    bool SceneSerializerJSON::m_SaveDoc(const std::string& filePath)
    {
        using namespace rapidjson;

        StringBuffer buf;
        PrettyWriter<StringBuffer> writer(buf);

        m_doc.Accept(writer);

        // Get the JSON string from the buffer
        std::string json = buf.GetString();

        // Write the JSON to a file
        std::ofstream of(filePath, std::ios::out | std::ios::trunc);
        of << json;

        if (!of.good()) {
            std::string errorMsg = "Error saving json to file '" + filePath + "'";
            LOG_ERROR(errorMsg.c_str());
            return false;
        }

        of.close();

        return true;
    }

    bool SceneSerializerJSON::m_ParseSceneToDoc(Scene& sceneIn)
    {
        using namespace rapidjson;

        Document::AllocatorType& allocator = m_doc.GetAllocator();
        m_doc.SetArray();

        // Singleton components
        //-------------------------------
        Value gridObject;
        gridObject.SetObject();

        Value gridBroadphaseObject;
        gridBroadphaseObject.SetObject();

        GridBroadphaseComponent* pGridBroadphase = PhysicsLocator::GetGridBroadphase();
        m_ParseGridBroadphaseToDoc(gridBroadphaseObject, *pGridBroadphase, allocator);
        gridObject.AddMember("gridBroadphase", gridBroadphaseObject, allocator);

        m_doc.PushBack(gridObject, allocator);

        // Entities
        //-------------------------------
        EntityManager* pEntityManager = sceneIn.GetEntitymanager();
        for (const Entity& entity : pEntityManager->GetEntities())
        {
            // Create a JSON object for each entity
            Value entityObject;
            entityObject.SetObject();

            // Create a RapidJSON object for each component
            if (pEntityManager->HasComponent(entity, sceneIn.GetComponentType<CameraComponent>()))
            {
                Value cameraObject;
                cameraObject.SetObject();

                CameraComponent* pCamera = sceneIn.Get<CameraComponent>(entity);
                m_ParseCameraToDoc(cameraObject, *pCamera, allocator);
                entityObject.AddMember("camera", cameraObject, allocator);
            }
            if (pEntityManager->HasComponent(entity, sceneIn.GetComponentType<TagComponent>()))
            {
                Value tagObject;
                tagObject.SetObject();

                TagComponent* pTag = sceneIn.Get<TagComponent>(entity);
                m_ParseTagToDoc(tagObject, *pTag, allocator);
                entityObject.AddMember("tag", tagObject, allocator);
            }
            if(pEntityManager->HasComponent(entity, sceneIn.GetComponentType<TransformComponent>()))
            {
                Value transformObject;
                transformObject.SetObject();

                TransformComponent* pTransform = sceneIn.Get<TransformComponent>(entity);
                m_ParseTransformToDoc(transformObject, *pTransform, allocator);
                entityObject.AddMember("transform", transformObject, allocator);
            }
            if (pEntityManager->HasComponent(entity, sceneIn.GetComponentType<MovementComponent>()))
            {
                Value movementObject;
                movementObject.SetObject();

                MovementComponent* pMovement = sceneIn.Get<MovementComponent>(entity);
                m_ParseMovementToDoc(movementObject, *pMovement, allocator);
                entityObject.AddMember("movement", movementObject, allocator);
            }
            if (pEntityManager->HasComponent(entity, sceneIn.GetComponentType<RotationComponent>()))
            {
                Value rotationObject;
                rotationObject.SetObject();

                RotationComponent* pRotation = sceneIn.Get<RotationComponent>(entity);
                m_ParseRotationToDoc(rotationObject, *pRotation, allocator);
                entityObject.AddMember("rotation", rotationObject, allocator);
            }
            if (pEntityManager->HasComponent(entity, sceneIn.GetComponentType<ParentComponent>()))
            {
                Value parentObject;
                parentObject.SetObject();

                ParentComponent* pParent = sceneIn.Get<ParentComponent>(entity);
                m_ParseParentToDoc(parentObject, *pParent, allocator);
                entityObject.AddMember("parent", parentObject, allocator);
            }
            if (pEntityManager->HasComponent(entity, sceneIn.GetComponentType<TextureComponent>()))
            {
                Value textureObject;
                textureObject.SetObject();

                TextureComponent* pTexture = sceneIn.Get<TextureComponent>(entity);
                m_ParseTextureToDoc(textureObject, *pTexture, allocator);
                entityObject.AddMember("texture", textureObject, allocator);
            }
            if (pEntityManager->HasComponent(entity, sceneIn.GetComponentType<MaterialComponent>()))
            {
                Value materialObject;
                materialObject.SetObject();

                MaterialComponent* pMaterial = sceneIn.Get<MaterialComponent>(entity);
                m_ParseMaterialToDoc(materialObject, *pMaterial, allocator);
                entityObject.AddMember("material", materialObject, allocator);
            }
            if (pEntityManager->HasComponent(entity, sceneIn.GetComponentType<LightComponent>()))
            {
                Value lightObject;
                lightObject.SetObject();

                LightComponent* pLight = sceneIn.Get<LightComponent>(entity);
                m_ParseLightToDoc(lightObject, *pLight, allocator);
                entityObject.AddMember("light", lightObject, allocator);
            }
            if (pEntityManager->HasComponent(entity, sceneIn.GetComponentType<ModelComponent>()))
            {
                Value modelObject;
                modelObject.SetObject();

                ModelComponent* pModel = sceneIn.Get<ModelComponent>(entity);
                m_ParseModelToDoc(modelObject, *pModel, allocator);
                entityObject.AddMember("model", modelObject, allocator);
            }
            if (pEntityManager->HasComponent(entity, sceneIn.GetComponentType<FrameBufferComponent>()))
            {
                Value framebufferObject;
                framebufferObject.SetObject();

                FrameBufferComponent* pFrameBuffer = sceneIn.Get<FrameBufferComponent>(entity);
                m_ParseFrameBufferToDoc(framebufferObject, *pFrameBuffer, allocator);
                entityObject.AddMember("framebuffer", framebufferObject, allocator);
            }
            if (pEntityManager->HasComponent(entity, sceneIn.GetComponentType<FrameBufferViewComponent>()))
            {
                Value framebufferViewObject;
                framebufferViewObject.SetObject();

                FrameBufferViewComponent* pFrameBufferView = sceneIn.Get<FrameBufferViewComponent>(entity);
                m_ParseFrameBufferViewToDoc(framebufferViewObject, *pFrameBufferView, allocator);
                entityObject.AddMember("framebufferView", framebufferViewObject, allocator);
            }
            if (pEntityManager->HasComponent(entity, sceneIn.GetComponentType<TransformAnimationComponent>()))
            {
                Value transformAnimationObject;
                transformAnimationObject.SetObject();

                TransformAnimationComponent* pTransformAnimation = sceneIn.Get<TransformAnimationComponent>(entity);
                m_ParseTransformAnimationToDoc(transformAnimationObject, *pTransformAnimation, allocator);
                entityObject.AddMember("transformAnimation", transformAnimationObject, allocator);
            }
            if (pEntityManager->HasComponent(entity, sceneIn.GetComponentType<TilingComponent>()))
            {
                Value tilingObject;
                tilingObject.SetObject();

                TilingComponent* pTiling = sceneIn.Get<TilingComponent>(entity);
                m_ParseTilingToDoc(tilingObject, *pTiling, allocator);
                entityObject.AddMember("tiling", tilingObject, allocator);
            }
            if (pEntityManager->HasComponent(entity, sceneIn.GetComponentType<EmitterComponent>()))
            {
                Value emitterObject;
                emitterObject.SetObject();

                EmitterComponent* pEmitter = sceneIn.Get<EmitterComponent>(entity);
                m_ParseEmitterToDoc(emitterObject, *pEmitter, allocator);
                entityObject.AddMember("emitter", emitterObject, allocator);
            }
            if (pEntityManager->HasComponent(entity, sceneIn.GetComponentType<GravityComponent>()))
            {
                Value gravityObject;
                gravityObject.SetObject();

                GravityComponent* pGravity = sceneIn.Get<GravityComponent>(entity);
                m_ParseGravityToDoc(gravityObject, *pGravity, allocator);
                entityObject.AddMember("gravity", gravityObject, allocator);
            }
            if (pEntityManager->HasComponent(entity, sceneIn.GetComponentType<RigidBodyComponent>()))
            {
                Value rigidBodyObject;
                rigidBodyObject.SetObject();

                RigidBodyComponent* pRigidBody = sceneIn.Get<RigidBodyComponent>(entity);
                m_ParseRigidBodyToDoc(rigidBodyObject, *pRigidBody, allocator);
                entityObject.AddMember("rigidBody", rigidBodyObject, allocator);
            }
            if (pEntityManager->HasComponent(entity, sceneIn.GetComponentType<SphereColliderComponent>()))
            {
                Value sphereColliderObject;
                sphereColliderObject.SetObject();

                SphereColliderComponent* pSphereCollider = sceneIn.Get<SphereColliderComponent>(entity);
                m_ParseSphereColliderToDoc(sphereColliderObject, *pSphereCollider, allocator);
                entityObject.AddMember("sphereCollider", sphereColliderObject, allocator);
            }
            if (pEntityManager->HasComponent(entity, sceneIn.GetComponentType<AABBColliderComponent>()))
            {
                Value aabbColliderObject;
                aabbColliderObject.SetObject();

                AABBColliderComponent* pAABBCollider = sceneIn.Get<AABBColliderComponent>(entity);
                m_ParseAABBColliderToDoc(aabbColliderObject, *pAABBCollider, allocator);
                entityObject.AddMember("aabbCollider", aabbColliderObject, allocator);
            }
            if (pEntityManager->HasComponent(entity, sceneIn.GetComponentType<SoftBodyComponent>()))
            {
                Value softBodyObject;
                softBodyObject.SetObject();

                SoftBodyComponent* pSoftBody = sceneIn.Get<SoftBodyComponent>(entity);
                m_ParseSoftBodyToDoc(softBodyObject, *pSoftBody, allocator);
                entityObject.AddMember("softBody", softBodyObject, allocator);
            }
            if (pEntityManager->HasComponent(entity, sceneIn.GetComponentType<PlayerComponent>()))
            {
                Value playerObject;
                playerObject.SetObject();

                PlayerComponent* pPlayer = sceneIn.Get<PlayerComponent>(entity);
                m_ParsePlayerToDoc(playerObject, *pPlayer, allocator);
                entityObject.AddMember("player", playerObject, allocator);
            }
            if (pEntityManager->HasComponent(entity, sceneIn.GetComponentType<SteeringBehaviorComponent>()))
            {
                Value steeringBehaviorObject;
                steeringBehaviorObject.SetObject();

                SteeringBehaviorComponent* pSteeringBehavior = sceneIn.Get<SteeringBehaviorComponent>(entity);
                m_ParseSteeringBehaviorToDoc(steeringBehaviorObject, *pSteeringBehavior, allocator);
                entityObject.AddMember("steeringBehavior", steeringBehaviorObject, allocator);
            }

            // Add the entityObject to the main JSON array only if has any component
            if (entityObject.MemberBegin() == entityObject.MemberEnd())
            {
                continue;
            }
            m_doc.PushBack(entityObject, allocator);
        }

        return true;
    }

    bool SceneSerializerJSON::m_ParseTagToDoc(rapidjson::Value& jsonObject, 
                                              TagComponent& tagIn,
							                  rapidjson::Document::AllocatorType& allocator)
    {
        using namespace rapidjson;

        ParserJSON parser = ParserJSON();

        parser.SetMember(jsonObject, "name", tagIn.name, allocator);

        return true;
    }

    bool SceneSerializerJSON::m_ParseTransformToDoc(rapidjson::Value& jsonObject, 
                                                    TransformComponent& transformIn,
							                        rapidjson::Document::AllocatorType& allocator)
    {
        using namespace rapidjson;

        ParserJSON parser = ParserJSON();

        parser.SetMember(jsonObject, "position", transformIn.position, allocator);
        parser.SetMember(jsonObject, "orientation", transformIn.orientation, allocator);
        parser.SetMember(jsonObject, "scale", transformIn.scale, allocator);

        return true;
    }

    bool SceneSerializerJSON::m_ParseMovementToDoc(rapidjson::Value& jsonObject, 
                                                   MovementComponent& movementIn,
							                       rapidjson::Document::AllocatorType& allocator)
    {
        using namespace rapidjson;

        ParserJSON parser = ParserJSON();

        parser.SetMember(jsonObject, "velocity", movementIn.velocity, allocator);
        parser.SetMember(jsonObject, "acceleration", movementIn.acceleration, allocator);
        parser.SetMember(jsonObject, "maxSpeed", movementIn.maxSpeed, allocator);
        parser.SetMember(jsonObject, "drag", movementIn.drag, allocator);

        return true;
    }

    bool SceneSerializerJSON::m_ParseRotationToDoc(rapidjson::Value& jsonObject, RotationComponent& rotationIn, rapidjson::Document::AllocatorType& allocator)
    {
        using namespace rapidjson;

        ParserJSON parser = ParserJSON();

        parser.SetMember(jsonObject, "velocity", rotationIn.velocity, allocator);
        parser.SetMember(jsonObject, "acceleration", rotationIn.acceleration, allocator);
        parser.SetMember(jsonObject, "maxSpeed", rotationIn.maxSpeed, allocator);
        parser.SetMember(jsonObject, "drag", rotationIn.drag, allocator);

        return true;
    }

    bool SceneSerializerJSON::m_ParseParentToDoc(rapidjson::Value& jsonObject, ParentComponent& parentIn, rapidjson::Document::AllocatorType& allocator)
    {
        using namespace rapidjson;

        ParserJSON parser = ParserJSON();

        parser.SetMember(jsonObject, "parentId", parentIn.parentId, allocator);

        return true;
    }

    bool SceneSerializerJSON::m_ParseTextureToDoc(rapidjson::Value& jsonObject, 
                                                  TextureComponent& textureIn, 
                                                  rapidjson::Document::AllocatorType& allocator)
    {
        ParserJSON parser = ParserJSON();

        parser.SetMember(jsonObject, "fileName", textureIn.fileName, allocator);
        parser.SetMember(jsonObject, "textureType", textureIn.textureType, allocator);
        parser.SetMember(jsonObject, "vecTextures", textureIn.vecTextures, allocator);

        return true;
    }

    bool SceneSerializerJSON::m_ParseMaterialToDoc(rapidjson::Value& jsonObject, 
                                                   MaterialComponent& materialIn, 
                                                   rapidjson::Document::AllocatorType& allocator)
    {
        using namespace rapidjson;

        ParserJSON parser = ParserJSON();

        parser.SetMember(jsonObject, "name", materialIn.name, allocator);
        parser.SetMember(jsonObject, "alphaTexture", materialIn.alphaTexture, allocator);
        parser.SetMember(jsonObject, "colorTextures", materialIn.colorTextures, allocator);
        parser.SetMember(jsonObject, "colorTexturesRatios", materialIn.colorTexturesRatios, allocator);
        parser.SetMember(jsonObject, "cubeTexture", materialIn.cubeTexture, allocator);
        parser.SetMember(jsonObject, "offset", materialIn.offset, allocator);
        parser.SetMember(jsonObject, "currOffset", materialIn.currOffset, allocator);
        parser.SetMember(jsonObject, "offsetHeightMap", materialIn.offsetHeightMap, allocator);
        parser.SetMember(jsonObject, "currOffsetHeightMap", materialIn.currOffsetHeightMap, allocator);
        parser.SetMember(jsonObject, "offsetMove", materialIn.offsetMove, allocator);
        parser.SetMember(jsonObject, "discardTexture", materialIn.discardTexture, allocator);
        parser.SetMember(jsonObject, "heightMapTexture", materialIn.heightMapTexture, allocator);
        parser.SetMember(jsonObject, "heightScale", materialIn.heightScale, allocator);
        parser.SetMember(jsonObject, "isEmissive", materialIn.isEmissive, allocator);
        parser.SetMember(jsonObject, "normalTexture", materialIn.normalTexture, allocator);
        parser.SetMember(jsonObject, "specularTexture", materialIn.specularTexture, allocator);
        parser.SetMember(jsonObject, "useAlphaTexture", materialIn.useAlphaTexture, allocator);
        parser.SetMember(jsonObject, "useCubeTexture", materialIn.useCubeTexture, allocator);
        parser.SetMember(jsonObject, "useDiscardTexture", materialIn.useDiscardTexture, allocator);
        parser.SetMember(jsonObject, "useHeightMap", materialIn.useHeightMap, allocator);
        parser.SetMember(jsonObject, "useNormalTexture", materialIn.useNormalTexture, allocator);
        parser.SetMember(jsonObject, "useSpecularTexture", materialIn.useSpecularTexture, allocator);

        return true;
    }

    bool SceneSerializerJSON::m_ParseLightToDoc(rapidjson::Value& jsonObject, LightComponent& lightIn, rapidjson::Document::AllocatorType& allocator)
    {
        using namespace rapidjson;

        ParserJSON parser = ParserJSON();

        parser.SetMember(jsonObject, "distanceOffset", lightIn.distanceOffset, allocator);
        parser.SetMember(jsonObject, "atten", lightIn.atten, allocator);
        parser.SetMember(jsonObject, "diffuse", lightIn.diffuse, allocator);
        parser.SetMember(jsonObject, "direction", lightIn.direction, allocator);
        parser.SetMember(jsonObject, "directionOffset", lightIn.directionOffset, allocator);
        parser.SetMember(jsonObject, "positionOffset", lightIn.positionOffset, allocator);
        parser.SetMember(jsonObject, "params", lightIn.params, allocator);
        parser.SetMember(jsonObject, "specular", lightIn.specular, allocator);
        parser.SetMember(jsonObject, "status", lightIn.status, allocator);

        return true;
    }

    bool SceneSerializerJSON::m_ParseModelToDoc(rapidjson::Value& jsonObject, ModelComponent& modelIn, rapidjson::Document::AllocatorType& allocator)
    {
        using namespace rapidjson;

        ParserJSON parser = ParserJSON();

        parser.SetMember(jsonObject, "isActive", modelIn.isActive, allocator);
        parser.SetMember(jsonObject, "isDoubleSideFace", modelIn.isDoubleSideFace, allocator);
        parser.SetMember(jsonObject, "doNotLight", modelIn.doNotLight, allocator);
        parser.SetMember(jsonObject, "isWireframe", modelIn.isWireframe, allocator);
        parser.SetMember(jsonObject, "isDynamic", modelIn.isDynamic, allocator);
        parser.SetMember(jsonObject, "useDefaultColor", modelIn.useDefaultColor, allocator);
        parser.SetMember(jsonObject, "useTransparency", modelIn.useTransparency, allocator);
        parser.SetMember(jsonObject, "defaultColor", modelIn.defaultColor, allocator);
        parser.SetMember(jsonObject, "useColorTexture", modelIn.useColorTexture, allocator);
        parser.SetMember(jsonObject, "material", modelIn.material, allocator);
        parser.SetMember(jsonObject, "models", modelIn.models, allocator);
        parser.SetMember(jsonObject, "FBOIDs", modelIn.FBOIDs, allocator);

        return true;
    }

    bool SceneSerializerJSON::m_ParseFrameBufferToDoc(rapidjson::Value& jsonObject, FrameBufferComponent& framebufferIn, rapidjson::Document::AllocatorType& allocator)
    {
        using namespace rapidjson;

        ParserJSON parser = ParserJSON();

        parser.SetMember(jsonObject, "cameraId", framebufferIn.cameraId, allocator);
        parser.SetMember(jsonObject, "width", framebufferIn.width, allocator);
        parser.SetMember(jsonObject, "height", framebufferIn.height, allocator);

        return true;
    }

    bool SceneSerializerJSON::m_ParseFrameBufferViewToDoc(rapidjson::Value& jsonObject, FrameBufferViewComponent& framebufferViewIn, rapidjson::Document::AllocatorType& allocator)
    {
        using namespace rapidjson;

        ParserJSON parser = ParserJSON();

        parser.SetMember(jsonObject, "FBOID", framebufferViewIn.FBOID, allocator);
        parser.SetMember(jsonObject, "filter", framebufferViewIn.filter, allocator);

        return true;
    }

    bool SceneSerializerJSON::m_ParseTransformAnimationToDoc(rapidjson::Value& jsonObject, TransformAnimationComponent& animationIn, rapidjson::Document::AllocatorType& allocator)
    {
        // HACK: This should have a parser, but i dont want to put other properties into the json parser 
        using namespace rapidjson;

        ParserJSON parser = ParserJSON();

        // Serialize time
        jsonObject.AddMember("time", animationIn.time, allocator);
        jsonObject.AddMember("isActive", animationIn.isActive, allocator);

        // Serialize position keyframes
        Value positionKeyFramesArray(kArrayType);
        for (const auto& positionKeyFrame : animationIn.positionKeyFrames)
        {
            Value positionKeyFrameObject(kObjectType);
            positionKeyFrameObject.AddMember("time", positionKeyFrame.time, allocator);
            positionKeyFrameObject.AddMember("easeType", (int)positionKeyFrame.easeType, allocator);
            positionKeyFrameObject.AddMember("isKeyEvent", positionKeyFrame.isKeyEvent, allocator);

            // Serialize position value
            Value positionValueArray(kArrayType);
            positionValueArray.PushBack(positionKeyFrame.value.x, allocator);
            positionValueArray.PushBack(positionKeyFrame.value.y, allocator);
            positionValueArray.PushBack(positionKeyFrame.value.z, allocator);
            positionKeyFrameObject.AddMember("value", positionValueArray, allocator);

            positionKeyFramesArray.PushBack(positionKeyFrameObject, allocator);
        }
        jsonObject.AddMember("positionKeyFrames", positionKeyFramesArray, allocator);

        // Serialize scale keyframes
        Value scaleKeyFramesArray(kArrayType);
        for (const auto& scaleKeyFrame : animationIn.scaleKeyFrames)
        {
            Value scaleKeyFrameObject(kObjectType);
            scaleKeyFrameObject.AddMember("time", scaleKeyFrame.time, allocator);
            scaleKeyFrameObject.AddMember("easeType", (int)scaleKeyFrame.easeType, allocator);
            scaleKeyFrameObject.AddMember("isKeyEvent", scaleKeyFrame.isKeyEvent, allocator);

            // Serialize scale value
            Value scaleValue(kNumberType);
            scaleValue = scaleKeyFrame.value;
            scaleKeyFrameObject.AddMember("value", scaleValue, allocator);

            scaleKeyFramesArray.PushBack(scaleKeyFrameObject, allocator);
        }
        jsonObject.AddMember("scaleKeyFrames", scaleKeyFramesArray, allocator);

        // Serialize rotation keyframes
        Value rotationKeyFramesArray(kArrayType);
        for (const auto& rotationKeyFrame : animationIn.rotationKeyFrames)
        {
            Value rotationKeyFrameObject(kObjectType);
            rotationKeyFrameObject.AddMember("time", rotationKeyFrame.time, allocator);
            rotationKeyFrameObject.AddMember("easeType", (int)rotationKeyFrame.easeType, allocator);
            rotationKeyFrameObject.AddMember("isKeyEvent", rotationKeyFrame.isKeyEvent, allocator);

            // Serialize rotation value
            Value rotationValueArray(kArrayType);
            rotationValueArray.PushBack(rotationKeyFrame.value.w, allocator);
            rotationValueArray.PushBack(rotationKeyFrame.value.x, allocator);
            rotationValueArray.PushBack(rotationKeyFrame.value.y, allocator);
            rotationValueArray.PushBack(rotationKeyFrame.value.z, allocator);
            rotationKeyFrameObject.AddMember("value", rotationValueArray, allocator);

            rotationKeyFramesArray.PushBack(rotationKeyFrameObject, allocator);
        }
        jsonObject.AddMember("rotationKeyFrames", rotationKeyFramesArray, allocator);

        return true;
    }

    bool SceneSerializerJSON::m_ParseTilingToDoc(rapidjson::Value& jsonObject, TilingComponent& tilingIn, rapidjson::Document::AllocatorType& allocator)
    {
        using namespace rapidjson;

        ParserJSON parser = ParserJSON();

        parser.SetMember(jsonObject, "axis", tilingIn.axis, allocator);
        parser.SetMember(jsonObject, "offset", tilingIn.offset, allocator);

        return true;
    }

    bool SceneSerializerJSON::m_ParseEmitterToDoc(rapidjson::Value& jsonObject, EmitterComponent& emitterIn, rapidjson::Document::AllocatorType& allocator)
    {
        ParserJSON parser = ParserJSON();

        parser.SetMember(jsonObject, "emitRateMin", emitterIn.emitRateMin, allocator);
        parser.SetMember(jsonObject, "emitRateMax", emitterIn.emitRateMax, allocator);
        parser.SetMember(jsonObject, "maxParticles", emitterIn.maxParticles, allocator);
        parser.SetMember(jsonObject, "isActive", emitterIn.isActive, allocator);

        parser.SetMember(jsonObject, "FBOIDs", emitterIn.properties.FBOIDs, allocator);
        parser.SetMember(jsonObject, "meshName", emitterIn.properties.meshName, allocator);
        parser.SetMember(jsonObject, "material", emitterIn.properties.material, allocator);
        parser.SetMember(jsonObject, "colorInitial", emitterIn.properties.colorInitial, allocator);
        parser.SetMember(jsonObject, "colorChange", emitterIn.properties.colorChange, allocator);
        parser.SetMember(jsonObject, "constForce", emitterIn.properties.constForce, allocator);
        parser.SetMember(jsonObject, "maxLifeTime", emitterIn.properties.maxLifeTime, allocator);
        parser.SetMember(jsonObject, "minLifeTime", emitterIn.properties.minLifeTime, allocator);
        parser.SetMember(jsonObject, "oriMax", emitterIn.properties.oriMax, allocator);
        parser.SetMember(jsonObject, "oriMin", emitterIn.properties.oriMin, allocator);
        parser.SetMember(jsonObject, "posMax", emitterIn.properties.posMax, allocator);
        parser.SetMember(jsonObject, "posMin", emitterIn.properties.posMin, allocator);
        parser.SetMember(jsonObject, "rotMax", emitterIn.properties.rotMax, allocator);
        parser.SetMember(jsonObject, "rotMin", emitterIn.properties.rotMin, allocator);
        parser.SetMember(jsonObject, "scaMax", emitterIn.properties.scaMax, allocator);
        parser.SetMember(jsonObject, "scaMin", emitterIn.properties.scaMin, allocator);
        parser.SetMember(jsonObject, "velMax", emitterIn.properties.velMax, allocator);
        parser.SetMember(jsonObject, "velMin", emitterIn.properties.velMin, allocator);

        return true;
    }

    bool SceneSerializerJSON::m_ParseGravityToDoc(rapidjson::Value& jsonObject, GravityComponent& gravityIn, rapidjson::Document::AllocatorType& allocator)
    {
        using namespace rapidjson;

        ParserJSON parser = ParserJSON();

        parser.SetMember(jsonObject, "acceleration", gravityIn.acceleration, allocator);

        return true;
    }

    bool SceneSerializerJSON::m_ParseRigidBodyToDoc(rapidjson::Value& jsonObject, RigidBodyComponent& rigidBodyIn, rapidjson::Document::AllocatorType& allocator)
    {
        using namespace rapidjson;

        ParserJSON parser = ParserJSON();

        parser.SetMember(jsonObject, "bodyType", rigidBodyIn.bodyType, allocator);
        parser.SetMember(jsonObject, "shapeType", rigidBodyIn.shapeType, allocator);

        return true;
    }

    bool SceneSerializerJSON::m_ParseSphereColliderToDoc(rapidjson::Value& jsonObject, SphereColliderComponent& sphereIn, rapidjson::Document::AllocatorType& allocator)
    {
        using namespace rapidjson;

        ParserJSON parser = ParserJSON();

        parser.SetMember(jsonObject, "radius", sphereIn.radius, allocator);

        return true;
    }

    bool SceneSerializerJSON::m_ParseAABBColliderToDoc(rapidjson::Value& jsonObject, AABBColliderComponent& aabbIn, rapidjson::Document::AllocatorType& allocator)
    {
        using namespace rapidjson;

        ParserJSON parser = ParserJSON();

        parser.SetMember(jsonObject, "min", aabbIn.min, allocator);
        parser.SetMember(jsonObject, "max", aabbIn.max, allocator);

        return true;
    }

    bool SceneSerializerJSON::m_ParseMeshColliderToDoc(rapidjson::Value& jsonObject, MeshColliderComponent& meshIn, rapidjson::Document::AllocatorType& allocator)
    {
        using namespace rapidjson;

        ParserJSON parser = ParserJSON();

        parser.SetMember(jsonObject, "name", meshIn.name, allocator);

        return true;
    }

    bool SceneSerializerJSON::m_ParseCameraToDoc(rapidjson::Value& jsonObject, CameraComponent& cameraIn, rapidjson::Document::AllocatorType& allocator)
    {
        using namespace rapidjson;

        ParserJSON parser = ParserJSON();

        parser.SetMember(jsonObject, "upVector", cameraIn.upVector, allocator);
        parser.SetMember(jsonObject, "distance", cameraIn.distance, allocator);
        parser.SetMember(jsonObject, "height", cameraIn.height, allocator);
        parser.SetMember(jsonObject, "offsetTarget", cameraIn.offsetTarget, allocator);
        parser.SetMember(jsonObject, "fovy", cameraIn.fovy, allocator);
        parser.SetMember(jsonObject, "zNear", cameraIn.zNear, allocator);
        parser.SetMember(jsonObject, "zFar", cameraIn.zFar, allocator);

        return true;
    }

    bool SceneSerializerJSON::m_ParseGridBroadphaseToDoc(rapidjson::Value& jsonObject, GridBroadphaseComponent& gridBroadphaseIn, rapidjson::Document::AllocatorType& allocator)
    {
        using namespace rapidjson;

        ParserJSON parser = ParserJSON();

        parser.SetMember(jsonObject, "lengthPerBox", gridBroadphaseIn.lengthPerBox, allocator);

        return true;
    }

    bool SceneSerializerJSON::m_ParseSoftBodyToDoc(rapidjson::Value& jsonObject, SoftBodyComponent& softBodyIn, rapidjson::Document::AllocatorType& allocator)
    {
        ParserJSON parser = ParserJSON();

        parser.SetMember(jsonObject, "meshName", softBodyIn.meshName, allocator);
        parser.SetMember(jsonObject, "springStrength", softBodyIn.springStrength, allocator);
        parser.SetMember(jsonObject, "wireframeStrength", softBodyIn.wireframeStrength, allocator);
        parser.SetMember(jsonObject, "iterations", softBodyIn.iterations, allocator);
        parser.SetMember(jsonObject, "isWireframe", softBodyIn.isWireframe, allocator);
        parser.SetMember(jsonObject, "isInternalSprings", softBodyIn.isInternalSprings, allocator);

        return true;
    }

    bool SceneSerializerJSON::m_ParsePlayerToDoc(rapidjson::Value& jsonObject, PlayerComponent& playerIn, rapidjson::Document::AllocatorType& allocator)
    {
        using namespace rapidjson;

        ParserJSON parser = ParserJSON();

        parser.SetMember(jsonObject, "speed", playerIn.speed, allocator);

        return true;
    }

    bool SceneSerializerJSON::m_ParseSteeringBehaviorToDoc(rapidjson::Value& jsonObject, SteeringBehaviorComponent& steeringIn, rapidjson::Document::AllocatorType& allocator)
    {
        ParserJSON parser = ParserJSON();

        parser.SetMember(jsonObject, "steeringType", steeringIn.steeringType, allocator);
        parser.SetMember(jsonObject, "targetId", steeringIn.targetId, allocator);
        parser.SetMember(jsonObject, "speed", steeringIn.speed, allocator);
        parser.SetMember(jsonObject, "maxDistance", steeringIn.maxDistance, allocator);
        parser.SetMember(jsonObject, "futureTime", steeringIn.futureTime, allocator);

        return true;
    }

    bool SceneSerializerJSON::m_ParseDocToScene(Scene& sceneOut)
    {
        using namespace rapidjson;

        bool isValid = m_doc.IsArray();
        if (!isValid)
        {
            LOG_ERROR("File not valid, expected array of entities!\n");
            return false;
        }

        // Go over each entity
        for (unsigned int entityIndex = 0; entityIndex < m_doc.Size(); entityIndex++)
        {
            Entity entityId = sceneOut.CreateEntity();

            Value& entityObject = m_doc[entityIndex];
            bool isValid = entityObject.IsObject();
            if (!isValid)
            {
                std::string error = "Entity #" + std::to_string(entityIndex) + 
                    " not valid, expected object of components!\n";
                LOG_ERROR(error);
                return false;
            }

            // Inside EntityID iterate over each component
            for (Value::ConstMemberIterator entityMember = entityObject.MemberBegin(); 
                 entityMember != entityObject.MemberEnd(); ++entityMember)
            {
                std::string componentName = entityMember->name.GetString();

                Value& componentObject = m_doc[entityIndex][entityMember->name];
                bool isValid = componentObject.IsObject();
                if (!isValid)
                {
                    std::string error = "Entity #" + std::to_string(entityIndex) + 
                        ", component '" + componentName + "' not valid\n";
                    LOG_ERROR(error);
                    return false;
                }

                if (componentName == "tag")
                {
                    TagComponent* pTag = sceneOut.AddComponent<TagComponent>(entityId);
                    m_ParseDocToTag(componentObject, *pTag);
                }
                else if (componentName == "camera")
                {
                    CameraComponent* pCamera = sceneOut.AddComponent<CameraComponent>(entityId);
                    m_ParseDocToCamera(componentObject, *pCamera);
                }
                else if (componentName == "transform")
                {
                    TransformComponent* pTransform = sceneOut.AddComponent<TransformComponent>(entityId);
                    m_ParseDocToTransform(componentObject, *pTransform);
                }
                else if (componentName == "movement")
                {
                    MovementComponent* pMovement = sceneOut.AddComponent<MovementComponent>(entityId);
                    m_ParseDocToMovement(componentObject, *pMovement);
                }
                else if (componentName == "parent")
                {
                    ParentComponent* pParent = sceneOut.AddComponent<ParentComponent>(entityId);
                    m_ParseDocToParent(componentObject, *pParent);
                }
                else if (componentName == "rotation")
                {
                    RotationComponent* pRotation = sceneOut.AddComponent<RotationComponent>(entityId);
                    m_ParseDocToRotation(componentObject, *pRotation);
                }
                else if (componentName == "texture")
                {
                    TextureComponent* pTexture = sceneOut.AddComponent<TextureComponent>(entityId);
                    m_ParseDocToTexture(componentObject, *pTexture);
                }
                else if (componentName == "material")
                {
                    MaterialComponent* pMaterial = sceneOut.AddComponent<MaterialComponent>(entityId);
                    m_ParseDocToMaterial(componentObject, *pMaterial);
                }
                else if (componentName == "light")
                {
                    LightComponent* pLight = sceneOut.AddComponent<LightComponent>(entityId);
                    m_ParseDocToLight(componentObject, *pLight);
                }
                else if (componentName == "model")
                {
                    ModelComponent* pModel = sceneOut.AddComponent<ModelComponent>(entityId);
                    m_ParseDocToModel(componentObject, *pModel);
                }
                else if (componentName == "framebuffer")
                {
                    FrameBufferComponent* pFrameBuffer = sceneOut.AddComponent<FrameBufferComponent>(entityId);
                    m_ParseDocToFrameBuffer(componentObject, *pFrameBuffer);
                }
                else if (componentName == "framebufferView")
                {
                    FrameBufferViewComponent* pFrameBufferView = sceneOut.AddComponent<FrameBufferViewComponent>(entityId);
                    m_ParseDocToFrameBufferView(componentObject, *pFrameBufferView);
                }
                else if (componentName == "transformAnimation")
                {
                    TransformAnimationComponent* pTransformAnimation = sceneOut.AddComponent<TransformAnimationComponent>(entityId);
                    m_ParseDocToTransformAnimation(componentObject, *pTransformAnimation);
                }
                else if (componentName == "tiling")
                {
                    TilingComponent* pTiling = sceneOut.AddComponent<TilingComponent>(entityId);
                    m_ParseDocToTiling(componentObject, *pTiling);
                }
                else if (componentName == "emitter")
                {
                    EmitterComponent* pEmitter = sceneOut.AddComponent<EmitterComponent>(entityId);
                    m_ParseDocToEmitter(componentObject, *pEmitter);
                }
                else if (componentName == "gravity")
                {
                    GravityComponent* pGravity = sceneOut.AddComponent<GravityComponent>(entityId);
                    m_ParseDocToGravity(componentObject, *pGravity);
                }
                else if (componentName == "rigidBody")
                {
                    RigidBodyComponent* pRigidBody = sceneOut.AddComponent<RigidBodyComponent>(entityId);
                    m_ParseDocToRigidBody(componentObject, *pRigidBody);
                }
                else if (componentName == "sphereCollider")
                {
                    SphereColliderComponent* pSphereCollider = sceneOut.AddComponent<SphereColliderComponent>(entityId);
                    m_ParseDocToSphereCollider(componentObject, *pSphereCollider);
                }
                else if (componentName == "aabbCollider")
                {
                    AABBColliderComponent* pAABBCollider = sceneOut.AddComponent<AABBColliderComponent>(entityId);
                    m_ParseDocToAABBCollider(componentObject, *pAABBCollider);
                }
                else if (componentName == "softBody")
                {
                    SoftBodyComponent* pSoftBody = sceneOut.AddComponent<SoftBodyComponent>(entityId);
                    m_ParseDocToSoftBody(componentObject, *pSoftBody);
                }
                else if (componentName == "player")
                {
                    PlayerComponent* pPlayer = sceneOut.AddComponent<PlayerComponent>(entityId);
                    m_ParseDocToPlayer(componentObject, *pPlayer);
                }
                else if (componentName == "steeringBehavior")
                {
                    SteeringBehaviorComponent* pSteeringBehavior = sceneOut.AddComponent<SteeringBehaviorComponent>(entityId);
                    m_ParseDocToSteeringBehavior(componentObject, *pSteeringBehavior);
                }
                // All single components goes into the first entity, 
                // so the scene will always have the first entity empty
                else if (componentName == "window")
                {
                    WindowComponent* pWindow = GraphicsLocator::GetWindow();
                    m_ParseDocToWindow(componentObject, *pWindow);
                }
                else if (componentName == "configPath")
                {
                    ConfigPathComponent* pConfigPath = CoreLocator::GetConfigPath();
                    m_ParseDocToConfigPath(componentObject, *pConfigPath);
                }
                else if (componentName == "gridBroadphase")
                {
                    GridBroadphaseComponent* pGridBroadphase = PhysicsLocator::GetGridBroadphase();
                    m_ParseDocToGridBroadphase(componentObject, *pGridBroadphase);
                }
            }
        }

        return true;
    }

    bool SceneSerializerJSON::m_ParseDocToTag(rapidjson::Value& jsonObject, TagComponent& tagOut)
    {
        using namespace rapidjson;

        ParserJSON parser = ParserJSON();

        parser.GetValue(jsonObject["name"], tagOut.name);

        return true;
    }

    bool SceneSerializerJSON::m_ParseDocToTransform(rapidjson::Value& jsonObject, 
                                                    TransformComponent& transformOut)
    {
        using namespace rapidjson;

        ParserJSON parser = ParserJSON();

        parser.GetValue(jsonObject["position"], transformOut.position);
        parser.GetValue(jsonObject["orientation"], transformOut.orientation);
        parser.GetValue(jsonObject["scale"], transformOut.scale);

        // TODO: This should be separated from here, maybe a system to handle this hierachy
        transformOut.worldPosition = transformOut.position;
        transformOut.worldOrientation = transformOut.orientation;
        transformOut.worldScale = transformOut.scale;

        return true;
    }

    bool SceneSerializerJSON::m_ParseDocToMovement(rapidjson::Value& jsonObject, 
                                                   MovementComponent& movementOut)
    {
        using namespace rapidjson;

        ParserJSON parser = ParserJSON();

        parser.GetValue(jsonObject["velocity"], movementOut.velocity);
        parser.GetValue(jsonObject["acceleration"], movementOut.acceleration);
        parser.GetValue(jsonObject["maxSpeed"], movementOut.maxSpeed);
        parser.GetValue(jsonObject["drag"], movementOut.drag);

        return true;
    }

    bool SceneSerializerJSON::m_ParseDocToRotation(rapidjson::Value& jsonObject, RotationComponent& rotationOut)
    {
        using namespace rapidjson;

        ParserJSON parser = ParserJSON();

        parser.GetValue(jsonObject["velocity"], rotationOut.velocity);
        parser.GetValue(jsonObject["acceleration"], rotationOut.acceleration);
        parser.GetValue(jsonObject["maxSpeed"], rotationOut.maxSpeed);
        parser.GetValue(jsonObject["drag"], rotationOut.drag);

        return true;
    }

    bool SceneSerializerJSON::m_ParseDocToParent(rapidjson::Value& jsonObject, ParentComponent& parentOut)
    {
        using namespace rapidjson;

        ParserJSON parser = ParserJSON();

        parser.GetValue(jsonObject["parentId"], parentOut.parentId);

        return true;
    }

    bool SceneSerializerJSON::m_ParseDocToTexture(rapidjson::Value& jsonObject, 
                                                  TextureComponent& textureOut)
    {
        using namespace rapidjson;

        ParserJSON parser = ParserJSON();
        int textureType = 0;

        parser.GetValue(jsonObject["fileName"], textureOut.fileName);
        parser.GetValue(jsonObject["textureType"], textureType);
        parser.GetValue(jsonObject["vecTextures"], textureOut.vecTextures);

        textureOut.textureType = (eTextureType)textureType;

        return true;
    }
    bool SceneSerializerJSON::m_ParseDocToMaterial(rapidjson::Value& jsonObject, 
                                                   MaterialComponent& materialOut)
    {
        using namespace rapidjson;

        ParserJSON parser = ParserJSON();

        parser.GetValue(jsonObject["name"], materialOut.name);
        parser.GetValue(jsonObject["alphaTexture"], materialOut.alphaTexture);
        parser.GetValue(jsonObject["colorTextures"], materialOut.colorTextures);
        parser.GetValue(jsonObject["colorTexturesRatios"], materialOut.colorTexturesRatios);
        parser.GetValue(jsonObject["cubeTexture"], materialOut.cubeTexture);
        parser.GetValue(jsonObject["offset"], materialOut.offset);
        parser.GetValue(jsonObject["offsetHeightMap"], materialOut.offsetHeightMap);
        parser.GetValue(jsonObject["offsetMove"], materialOut.offsetMove);
        parser.GetValue(jsonObject["discardTexture"], materialOut.discardTexture);
        parser.GetValue(jsonObject["heightMapTexture"], materialOut.heightMapTexture);
        parser.GetValue(jsonObject["heightScale"], materialOut.heightScale);
        parser.GetValue(jsonObject["isEmissive"], materialOut.isEmissive);
        parser.GetValue(jsonObject["normalTexture"], materialOut.normalTexture);
        parser.GetValue(jsonObject["specularTexture"], materialOut.specularTexture);
        parser.GetValue(jsonObject["useAlphaTexture"], materialOut.useAlphaTexture);
        parser.GetValue(jsonObject["useCubeTexture"], materialOut.useCubeTexture);
        parser.GetValue(jsonObject["useDiscardTexture"], materialOut.useDiscardTexture);
        parser.GetValue(jsonObject["useHeightMap"], materialOut.useHeightMap);
        parser.GetValue(jsonObject["useNormalTexture"], materialOut.useNormalTexture);
        parser.GetValue(jsonObject["useSpecularTexture"], materialOut.useSpecularTexture);

        return true;
    }
    bool SceneSerializerJSON::m_ParseDocToLight(rapidjson::Value& jsonObject, LightComponent& lightOut)
    {
        using namespace rapidjson;

        ParserJSON parser = ParserJSON();

        parser.GetValue(jsonObject["distanceOffset"], lightOut.distanceOffset);
        parser.GetValue(jsonObject["atten"], lightOut.atten);
        parser.GetValue(jsonObject["diffuse"], lightOut.diffuse);
        parser.GetValue(jsonObject["direction"], lightOut.direction);
        parser.GetValue(jsonObject["directionOffset"], lightOut.directionOffset);
        parser.GetValue(jsonObject["positionOffset"], lightOut.positionOffset);
        parser.GetValue(jsonObject["params"], lightOut.params);
        parser.GetValue(jsonObject["specular"], lightOut.specular);
        parser.GetValue(jsonObject["status"], lightOut.status);

        return true;
    }
    bool SceneSerializerJSON::m_ParseDocToModel(rapidjson::Value& jsonObject, ModelComponent& modelOut)
    {
        ParserJSON parser = ParserJSON();

        parser.GetValue(jsonObject["isActive"], modelOut.isActive);
        parser.GetValue(jsonObject["isDoubleSideFace"], modelOut.isDoubleSideFace);
        parser.GetValue(jsonObject["doNotLight"], modelOut.doNotLight);
        parser.GetValue(jsonObject["isWireframe"], modelOut.isWireframe);
        parser.GetValue(jsonObject["defaultColor"], modelOut.defaultColor);
        parser.GetValue(jsonObject["useDefaultColor"], modelOut.useDefaultColor);
        parser.GetValue(jsonObject["useColorTexture"], modelOut.useColorTexture);
        parser.GetValue(jsonObject["useTransparency"], modelOut.useTransparency);
        parser.GetValue(jsonObject["material"], modelOut.material);
        parser.GetValue(jsonObject["models"], modelOut.models);
        parser.GetValue(jsonObject["FBOIDs"], modelOut.FBOIDs);

        return true;
    }

    bool SceneSerializerJSON::m_ParseDocToFrameBuffer(rapidjson::Value& jsonObject, FrameBufferComponent& framebufferOut)
    {
        ParserJSON parser = ParserJSON();

        parser.GetValue(jsonObject["cameraId"], framebufferOut.cameraId);
        parser.GetValue(jsonObject["width"], framebufferOut.width);
        parser.GetValue(jsonObject["height"], framebufferOut.height);

        return true;
    }

    bool SceneSerializerJSON::m_ParseDocToFrameBufferView(rapidjson::Value& jsonObject, FrameBufferViewComponent& framebufferViewOut)
    {
        ParserJSON parser = ParserJSON();

        parser.GetValue(jsonObject["FBOID"], framebufferViewOut.FBOID);

        int filter = static_cast<int>(framebufferViewOut.filter);
        parser.GetValue(jsonObject["filter"], filter);
        framebufferViewOut.filter = static_cast<eFBFilter>(filter);

        return true;
    }

    bool SceneSerializerJSON::m_ParseDocToTransformAnimation(rapidjson::Value& jsonObject, TransformAnimationComponent& animationOut)
    {
        // HACK: This should have a parser, but i dont want to put other properties into the json parser 
        using namespace rapidjson;

        if (!jsonObject["positionKeyFrames"].IsArray())
        {
            LOG_ERROR("Expected array of objects for position animation component");
            return false;
        }

        if (!jsonObject["scaleKeyFrames"].IsArray())
        {
            LOG_ERROR("Expected array of objects for scale animation component");
            return false;
        }

        if (!jsonObject["rotationKeyFrames"].IsArray())
        {
            LOG_ERROR("Expected array of objects for rotation animation component");
            return false;
        }

        ParserJSON parser = ParserJSON();

        for (unsigned int i = 0; i < jsonObject["positionKeyFrames"].Size(); i++)
        {
            // Create keyframes
            PositionKeyFrame keyFrame = PositionKeyFrame();

            Value& positionKeyFrames = jsonObject["positionKeyFrames"][i];

            Value& positionObj = positionKeyFrames["value"];
            parser.GetValue(positionObj, keyFrame.value);

            Value& timeObj = positionKeyFrames["time"];
            parser.GetValue(timeObj, keyFrame.time);

            Value& isKeyEventObj = positionKeyFrames["isKeyEvent"];
            parser.GetValue(isKeyEventObj, keyFrame.isKeyEvent);

            int type = 0;
            Value& typeObj = positionKeyFrames["easeType"];
            parser.GetValue(typeObj, type);
            keyFrame.easeType = (eEasingType)type;

            animationOut.positionKeyFrames.push_back(keyFrame);
        }

        for (unsigned int i = 0; i < jsonObject["rotationKeyFrames"].Size(); i++)
        {
            // Create keyframes
            RotationKeyFrame keyFrame = RotationKeyFrame();

            Value& rotationKeyFrames = jsonObject["rotationKeyFrames"][i];

            Value& rotationObj = rotationKeyFrames["value"];
            parser.GetValue(rotationObj, keyFrame.value);

            Value& timeObj = rotationKeyFrames["time"];
            parser.GetValue(timeObj, keyFrame.time);

            Value& isKeyEventObj = rotationKeyFrames["isKeyEvent"];
            parser.GetValue(isKeyEventObj, keyFrame.isKeyEvent);

            int type = 0;
            Value& typeObj = rotationKeyFrames["easeType"];
            parser.GetValue(typeObj, type);
            keyFrame.easeType = (eEasingType)type;

            animationOut.rotationKeyFrames.push_back(keyFrame);
        }

        for (unsigned int i = 0; i < jsonObject["scaleKeyFrames"].Size(); i++)
        {
            // Create keyframes
            ScaleKeyFrame keyFrame = ScaleKeyFrame();

            Value& scaleKeyFrames = jsonObject["scaleKeyFrames"][i];

            Value& scaleObj = scaleKeyFrames["value"];
            parser.GetValue(scaleObj, keyFrame.value);

            Value& timeObj = scaleKeyFrames["time"];
            parser.GetValue(timeObj, keyFrame.time);

            Value& isKeyEventObj = scaleKeyFrames["isKeyEvent"];
            parser.GetValue(isKeyEventObj, keyFrame.isKeyEvent);

            int type = 0;
            Value& typeObj = scaleKeyFrames["easeType"];
            parser.GetValue(typeObj, type);
            keyFrame.easeType = (eEasingType)type;

            animationOut.scaleKeyFrames.push_back(keyFrame);
        }

        if (jsonObject.HasMember("isActive"))
        {
            Value& activeObj = jsonObject["isActive"];
            parser.GetValue(activeObj, animationOut.isActive);
        }

        return true;
    }

    bool SceneSerializerJSON::m_ParseDocToTiling(rapidjson::Value& jsonObject, TilingComponent& tilingOut)
    {
        ParserJSON parser = ParserJSON();

        parser.GetValue(jsonObject["axis"], tilingOut.axis);
        parser.GetValue(jsonObject["offset"], tilingOut.offset);

        return true;
    }

    bool SceneSerializerJSON::m_ParseDocToEmitter(rapidjson::Value& jsonObject, EmitterComponent& emitterOut)
    {
        ParserJSON parser = ParserJSON();

        parser.GetValue(jsonObject["emitRateMin"], emitterOut.emitRateMin);
        parser.GetValue(jsonObject["emitRateMax"], emitterOut.emitRateMax);
        parser.GetValue(jsonObject["maxParticles"], emitterOut.maxParticles);
        parser.GetValue(jsonObject["isActive"], emitterOut.isActive);

        parser.GetValue(jsonObject["FBOIDs"], emitterOut.properties.FBOIDs);
        parser.GetValue(jsonObject["meshName"], emitterOut.properties.meshName);
        parser.GetValue(jsonObject["material"], emitterOut.properties.material);
        parser.GetValue(jsonObject["colorInitial"], emitterOut.properties.colorInitial);
        parser.GetValue(jsonObject["colorChange"], emitterOut.properties.colorChange);
        parser.GetValue(jsonObject["constForce"], emitterOut.properties.constForce);
        parser.GetValue(jsonObject["maxLifeTime"], emitterOut.properties.maxLifeTime);
        parser.GetValue(jsonObject["minLifeTime"], emitterOut.properties.minLifeTime);
        parser.GetValue(jsonObject["oriMax"], emitterOut.properties.oriMax);
        parser.GetValue(jsonObject["oriMin"], emitterOut.properties.oriMin);
        parser.GetValue(jsonObject["posMax"], emitterOut.properties.posMax);
        parser.GetValue(jsonObject["posMin"], emitterOut.properties.posMin);
        parser.GetValue(jsonObject["rotMax"], emitterOut.properties.rotMax);
        parser.GetValue(jsonObject["rotMin"], emitterOut.properties.rotMin);
        parser.GetValue(jsonObject["scaMax"], emitterOut.properties.scaMax);
        parser.GetValue(jsonObject["scaMin"], emitterOut.properties.scaMin);
        parser.GetValue(jsonObject["velMax"], emitterOut.properties.velMax);
        parser.GetValue(jsonObject["velMin"], emitterOut.properties.velMin);

        return true;
    }

    bool SceneSerializerJSON::m_ParseDocToGravity(rapidjson::Value& jsonObject, GravityComponent& gravityOut)
    {
        ParserJSON parser = ParserJSON();

        parser.GetValue(jsonObject["acceleration"], gravityOut.acceleration);

        return true;
    }

    bool SceneSerializerJSON::m_ParseDocToRigidBody(rapidjson::Value& jsonObject, RigidBodyComponent& rigidbodyOut)
    {
        ParserJSON parser = ParserJSON();
        int bodyType = 0;
        int shapeType = 0;

        parser.GetValue(jsonObject["bodyType"], bodyType);
        parser.GetValue(jsonObject["shapeType"], shapeType);

        rigidbodyOut.bodyType = (eBody)bodyType;
        rigidbodyOut.shapeType = (eShape)shapeType;

        return true;
    }

    bool SceneSerializerJSON::m_ParseDocToSphereCollider(rapidjson::Value& jsonObject, SphereColliderComponent& sphereOut)
    {
        ParserJSON parser = ParserJSON();

        parser.GetValue(jsonObject["radius"], sphereOut.radius);

        return true;
    }

    bool SceneSerializerJSON::m_ParseDocToAABBCollider(rapidjson::Value& jsonObject, AABBColliderComponent& aabbOut)
    {
        ParserJSON parser = ParserJSON();

        parser.GetValue(jsonObject["min"], aabbOut.min);
        parser.GetValue(jsonObject["max"], aabbOut.max);

        return true;
    }

    bool SceneSerializerJSON::m_ParseDocToMeshCollider(rapidjson::Value& jsonObject, MeshColliderComponent& meshOut)
    {
        ParserJSON parser = ParserJSON();

        parser.GetValue(jsonObject["name"], meshOut.name);

        return true;
    }

    bool SceneSerializerJSON::m_ParseDocToCamera(rapidjson::Value& jsonObject, CameraComponent& cameraOut)
    {
        ParserJSON parser = ParserJSON();

        parser.GetValue(jsonObject["upVector"], cameraOut.upVector);
        parser.GetValue(jsonObject["distance"], cameraOut.distance);
        parser.GetValue(jsonObject["height"], cameraOut.height);
        parser.GetValue(jsonObject["offsetTarget"], cameraOut.offsetTarget);
        parser.GetValue(jsonObject["fovy"], cameraOut.fovy);
        parser.GetValue(jsonObject["zNear"], cameraOut.zNear);
        parser.GetValue(jsonObject["zFar"], cameraOut.zFar);

        return true;
    }
    bool SceneSerializerJSON::m_ParseDocToWindow(rapidjson::Value& jsonObject, WindowComponent& windowOut)
    {
        ParserJSON parser = ParserJSON();

        parser.GetValue(jsonObject["name"], windowOut.name);
        parser.GetValue(jsonObject["height"], windowOut.height);
        parser.GetValue(jsonObject["width"], windowOut.width);

        return true;
    }
    bool SceneSerializerJSON::m_ParseDocToConfigPath(rapidjson::Value& jsonObject, ConfigPathComponent& configPathOut)
    {
        ParserJSON parser = ParserJSON();

        parser.GetValue(jsonObject["pathAudio"], configPathOut.pathAudio);
        parser.GetValue(jsonObject["pathModels"], configPathOut.pathModels);
        parser.GetValue(jsonObject["pathScripts"], configPathOut.pathScripts);
        parser.GetValue(jsonObject["pathShaders"], configPathOut.pathShaders);
        parser.GetValue(jsonObject["pathTextures"], configPathOut.pathTextures);

        parser.GetValue(jsonObject["pathDebugSquare"], configPathOut.pathDebugSquare);
        parser.GetValue(jsonObject["pathDebugSphere"], configPathOut.pathDebugSphere);

        return true;
    }

    bool SceneSerializerJSON::m_ParseDocToGridBroadphase(rapidjson::Value& jsonObject, GridBroadphaseComponent& gridBroadphaseOut)
    {
        ParserJSON parser = ParserJSON();

        parser.GetValue(jsonObject["lengthPerBox"], gridBroadphaseOut.lengthPerBox);

        return true;
    }

    bool SceneSerializerJSON::m_ParseDocToSoftBody(rapidjson::Value& jsonObject, SoftBodyComponent& softBodyOut)
    {
        ParserJSON parser = ParserJSON();

        parser.GetValue(jsonObject["meshName"], softBodyOut.meshName);
        parser.GetValue(jsonObject["springStrength"], softBodyOut.springStrength);
        parser.GetValue(jsonObject["wireframeStrength"], softBodyOut.wireframeStrength);
        parser.GetValue(jsonObject["iterations"], softBodyOut.iterations);
        parser.GetValue(jsonObject["isWireframe"], softBodyOut.isWireframe);
        parser.GetValue(jsonObject["isInternalSprings"], softBodyOut.isInternalSprings);

        return true;
    }

    bool SceneSerializerJSON::m_ParseDocToPlayer(rapidjson::Value& jsonObject, PlayerComponent& playerOut)
    {
        ParserJSON parser = ParserJSON();

        parser.GetValue(jsonObject["speed"], playerOut.speed);

        return true;
    }
    bool SceneSerializerJSON::m_ParseDocToSteeringBehavior(rapidjson::Value& jsonObject, SteeringBehaviorComponent& steeringOut)
    {
        ParserJSON parser = ParserJSON();

        int type;
        parser.GetValue(jsonObject["steeringType"], type);
        parser.GetValue(jsonObject["targetId"], steeringOut.targetId);
        parser.GetValue(jsonObject["speed"], steeringOut.speed);
        parser.GetValue(jsonObject["maxDistance"], steeringOut.maxDistance);
        parser.GetValue(jsonObject["futureTime"], steeringOut.futureTime);

        steeringOut.steeringType = static_cast<eSteeringTypes>(type);

        return true;
    }
}
