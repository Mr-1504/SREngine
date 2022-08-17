//
// Created by Monika on 28.07.2022.
//

#ifndef SRENGINE_RIGIDBODY_H
#define SRENGINE_RIGIDBODY_H

#include <Physics/PhysicsLib.h>
#include <Utils/ECS/Component.h>
#include <Utils/Types/SafePointer.h>

namespace SR_PHYSICS_NS {
    class PhysicsScene;
}

namespace SR_PHYSICS_NS::Types {
    class Rigidbody : public SR_UTILS_NS::Component {
        friend class SR_PHYSICS_NS::PhysicsScene;
    public:
        using ComponentPtr = SR_UTILS_NS::Component*;
        using PhysicsScenePtr = SR_HTYPES_NS::SafePtr<PhysicsScene>;

    public:
        Rigidbody();
        ~Rigidbody() override;

    public:
        virtual void UpdateMatrix();

    protected:
        void OnAttached() override;

        void OnMatrixDirty() override;

        virtual bool InitShape();
        virtual bool InitBody();

        PhysicsScenePtr GetPhysicsScene();

    private:
        PhysicsScenePtr m_physicsScene;

        btRigidBody* m_rigidbody = nullptr;
        btCollisionShape* m_shape = nullptr;
        btMotionState* m_motionState = nullptr;

        bool m_dirty = false;

        float_t m_mass = 0.1f;

    };
}

#endif //SRENGINE_RIGIDBODY_H
