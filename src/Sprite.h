#ifndef __SHIP__
#define __SHIP__

#include <set>
#include <SDL2_image/SDL_image.h>
#include <ECS/Component/entityinformationcomponent.h>

#include "ECS/Entity/entitymanager.h"
#include "ECS/Component/componentmanager.h"

SDL_Texture* SDLCreateTextureFromSurface(SDL_Surface* pSurface);
namespace GTech{


    class Sprite {

    public:
        static unsigned int CreateSprite(std::string path){

            auto& entityManager = ECS::EntityManager::GetInstance();
            auto& componentManager = ECS::ComponentManager::GetInstance();

            //Create Sprite Entity_a
            auto spriteId = entityManager.CreateEntity();

            //Create Texture Id
            auto textureComponentId = componentManager.CreateComponent<ECS::TextureComponent_>();
            auto textureComponentRP = componentManager.GetComponentRaw<ECS::TextureComponent_>(textureComponentId);

            //SetTexture
            textureComponentRP->SetTexture(path);

            //Add Components to spriteId
            auto accelerationComponentId = componentManager.CreateComponent<ECS::AccelerationComponent_>();
            auto positionComponentId     = componentManager.CreateComponent<ECS::PositionComponent_>();
            auto speedComponentId        = componentManager.CreateComponent<ECS::SpeedComponent_>();

            entityManager.AddComponent(spriteId, accelerationComponentId);
            entityManager.AddComponent(spriteId, positionComponentId);
            entityManager.AddComponent(spriteId, speedComponentId);
            entityManager.AddComponent(spriteId, textureComponentId);

            //Subscribe to kinematic elements
            auto informationId  = entityManager.GetComponentsIds(spriteId)[0];
            auto informationRP  = componentManager.GetComponentRaw<ECS::EntityInformationComponent_>(informationId);
            informationRP->SetKinematicTupleIds(positionComponentId, speedComponentId, accelerationComponentId);
            informationRP->SetRenderingTupleIds(positionComponentId, textureComponentId);

            return spriteId;

        }


        static void SetPosition(unsigned int shipId, glm::vec3 position) {

            auto& componentManager = ECS::ComponentManager::GetInstance();
            auto  infoComponentRP = componentManager.GetComponentRaw<ECS::EntityInformationComponent_>(ECS::EntityManager::GetInstance().GetComponentsIds(shipId)[0]);
            auto  [positionId, textureId] = infoComponentRP->GetRenderingTupleIds();
            auto  positionComponent = componentManager.GetComponentRaw<ECS::PositionComponent_>(positionId);
            positionComponent->position = position;

        }




    };
}


#endif /* __SHIP__ */