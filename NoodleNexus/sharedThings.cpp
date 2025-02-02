#include "sharedThings.h"

cBasicFlyCamera* g_pFlyCamera = NULL;

cLightManager* g_pLightManager = NULL;
bool g_bShowDebugSpheres = false;

unsigned int g_selectedLightIndex = 0;
unsigned int g_selectedMeshIndex = 0;

bool moveCamera = true;
glm::vec3 cubePos = glm::vec3(0.0f,0.0f,0.0f);

cPhysics* g_pPhysicEngine = NULL;

glm::vec3 g_rgb_from_HTML(unsigned int red, unsigned int green, unsigned int blue)
{
	return glm::vec3((float)red / 255.0f, (float)green / 255.0f, (float)blue / 255.0f);
}

// HACK: Show "targeting LASER"
bool g_bShowLASERBeam = true;