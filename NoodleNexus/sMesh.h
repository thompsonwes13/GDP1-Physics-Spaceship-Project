#pragma once

// This is the 3D drawing information
//	to draw a single mesh (single PLY file)

#include <string>
#include <glm/glm.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

struct sMesh
{
	std::string modelFileName;			// "bunny.ply"
	std::string uniqueFriendlyName;		// "Fred", "Ali", "Bunny_007"

	glm::vec3 positionXYZ;
	glm::vec3 rotationEulerXYZ;		// 90 degrees around the x axis
	float uniformScale = 1.0f;				// Same for each axis

	glm::vec4 objectColourRGBA;
	// If true, it uses the colour above
	bool bOverrideObjectColour = true;

	bool bIsWireframe = false;
	bool bIsVisible = true;
	bool bDoNotLight = false;

	unsigned int uniqueID = 0;

	bool growing_shrinking;
	float count;

//	sMesh* pChildMeshes[100];
};