//
// Created by Monika on 22.11.2022.
//

#ifndef SR_ENGINE_BULLET3RIGIDBODY3D_H
#define SR_ENGINE_BULLET3RIGIDBODY3D_H

#include <Physics/3D/Rigidbody3D.h>

class btRigidBody;
class btMotionState;

namespace SR_PTYPES_NS {
    class Bullet3Rigidbody3D : public Rigidbody3D {
        using Super = Rigidbody3D;
    public:
        explicit Bullet3Rigidbody3D(LibraryPtr pLibrary);
        ~Bullet3Rigidbody3D() override;

    public:
        SR_NODISCARD void* GetHandle() const noexcept override;

    public:
        void UpdateInertia() override;
        bool InitBody() override;

        bool UpdateMatrix(bool force) override;
        bool UpdateShapeInternal() override;

        void AddLinearVelocity(const SR_MATH_NS::FVector3& velocity) override;
        void AddAngularVelocity(const SR_MATH_NS::FVector3& velocity) override;
        //void SetVelocity(const SR_MATH_NS::FVector3& velocity) override;

    private:
        btRigidBody* m_rigidbody = nullptr;
        btMotionState* m_motionState = nullptr;

    };
}

#endif //SR_ENGINE_BULLET3RIGIDBODY3D_H
