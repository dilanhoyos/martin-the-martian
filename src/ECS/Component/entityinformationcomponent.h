#ifndef __ENTITYINFORMATIONCOMPONENT_H__
#define __ENTITYINFORMATIONCOMPONENT_H__

#include <tuple>
#include <ECS/Component/component.h>


namespace ECS {

    class EntityInformationComponent_ : public Component_ {
        std::tuple<unsigned int, unsigned int, unsigned int> kinematicTupleIds;
        std::tuple<unsigned int, unsigned int> renderingTupleIds;
    public:
        EntityInformationComponent_() = default;

        std::tuple<unsigned int&, unsigned int&, unsigned int&> GetKinematicTupleIds();
        void SetKinematicTupleIds(unsigned int posId, unsigned int speedId, unsigned int accelId);


        std::tuple<unsigned int&, unsigned int&> GetRenderingTupleIds();
        void SetRenderingTupleIds(unsigned int posId, unsigned int speedId);

    };

}


#endif //__ENTITYINFORMATIONCOMPONENT_H__