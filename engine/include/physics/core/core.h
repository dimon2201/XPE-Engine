#pragma once
#include <physics/physics.h>
#include <geometry/geometries.h>

namespace xpe {

	namespace physics {

		using namespace render;
		using namespace math;

		struct PhysicShapeCollision : public Object // represents the collision of the object on which we iterate for rendering. 
		{
			std::atomic<float> PosisionX;
			std::atomic<float> PosisionY;
			std::atomic<float> PosisionZ;

			std::atomic<float> RotationX;
			std::atomic<float> RotationY;
			std::atomic<float> RotationZ;

			std::atomic<float> ScaleX;
			std::atomic<float> ScaleY;
			std::atomic<float> ScaleZ;

			Ref<Geometry<Vertex3D>> Geometry;
		};

		struct PhysicShape : public Object 
		{
			PxShape* Shape;
			PhysicShapeCollision* Collision;
		};

		struct PhysicDynamicActor : public Object // created to update the parameters we need after simulate() 
		{
			PxRigidDynamic* Actor;
			vector<PhysicShape> Shapes;
		};

		struct PhysicMaterial : public Object // this structure was designed to create figures
		{
			f32 StaticFriction = 0.5f;
			f32 DynamicFriction = 0.5f;
			f32 Restitution = 0.5f;
		};

		struct PhysicShapeData : public Object // need some review // this structure was designed to create figures
		{
			glm::vec3 LocalPos = { 0.0f, 0.0f, 0.0f };

			f32 ContactOffset = 0.1f;
			f32 RestOffset = 0.0f;

			u32 FilterWord0 = 0;
			u32 FilterWord1 = 0;
			u32 FilterWord2 = 0;
			u32 FilterWord3 = 0;
		};
	}
}