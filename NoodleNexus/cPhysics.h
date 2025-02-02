#pragma once

#include <vector>
#include <glm/glm.hpp>
#include <glm/vec3.hpp>
#include <vector>
// 
#include "sMesh.h"
#include "cVAOManager/cVAOManager.h"
#include <stdlib.h>

class cPhysics
{
public:

	// Info for the physics movement, etc.
	struct sPhysInfo
	{
		sPhysInfo()
		{
			this->position = glm::vec3(0.0f);
			this->acceleration = glm::vec3(0.0f);
			this->velocity = glm::vec3(0.0f);
			this->bDoesntMove = false;
		}		
		glm::vec3 position;
		glm::vec3 velocity;
		glm::vec3 acceleration;
		// If true, then we skip the integration step
		bool bDoesntMove;	// i.e. doesn't move
		// TODO: 
//		float mass;
//		float inverseMass;		// Usually this is what we keep

		// The drawing mesh that's connected to this physics object
		sMesh* pAssociatedDrawingMeshInstance = NULL;

		// Maybe other things, too
//		float mass;
//		glm::vec3 orientation;
	};

	// Types of primatives
	struct sSphere 
	{
		sSphere() 
		{
			this->centre = glm::vec3(0.0f);
			this->radius = 0.0f;
			this->pPhysicInfo = new sPhysInfo();
		}
		~sSphere()
		{
			delete this->pPhysicInfo;
		}
		glm::vec3 centre;
		float radius;
		sPhysInfo* pPhysicInfo = NULL;
//		unsigned int uniqueID;
		//unsigned int matchingMeshID;
	};

	struct sAABB		// Axis Aligned Bounding Box
	{
		sAABB()
		{
			this->centreXYZ = glm::vec3(0.0f);
			this->minXYZ = glm::vec3(0.0f);
			this->maxXYZ = glm::vec3(0.0f);
			this->pPhysicInfo = new sPhysInfo();
		}
		~sAABB()
		{
			delete this->pPhysicInfo;
		}
		glm::vec3 centreXYZ;
		glm::vec3 minXYZ;
		glm::vec3 maxXYZ;

		// Calculate the other stuff
		// TODO:
		glm::vec3 getExtentsXYZ(void);	//	glm::vec3 extentsXYZ;	// height, width, depth

//		glm::vec3 getMinXYZ(void);
//		glm::vec3 getMaxYYZ(void);
//		glm::vec3 getHalfExtent(void);	// Distance from centre on that axis
		//
		sPhysInfo* pPhysicInfo = NULL;
	
		//		unsigned int uniqueID;
	};

	struct sLine
	{
		sLine()
		{
			this->startXYZ = glm::vec3(0.0f);
			this->endXYZ = glm::vec3(0.0f);
		}
		glm::vec3 startXYZ;
		glm::vec3 endXYZ;
		glm::vec3 direction;
		float timer;
		int isRocket;
		int isLaser;
		float getLength(void)
		{
			return glm::distance(this->endXYZ, this->startXYZ);
		}
	};

	struct sTriangle
	{
		glm::vec3 vertices[3];
		glm::vec3 normal;
		// Maybe other things?
		glm::vec3 intersectionPoint;
	};
	
	struct sTriangleMesh
	{
		std::string meshModelName;
		std::string meshInstanceName;	// Friendly name?
		std::vector<sTriangle> vecTriangles;
	};

	// Other types soon, likely
	// struct sPlane
	// struct sCapsule
	// etc.


	std::vector<sSphere*> vecSpheres;
	std::vector<sAABB*> vecAABBs;
	std::vector< sTriangleMesh*> vecMeshes;


	// Looks up the mesh information from the VAO manager
	void setVAOManager(cVAOManager* pTheVAOManager);
private:
	cVAOManager* m_pVAOManager = NULL;
public:
	// Look up the mesh from the VAO manager
	// (if it's loaded)
	bool addTriangleMesh(std::string meshModelName);
	// What if the mesh has been placed somewhere else in the world? 
	// TODO: 
	bool addTriangleMesh(
		std::string meshModelName,
		std::string meshFileName, 
		glm::vec3 meshWorldPosition,
		glm::vec3 meshWorldOrientation, 
		float uniformScale);
	//bool addTriangleMesh(std::string meshModelName, glm::mat4 modelMatrix);
	
	// Takes triangle information directly 
	//bool addTriangleMesh(std::string meshModelName, sTriangleMesh& vecMeshes);

//	std::vector<sPhysicObject*> vecAllTheObjects;
//	std::vector< sPhysInfo* > vec_pPhysInfos;

	// Called every time we want a collision detection step
	void StepTick(double deltaTime);


////	bool DoesRayCollide(glm::vec3 rayStart, glm::vec3 rayEnd);
////	bool DoesRayCollide(glm::vec3 rayStart, glm::vec3 rayEnd, vec_pMeshesIHit);
//	// Store the "vec_pMeshesIHit" like the collision events
//	void DoesRayCollide(glm::vec3 rayStart, glm::vec3 rayEnd);
//	// If there's a intersection, it will be added to 
//	// std::vector<sMeshCollionEven> vec_pMeshesRayHit;


	// Could do this... like a callback or event 
	// This is 100% OK to do this
//	void SphereAABBCollided(sSphere* pSphere, sAABB* pAABB);

	// Here's a list of all the event that happened in the last step
	// 
	// These represent a collision "event" 
	struct sCollision_SphereAABB
	{
		double timeOfCollision;

		sSphere* pTheSphere;
		sAABB* pTheAABB;
		glm::vec3 collisionWorldPoint;
		float nearestDistance;
		glm::vec3 sphereVelocity;	glm::vec3 spherePosition;	
		// velocity will also give direction
		// glm::vec3 sphereDirection;

		glm::vec3 AABB_Velocity;	glm::vec3 AABB_Position;	
		// velocity will also give direction
		// glm::vec3 sphereDirection;
	};
	std::vector<sCollision_SphereAABB> vec_SphereAABB_Collisions;

	struct sCollision_SphereSphere
	{
		double timeOfCollision;

		sSphere* pSpheres[2];
		glm::vec3 collisionWorldPoint;
		glm::vec3 velocities[2];	
		glm::vec3 position[2];	
		// velocity will also give direction
		// glm::vec3 sphereDirection;
	};
	std::vector<sCollision_SphereAABB> vec_SphereSphere_Collisions;

	struct sCollision_RayTriangleInMesh
	{
		double timeOfCollision;

		sLine theRay;
		std::string meshInstanceName;
		std::vector<sTriangle> vecTriangles;
		// Any other things you might want
	};
	std::vector<sCollision_RayTriangleInMesh> vec_RayTriangle_Collisions;

	struct sCollision_AABBMesh
	{
		double timeOfCollision;

		sAABB* pTheAABB;
		sTriangleMesh* pTheTriangleMesh;
		glm::vec3 collisionWorldPoint;
		std::string meshInstanceName;
		std::vector<sTriangle> vecTriangles;
		sTriangle theTriangle;
	};
	std::vector<sCollision_AABBMesh> vec_AABBMesh_Collisions;

	// Returns immediately with the triangles we hit
	bool rayCast(glm::vec3 start, glm::vec3 end,
		std::vector<sCollision_RayTriangleInMesh>& vec_RayTriangle_Collisions, 
		bool bIgnoreBackFacingTriangles = true);
	// This one adds the collision to the vec_RayTriangle_Collisions
	void rayCast(glm::vec3 start, glm::vec3 end, bool bIgnoreBackFacingTriangles = true);

	bool AABB_PlaneCollision(sAABB* pAABB, glm::vec3 n, float d);
	bool Triangle_AABBCollision(sTriangle* theTri, sAABB* pAABB);

	// Check to see if they collided. 
	// We likely need other information, like where, when, etc.
	bool bSphereAABBCollision(sSphere* pSphere, sAABB* pAABB);
	bool bSphereSphereCollision(sSphere* pA, sSphere* pB);
	bool bAABB_ABBBCollision(sAABB* pA, sAABB* pB);
	// This is an infinite ray intersection test
	// The start and end define the line, but it extends forever
	bool bRay_TriangleCollision(sLine theLine, sTriangle theTri);
	// This is a line SEGMENT traingle test
	bool bLineSegment_TriangleCollision(sLine theLine, sTriangle theTri);
	// returns the barycentric coordinates(u, v, w)
	bool bLineSegment_TriangleCollision(sLine theLine, sTriangle theTri,
	                                    float& u, float& v, float& w, float& t);


private:
	void m_CheckForCollisions(double deltaTime);

};
