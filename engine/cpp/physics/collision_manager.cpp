#include <physics/collision_manager.h>
#include <geometry/geometry_manager.h>

namespace xpe {

	namespace physics {

		// temporary function created to convert quat to EulerXYZ
		glm::vec3 QuatToEuler(float w, float x, float y, float z) {
			glm::quat quaternion(w, x, y, z);
			return glm::eulerAngles(quaternion);
		}

		Ref<PhysicDynamicActor> CollisionSystem::AddPhysicDynamicActor(glm::vec3 position, glm::vec3 linearVelocity)
		{
			Ref<PhysicDynamicActor> actor;
			actor.Create(PhysicDynamicActor());

			actor->Actor = PhysicsManager::CreateDynamicActor(position);
			actor->Actor->setLinearVelocity(PxVec3(linearVelocity.x, linearVelocity.y, linearVelocity.z));

			PhysicsManager::AddActor(*actor->Actor);

			return actor;
		}

		// Urgently need to be broken down into several different functions.... 
		PhysicShapeCollision* CollisionSystem::AddPhysicShape(Ref<PhysicDynamicActor>& actor, PhysicMaterial material, PhysicShapeData data, glm::vec3 halfExtents)
		{
			actor->Shapes.emplace_back(PhysicShape());
			PhysicShape* physicShape = &actor->Shapes.back();

			PxRigidDynamic* Actor = actor->Actor;
			PxMaterial* Material = PhysicsManager::CreateMaterial(material.StaticFriction, material.DynamicFriction, material.Restitution);
			physicShape->Shape = PxRigidActorExt::createExclusiveShape(*Actor, PxBoxGeometry(halfExtents.x, halfExtents.y, halfExtents.z), *Material);

			physicShape->Shape->setFlag(PxShapeFlag::eVISUALIZATION, true);
			physicShape->Shape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, true);
			//shape->setFlag(PxShapeFlag::eTRIGGER_SHAPE, true);
			//shape->setFlag(PxShapeFlag::eSCENE_QUERY_SHAPE, true);

			physicShape->Shape->setLocalPose(PxTransform(data.LocalPos.x, data.LocalPos.y, data.LocalPos.z));
			physicShape->Shape->setContactOffset(data.ContactOffset);
			physicShape->Shape->setRestOffset(data.RestOffset);

			PxFilterData FilterData;
			FilterData.word0 = data.FilterWord0;
			FilterData.word1 = data.FilterWord1;
			FilterData.word2 = data.FilterWord2;
			FilterData.word3 = data.FilterWord3;

			physicShape->Shape->setSimulationFilterData(FilterData);

			physicShape->Collision = new PhysicShapeCollision(); //no data cleansing has been created
			physicShape->Collision->Geometry.Create(*GeometryManager::AddGeometry(Cube()));

			auto ATransform = Actor->getGlobalPose();
			auto STransform = physicShape->Shape->getLocalPose();

			// need push this to another func

			glm::vec3 ShapeEuler = QuatToEuler(ATransform.q.w, ATransform.q.x, ATransform.q.y, ATransform.q.z);

			physicShape->Collision->PosisionX.store(ATransform.p.x + STransform.p.x, std::memory_order_relaxed);
			physicShape->Collision->PosisionY.store(ATransform.p.y + STransform.p.y, std::memory_order_relaxed);
			physicShape->Collision->PosisionZ.store(ATransform.p.z + STransform.p.z, std::memory_order_relaxed);

			physicShape->Collision->RotationX.store(ShapeEuler.x, std::memory_order_relaxed);
			physicShape->Collision->RotationY.store(ShapeEuler.y, std::memory_order_relaxed);
			physicShape->Collision->RotationZ.store(ShapeEuler.z, std::memory_order_relaxed);

			physicShape->Collision->ScaleX.store(halfExtents.x * 2, std::memory_order_relaxed);
			physicShape->Collision->ScaleY.store(halfExtents.y * 2, std::memory_order_relaxed);
			physicShape->Collision->ScaleZ.store(halfExtents.z * 2, std::memory_order_relaxed);

			//--

			actor->Shapes.push_back(*physicShape);

			return physicShape->Collision;
		}
	}
}