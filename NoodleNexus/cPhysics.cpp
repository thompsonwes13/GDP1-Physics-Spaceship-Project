#include "cPhysics.h"

#include <glm/glm.hpp>
#include <glm/vec3.hpp> // glm::vec3
#include <glm/vec4.hpp> // glm::vec4
#include <glm/mat4x4.hpp> // glm::mat4
#include <glm/gtc/matrix_transform.hpp> 

#include <iostream>
#include <algorithm>	// for the sort::

#include "sharedThings.h"


glm::vec3 cPhysics::sAABB::getExtentsXYZ(void)
{
	glm::vec3 extentXYZ;
	extentXYZ.x = this->maxXYZ.x - this->minXYZ.z;
	extentXYZ.y = this->maxXYZ.y - this->minXYZ.y;
	extentXYZ.z = this->maxXYZ.z - this->minXYZ.z;
	return extentXYZ;
}

void cPhysics::StepTick(double deltaTime)
{
	// Clear all the collision from the last frame
	this->vec_SphereAABB_Collisions.clear();
	this->vec_SphereSphere_Collisions.clear();
	this->vec_AABBMesh_Collisions.clear();

	// ***********************************************************************************
	//    ___       _                       _                   _             
	//   |_ _|_ __ | |_ ___  __ _ _ __ __ _(_) ___  _ __    ___| |_ ___ _ __  
	//    | || '_ \| __/ _ \/ _` | '__/ _` | |/ _ \| '_ \  / __| __/ _ \ '_ \ 
	//    | || | | | ||  __/ (_| | | | (_| | | (_) | | | | \__ \ ||  __/ |_) |
	//   |___|_| |_|\__\___|\__, |_|  \__,_|_|\___/|_| |_| |___/\__\___| .__/ 
	//                      |___/                                      |_|    
	// Copy the "physics info" part to another vector to do the integration step
	std::vector< sPhysInfo* > vec_Temp_pPhysInfos;

	// COPY the sphere info
	for (unsigned int sphereIndex = 0; sphereIndex != this->vecSpheres.size(); sphereIndex++)
	{
		sSphere* pCurrentSphere = this->vecSpheres[sphereIndex];
		vec_Temp_pPhysInfos.push_back(pCurrentSphere->pPhysicInfo);
	}
	// COPY the AABB info
	for (unsigned int AABBindex = 0; AABBindex != this->vecAABBs.size(); AABBindex++)
	{
		sAABB* pCurrentAABB = this->vecAABBs[AABBindex];
		vec_Temp_pPhysInfos.push_back(pCurrentAABB->pPhysicInfo);
	}



	// Now ALL the physical properties we are integrating are in one step
	for (unsigned int index = 0; index != vec_Temp_pPhysInfos.size(); index++)
	{
		sPhysInfo* pCurObject = vec_Temp_pPhysInfos[index];

		// Can this object move? 
		if (pCurObject->bDoesntMove)
		{
			// No, so skip integration step
			continue;
		}

		//Update the velocity based on the acceleration
		glm::vec3 deltaVelocity = pCurObject->acceleration * (float)deltaTime;
		pCurObject->velocity += deltaVelocity;

		// Update the position based on the velocity
		// i.e. how much this objects velocity is changing THIS step (this frame)
		glm::vec3 deltaPosition = pCurObject->velocity * (float)deltaTime;
		pCurObject->position += deltaPosition;
	}//for (unsigned int index

	// ***********************************************************************************


	// Update the mesh locations to match the new physics positions, etc.
	for (unsigned int index = 0; index != vec_Temp_pPhysInfos.size(); index++)
	{
		if (moveCamera)
		{
			//sPhysInfo* pCurObject = vec_Temp_pPhysInfos[index];
			// 
			//pCurObject->pAssociatedDrawingMeshInstance->positionXYZ = pCurObject->position;
		}

		// TODO: If there's other things that update, too 
		// (rotation, etc.
	}//for (unsigned int index

	// Test for collisions
	this->m_CheckForCollisions(deltaTime);

	return;
}

void cPhysics::m_CheckForCollisions(double deltaTime)
{
	// For each sphere, test all the other spheres?

	

	// For each AABB, test all mesh triangles

	for (unsigned int AABBIndex = 0; AABBIndex != this->vecAABBs.size(); AABBIndex++)
	{
		sAABB* pCurrentAABB = this->vecAABBs[AABBIndex];

		for (std::vector< sTriangleMesh*>::iterator itMesh = this->vecMeshes.begin();
			itMesh != this->vecMeshes.end(); itMesh++)
		{
			// Dereference the iterator to get the mesh
			sTriangleMesh* pCurTriMesh = *itMesh;

			// Go through each triangle
			for (std::vector<sTriangle>::iterator itTri = pCurTriMesh->vecTriangles.begin();
				itTri != pCurTriMesh->vecTriangles.end(); itTri++)
			{
				sTriangle CurTriangle = *itTri;

				if (this->Triangle_AABBCollision(&CurTriangle, pCurrentAABB))
				{
					std::string AABBName = pCurrentAABB->pPhysicInfo->pAssociatedDrawingMeshInstance->uniqueFriendlyName;
					std::string meshName = pCurTriMesh->meshModelName;

					/*moveCamera = false;

					::g_pPhysicEngine->vecAABBs[0]->pPhysicInfo->position = cubePos;
					pCurrentAABB->pPhysicInfo->position = cubePos;
					::g_pPhysicEngine->vecAABBs[0]->centreXYZ = cubePos;
					pCurrentAABB->centreXYZ = cubePos;
					::g_pPhysicEngine->vecAABBs[0]->pPhysicInfo->position.z -= 1.0;
					pCurrentAABB->pPhysicInfo->position.z -= 1.0;
					::g_pPhysicEngine->vecAABBs[0]->centreXYZ.z -= 1.0;
					pCurrentAABB->centreXYZ.z = -1.0;

					::g_pFlyCamera->setEyeLocation(cubePos.x - 8, cubePos.y + 1, cubePos.z);

					moveCamera = true;*/

					sCollision_AABBMesh theCollision;
					theCollision.meshInstanceName = pCurTriMesh->meshInstanceName;
					theCollision.pTheAABB = pCurrentAABB;
					theCollision.pTheTriangleMesh = pCurTriMesh;
					theCollision.collisionWorldPoint = pCurrentAABB->pPhysicInfo->position;
					theCollision.theTriangle = CurTriangle;

					this->vec_AABBMesh_Collisions.push_back(theCollision);
				}
			}

		}
	}

	// For each sphere, test all the AABBs
	for (unsigned int sphereIndex = 0; sphereIndex != this->vecSpheres.size(); sphereIndex++)
	{
		sSphere* pCurrentSphere = this->vecSpheres[sphereIndex];

		for (unsigned int AABBIndex = 0; AABBIndex != this->vecAABBs.size(); AABBIndex++)
		{
			sAABB* pCurrentAABB = this->vecAABBs[AABBIndex];

			if (this->bSphereAABBCollision(pCurrentSphere, pCurrentAABB))
			{
				// They collided! 
				// Oh no... what are we supposed to do now?? 

//				// HACK: change direction of sphere
//				// Check if the sphere is going "down" -ve in the y
//				if (pCurrentSphere->pPhysicInfo->velocity.y < 0.0f)
//				{
//					// Yes, it's heading down
//					// So reverse the direction of velocity
//					pCurrentSphere->pPhysicInfo->velocity.y = fabs(pCurrentSphere->pPhysicInfo->velocity.y);
//				}

				// HACK:
//				std::cout << "A sphere and AABB collided! Huzzah!" << std::endl;
//				this->SphereAABBCollided(pCurrentSphere, pCurrentAABB);

				sCollision_SphereAABB theCollision;
				theCollision.pTheSphere = pCurrentSphere;
				theCollision.pTheAABB = pCurrentAABB;
				// And so on...

				this->vec_SphereAABB_Collisions.push_back(theCollision);

			}//if (this->bSphereAABBCollision(
		}//for (unsigned int AABBIndex 
	}//for (unsigned int sphereIndex


	// 100x100 --> 10,000 checks every frame

	// Check every sphere with every other sphere
	for (unsigned int outerLoopSphereIndex = 0; outerLoopSphereIndex != this->vecSpheres.size(); outerLoopSphereIndex++)
	{
		sSphere* pOuterLoopSphere = this->vecSpheres[outerLoopSphereIndex];

		for (unsigned int innerLoopSphereIndex = 0; innerLoopSphereIndex != this->vecSpheres.size(); innerLoopSphereIndex++)
		{
			sSphere* pInnerLoopSphere = this->vecSpheres[innerLoopSphereIndex];

			// Are these the same sphere? 
			if (pOuterLoopSphere == pInnerLoopSphere)
			{
				// Yes, so DON'T test these ('cuase they are the same, so always colliding)
				continue;
			}

			if ( this->bSphereSphereCollision(pInnerLoopSphere, pOuterLoopSphere) )
			{
//				std::cout << "Two spheres have collided!" << std::endl;
			}

		}//for (unsigned int innerLoopSphereIndex

	}//for (unsigned int outerLoopSphereIndex


	return;
}


bool cPhysics::bSphereAABBCollision(sSphere* pSphere, sAABB* pAABB)
{
	glm::vec3 AABBextents = pAABB->getExtentsXYZ();


	// Check if it HASN'T connected
	// Y axis first
	if ((pSphere->pPhysicInfo->position.y - pSphere->radius) > (pAABB->pPhysicInfo->position.y + AABBextents.y))
	{
		// Isn't colliding
		return false;
	}
	if ((pSphere->pPhysicInfo->position.y + pSphere->radius) < (pAABB->pPhysicInfo->position.y - AABBextents.y))
	{
		// Isn't colliding
		return false;
	}

	// X axis
	if ((pSphere->pPhysicInfo->position.x + pSphere->radius) < (pAABB->pPhysicInfo->position.x - AABBextents.x))
	{
		// Isn't colliding
		return false;
	}
	if ((pSphere->pPhysicInfo->position.x - pSphere->radius) > (pAABB->pPhysicInfo->position.x + AABBextents.x))
	{
		// Isn't colliding
		return false;
	}

	// Z axis
	if ((pSphere->pPhysicInfo->position.z + pSphere->radius) < (pAABB->pPhysicInfo->position.z - AABBextents.z))
	{
		// Isn't colliding
		return false;
	}
	if ((pSphere->pPhysicInfo->position.z - pSphere->radius) > (pAABB->pPhysicInfo->position.z + AABBextents.z))
	{
		// Isn't colliding
		return false;
	}

	// Overlapping on ALL axes, so must be colliding

	return true;
}

bool cPhysics::bSphereSphereCollision(sSphere* pA, sSphere* pB)
{

	float totalRadius = pA->radius + pB->radius;

		// Like 2D pythagorean theorem, you can calculate the distance between two points
		//	by taking the square root of the sum of the differences of each axis
//			float deltaX = pInnerLoopSphere->pPhysicInfo->position.x - pOuterLoopSphere->pPhysicInfo->position.x;
//			float deltaY = pInnerLoopSphere->pPhysicInfo->position.y - pOuterLoopSphere->pPhysicInfo->position.y;
//			float deltaZ = pInnerLoopSphere->pPhysicInfo->position.z - pOuterLoopSphere->pPhysicInfo->position.z;
//
//			float distance = sqrt(deltaX * deltaX + deltaY * deltaY + deltaZ * deltaZ);
			
	// Or use glm
	float distance = glm::distance(pA->pPhysicInfo->position, pB->pPhysicInfo->position);

	if (distance <= totalRadius)
	{
//		std::cout << "Two spheres have collided!" << std::endl;
		return true;
	}

	return false;
}

bool cPhysics::bAABB_ABBBCollision(sAABB* pA, sAABB* pB)
{
	// TODO: 

	return false;
}

// Taken from the Christer Ericsson collision book
// ==== Section 5.2.3: ============================================================ //
//
// TestAABBPlane() function
//
bool cPhysics::AABB_PlaneCollision(sAABB* pAABB, glm::vec3 n, float d)
{
//	printf("AABB Plane check!\n");
// 
	// These two lines not necessary with a (center, extents) AABB representation
	//Point c = (b.max + b.min) * 0.5f; // Compute AABB center
	//Point e = b.max - c; // Compute positive extents
	glm::vec3 e = pAABB->maxXYZ - pAABB->pPhysicInfo->position;
	
	// Compute the projection interval radius of b onto L(t) = b.c + t * p.n
	float r = e.x * abs(n.x) + e.y * abs(n.y) + e.z * abs(n.z);
	// Compute distance of box center from plane
	float s = glm::dot(n, pAABB->pPhysicInfo->position) - d;
	// Intersection occurs when distance s falls within [-r,+r] interval
	return s <= r;
}



// Taken from the Christer Ericsson collision book
// ==== Section 5.2.9: ============================================================ //
//
// TestTriangleAABB() function
//
bool cPhysics::Triangle_AABBCollision(sTriangle* theTri, sAABB* pAABB)
{
	float p0, p1, p2, r;
	// Compute box center and extents (if not already given in that format)
	glm::vec3 c = pAABB->pPhysicInfo->position;//pAABB->centreXYZ;
	float e0 = pAABB->getExtentsXYZ().x;
	float e1 = pAABB->getExtentsXYZ().y;
	float e2 = pAABB->getExtentsXYZ().z;
	// Translate triangle as conceptually moving AABB to origin
	glm::vec3 v0 = theTri->vertices[0] - c;
	glm::vec3 v1 = theTri->vertices[1] - c;
	glm::vec3 v2 = theTri->vertices[2] - c;
	// Compute edge vectors for triangle
	glm::vec3 f0 = v1 - v0, f1 = v2 - v1, f2 = v0 - v2;
	// Test axes a00..a22 (category 3)
	// Test axis a00
	p0 = v0.z * v1.y - v0.y * v1.z;
	p2 = v2.z * (v1.y - v0.y) - v2.z * (v1.z - v0.z);
	r = e1 * abs(f0.z) + e2 * abs(f0.y);
	if (std::max(-(std::max(p0, p2)), std::min(p0, p2)) > r) return 0; // Axis is a separating axis
	// Repeat similar tests for remaining axes a01..a22
	
		// Test the three axes corresponding to the face normals of AABB b (category 1).
		// Exit if...
		// ... [-e0, e0] and [min(v0.x,v1.x,v2.x), max(v0.x,v1.x,v2.x)] do not overlap
		if (std::max(v0.x, std::max(v1.x, v2.x)) < -e0 || std::min(v0.x, std::min(v1.x, v2.x)) > e0) return 0;
	// ... [-e1, e1] and [min(v0.y,v1.y,v2.y), max(v0.y,v1.y,v2.y)] do not overlap
	if (std::max(v0.y, std::max(v1.y, v2.y)) < -e1 || std::min(v0.y, std::min(v1.y, v2.y)) > e1) return 0;
	// ... [-e2, e2] and [min(v0.z,v1.z,v2.z), max(v0.z,v1.z,v2.z)] do not overlap
	if (std::max(v0.z, std::max(v1.z, v2.z)) < -e2 || std::min(v0.z, std::min(v1.z, v2.z)) > e2) return 0;
	// Test separating axis corresponding to triangle face normal (category 2)
	glm::vec3 n;
	float d;
	n = glm::cross(f0, f1);
	d = glm::dot(n, v0);
	return AABB_PlaneCollision(pAABB, n, d);
}


void cPhysics::setVAOManager(cVAOManager* pTheVAOManager)
{
	this->m_pVAOManager = pTheVAOManager;
	return;
}

bool cPhysics::addTriangleMesh(std::string meshModelName)
{
	// TODO: 
	return false;
}

bool cPhysics::addTriangleMesh(
	std::string meshModelName,
	std::string meshFileName,
	glm::vec3 meshWorldPosition,
	glm::vec3 meshWorldOrientation,
	float uniformScale)
{
	if (!this->m_pVAOManager)
	{
		// Haven't set the VAO manager pointer, yet
		return false;
	}

	std::vector<cVAOManager::sTriangle> vecVAOTriangles;
	if (!this->m_pVAOManager->getTriangleMeshInfo(meshFileName, vecVAOTriangles))
	{
		// Can't find mesh
		return false;
	}

	// At this point, the vecTriangles has all the triangle info.
	
	sTriangleMesh* pMesh = new sTriangleMesh();

	// Copy that vector into the local mesh format 
	for (std::vector<cVAOManager::sTriangle>::iterator itVAOTri = vecVAOTriangles.begin();
		itVAOTri != vecVAOTriangles.end(); itVAOTri++)
	{
		cVAOManager::sTriangle curVAOTri = *itVAOTri;

		sTriangle curTri;
		curTri.vertices[0] = curVAOTri.vertices[0];
		curTri.vertices[1] = curVAOTri.vertices[1];
		curTri.vertices[2] = curVAOTri.vertices[2];
		curTri.normal = curVAOTri.normal;

		pMesh->vecTriangles.push_back(curTri);

	}//for (std::vector<cVAOManager::sTriangle>::iterator itVAOTri
	 
	// Transform the triangles to where they are in the world
	for (std::vector<sTriangle>::iterator itTri = pMesh->vecTriangles.begin();
		itTri != pMesh->vecTriangles.end(); itTri++)
	{
		// Take the same matrix transformation technique 
		//	from the render call and apply to each vertex

		glm::mat4 matModel = glm::mat4(1.0f);

		// Translation (movement, position, placement...)
		glm::mat4 matTranslate
			= glm::translate(glm::mat4(1.0f),
				meshWorldPosition);

		// Rotation...
		// Caculate 3 Euler acix matrices...
		glm::mat4 matRotateX =
			glm::rotate(glm::mat4(1.0f),
				glm::radians(meshWorldOrientation.x), // Angle in radians
				glm::vec3(1.0f, 0.0, 0.0f));

		glm::mat4 matRotateY =
			glm::rotate(glm::mat4(1.0f),
				glm::radians(meshWorldOrientation.y), // Angle in radians
				glm::vec3(0.0f, 1.0, 0.0f));

		glm::mat4 matRotateZ =
			glm::rotate(glm::mat4(1.0f),
				glm::radians(meshWorldOrientation.z), // Angle in radians
				glm::vec3(0.0f, 0.0, 1.0f));


		// Scale
		glm::mat4 matScale = glm::scale(glm::mat4(1.0f),
			glm::vec3(uniformScale, uniformScale, uniformScale));


		// Calculate the final model/world matrix
		matModel *= matTranslate;     // matModel = matModel * matTranslate;
		matModel *= matRotateX;
		matModel *= matRotateY;
		matModel *= matRotateZ;
		matModel *= matScale;


		// Like from the vertex shader: 	
		// fvertexWorldLocation = matModel * vec4(finalVert, 1.0);

		sTriangle curTriangle = *itTri;
		// Transform the vertex to where they are in the world...
		glm::vec4 vert0World = matModel * glm::vec4(curTriangle.vertices[0], 1.0f);
		glm::vec4 vert1World = matModel * glm::vec4(curTriangle.vertices[1], 1.0f);
		glm::vec4 vert2World = matModel * glm::vec4(curTriangle.vertices[2], 1.0f);

		// Copy the transformed vertices bacl
		itTri->vertices[0] = vert0World;
		itTri->vertices[1] = vert1World;
		itTri->vertices[2] = vert2World;

		//itTri->vertices[0] = curTriangle.vertices[0];
		//itTri->vertices[1] = curTriangle.vertices[1];
		//itTri->vertices[2] = curTriangle.vertices[2];

		// Also rotate the normal
		glm::mat4 matModelInverseTranspose = glm::inverse(glm::transpose(matModel));

		itTri->normal 
			= glm::vec3( matModelInverseTranspose * glm::vec4(itTri->normal, 1.0f) );

	}//for (std::vector<sTriangle>::iterator itTri

	pMesh->meshInstanceName = meshFileName;
	pMesh->meshModelName = meshModelName;
//	pMesh->meshModelName = ??
	this->vecMeshes.push_back(pMesh);


	return true;
}

//struct sTriangle
//{
//	glm::vec3 vertices[3];
//	glm::vec3 normal;
//	// Maybe other things?
//	glm::vec3 intersectionPoint;
//};
// Calculate triangle to point distance
float distancePointToIntersectionTriangle(
	glm::vec3 thePoint, cPhysics::sTriangle theTriangle)
{
	return glm::distance(thePoint, theTriangle.intersectionPoint);
}

// Predicate function (binary)
// Sort takes two WHATEVERS and compares 
//	to see which is "less"
// (for us, "less" is "closer to"
glm::vec3 thePoint;
bool isTriACloserThanB(cPhysics::sTriangle triA, cPhysics::sTriangle triB)
{
	float distToA = distancePointToIntersectionTriangle(thePoint, triA);
	float distToB = distancePointToIntersectionTriangle(thePoint, triB);
	if (distToA < distToB)
	{
		return true;
	}
	// 
	return false;
}

class cSortCloseToPoint
{
public:
	cSortCloseToPoint(glm::vec3 thePoint)
	{
		this->m_thePoint = thePoint;
	}
private:
	glm::vec3 m_thePoint;
public:
	// Predicate comparison method (the "less than") method
	// on predicate function
	bool operator()(cPhysics::sTriangle triA, cPhysics::sTriangle triB)
	{
		float distToA = distancePointToIntersectionTriangle(thePoint, triA);
		float distToB = distancePointToIntersectionTriangle(thePoint, triB);
		if (distToA < distToB)
		{
			return true;
		}
		// 
		return false;
	}
};



//void Hey(void)
//{
//	cPhysics::sTriangle triA;
//	cPhysics::sTriangle triB;
//	if ( triA < triB )
//}

bool cPhysics::rayCast(glm::vec3 start, glm::vec3 end,
	std::vector<sCollision_RayTriangleInMesh>& vec_RayTriangle_Collisions, 
	bool bIgnoreBackFacingTriangles /*= true*/)
{
	sLine theRay;
	theRay.startXYZ = start;
	theRay.endXYZ = end;

	float closestDistance = 1000;
	std::vector< sTriangle > allTriangles;

	// Clear the vector being passed in...
	vec_RayTriangle_Collisions.clear();

	for (std::vector< sTriangleMesh*>::iterator itMesh = this->vecMeshes.begin();
		itMesh != this->vecMeshes.end(); itMesh++)
	{
		// Dereference the iterator to get the mesh
		sTriangleMesh* pCurTriMesh = *itMesh;

		for (std::vector<sTriangle>::iterator itTri = pCurTriMesh->vecTriangles.begin();
			itTri != pCurTriMesh->vecTriangles.end(); itTri++)
		{
			sTriangle CurTriangle = *itTri;
			allTriangles.push_back(CurTriangle);
		}

	}

	sCollision_RayTriangleInMesh intersectionInfo;
	intersectionInfo.theRay = theRay;

	for (std::vector<sTriangle>::iterator itTri = allTriangles.begin();
		itTri != allTriangles.end(); itTri++)
	{
		sTriangle CurTriangle = *itTri;


		// Sorry, but the method takes pointers to the 
		//	line(ray) and triangle.
		// I did this because the other methods did this
		float u, v, w, t;
		if (this->bLineSegment_TriangleCollision(theRay, CurTriangle, u, v, w, t))
		{
			// They intersect, so add this triangle to the "intersected triangles" of this mesh		
			CurTriangle.intersectionPoint.x =
				CurTriangle.vertices[0].x * u
				+ CurTriangle.vertices[1].x * v
				+ CurTriangle.vertices[2].x * w;
			CurTriangle.intersectionPoint.y =
				CurTriangle.vertices[0].y * u
				+ CurTriangle.vertices[1].y * v
				+ CurTriangle.vertices[2].y * w;
			CurTriangle.intersectionPoint.z =
				CurTriangle.vertices[0].z * u
				+ CurTriangle.vertices[1].z * v
				+ CurTriangle.vertices[2].z * w;

			intersectionInfo.vecTriangles.push_back(CurTriangle);
		}
	}

	thePoint = theRay.startXYZ;
	std::sort(intersectionInfo.vecTriangles.begin(),
		intersectionInfo.vecTriangles.end(),
		cSortCloseToPoint(theRay.startXYZ));

	if (!intersectionInfo.vecTriangles.empty())
	{
		vec_RayTriangle_Collisions.push_back(intersectionInfo);
	}

//	// Got through all the triangle mesh objects and test 
//	//	the ray triangle for each triangle in each mesh
//	for (std::vector< sTriangleMesh*>::iterator itMesh = this->vecMeshes.begin();
//		itMesh != this->vecMeshes.end(); itMesh++)
//	{
//		// Dereference the iterator to get the mesh
//		sTriangleMesh* pCurTriMesh = *itMesh;
//
//		sCollision_RayTriangleInMesh intersectionInfo;
//		intersectionInfo.theRay = theRay;
//		intersectionInfo.meshInstanceName = pCurTriMesh->meshInstanceName;
//
//		// Go through each triangle
//		for (std::vector<sTriangle>::iterator itTri = pCurTriMesh->vecTriangles.begin();
//			itTri != pCurTriMesh->vecTriangles.end(); itTri++)
//		{
//
//			sTriangle CurTriangle = *itTri;
//			// Sorry, but the method takes pointers to the 
//			//	line(ray) and triangle.
//			// I did this because the other methods did this
//			float u, v, w, t;
//			if (this->bLineSegment_TriangleCollision(theRay, CurTriangle, u, v, w, t))
//			{
//				// They intersect, so add this triangle to the "intersected triangles" of this mesh		
//				CurTriangle.intersectionPoint.x =
//					  CurTriangle.vertices[0].x * u 
//					+ CurTriangle.vertices[1].x * v 
//					+ CurTriangle.vertices[2].x * w;
//				CurTriangle.intersectionPoint.y =
//					  CurTriangle.vertices[0].y * u 
//					+ CurTriangle.vertices[1].y * v 
//					+ CurTriangle.vertices[2].y * w;
//				CurTriangle.intersectionPoint.z =
//					  CurTriangle.vertices[0].z * u 
//					+ CurTriangle.vertices[1].z * v 
//					+ CurTriangle.vertices[2].z * w;
//
////				// Ignoring backfacing triangle? 
////				if (bIgnoreBackFacingTriangles)
////				{
////					// See if this traingle is facing "away" from the ray
////					glm::vec3 rayDirection = (start - end);
////					rayDirection = glm::normalize(rayDirection);
////					float dotProd = glm::dot(rayDirection, CurTriangle.normal);
////					//
////					if (dotProd > 0.0f)
////					{
////						// Facing towards, so add
////						intersectionInfo.vecTriangles.push_back(CurTriangle);
////					}
////				}
////				else
////				{
//					// Add all triangles, even back facing
//				
//					
////				}
//
//				//if (glm::distance(theRay.startXYZ, CurTriangle.intersectionPoint) < closestDistance)
//				//{
//				//	std::cout << "Closest Point is: " << CurTriangle.intersectionPoint.x << " "
//				//			  << CurTriangle.intersectionPoint.y << " "
//				//			  << CurTriangle.intersectionPoint.z << std::endl;
//
//				//		closestDistance = glm::distance(theRay.startXYZ, CurTriangle.intersectionPoint);
//						intersectionInfo.vecTriangles.push_back(CurTriangle);
//				//}
//			}//if (this->bLineSegment_TriangleCollision...
//
//		}//for (std::vector<sTriangle>::iterator itTri
//
//		// Sort the traingles from nearest to farthest
//		// Compare the intersection point with the "glm::vec3 start" ray point
//		thePoint = theRay.startXYZ;
//		std::sort(intersectionInfo.vecTriangles.begin(),
//				  intersectionInfo.vecTriangles.end(), 
//			      cSortCloseToPoint(theRay.startXYZ) );
//		//std::sort(intersectionInfo.vecTriangles.begin(),
//		//		  intersectionInfo.vecTriangles.end(), 
//		//	      isTriACloserThanB);
//
//
//		// Add this mesh-triangle(s) to the vector of collisions
//		// (one entry per mesh)
//		if ( ! intersectionInfo.vecTriangles.empty() )
//		{
//			vec_RayTriangle_Collisions.push_back(intersectionInfo);
//		}
//
//	}//for (std::vector< sTriangleMesh*>::iterator itMesh



	// Any collisions? 
	if (vec_RayTriangle_Collisions.empty())
	{
		// No
		return false;
	}

	// There were collisions
	return true;
}

// This one adds the collision to the vec_RayTriangle_Collisions
void cPhysics::rayCast(glm::vec3 start, glm::vec3 end, bool bIgnoreBackFacingTriangles /*= true*/)
{
	// TODO: 

	return;
}

// Not sure what the AI was on about, but here's a much 
//	simpler version of the drek it spit out:
float ScalarTriple(glm::vec3 a, glm::vec3 b, glm::vec3 c) 
{
	return glm::dot (a, glm::cross(b, c));
}

// Reworked from the Christer Ericsson collision book
// == = Section 5.3.4: ============================================================
// 
// IntersectLineTriangle() function
// 
// Given line pq and ccw triangle abc, return whether line pierces triangle. If
// so, also return the barycentric coordinates (u,v,w) of the intersection point
//int IntersectLineTriangle(Point p, Point q, Point a, Point b, Point c,
//	float& u, float& v, float& w)
// Assume p = line start, q = line end
bool cPhysics::bRay_TriangleCollision(sLine theLine, sTriangle theTri)
{
	glm::vec3 pq = theLine.endXYZ - theLine.startXYZ;		//	Vector pq = q - p;
	glm::vec3 pa = theTri.vertices[0] - theLine.startXYZ;	//	Vector pa = a - p;
	glm::vec3 pb = theTri.vertices[1] - theLine.startXYZ;	//	Vector pb = b - p;
	glm::vec3 pc = theTri.vertices[2] - theLine.startXYZ;	//	Vector pc = c - p;
	//	Test if pq is inside the edges bc, ca and ab. Done by testing
	//	that the signed tetrahedral volumes, computed using scalar triple
	//	products, are all positive
	float u = ScalarTriple(pq, pc, pb);		//	u = ScalarTriple(pq, pc, pb);
	if (u < 0.0f)	{
		return false;
	}
	float v = ScalarTriple(pq, pa, pc);		//	v = ScalarTriple(pq, pa, pc);
	if (v < 0.0f)	{
		return false;
	}
	float w = ScalarTriple(pq, pb, pa);		//	w = ScalarTriple(pq, pb, pa);
	if (w < 0.0f) {
		return false;
	}

	//	Compute the barycentric coordinates (u, v, w) determining the
	//	intersection point r, r = u*a + v*b + w*c
	float denom = 1.0f / (u + v + w);
	u *= denom;
	v *= denom;
	w *= denom; // w = 1.0f - u - v;
	
	return true;
}


// Reworked from the Christer Ericsson collision book
// == = Section 5.3.6: ============================================================ =
// Given segment pq and triangle abc, returns whether segment intersects
// triangle and if so, also returns the barycentric coordinates (u,v,w)
// of the intersection point
//	int IntersectSegmentTriangle(Point p, Point q, Point a, Point b, Point c,
//		float& u, float& v, float& w, float& t)
bool cPhysics::bLineSegment_TriangleCollision(sLine theLine, sTriangle theTri)
{
	glm::vec3 ab = theTri.vertices[1] - theTri.vertices[0];		//	Vector ab = b - a;
	glm::vec3 ac = theTri.vertices[2] - theTri.vertices[0];		//	Vector ac = c - a;
	glm::vec3 qp = theLine.startXYZ - theLine.endXYZ;			//	Vector qp = p - q;
	//
	//	Compute triangle normal. Can be precalculated or cached if
	//	intersecting multiple segments against the same triangle
	glm::vec3 n = glm::cross(ab, ac);	//	Vector n = Cross(ab, ac);
	//
	//	Compute denominator d. If d <= 0, segment is parallel to or points
	//	away from triangle, so exit early
	float d = glm::dot(qp, n);		//	float d = Dot(qp, n);

	if (d <= 0.0f) {
		return false;
	}
	//
	//	Compute intersection t value of pq with plane of triangle. A ray
	//	intersects iff 0 <= t. Segment intersects iff 0 <= t <= 1. Delay
	//	dividing by d until intersection has been found to pierce triangle
	glm::vec3 ap = theLine.startXYZ - theTri.vertices[0];		//	Vector ap = p - a;
	float t = glm::dot(ap, n);									//	t = Dot(ap, n);

	if (t < 0.0f) {
		return false;
	}

	// For segment; exclude this code line for a ray test
	if (t > d) {
		return 0;
	}

	//	Compute barycentric coordinate components and test if within bounds
	glm::vec3 e = glm::cross(qp, ap);	//	Vector e = Cross(qp, ap);
	float v = glm::dot(ac, e);		//	v = Dot(ac, e);

	if (v < 0.0f || v > d) {
		return false;
	}

	float w = -glm::dot(ab, e);		//	w = -Dot(ab, e);

	if ((w < 0.0f) || (v + w > d)) {
		return 0;
	}

	//	Segment/ray intersects triangle. Perform delayed division and
	//	compute the last barycentric coordinate component
	// Discard this:
	float u = 0.0f;
	return this->bLineSegment_TriangleCollision(theLine, theTri, u, v, w, t);
}

// Reworked from the Christer Ericsson collision book
// == = Section 5.3.6: ============================================================ =
// Given segment pq and triangle abc, returns whether segment intersects
// triangle and if so, also returns the barycentric coordinates (u,v,w)
// of the intersection point
//	int IntersectSegmentTriangle(Point p, Point q, Point a, Point b, Point c,
//		float& u, float& v, float& w, float& t)
bool cPhysics::bLineSegment_TriangleCollision(
	sLine theLine, sTriangle theTri,
    float& u, float& v, float& w, float& t)
{
	glm::vec3 ab = theTri.vertices[1] - theTri.vertices[0];		//	Vector ab = b - a;
	glm::vec3 ac = theTri.vertices[2] - theTri.vertices[0];		//	Vector ac = c - a;
	glm::vec3 qp = theLine.startXYZ - theLine.endXYZ;			//	Vector qp = p - q;
	//
	//	Compute triangle normal. Can be precalculated or cached if
	//	intersecting multiple segments against the same triangle
	glm::vec3 n = glm::cross(ab, ac);	//	Vector n = Cross(ab, ac);
	//
	//	Compute denominator d. If d <= 0, segment is parallel to or points
	//	away from triangle, so exit early
	float d = glm::dot(qp, n);		//	float d = Dot(qp, n);
	
	if (d <= 0.0f) {
		return false;
	}
	//
	//	Compute intersection t value of pq with plane of triangle. A ray
	//	intersects iff 0 <= t. Segment intersects iff 0 <= t <= 1. Delay
	//	dividing by d until intersection has been found to pierce triangle
	glm::vec3 ap = theLine.startXYZ - theTri.vertices[0];		//	Vector ap = p - a;
	t = glm::dot(ap, n);									//	t = Dot(ap, n);
	
	if (t < 0.0f) {
		return false;
	}

	// For segment; exclude this code line for a ray test
	if (t > d) {
		return 0;
	}

	//	Compute barycentric coordinate components and test if within bounds
	glm::vec3 e = glm::cross(qp, ap);	//	Vector e = Cross(qp, ap);
	v = glm::dot(ac, e);		//	v = Dot(ac, e);

	if (v < 0.0f || v > d) {
		return false;
	}

	w = -glm::dot(ab, e);		//	w = -Dot(ab, e);

	if ( (w < 0.0f) || (v + w > d) ) {
		return 0;
	}
	
	//	Segment/ray intersects triangle. Perform delayed division and
	//	compute the last barycentric coordinate component
	float ood = 1.0f / d;
	t *= ood;
	v *= ood;
	w *= ood;
	u = 1.0f - v - w;
	
	return true;
}
















	// Feeney asked chatGPT this:
	// 
	// "give me the triple product of three vectors in C++"
	// 
	// And got this:
//#include <iostream>
//#include <vector>
//	using namespace std;
//
//	// Function to compute the cross product of two 3D vectors
//	vector<int> crossProduct(vector<int> a, vector<int> b) {
//		vector<int> result(3);
//		result[0] = a[1] * b[2] - a[2] * b[1];
//		result[1] = a[2] * b[0] - a[0] * b[2];
//		result[2] = a[0] * b[1] - a[1] * b[0];
//		return result;
//	}
//
//	// Function to compute the dot product of two 3D vectors
//	int dotProduct(vector<int> a, vector<int> b) {
//		return a[0] * b[0] + a[1] * b[1] + a[2] * b[2];
//	}
//
//	// Function to compute the scalar triple product
//	int scalarTripleProduct(vector<int> a, vector<int> b, vector<int> c) {
//		vector<int> bCrossC = crossProduct(b, c);
//		return dotProduct(a, bCrossC);
//	}
//
//	int main() {
//		vector<int> vecA(3), vecB(3), vecC(3);
//
//		// Input the three 3D vectors
//		cout << "Enter the components of the first vector (x y z): ";
//		cin >> vecA[0] >> vecA[1] >> vecA[2];
//
//		cout << "Enter the components of the second vector (x y z): ";
//		cin >> vecB[0] >> vecB[1] >> vecB[2];
//
//		cout << "Enter the components of the third vector (x y z): ";
//		cin >> vecC[0] >> vecC[1] >> vecC[2];
//
//		// Calculate the scalar triple product
//		int result = scalarTripleProduct(vecA, vecB, vecC);
//
//		// Display the result
//		cout << "The scalar triple product is: " << result << endl;
//
//		return 0;
//	}