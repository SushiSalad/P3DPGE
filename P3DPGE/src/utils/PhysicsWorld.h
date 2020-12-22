#pragma once
//#include "../components/Transform.h"
//#include "../components/Physics.h"
//#include "../math/Math.h"

enum struct CollisionDetectionMode {
	DISCRETE, /*CONTINUOUS, GJK,*/ NONE
};

enum struct IntegrationMode {
	/*RK4, VERLET,*/ EULER
};

//struct PhysEntity {
//	Vector3 position;
//	Vector3 velocity;
//	Vector3 acceleration;
//
//	Vector3 rotation;
//	Vector3 rotVelocity;
//	Vector3 rotAcceleration;
//};

//TODO(p,delle) look into maybe having physics here instead

struct PhysicsWorld {
	//std::map<EntityID, PhysEntity> entityTuples;

	IntegrationMode integrationMode			= IntegrationMode::EULER;
	CollisionDetectionMode collisionMode	= CollisionDetectionMode::DISCRETE;

	float maxVelocity = 50.f;
	float minVelocity = 0.1f;

	float gravity		= 9.81f;
	float frictionAir	= 0.01f; //TODO(p,delle) this should depend on object shape

	PhysicsWorld() {
		this->integrationMode	= IntegrationMode::EULER;
		this->collisionMode		= CollisionDetectionMode::DISCRETE;
		this->gravity			= 9.81f;
		this->frictionAir		= 0.01f;
	}

	PhysicsWorld(IntegrationMode im, CollisionDetectionMode cm, 
				float gravity = 9.81f, float frictionAir = 0.01f) {
		this->integrationMode = im;
		this->collisionMode = cm;
		this->gravity = gravity;
		this->frictionAir = frictionAir;
	}
};