/*
* All or portions of this file Copyright (c) Amazon.com, Inc. or its affiliates or
* its licensors.
*
* For complete copyright and license terms please see the LICENSE at the root of this
* distribution (the "License"). All use of this software is governed by the License,
* or, if provided, by the license below or the license accompanying this file. Do not
* remove or modify any license notices. This file is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
*
*/

#include <PhysX_precompiled.h>
#include "PhysXTestFixtures.h"
#include "PhysXTestUtil.h"
#include "PhysXTestCommon.h"

#include <AzCore/Script/ScriptContext.h>
#include <AzCore/Script/lua/lua.h>
#include <AzCore/RTTI/BehaviorContext.h>
#include <AzCore/Math/MathReflection.h>
#include <AzCore/UnitTest/TestTypes.h>
#include <AzFramework/Physics/Utils.h>
#include <AzCore/Math/MathReflection.h>
#include <AzFramework/Entity/EntityContext.h>

namespace PhysX
{
    static AZStd::map<AZStd::string, EntityPtr> s_testEntities;

    AZ::EntityId GetTestEntityId(const char* name)
    {
        if (auto it = s_testEntities.find(name);
            it != s_testEntities.end())
        {
            return it->second->GetId();
        }

        return AZ::EntityId();
    }

    void ExpectTrue(bool check)
    {
        EXPECT_TRUE(check);
    }

    class PhysXScriptTest
        : public PhysXDefaultWorldTest
    {
    public:
        AZ_TYPE_INFO(PhysXScriptTest, "{337A9DB4-ACF7-42A7-92E5-48A9FF14B49C}");

        void SetUp() override
        {
            PhysXDefaultWorldTest::SetUp();

            m_behaviorContext = AZStd::make_unique<AZ::BehaviorContext>();
            AZ::Entity::Reflect(m_behaviorContext.get());
            AZ::MathReflect(m_behaviorContext.get());
            AzFramework::EntityContext::Reflect(m_behaviorContext.get());
            Physics::ReflectionUtils::ReflectPhysicsApi(m_behaviorContext.get());
            m_behaviorContext->Method("ExpectTrue", &ExpectTrue);
            m_behaviorContext->Method("GetTestEntityId", &GetTestEntityId);
            m_scriptContext = AZStd::make_unique<AZ::ScriptContext>();
            m_scriptContext->BindTo(m_behaviorContext.get());
        }

        void TearDown() override
        {
            s_testEntities.clear();

            m_scriptContext.reset();
            m_behaviorContext.reset();

            PhysXDefaultWorldTest::TearDown();
        }

        AZ::BehaviorContext* GetBehaviorContext()
        {
            return m_behaviorContext.get();
        }

        AZ::ScriptContext* GetScriptContext()
        {
            return m_scriptContext.get();
        }

    private:
        AZStd::unique_ptr<AZ::BehaviorContext> m_behaviorContext;
        AZStd::unique_ptr<AZ::ScriptContext> m_scriptContext;
    };

    TEST_F(PhysXScriptTest, ScriptedRaycast_RaycastNotIntersectingBox_ReturnsNoHits)
    {
        s_testEntities.insert(
            {
                "Box",
                TestUtils::AddStaticUnitTestObject<BoxColliderComponent>(AZ::Vector3::CreateZero(), "Box")
            });

        const char luaCode[] =
            R"(
                boxId = GetTestEntityId("Box")
                request = RayCastRequest()
                request.Start = Vector3(5.0, 0.0, 5.0)
                request.Direction = Vector3(0.0, 0.0, -1.0)
                request.Distance = 10.0
                request.MaxResults = 1
                hit = WorldBodyRequestBus.Event.RayCast(boxId, request)
                ExpectTrue(hit.EntityId == EntityId())
            )";

        EXPECT_TRUE(GetScriptContext()->Execute(luaCode));
    }

    TEST_F(PhysXScriptTest, ScriptedRaycast_RaycastIntersectingBox_ReturnsHitOnBox)
    {
        s_testEntities.insert(
            {
                "Box",
                TestUtils::AddStaticUnitTestObject<BoxColliderComponent>(AZ::Vector3::CreateZero(), "Box")
            });

        const char luaCode[] =
            R"(
                boxId = GetTestEntityId("Box")
                request = RayCastRequest()
                request.Start = Vector3(0.0, 0.0, 5.0)
                request.Direction = Vector3(0.0, 0.0, -1.0)
                request.Distance = 10.0
                request.MaxResults = 1
                hit = WorldBodyRequestBus.Event.RayCast(boxId, request)
                ExpectTrue(hit.EntityId == boxId)
            )";

        EXPECT_TRUE(GetScriptContext()->Execute(luaCode));
    }

    TEST_F(PhysXScriptTest, ScriptedRaycast_RaycastNotInteractingCollisionFilters_ReturnsNoHit)
    {
        s_testEntities.insert(
            {
                "Box",
                TestUtils::AddStaticUnitTestObject<BoxColliderComponent>(AZ::Vector3::CreateZero(), "Box")
            });

        const char luaCode[] =
            R"(
                boxId = GetTestEntityId("Box")
                request = RayCastRequest()
                request.Start = Vector3(0.0, 0.0, 5.0)
                request.Direction = Vector3(0.0, 0.0, -1.0)
                request.Distance = 10.0
                request.MaxResults = 1
                request.Collision = CollisionGroup("None")
                hit = WorldBodyRequestBus.Event.RayCast(boxId, request)
                ExpectTrue(hit.EntityId == EntityId())
            )";

        EXPECT_TRUE(GetScriptContext()->Execute(luaCode));
    }
} // namespace PhysX
