//#define GLAD_GL_IMPLEMENTATION
//#include <glad/glad.h>
//
//#define GLFW_INCLUDE_NONE
//#include <GLFW/glfw3.h>
#include "GLCommon.h"

//#include "linmath.h"
#include <glm/glm.hpp>
#include <glm/vec3.hpp> // glm::vec3
#include <glm/vec4.hpp> // glm::vec4
#include <glm/mat4x4.hpp> // glm::mat4
#include <glm/gtc/matrix_transform.hpp> 
// glm::translate, glm::rotate, glm::scale, glm::perspective
#include <glm/gtc/type_ptr.hpp> // glm::value_ptr

#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>

#include <iostream>     // "input output" stream
#include <fstream>      // "file" stream
#include <sstream>      // "string" stream ("string builder" in Java c#, etc.)
#include <string>
#include <vector>

//void ReadPlyModelFromFile(std::string plyFileName);
#include "PlyFileLoaders.h"
#include "Basic_Shader_Manager/cShaderManager.h"
#include "sMesh.h"
#include "cVAOManager/cVAOManager.h"
#include "sharedThings.h"       // Fly camera
#include "cPhysics.h"
#include "cLightManager.h"
#include <windows.h>    // Includes ALL of windows... MessageBox
#include "cLightHelper/cLightHelper.h"

std::vector<sMesh*> g_vecMeshesToDraw;
std::vector<cPhysics::sLine*> g_vecLasers;

int g_floorCount = 1;
int g_wallCount = 1;
int g_ceilingCount = 1;

int g_propCount = 1;
//glm::vec3 cubePos;

//cPhysics* g_pPhysicEngine = NULL;
// This loads the 3D models for drawing, etc.
cVAOManager* g_pMeshManager = NULL;

void AddModelsToScene();
void DrawDungeon();

void DrawMesh(sMesh* pCurMesh, GLuint program);

void ShootLaser();
void DrawExplosion(glm::vec3 location);
void DrawRocketExplosion(glm::vec3 location);
void DrawExhaust(glm::vec3 location);

void ShootRocket();

static void error_callback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}

// Returns NULL if NOT found
sMesh* pFindMeshByFriendlyName(std::string theNameToFind)
{
    for (unsigned int index = 0; index != ::g_vecMeshesToDraw.size(); index++)
    {
        if (::g_vecMeshesToDraw[index]->uniqueFriendlyName == theNameToFind)
        {
            return ::g_vecMeshesToDraw[index];
        }
    }
    // Didn't find it
    return NULL;
}

void SaveMeshes()
{
    // Save state to file

    if (MessageBox(NULL, L"Would you like to save the mesh locations?", L"SAVE", MB_YESNO) == IDYES)
    {
        std::ofstream meshInfo("meshInfo.txt");

        for (int i = 0; i < g_vecMeshesToDraw.size(); i++)
        {
            if (::g_vecMeshesToDraw[i] == pFindMeshByFriendlyName("Light_Sphere"))
            {
            }
            else if (::g_vecMeshesToDraw[i] == pFindMeshByFriendlyName("Debug_Sphere"))
            {
            }
            else
            {
                meshInfo << ::g_vecMeshesToDraw[i]->uniqueFriendlyName << "\n";

                meshInfo << "Position - ";
                meshInfo << "x: " << ::g_vecMeshesToDraw[i]->positionXYZ.x << " ";
                meshInfo << "y: " << ::g_vecMeshesToDraw[i]->positionXYZ.y << " ";
                meshInfo << "z: " << ::g_vecMeshesToDraw[i]->positionXYZ.z << "\n";

                meshInfo << "Rotation - ";
                meshInfo << "x: " << ::g_vecMeshesToDraw[i]->rotationEulerXYZ.x << " ";
                meshInfo << "y: " << ::g_vecMeshesToDraw[i]->rotationEulerXYZ.y << " ";
                meshInfo << "z: " << ::g_vecMeshesToDraw[i]->rotationEulerXYZ.z << "\n";

                meshInfo << "Scale - ";
                meshInfo << ::g_vecMeshesToDraw[i]->uniformScale << "\n\n";
            }
        }

        meshInfo.close();

        std::cout << "Saved Meshes!" << std::endl;
    }
    else
    {
        std::cout << "Cancelled Save" << std::endl;
    }

    return;
}

void LoadMeshes()
{
    // Load state to file
            // https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-messagebox

    if (MessageBox(NULL, L"Would you like to load the mesh locations?", L"LOAD", MB_YESNO) == IDYES)
    {
        std::ifstream meshInfo("meshInfo.txt");

        for (int i = 0; i < g_vecMeshesToDraw.size(); i++)
        {
            float xPos;
            float yPos;
            float zPos;

            float xRot;
            float yRot;
            float zRot;

            float scale;

            std::string discard;

            if (::g_vecMeshesToDraw[i] == pFindMeshByFriendlyName("Light_Sphere"))
            {
            }
            else if (::g_vecMeshesToDraw[i] == pFindMeshByFriendlyName("Debug_Sphere"))
            {
            }
            else
            {
                meshInfo >> discard    // Mesh Name
                    >> discard    // "Position"
                    >> discard    // "-"
                    >> discard    // "x:"
                    >> xPos       // x val
                    >> discard    // "y:"
                    >> yPos       // y val
                    >> discard    // "z:"
                    >> zPos       // z val

                    >> discard    // "Rotation"
                    >> discard    // "-"
                    >> discard    // "x:"
                    >> xRot       // x val
                    >> discard    // "y:"
                    >> yRot       // y val
                    >> discard    // "z:"
                    >> zRot       // z val

                    >> discard    // "Scale"
                    >> discard    // "-"
                    >> scale;     // scale

                ::g_vecMeshesToDraw[i]->positionXYZ.x = xPos;
                ::g_vecMeshesToDraw[i]->positionXYZ.y = yPos;
                ::g_vecMeshesToDraw[i]->positionXYZ.z = zPos;

                ::g_vecMeshesToDraw[i]->rotationEulerXYZ.x = xRot;
                ::g_vecMeshesToDraw[i]->rotationEulerXYZ.y = yRot;
                ::g_vecMeshesToDraw[i]->rotationEulerXYZ.z = zRot;

                ::g_vecMeshesToDraw[i]->uniformScale = scale;
            }
        }

        std::cout << "Loaded Meshes!" << std::endl;
    }
    else
    {
        std::cout << "Cancelled Load" << std::endl;
    }

    return;
}

void SaveLights()
{
    if (MessageBox(NULL, L"Would you like to save the light locations?", L"SAVE", MB_YESNO) == IDYES)
    {
        std::ofstream lightInfo("lightInfo.txt");

        for (int i = 0; i < cLightManager::NUMBEROFLIGHTS; i++)
        {
            lightInfo << "Light " << i + 1 << "\n";
            lightInfo << "x: " << ::g_pLightManager->theLights[i].position.x << " ";
            lightInfo << "y: " << ::g_pLightManager->theLights[i].position.y << " ";
            lightInfo << "z: " << ::g_pLightManager->theLights[i].position.z << "\n";
            lightInfo << "Linear: " << ::g_pLightManager->theLights[i].atten.y << " ";
            lightInfo << "Quadratic: " << ::g_pLightManager->theLights[i].atten.z << "\n\n";
        }

        lightInfo.close();

        std::cout << "Saved Lights!" << std::endl;
    }
    else
    {
        std::cout << "Cancelled Save" << std::endl;
    }

    return;
}

void LoadLights()
{
    if (MessageBox(NULL, L"Would you like to load the light locations?", L"LOAD", MB_YESNO) == IDYES)
    {
        std::ifstream lightInfo("lightInfo.txt");

        for (int i = 0; i < cLightManager::NUMBEROFLIGHTS; i++)
        {
            float x;
            float y;
            float z;
            float linear;
            float quadratic;
            std::string discard;

            lightInfo >> discard    // "Light"
                >> discard    // Light #
                >> discard    // "x:"
                >> x          // x val
                >> discard    // "y:"
                >> y          // y val
                >> discard    // "z:"
                >> z          // z val
                >> discard    // "Linear:"
                >> linear     // Linear value
                >> discard    // "Quardratic
                >> quadratic;  // Quadratic value

            ::g_pLightManager->theLights[i].position.x = x;
            ::g_pLightManager->theLights[i].position.y = y;
            ::g_pLightManager->theLights[i].position.z = z;
            ::g_pLightManager->theLights[i].atten.y = linear;
            ::g_pLightManager->theLights[i].atten.z = quadratic;
        }

        std::cout << "Loaded Lights!" << std::endl;
    }
    else
    {
        std::cout << "Cancelled Load" << std::endl;
    }

    return;
}

void SetCameraPosition()
{
    std::ifstream cameraInfo("cameraInfo.txt");

    float x;
    float y;
    float z;
    std::string discard;

    cameraInfo >> discard // "Camera"
        >> discard        // "x:"
        >> x              // x value
        >> discard        // "y:"
        >> y              // y value
        >> discard        // "z:"
        >> z;              // z value

    ::g_pFlyCamera->setEyeLocation(x, y, z);

    std::cout << "Camera Position Loaded!" << std::endl;

    return;
}

void ModelsToVAO(const GLuint program)
{
    std::ifstream loadMeshes("loadMeshes.txt");

    //std::string discard;
    //std::string name;
    std::string path;
    //std::string isWireFrame;

    while (!loadMeshes.eof())
    {
        loadMeshes >> path;           // Path value

        sModelDrawInfo mesh;
        ::g_pMeshManager->LoadModelIntoVAO(path, mesh, program);
        std::cout << mesh.numberOfVertices << " vertices loaded" << std::endl;
    }
}

void DrawMeshes()
{
    std::ifstream drawMeshes("drawMeshes.txt");

    std::string discard;
    std::string name;
    std::string path;
    std::string isWireFrame;

    bool wireframe;

    float r;
    float g;
    float b;

    while (!drawMeshes.eof())
    {
        drawMeshes >> discard // "Name:"
            >> name           // Name value
            >> discard        // "Path:"
            >> path           // Path value
            >> discard        // "isWireframe"
            >> isWireFrame    // isWireFrame value
            >> discard        // "Colour:"
            >> r              // r value
            >> g              // g value
            >> b;             // b value

        if (drawMeshes.eof())
        {
            break;
        }

        if (isWireFrame == "true")
        {
            wireframe = true;
        }
        else
        {
            wireframe = false;
        }

        sMesh* pMesh = new sMesh();
        pMesh->modelFileName = path;
        pMesh->positionXYZ = glm::vec3(0.0f, 0.0f, 0.0f);
        pMesh->bIsWireframe = wireframe;
        pMesh->uniformScale = 0.1f;
        pMesh->objectColourRGBA = glm::vec4((r / 255.0f), (g / 255.0f), (b / 255.0f), 1.0f);
        pMesh->uniqueFriendlyName = name;

        ::g_vecMeshesToDraw.push_back(pMesh);
    }
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);

    const float CAMERA_MOVE_SPEED = 0.1f;

    /*
    if (mods == GLFW_MOD_SHIFT)
    {
        if (key == GLFW_KEY_1 && action == GLFW_PRESS)
        {
            // Save state to file

            SaveMeshes();
        }
        if (key == GLFW_KEY_2 && action == GLFW_PRESS)
        {
            // Load state from file

            LoadMeshes();
        }
    }

    // Save the light locations, rotations, etc.
    if (mods == GLFW_MOD_CONTROL)
    {
        if (key == GLFW_KEY_1 && action == GLFW_PRESS)
        {
            // Save state to file
//            MyAmazingStateThing->saveToFile("MySaveFile.sexy");

            SaveLights();

        }
        if (key == GLFW_KEY_2 && action == GLFW_PRESS)
        {
            // Load state to file
            // https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-messagebox

            LoadLights();

        }
    }
    */
    if (key == GLFW_KEY_3 && action == GLFW_PRESS)
    {
        // check if you are out of bounds
        if (::g_selectedLightIndex > 0)
        {

            ::g_selectedLightIndex--;
        }
    }
    if (key == GLFW_KEY_4 && action == GLFW_PRESS)
    {
        ::g_selectedLightIndex++;
        if (::g_selectedLightIndex >= 10)
        {
            ::g_selectedLightIndex = 9;
        }
    }

    if (key == GLFW_KEY_SPACE && action == GLFW_PRESS)
    {
        if (::g_pPhysicEngine->vecAABBs[0]->pPhysicInfo->velocity.y == 0.0 && ::g_pPhysicEngine->vecAABBs[0]->pPhysicInfo->position.y < 0.2f)
        {
            ::g_pPhysicEngine->vecAABBs[0]->pPhysicInfo->velocity.y = 0.05f;
        }

        /*if (::g_pLightManager->theLights[g_selectedLightIndex].param2.x == 1.0)
        {
            ::g_pLightManager->theLights[g_selectedLightIndex].param2.x = 0.0;
        }
        else if (::g_pLightManager->theLights[g_selectedLightIndex].param2.x == 0.0)
        {
            ::g_pLightManager->theLights[g_selectedLightIndex].param2.x = 1.0;
        }*/
    }

    if (key == GLFW_KEY_F && action == GLFW_PRESS)
    {
        ShootLaser();
    }
    if (key == GLFW_KEY_R && action == GLFW_PRESS)
    {
        ShootRocket();
    }

    if (key == GLFW_KEY_MINUS && action == GLFW_PRESS)
    {
        //back in meshes
        if (::g_selectedMeshIndex > 0)
        {
            ::g_selectedMeshIndex--;
        }
    }
    if (key == GLFW_KEY_EQUAL && action == GLFW_PRESS)
    {
        //forward in meshes
        ::g_selectedMeshIndex++;
        if (::g_selectedMeshIndex >= g_vecMeshesToDraw.size())
        {
            ::g_selectedMeshIndex = g_vecMeshesToDraw.size() - 1;
        }
    }

    if (key == GLFW_KEY_C && action == GLFW_PRESS)
    {
        if (g_bShowDebugSpheres)
        {
            g_bShowDebugSpheres = false;
        }
        else if (!g_bShowDebugSpheres)
        {
            g_bShowDebugSpheres = true;
        }
    }

    return;
}


// https://stackoverflow.com/questions/5289613/generate-random-float-between-two-floats
float getRandomFloat(float a, float b) {
    float random = ((float)rand()) / (float)RAND_MAX;
    float diff = b - a;
    float r = random * diff;
    return a + r;
}

int main(void)
{
    glfwSetErrorCallback(error_callback);

    if (!glfwInit())
        exit(EXIT_FAILURE);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(640, 480, "OpenGL Triangle", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    // Callback for keyboard, but for "typing"
    // Like it captures the press and release and repeat
    glfwSetKeyCallback(window, key_callback);

    // 
    glfwSetCursorPosCallback(window, cursor_position_callback);
    glfwSetWindowFocusCallback(window, cursor_enter_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetScrollCallback(window, scroll_callback);

    glfwMakeContextCurrent(window);
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    glfwSwapInterval(1);

    // NOTE: OpenGL error checks have been omitted for brevity

    cShaderManager* pShaderManager = new cShaderManager();

    cShaderManager::cShader vertexShader;
    vertexShader.fileName = "assets/shaders/vertex01.glsl";

    cShaderManager::cShader fragmentShader;
    fragmentShader.fileName = "assets/shaders/fragment01.glsl";

    if (!pShaderManager->createProgramFromFile("shader01",
        vertexShader, fragmentShader))
    {
        std::cout << "Error: " << pShaderManager->getLastError() << std::endl;
    }
    else
    {
        std::cout << "Shader built OK" << std::endl;
    }

    const GLuint program = pShaderManager->getIDFromFriendlyName("shader01");

    glUseProgram(program);


    // Loading the TYPES of models I can draw...

//    cVAOManager* pMeshManager = new cVAOManager();
    ::g_pMeshManager = new cVAOManager();

    ModelsToVAO(program);

    ::g_pPhysicEngine = new cPhysics();
    // For triangle meshes, let the physics object "know" about the VAO manager
    ::g_pPhysicEngine->setVAOManager(::g_pMeshManager);

    ::g_pFlyCamera = new cBasicFlyCamera();
    ::g_pFlyCamera->setEyeLocation(glm::vec3(55.0f, 2.0f, -102.2f));
    // Rotate the camera 180 degrees
    ::g_pFlyCamera->rotateLeftRight_Yaw_NoScaling(glm::radians(250.0f));

    // This also adds physics objects to the phsyics system

    AddModelsToScene();

    DrawDungeon();

    glUseProgram(program);

    // Enable depth buffering (z buffering)
    // https://registry.khronos.org/OpenGL-Refpages/gl4/html/glEnable.xhtml
    glEnable(GL_DEPTH_TEST);

    double currentFrameTime = glfwGetTime();
    double lastFrameTime = glfwGetTime();


    // Set up the lights
    ::g_pLightManager = new cLightManager();
    // Called only once
    ::g_pLightManager->loadUniformLocations(program);

    // Ambient Light (Directional)
    ::g_pLightManager->theLights[0].position = glm::vec4(70.0f, 10.0f, -130.0f, 1.0f);
    ::g_pLightManager->theLights[0].diffuse = glm::vec4(1.0f, 1.0f, 1.0f, 1.0);
    ::g_pLightManager->theLights[0].atten.y = 0.14f;
    ::g_pLightManager->theLights[0].atten.z = 0.006f;

    ::g_pLightManager->theLights[0].param1.x = 0.0f;    // Directional light (see shader)
    ::g_pLightManager->theLights[0].param2.x = 1.0f;    // Turn on (see shader)

    ::g_pLightManager->theLights[0].direction = glm::vec4(-0.25f, -0.9f, -0.25f, 1.0f);
    ::g_pLightManager->theLights[0].param1.y = 5.0f;   //  y = inner angle
    ::g_pLightManager->theLights[0].param1.z = 10.0f;   //  z = outer angle

    // Ambient Light (Directional)
    ::g_pLightManager->theLights[1].position = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
    ::g_pLightManager->theLights[1].diffuse = glm::vec4(1.0f, 1.0f, 1.0f, 1.0);
    ::g_pLightManager->theLights[1].atten.y = 0.01f;
    ::g_pLightManager->theLights[1].atten.z = 0.001f;

    ::g_pLightManager->theLights[1].param1.x = 2.0f;    // Directional light (see shader)
    ::g_pLightManager->theLights[1].param2.x = 1.0f;    // Turn on (see shader)

    ::g_pLightManager->theLights[1].direction = glm::vec4(-0.25f, -0.7f, -0.25f, 1.0f);
    ::g_pLightManager->theLights[1].param1.y = 5.0f;   //  y = inner angle
    ::g_pLightManager->theLights[1].param1.z = 10.0f;   //  z = outer angle

    cLightHelper TheLightHelper;

    while (!glfwWindowShouldClose(window))
    {
        float ratio;
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        ratio = width / (float)height;
        glViewport(0, 0, width, height);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //        glm::mat4 m, p, v, mvp;
        glm::mat4 matProjection = glm::mat4(1.0f);

        matProjection = glm::perspective(0.6f,           // FOV
            ratio,          // Aspect ratio of screen
            0.1f,           // Near plane
            2000.0f);       // Far plane

        // View or "camera"
        glm::mat4 matView = glm::mat4(1.0f);

        //        glm::vec3 cameraEye = glm::vec3(0.0, 0.0, 4.0f);
        glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
        glm::vec3 upVector = glm::vec3(0.0f, 1.0f, 0.0f);

        matView = glm::lookAt(::g_pFlyCamera->getEyeLocation(),
            ::g_pFlyCamera->getTargetLocation(),
            upVector);
        //        matView = glm::lookAt( cameraEye,
        //                               cameraTarget,
        //                               upVector);


        const GLint matView_UL = glGetUniformLocation(program, "matView");
        glUniformMatrix4fv(matView_UL, 1, GL_FALSE, (const GLfloat*)&matView);

        const GLint matProjection_UL = glGetUniformLocation(program, "matProjection");
        glUniformMatrix4fv(matProjection_UL, 1, GL_FALSE, (const GLfloat*)&matProjection);

        //        // *******************************************************************
        //        // Place light #0 where the little yellow "light sphere" is
        //        // Find the Light_Sphere
        //        sMesh* pLightSphere = pFindMeshByFriendlyName("Light_Sphere");
        //        // 
        //        pLightSphere->positionXYZ = ::g_pLightManager->theLights[::g_selectedLightIndex].position;

                // Update the light info in the shader
                // (Called every frame)
        ::g_pLightManager->updateShaderWithLightInfo();
        // *******************************************************************


        //    ____                       _                      
        //   |  _ \ _ __ __ ___      __ | |    ___   ___  _ __  
        //   | | | | '__/ _` \ \ /\ / / | |   / _ \ / _ \| '_ \ 
        //   | |_| | | | (_| |\ V  V /  | |__| (_) | (_) | |_) |
        //   |____/|_|  \__,_| \_/\_/   |_____\___/ \___/| .__/ 
        //                                               |_|    
        // Draw all the objects
        //for (unsigned int meshIndex = 0; meshIndex != ::g_NumberOfMeshesToDraw; meshIndex++)
        for (unsigned int meshIndex = 0; meshIndex != ::g_vecMeshesToDraw.size(); meshIndex++)
        {
            //            sMesh* pCurMesh = ::g_myMeshes[meshIndex];
            sMesh* pCurMesh = ::g_vecMeshesToDraw[meshIndex];

            DrawMesh(pCurMesh, program);

            // Laser impact
            {
                // Switch to shrink
                if (::g_vecMeshesToDraw[meshIndex]->uniqueFriendlyName == "Red_Sphere" && ::g_vecMeshesToDraw[meshIndex]->uniformScale >= 0.59)
                {
                    ::g_vecMeshesToDraw[meshIndex]->growing_shrinking = true;
                }

                // Grow
                if (::g_vecMeshesToDraw[meshIndex]->uniqueFriendlyName == "Red_Sphere" && ::g_vecMeshesToDraw[meshIndex]->uniformScale < 0.6 && ::g_vecMeshesToDraw[meshIndex]->growing_shrinking == false)
                {
                    ::g_vecMeshesToDraw[meshIndex]->uniformScale *= 1.05;
                }
                //Shrink
                else if (::g_vecMeshesToDraw[meshIndex]->uniqueFriendlyName == "Red_Sphere" && ::g_vecMeshesToDraw[meshIndex]->growing_shrinking == true)
                {
                    ::g_vecMeshesToDraw[meshIndex]->uniformScale *= 0.95;
                }

                // Erase
                if (::g_vecMeshesToDraw[meshIndex]->uniqueFriendlyName == "Red_Sphere" && ::g_vecMeshesToDraw[meshIndex]->uniformScale <= 0.1)
                {
                    ::g_vecMeshesToDraw.erase(::g_vecMeshesToDraw.begin() + meshIndex);
                    break;
                }
            }

            // Rocket impact
            {
                // Grow
                if (::g_vecMeshesToDraw[meshIndex]->uniqueFriendlyName == "Red_Rocket_Sphere" && ::g_vecMeshesToDraw[meshIndex]->uniformScale < 2.5)
                {
                    ::g_vecMeshesToDraw[meshIndex]->uniformScale *= 1.15;
                }
                // Erase
                if (::g_vecMeshesToDraw[meshIndex]->uniqueFriendlyName == "Red_Rocket_Sphere" && ::g_vecMeshesToDraw[meshIndex]->uniformScale >= 2.4)
                {
                    ::g_vecMeshesToDraw.erase(::g_vecMeshesToDraw.begin() + meshIndex);
                    break;
                }
            }

            // Rocket Exhaust
            {
                if (::g_vecMeshesToDraw[meshIndex]->uniqueFriendlyName == "Exhaust_Sphere")
                {
                    if (::g_vecMeshesToDraw[meshIndex]->count < 20)
                    {
                        ::g_vecMeshesToDraw[meshIndex]->count++;
                        ::g_vecMeshesToDraw[meshIndex]->count *= 1.05;
                    }
                    else
                    {
                        ::g_vecMeshesToDraw[meshIndex]->objectColourRGBA = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);
                        ::g_vecMeshesToDraw[meshIndex]->bIsVisible = false;
                        //::g_vecMeshesToDraw.erase(::g_vecMeshesToDraw.begin() + meshIndex);
                        //break;
                    }
                }

                if (::g_vecMeshesToDraw[meshIndex]->bIsVisible == false)
                {
                    //::g_vecMeshesToDraw.erase(::g_vecMeshesToDraw.begin() + meshIndex);
                    
                }
            }

        }//for (unsigned int meshIndex..


        for (unsigned int laserIndex = 0; laserIndex != g_vecLasers.size(); laserIndex++)
        {
            cPhysics::sLine* pCurLine = ::g_vecLasers[laserIndex];
            glm::vec3 intersectionPoint = pCurLine->endXYZ;

            // LASER
            if (pCurLine->isLaser == 1)
            {
                if (pCurLine->timer < 20.0)
                {
                    glm::vec3 LASERbeam_Offset = glm::vec3(0.0f, -0.5f, 0.0f);
                    glm::vec3 LASER_ball_location = pCurLine->startXYZ;

                    pCurLine->timer += 1.0;

                    // Now draw a different coloured ball wherever we get a collision with a triangle
                    std::vector<cPhysics::sCollision_RayTriangleInMesh> vec_RayTriangle_Collisions;
                    ::g_pPhysicEngine->rayCast(pCurLine->startXYZ, pCurLine->endXYZ, vec_RayTriangle_Collisions, false);

                    if (vec_RayTriangle_Collisions.size() > 0)
                    {
                        cPhysics::sCollision_RayTriangleInMesh tri = vec_RayTriangle_Collisions.at(0);
                        std::vector<cPhysics::sTriangle> tri2 = tri.vecTriangles;

                        //DrawDebugSphere(tri2.begin()->intersectionPoint, glm::vec4(0.5f, 0.5f, 0.5f, 1.0f), 0.25 * 2, program);
                        intersectionPoint = tri2.begin()->intersectionPoint;
                        if (pCurLine->timer == 1.0)
                        {
                            DrawExplosion(intersectionPoint);
                        }
                    }

                    while (glm::distance(::g_pFlyCamera->getEyeLocation(), LASER_ball_location) < glm::distance(::g_pFlyCamera->getEyeLocation(), intersectionPoint))
                    {
                        // Move the next ball 0.1 times the normalized camera direction
                        LASER_ball_location += (pCurLine->direction * 0.10f);
                        DrawDebugSphere(LASER_ball_location, glm::vec4((150 / 255.0f), (200 / 255.0f), (255 / 255.0f), 1.0f), 0.02f, program);
                    }
                }
                else
                {
                    g_vecLasers.erase(g_vecLasers.begin() + laserIndex);
                    break;
                }
            }

            // ROCKET
            else if (pCurLine->isRocket == 1)
            {
                if (pCurLine->timer < 1000.0)
                {
                    //std::cout << "Laser " << laserIndex << std::endl;

                    glm::vec3 LASERbeam_Offset = glm::vec3(0.0f, -0.5f, 0.0f);
                    glm::vec3 LASER_ball_location = pCurLine->startXYZ;

                    pCurLine->timer += 0.35;
                    pCurLine->timer *= 1.05;

                    // Now draw a different coloured ball wherever we get a collision with a triangle
                    std::vector<cPhysics::sCollision_RayTriangleInMesh> vec_RayTriangle_Collisions;
                    ::g_pPhysicEngine->rayCast(pCurLine->startXYZ, pCurLine->endXYZ, vec_RayTriangle_Collisions, false);

                    if (vec_RayTriangle_Collisions.size() > 0)
                    {
                        cPhysics::sCollision_RayTriangleInMesh tri = vec_RayTriangle_Collisions.at(0);
                        std::vector<cPhysics::sTriangle> tri2 = tri.vecTriangles;

                        //DrawDebugSphere(tri2.begin()->intersectionPoint, glm::vec4(0.5f, 0.5f, 0.5f, 1.0f), 0.25 * 2, program);
                        intersectionPoint = tri2.begin()->intersectionPoint;
                    }

                    float flyingCount = 1;
                    float lastDistCount = 1;

                    while (glm::distance(::g_pFlyCamera->getEyeLocation(), LASER_ball_location) < glm::distance(::g_pFlyCamera->getEyeLocation(), intersectionPoint))
                    {
                        // Move the next ball 0.1 times the normalized camera direction
                        LASER_ball_location += (pCurLine->direction * 0.10f);
                        flyingCount++;
                        
                        if (flyingCount == (int)pCurLine->timer)
                        {
                            DrawDebugSphere(LASER_ball_location, glm::vec4(0.0f, 0.0f, 0.5f, 1.0f), 0.2f, program);

                            float distance = glm::distance(LASER_ball_location, intersectionPoint);
                            if (distance / flyingCount != lastDistCount)
                            {
                                DrawExhaust(LASER_ball_location);
                            }

                            lastDistCount = distance / flyingCount;

                            if (distance/flyingCount < 0.0055)
                            {
                                DrawRocketExplosion(intersectionPoint);
                            }
                        }

                    }
                }
                else
                {
                    g_vecLasers.erase(g_vecLasers.begin() + laserIndex);
                    break;
                }
            }
            
        }

        // Draw the LASER beam
        cPhysics::sLine LASERbeam;
        glm::vec3 LASERbeam_Offset = glm::vec3(0.0f, -0.5f, 0.0f);

        if (::g_bShowLASERBeam)
        {
            // Draw a bunch of little balls along a line from the camera
            //  to some place in the distance

            // The fly camera is always "looking at" something 1.0 unit away
            glm::vec3 cameraDirection = ::g_pFlyCamera->getTargetRelativeToCamera();     //0,0.1,0.9


            LASERbeam.startXYZ = ::g_pFlyCamera->getEyeLocation();

            // Move the LASER below the camera
            LASERbeam.startXYZ += LASERbeam_Offset;
            glm::vec3 LASER_ball_location = LASERbeam.startXYZ;

            // Is the LASER less than 500 units long?
            // (is the last LAZER ball we drew beyond 500 units form the camera?)
            while (glm::distance(::g_pFlyCamera->getEyeLocation(), LASER_ball_location) < 150.0f)
            {
                // Move the next ball 0.1 times the normalized camera direction
                LASER_ball_location += (cameraDirection * 0.10f);
                //DrawDebugSphere(LASER_ball_location, glm::vec4(0.0f, 0.0f, 1.0f, 1.0f), 0.05f, program);
            }

            // Set the end of the LASER to the last location of the beam
            LASERbeam.endXYZ = LASER_ball_location;
            //LASERbeam.endXYZ.y -= (LASERbeam_Offset.y * 5);

        }//if (::g_bShowLASERBeam)

        // Draw the end of this LASER beam
        //DrawDebugSphere(LASERbeam.endXYZ, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), 0.1f, program);

        // Now draw a different coloured ball wherever we get a collision with a triangle
        std::vector<cPhysics::sCollision_RayTriangleInMesh> vec_RayTriangle_Collisions;
        ::g_pPhysicEngine->rayCast(LASERbeam.startXYZ, LASERbeam.endXYZ, vec_RayTriangle_Collisions, false);

        glm::vec4 triColour = glm::vec4(1.0f, 1.0f, 0.0f, 1.0f);
        float triangleSize = 0.25f;

        for (std::vector<cPhysics::sCollision_RayTriangleInMesh>::iterator itTriList = vec_RayTriangle_Collisions.begin();
            itTriList != vec_RayTriangle_Collisions.end(); itTriList++)
        {
            for (std::vector<cPhysics::sTriangle>::iterator itTri = itTriList->vecTriangles.begin();
                itTri != itTriList->vecTriangles.end(); itTri++)
            {
                // Draw a sphere at the centre of the triangle
//                glm::vec3 triCentre = (itTri->vertices[0] + itTri->vertices[1] + itTri->vertices[2]) / 3.0f;
//                DrawDebugSphere(triCentre, glm::vec4(1.0f, 0.0f, 0.0f, 1.0f), 0.5f, program);

                //DrawDebugSphere(itTri->intersectionPoint, triColour, triangleSize, program);
                triColour.r -= 0.1f;
                triColour.g -= 0.1f;
                triColour.b += 0.2f;


            }//for (std::vector<cPhysics::sTriangle>::iterator itTri = itTriList->vecTriangles

        }//for (std::vector<cPhysics::sCollision_RayTriangleInMesh>::iterator itTriList = vec_RayTriangle_Collisions

        if (vec_RayTriangle_Collisions.size() > 0)
        {
            cPhysics::sCollision_RayTriangleInMesh tri = vec_RayTriangle_Collisions.at(0);
            std::vector<cPhysics::sTriangle> tri2 = tri.vecTriangles;

            DrawDebugSphere(tri2.begin()->intersectionPoint, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), triangleSize/2, program);
        }

        if (::g_pPhysicEngine->vecAABBs[0]->pPhysicInfo->position.y > 1.5f)
        {
            ::g_pPhysicEngine->vecAABBs[0]->pPhysicInfo->velocity.y = 0.0f;
        }

        // **********************************************************************************
        if (::g_bShowDebugSpheres)
        {

            DrawDebugSphere(::g_pLightManager->theLights[::g_selectedLightIndex].position,
                glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), 0.1f, program);

            const float DEBUG_LIGHT_BRIGHTNESS = 0.3f;

            const float ACCURACY = 0.1f;       // How many units distance
            float distance_75_percent =
                TheLightHelper.calcApproxDistFromAtten(0.75f, ACCURACY, FLT_MAX,
                    ::g_pLightManager->theLights[::g_selectedLightIndex].atten.x,   // Const attent
                    ::g_pLightManager->theLights[::g_selectedLightIndex].atten.y,   // Linear attenuation
                    ::g_pLightManager->theLights[::g_selectedLightIndex].atten.z);  // Quadratic attenuation

            DrawDebugSphere(::g_pLightManager->theLights[::g_selectedLightIndex].position,
                glm::vec4(DEBUG_LIGHT_BRIGHTNESS, 0.0f, 0.0f, 1.0f),
                distance_75_percent,
                program);


            float distance_50_percent =
                TheLightHelper.calcApproxDistFromAtten(0.5f, ACCURACY, FLT_MAX,
                    ::g_pLightManager->theLights[::g_selectedLightIndex].atten.x,   // Const attent
                    ::g_pLightManager->theLights[::g_selectedLightIndex].atten.y,   // Linear attenuation
                    ::g_pLightManager->theLights[::g_selectedLightIndex].atten.z);  // Quadratic attenuation

            DrawDebugSphere(::g_pLightManager->theLights[::g_selectedLightIndex].position,
                glm::vec4(0.0f, DEBUG_LIGHT_BRIGHTNESS, 0.0f, 1.0f),
                distance_50_percent,
                program);

            float distance_25_percent =
                TheLightHelper.calcApproxDistFromAtten(0.25f, ACCURACY, FLT_MAX,
                    ::g_pLightManager->theLights[::g_selectedLightIndex].atten.x,   // Const attent
                    ::g_pLightManager->theLights[::g_selectedLightIndex].atten.y,   // Linear attenuation
                    ::g_pLightManager->theLights[::g_selectedLightIndex].atten.z);  // Quadratic attenuation

            DrawDebugSphere(::g_pLightManager->theLights[::g_selectedLightIndex].position,
                glm::vec4(0.0f, 0.0f, DEBUG_LIGHT_BRIGHTNESS, 1.0f),
                distance_25_percent,
                program);

            float distance_05_percent =
                TheLightHelper.calcApproxDistFromAtten(0.05f, ACCURACY, FLT_MAX,
                    ::g_pLightManager->theLights[::g_selectedLightIndex].atten.x,   // Const attent
                    ::g_pLightManager->theLights[::g_selectedLightIndex].atten.y,   // Linear attenuation
                    ::g_pLightManager->theLights[::g_selectedLightIndex].atten.z);  // Quadratic attenuation

            DrawDebugSphere(::g_pLightManager->theLights[::g_selectedLightIndex].position,
                glm::vec4(DEBUG_LIGHT_BRIGHTNESS, DEBUG_LIGHT_BRIGHTNESS, 0.0f, 1.0f),
                distance_05_percent,
                program);

        }
        // **********************************************************************************

        // Calculate elapsed time
        // We'll enhance this
        currentFrameTime = glfwGetTime();
        double deltaTime = currentFrameTime - lastFrameTime;
        lastFrameTime = currentFrameTime;

        // Handle async IO stuff
        handleKeyboardAsync(window);
        handleMouseAsync(window);

        ::g_pPhysicEngine->vecAABBs[0]->pPhysicInfo->position.y += ::g_pPhysicEngine->vecAABBs[0]->pPhysicInfo->velocity.y;
        ::g_pPhysicEngine->vecAABBs[0]->pPhysicInfo->position.y -= 0.01;

        // Physic update and test 
        ::g_pPhysicEngine->StepTick(deltaTime);

        // Handle any collisions

        //Sphere AABB Collision
        if (::g_pPhysicEngine->vec_SphereAABB_Collisions.size() > 0)
        {
            // Yes, there were collisions

            for (unsigned int index = 0; index != ::g_pPhysicEngine->vec_SphereAABB_Collisions.size(); index++)
            {
                cPhysics::sCollision_SphereAABB thisCollisionEvent = ::g_pPhysicEngine->vec_SphereAABB_Collisions[index];

                if (thisCollisionEvent.pTheSphere->pPhysicInfo->velocity.y < 0.0f)
                {
                    // Yes, it's heading down
                    // So reverse the direction of velocity
                    thisCollisionEvent.pTheSphere->pPhysicInfo->velocity.y = fabs(thisCollisionEvent.pTheSphere->pPhysicInfo->velocity.y);
                }

            }//for (unsigned int index

        }//if (::g_pPhysicEngine->vec_SphereAABB_Collisions

        //AABB Mesh Collision
        if (::g_pPhysicEngine->vec_AABBMesh_Collisions.size() > 0)
        {
            // Yes, there were collisions

            //moveCamera = true;

            for (unsigned int index = 0; index != ::g_pPhysicEngine->vec_AABBMesh_Collisions.size(); index++)
            {
                cPhysics::sCollision_AABBMesh thisCollisionEvent = ::g_pPhysicEngine->vec_AABBMesh_Collisions[index];

                std::vector<float> collisionDistance;
                collisionDistance.push_back(thisCollisionEvent.collisionWorldPoint.y - thisCollisionEvent.theTriangle.vertices[0].y);
                collisionDistance.push_back(thisCollisionEvent.collisionWorldPoint.y - thisCollisionEvent.theTriangle.vertices[1].y);
                collisionDistance.push_back(thisCollisionEvent.collisionWorldPoint.y - thisCollisionEvent.theTriangle.vertices[2].y);

                if (((collisionDistance[0] < 1.0 && collisionDistance[0] > -1.0) &&
                    (collisionDistance[1] < 1.0 && collisionDistance[1] > -1.0) &&
                    (collisionDistance[2] < 1.0 && collisionDistance[2] > -1.0)) ||
                    thisCollisionEvent.theTriangle.normal.y >= 1.0f)
                {
                    moveCamera = true;
                    ::g_pPhysicEngine->vecAABBs[0]->pPhysicInfo->position.y = cubePos.y;
                    ::g_pPhysicEngine->vecAABBs[0]->centreXYZ.y = cubePos.y;
                }
                else
                    //if (thisCollisionEvent.meshInstanceName != "assets/models/spaceship/SM_Env_Floor_01_xyz_n_rgba_uv.ply" &&
                    //    thisCollisionEvent.meshInstanceName != "assets/models/spaceship/SM_Bld_Hanger_01_NO_DOORS_or_WINDOWS_flatshaded_xyz_n.ply")
                {
                    moveCamera = false;
                    ::g_pPhysicEngine->vecAABBs[0]->pPhysicInfo->position = cubePos;
                    ::g_pPhysicEngine->vecAABBs[0]->centreXYZ = cubePos;
                }


                //if (thisCollisionEvent.pTheSphere->pPhysicInfo->velocity.y < 0.0f)
                //{
                    // Yes, it's heading down
                    // So reverse the direction of velocity
                //    thisCollisionEvent.pTheSphere->pPhysicInfo->velocity.y = fabs(thisCollisionEvent.pTheSphere->pPhysicInfo->velocity.y);
                //}

            }//for (unsigned int index

        }//if (::g_pPhysicEngine->vec_AABBMesh_Collisions

        if (moveCamera)
        {
            cubePos = ::g_pPhysicEngine->vecAABBs[0]->pPhysicInfo->pAssociatedDrawingMeshInstance->positionXYZ;
            ::g_pPhysicEngine->vecAABBs[0]->pPhysicInfo->pAssociatedDrawingMeshInstance->positionXYZ = ::g_pPhysicEngine->vecAABBs[0]->pPhysicInfo->position;
        }

        moveCamera = true;

        ::g_pFlyCamera->setEyeLocation(::g_pPhysicEngine->vecAABBs[0]->pPhysicInfo->position.x,
            ::g_pPhysicEngine->vecAABBs[0]->pPhysicInfo->position.y + 2,
            ::g_pPhysicEngine->vecAABBs[0]->pPhysicInfo->position.z);


        glfwSwapBuffers(window);
        glfwPollEvents();

        std::string lightOn;

        if (::g_pLightManager->theLights[g_selectedLightIndex].param2.x == true)
        {
            lightOn = "ON";
        }
        else
        {
            lightOn = "OFF";
        }

        //std::cout << "Camera: "
        std::stringstream ssTitle;
        ssTitle << "Camera: "
            << ::g_pFlyCamera->getEyeLocation().x << ", "
            << ::g_pFlyCamera->getEyeLocation().y << ", "
            << ::g_pFlyCamera->getEyeLocation().z
            << "   ";
        ssTitle << "Light[" << g_selectedLightIndex << "]: "
            << lightOn << " "
            << ::g_pLightManager->theLights[g_selectedLightIndex].position.x << ", "
            << ::g_pLightManager->theLights[g_selectedLightIndex].position.y << ", "
            << ::g_pLightManager->theLights[g_selectedLightIndex].position.z
            << "   "
            << "linear: " << ::g_pLightManager->theLights[g_selectedLightIndex].atten.y
            << "   "
            << "quad: " << ::g_pLightManager->theLights[g_selectedLightIndex].atten.z
            << "   ";
        ssTitle << "mesh[" << g_selectedMeshIndex << "] "
            << ::g_vecMeshesToDraw[g_selectedMeshIndex]->uniqueFriendlyName << "  "
            << ::g_vecMeshesToDraw[g_selectedMeshIndex]->positionXYZ.x << ", "
            << ::g_vecMeshesToDraw[g_selectedMeshIndex]->positionXYZ.y << ", "
            << ::g_vecMeshesToDraw[g_selectedMeshIndex]->positionXYZ.z
            << "  ";

        glfwSetWindowTitle(window, ssTitle.str().c_str());

    }// End of the draw loop


    // Delete everything
    delete ::g_pFlyCamera;
    delete ::g_pPhysicEngine;

    glfwDestroyWindow(window);

    glfwTerminate();
    exit(EXIT_SUCCESS);
}

void AddModelsToScene()
{
    //DrawMeshes();
    //LoadMeshes();

    /*sMesh* pHanger = new sMesh();
    pHanger->modelFileName = "assets/models/spaceship/SM_Bld_Hanger_01_xyz_n_uv.ply";
    pHanger->positionXYZ = glm::vec3(73.15, 0.0f, -133.0);
    pHanger->bIsWireframe = false;
    pHanger->rotationEulerXYZ.y = 180.0f;
    pHanger->uniformScale = 0.0154f;
    pHanger->objectColourRGBA = glm::vec4((255 / 255.0f), (255 / 255.0f), (255 / 255.0f), 1.0f);
    pHanger->uniqueFriendlyName = "Hanger";

    ::g_vecMeshesToDraw.push_back(pHanger);

    ::g_pPhysicEngine->addTriangleMesh(
        pHanger->uniqueFriendlyName,
        "assets/models/spaceship/SM_Bld_Hanger_01_xyz_n_uv.ply",
        pHanger->positionXYZ,
        pHanger->rotationEulerXYZ,
        pHanger->uniformScale);*/

    sMesh* pHanger = new sMesh();
    pHanger->modelFileName = "assets/models/spaceship/SM_Bld_Hanger_01_NO_DOORS_or_WINDOWS_flatshaded_xyz_n.ply";
    pHanger->positionXYZ = glm::vec3(73.15, 0.0f, -133.0);
    pHanger->bIsWireframe = false;
    pHanger->rotationEulerXYZ.y = 180.0f;
    pHanger->uniformScale = 0.0154f;
    pHanger->objectColourRGBA = glm::vec4((255 / 255.0f), (255 / 255.0f), (255 / 255.0f), 1.0f);
    pHanger->uniqueFriendlyName = "Hanger";

    ::g_vecMeshesToDraw.push_back(pHanger);

    ::g_pPhysicEngine->addTriangleMesh(
        pHanger->uniqueFriendlyName,
        "assets/models/spaceship/SM_Bld_Hanger_01_NO_DOORS_or_WINDOWS_flatshaded_xyz_n.ply",
        pHanger->positionXYZ,
        pHanger->rotationEulerXYZ,
        pHanger->uniformScale);

    sMesh* pHangerPlatform = new sMesh();
    pHangerPlatform->modelFileName = "assets/models/spaceship/SM_Bld_HangerPlatform_01_xyz_n_uv.ply";
    pHangerPlatform->positionXYZ = glm::vec3(73.15, 0.5f, -133.0);
    pHangerPlatform->bIsWireframe = false;
    pHangerPlatform->rotationEulerXYZ.y = 180.0f;
    pHangerPlatform->uniformScale = 0.0154f;
    pHangerPlatform->objectColourRGBA = glm::vec4((255 / 255.0f), (255 / 255.0f), (255 / 255.0f), 1.0f);
    pHangerPlatform->uniqueFriendlyName = "Hanger";

    ::g_vecMeshesToDraw.push_back(pHangerPlatform);

    ::g_pPhysicEngine->addTriangleMesh(
        pHangerPlatform->uniqueFriendlyName,
        "assets/models/spaceship/SM_Bld_HangerPlatform_01_xyz_n_uv.ply",
        pHangerPlatform->positionXYZ,
        pHangerPlatform->rotationEulerXYZ,
        pHangerPlatform->uniformScale);

    sMesh* pCube = new sMesh();
    pCube->modelFileName = "assets/models/Cube_xyz_N.ply";
    pCube->positionXYZ = glm::vec3(58.0f, 0.6f, -101.5f);
    pCube->bIsWireframe = false;
    pCube->rotationEulerXYZ.y = 0.0f;
    pCube->uniformScale = 0.5f;
    pCube->objectColourRGBA = glm::vec4((0 / 255.0f), (0 / 255.0f), (255 / 255.0f), 1.0f);
    pCube->uniqueFriendlyName = "Cube";
    pCube->bIsVisible = false;

    cubePos = ::g_pFlyCamera->getEyeLocation();
    cubePos.x += 8;
    cubePos.y -= 1.0;

    ::g_vecMeshesToDraw.push_back(pCube);

    cPhysics::sAABB* pAABBCube = new cPhysics::sAABB();
    pAABBCube->centreXYZ = pCube->positionXYZ;//glm::vec3(0.0f, 0.0f, 0.0f);
    sModelDrawInfo cubeMeshInfo;
    ::g_pMeshManager->FindDrawInfoByModelName(pCube->modelFileName, cubeMeshInfo);

    cubeMeshInfo.calculateExtents();

    // How far from the centre the XYZ min and max are
    // This information is from the mesh we loaded
    // WARNING: We need to be careful about the scale
    pAABBCube->minXYZ = cubeMeshInfo.minXYZ * pCube->uniformScale;
    pAABBCube->maxXYZ = cubeMeshInfo.maxXYZ * pCube->uniformScale;

    // Copy the physics object position from the initial mesh position
    pAABBCube->pPhysicInfo->position = pCube->positionXYZ;

    // Don't move this ground (skip integration step)
    pAABBCube->pPhysicInfo->bDoesntMove = true;
    pAABBCube->pPhysicInfo->velocity = glm::vec3(0.0f, 0.0f, 0.0f);
    pAABBCube->pPhysicInfo->acceleration = glm::vec3(0.0f, 0.0f, 0.0f);

    //pAABBCube->pPhysicInfo->velocity.x = 1.0f;
    //pAABBCube->pPhysicInfo->velocity.z = -1.0f;
    //pAABBCube->pPhysicInfo->velocity.y = -0.2f;

    pAABBCube->pPhysicInfo->pAssociatedDrawingMeshInstance = pCube;

    ::g_pPhysicEngine->vecAABBs.push_back(pAABBCube);
}

void DrawDungeon()
{
    std::ifstream mazeTxt("Dungeon (Medium Rooms).tsv");
    //std::ofstream testOutput("test.txt");

    char** textArr = new char* [30];
    std::string discard;

    float xLoc;
    float yLoc;

    for (int x = 0; x < 30; x++)
    {
        textArr[x] = new char[24];
        getline(mazeTxt, discard);
        for (int y = 0; y < 24; y++)
        {
            if (x > 0)
            {
                xLoc = x * 5;
            }
            else
            {
                xLoc = 0;
            }

            if (y > 0)
            {
                yLoc = y * 5;
            }
            else
            {
                yLoc = 0;
            }

            if (discard[y] == 'F')
            {
                srand(g_floorCount);

                int propType = rand() % 15;
                int rotationType = rand() % 4;
                std::string path;
                float rotation = 0.0f;
                float scale = 1.0;

                if (propType == 0)
                {
                    path = "assets/models/spaceship/SM_Env_Construction_Block_01_xyz_n_rgba_uv.ply";
                    scale = 0.2f;
                }
                else if (propType == 1)
                {
                    path = "assets/models/spaceship/SM_Prop_Stairs_01_xyz_n_rgba_uv.ply";
                    scale = 0.9f;
                }
                else if (propType == 2)
                {
                    path = "assets/models/spaceship/SM_Prop_StepLadder_01_xyz_n_rgba_uv.ply";
                    scale = 0.9f;
                }

                if (rotationType == 0)
                {
                    rotation = 0.0f;
                }
                else if (rotationType == 1)
                {
                    rotation = 90.0f;
                }
                else if (rotationType == 2)
                {
                    rotation = 180.0f;
                }
                else if (rotationType == 3)
                {
                    rotation = 270.0f;
                }

                float xOffset = getRandomFloat(xLoc + 1.0, xLoc + 4.0);
                float zOffset = getRandomFloat(yLoc - 1.0, yLoc - 4.0);

                sMesh* pProp = new sMesh();
                pProp->modelFileName = path;
                pProp->positionXYZ = glm::vec3(xOffset, 0.0f, -zOffset);
                pProp->rotationEulerXYZ.y = rotation;
                pProp->bIsWireframe = false;
                pProp->uniformScale = scale;
                pProp->objectColourRGBA = glm::vec4((250 / 255.0f), (250 / 255.0f), (250 / 255.0f), 1.0f);
                pProp->uniqueFriendlyName = "Prop" + std::to_string(g_floorCount);

                ::g_vecMeshesToDraw.push_back(pProp);

                ::g_pPhysicEngine->addTriangleMesh(
                    pProp->uniqueFriendlyName,
                    path,
                    pProp->positionXYZ,
                    pProp->rotationEulerXYZ,
                    pProp->uniformScale);

                sMesh* pFloor = new sMesh();
                pFloor->modelFileName = "assets/models/spaceship/SM_Env_Floor_01_xyz_n_rgba_uv.ply";
                pFloor->positionXYZ = glm::vec3(xLoc, 0.0f, -yLoc);
                pFloor->bIsWireframe = false;
                pFloor->uniformScale = 1.0f;
                pFloor->objectColourRGBA = glm::vec4((250 / 255.0f), (250 / 255.0f), (250 / 255.0f), 1.0f);
                pFloor->uniqueFriendlyName = "Floor" + std::to_string(g_floorCount);

                g_floorCount++;

                sMesh* pCeiling = new sMesh();
                pCeiling->modelFileName = "assets/models/spaceship/SM_Env_Floor_01_xyz_n_rgba_uv.ply";
                pCeiling->positionXYZ = glm::vec3(xLoc - 5, 5.0f, -yLoc);
                pCeiling->bIsWireframe = false;
                pCeiling->rotationEulerXYZ.z = 180.0f;
                pCeiling->uniformScale = 1.0f;
                pCeiling->objectColourRGBA = glm::vec4((105 / 255.0f), (106 / 255.0f), (90 / 255.0f), 1.0f);
                pCeiling->uniqueFriendlyName = "Ceiling" + std::to_string(g_ceilingCount);

                g_ceilingCount++;

                ::g_vecMeshesToDraw.push_back(pFloor);
                ::g_vecMeshesToDraw.push_back(pCeiling);

                ::g_pPhysicEngine->addTriangleMesh(
                    pFloor->uniqueFriendlyName,
                    "assets/models/spaceship/SM_Env_Floor_01_xyz_n_rgba_uv.ply",
                    pFloor->positionXYZ,
                    pFloor->rotationEulerXYZ,
                    pFloor->uniformScale);

                ::g_pPhysicEngine->addTriangleMesh(
                    pCeiling->uniqueFriendlyName,
                    "assets/models/spaceship/SM_Env_Floor_01_xyz_n_rgba_uv.ply",
                    pCeiling->positionXYZ,
                    pCeiling->rotationEulerXYZ,
                    pCeiling->uniformScale);
            }
            else if (discard[y] == 'H')
            {
                sMesh* pWall1 = new sMesh();
                pWall1->modelFileName = "assets/models/spaceship/SM_Env_Wall_013_xyz_n_rgba_uv.ply";
                pWall1->positionXYZ = glm::vec3(xLoc, 0.0f, -yLoc);
                pWall1->rotationEulerXYZ.y = 90.0f;
                pWall1->bIsWireframe = false;
                pWall1->uniformScale = 1.0f;
                pWall1->objectColourRGBA = glm::vec4((124 / 255.0f), (133 / 255.0f), (123 / 255.0f), 1.0f);
                pWall1->uniqueFriendlyName = "Wall" + std::to_string(g_wallCount);

                g_wallCount++;

                sMesh* pWall15 = new sMesh();
                pWall15->modelFileName = "assets/models/spaceship/SM_Env_Wall_013_xyz_n_rgba_uv.ply";
                pWall15->positionXYZ = glm::vec3(xLoc, 0.0f, -yLoc);
                pWall15->rotationEulerXYZ.y = -90.0f;
                pWall15->bIsWireframe = false;
                pWall15->uniformScale = 1.0f;
                pWall15->objectColourRGBA = glm::vec4((124 / 255.0f), (133 / 255.0f), (123 / 255.0f), 1.0f);
                pWall15->uniqueFriendlyName = "Wall" + std::to_string(g_wallCount);

                g_wallCount++;

                sMesh* pWall2 = new sMesh();
                pWall2->modelFileName = "assets/models/spaceship/SM_Env_Wall_013_xyz_n_rgba_uv.ply";
                pWall2->positionXYZ = glm::vec3(xLoc - 5, 0.0f, -yLoc + 5);
                pWall2->rotationEulerXYZ.y = -90.0f;
                pWall2->bIsWireframe = false;
                pWall2->uniformScale = 1.0f;
                pWall2->objectColourRGBA = glm::vec4((124 / 255.0f), (133 / 255.0f), (123 / 255.0f), 1.0f);
                pWall2->uniqueFriendlyName = "Wall" + std::to_string(g_wallCount);

                g_wallCount++;

                ::g_vecMeshesToDraw.push_back(pWall1);
                ::g_vecMeshesToDraw.push_back(pWall15);
                ::g_vecMeshesToDraw.push_back(pWall2);

                ::g_pPhysicEngine->addTriangleMesh(
                    pWall1->uniqueFriendlyName,
                    "assets/models/spaceship/SM_Env_Wall_013_xyz_n_rgba_uv.ply",
                    pWall1->positionXYZ,
                    pWall1->rotationEulerXYZ,
                    pWall1->uniformScale);

                ::g_pPhysicEngine->addTriangleMesh(
                    pWall15->uniqueFriendlyName,
                    "assets/models/spaceship/SM_Env_Wall_013_xyz_n_rgba_uv.ply",
                    pWall15->positionXYZ,
                    pWall15->rotationEulerXYZ,
                    pWall15->uniformScale);

                ::g_pPhysicEngine->addTriangleMesh(
                    pWall2->uniqueFriendlyName,
                    "assets/models/spaceship/SM_Env_Wall_013_xyz_n_rgba_uv.ply",
                    pWall2->positionXYZ,
                    pWall2->rotationEulerXYZ,
                    pWall2->uniformScale);
            }
            else if (discard[y] == 'V')
            {
                sMesh* pWall1 = new sMesh();
                pWall1->modelFileName = "assets/models/spaceship/SM_Env_Wall_05_xyz_n_rgba_uv.ply";
                pWall1->positionXYZ = glm::vec3(xLoc - 5, 0.0f, -yLoc);
                pWall1->rotationEulerXYZ.y = 180.0f;
                pWall1->bIsWireframe = false;
                pWall1->uniformScale = 1.0f;
                pWall1->objectColourRGBA = glm::vec4((124 / 255.0f), (133 / 255.0f), (123 / 255.0f), 1.0f);
                pWall1->uniqueFriendlyName = "Wall" + std::to_string(g_wallCount);

                g_wallCount++;

                sMesh* pWall2 = new sMesh();
                pWall2->modelFileName = "assets/models/spaceship/SM_Env_Wall_06_xyz_n_rgba_uv.ply";
                pWall2->positionXYZ = glm::vec3(xLoc, 0.0f, -yLoc + 5);
                pWall2->rotationEulerXYZ.y = 0.0f;
                pWall2->bIsWireframe = false;
                pWall2->uniformScale = 1.0f;
                pWall2->objectColourRGBA = glm::vec4((124 / 255.0f), (133 / 255.0f), (123 / 255.0f), 1.0f);
                pWall2->uniqueFriendlyName = "Wall" + std::to_string(g_wallCount);

                g_wallCount++;

                ::g_vecMeshesToDraw.push_back(pWall1);
                ::g_vecMeshesToDraw.push_back(pWall2);

                ::g_pPhysicEngine->addTriangleMesh(
                    pWall1->uniqueFriendlyName,
                    "assets/models/spaceship/SM_Env_Wall_05_xyz_n_rgba_uv.ply",
                    pWall1->positionXYZ,
                    pWall1->rotationEulerXYZ,
                    pWall1->uniformScale);

                ::g_pPhysicEngine->addTriangleMesh(
                    pWall2->uniqueFriendlyName,
                    "assets/models/spaceship/SM_Env_Wall_06_xyz_n_rgba_uv.ply",
                    pWall2->positionXYZ,
                    pWall2->rotationEulerXYZ,
                    pWall2->uniformScale);
            }
            else if (discard[y] == 'C')
            {
                sMesh* pWall1 = new sMesh();
                pWall1->modelFileName = "assets/models/spaceship/SM_Env_Wall_015_xyz_n_rgba_uv.ply";
                pWall1->positionXYZ = glm::vec3(xLoc, 0.0f, -yLoc);
                pWall1->rotationEulerXYZ.y = 90.0f;
                pWall1->bIsWireframe = false;
                pWall1->uniformScale = 1.0f;
                pWall1->objectColourRGBA = glm::vec4((124 / 255.0f), (133 / 255.0f), (123 / 255.0f), 1.0f);
                pWall1->uniqueFriendlyName = "Wall" + std::to_string(g_wallCount);

                g_wallCount++;

                sMesh* pWall15 = new sMesh();
                pWall15->modelFileName = "assets/models/spaceship/SM_Env_Wall_015_xyz_n_rgba_uv.ply";
                pWall15->positionXYZ = glm::vec3(xLoc, 0.0f, -yLoc + 5);
                pWall15->rotationEulerXYZ.y = -90.0f;
                pWall15->bIsWireframe = false;
                pWall15->uniformScale = 1.0f;
                pWall15->objectColourRGBA = glm::vec4((124 / 255.0f), (133 / 255.0f), (123 / 255.0f), 1.0f);
                pWall15->uniqueFriendlyName = "Wall" + std::to_string(g_wallCount);

                g_wallCount++;

                sMesh* pWall2 = new sMesh();
                pWall2->modelFileName = "assets/models/spaceship/SM_Env_Wall_015_xyz_n_rgba_uv.ply";
                pWall2->positionXYZ = glm::vec3(xLoc - 5, 0.0f, -yLoc + 5);
                pWall2->rotationEulerXYZ.y = -90.0f;
                pWall2->bIsWireframe = false;
                pWall2->uniformScale = 1.0f;
                pWall2->objectColourRGBA = glm::vec4((124 / 255.0f), (133 / 255.0f), (123 / 255.0f), 1.0f);
                pWall2->uniqueFriendlyName = "Wall" + std::to_string(g_wallCount);

                g_wallCount++;

                sMesh* pWall3 = new sMesh();
                pWall3->modelFileName = "assets/models/spaceship/SM_Env_Wall_015_xyz_n_rgba_uv.ply";
                pWall3->positionXYZ = glm::vec3(xLoc - 5, 0.0f, -yLoc);
                pWall3->rotationEulerXYZ.y = 180.0f;
                pWall3->bIsWireframe = false;
                pWall3->uniformScale = 1.0f;
                pWall3->objectColourRGBA = glm::vec4((124 / 255.0f), (133 / 255.0f), (123 / 255.0f), 1.0f);
                pWall3->uniqueFriendlyName = "Wall" + std::to_string(g_wallCount);

                g_wallCount++;

                sMesh* pWall4 = new sMesh();
                pWall4->modelFileName = "assets/models/spaceship/SM_Env_Wall_015_xyz_n_rgba_uv.ply";
                pWall4->positionXYZ = glm::vec3(xLoc, 0.0f, -yLoc + 5);
                pWall4->rotationEulerXYZ.y = 0.0f;
                pWall4->bIsWireframe = false;
                pWall4->uniformScale = 1.0f;
                pWall4->objectColourRGBA = glm::vec4((124 / 255.0f), (133 / 255.0f), (123 / 255.0f), 1.0f);
                pWall4->uniqueFriendlyName = "Wall" + std::to_string(g_wallCount);

                g_wallCount++;

                ::g_vecMeshesToDraw.push_back(pWall1);
                ::g_vecMeshesToDraw.push_back(pWall15);
                ::g_vecMeshesToDraw.push_back(pWall2);
                ::g_vecMeshesToDraw.push_back(pWall3);
                ::g_vecMeshesToDraw.push_back(pWall4);

                ::g_pPhysicEngine->addTriangleMesh(
                    pWall1->uniqueFriendlyName,
                    "assets/models/spaceship/SM_Env_Wall_015_xyz_n_rgba_uv.ply",
                    pWall1->positionXYZ,
                    pWall1->rotationEulerXYZ,
                    pWall1->uniformScale);

                ::g_pPhysicEngine->addTriangleMesh(
                    pWall15->uniqueFriendlyName,
                    "assets/models/spaceship/SM_Env_Wall_015_xyz_n_rgba_uv.ply",
                    pWall15->positionXYZ,
                    pWall15->rotationEulerXYZ,
                    pWall15->uniformScale);

                ::g_pPhysicEngine->addTriangleMesh(
                    pWall2->uniqueFriendlyName,
                    "assets/models/spaceship/SM_Env_Wall_015_xyz_n_rgba_uv.ply",
                    pWall2->positionXYZ,
                    pWall2->rotationEulerXYZ,
                    pWall2->uniformScale);

                ::g_pPhysicEngine->addTriangleMesh(
                    pWall3->uniqueFriendlyName,
                    "assets/models/spaceship/SM_Env_Wall_015_xyz_n_rgba_uv.ply",
                    pWall3->positionXYZ,
                    pWall3->rotationEulerXYZ,
                    pWall3->uniformScale);

                ::g_pPhysicEngine->addTriangleMesh(
                    pWall4->uniqueFriendlyName,
                    "assets/models/spaceship/SM_Env_Wall_015_xyz_n_rgba_uv.ply",
                    pWall4->positionXYZ,
                    pWall4->rotationEulerXYZ,
                    pWall4->uniformScale);
            }
            else if (discard[y] == 'A')
            {

            }
            else
            {

            }
        }
    }

    return;
}

void ShootLaser()
{
    // Create laser
    cPhysics::sLine* laser = new cPhysics::sLine;

    // Draw a bunch of little balls along a line from the camera
    //  to some place in the distance

    // The fly camera is always "looking at" something 1.0 unit away
    laser->direction = ::g_pFlyCamera->getTargetRelativeToCamera();     //0,0.1,0.9
    glm::vec3 LASERbeam_Offset = glm::vec3(0.0f, -0.5f, 0.0f);

    laser->startXYZ = ::g_pFlyCamera->getEyeLocation();

    // Move the LASER below the camera
    laser->startXYZ += LASERbeam_Offset;
    glm::vec3 LASER_ball_location = laser->startXYZ;

    // Is the LASER less than 500 units long?
    // (is the last LAZER ball we drew beyond 500 units form the camera?)
    while (glm::distance(::g_pFlyCamera->getEyeLocation(), LASER_ball_location) < 150.0f)
    {
        // Move the next ball 0.1 times the normalized camera direction
        LASER_ball_location += (laser->direction * 0.10f);
        //DrawDebugSphere(LASER_ball_location, glm::vec4(0.0f, 0.0f, 1.0f, 1.0f), 0.05f, program);
    }

    // Set the end of the LASER to the last location of the beam
    laser->endXYZ = LASER_ball_location;
    //laser->endXYZ.y -= (LASERbeam_Offset.y * 5);

    laser->timer = 0.0f;
    laser->isLaser = 1;
    laser->isRocket = -1;

    // Add laser to vector
    g_vecLasers.push_back(laser);

    return;
}

void DrawExplosion(glm::vec3 location)
{
    sMesh* pRedSphere = new sMesh();
    pRedSphere->modelFileName = "assets/models/Sphere_radius_1_xyz_N.ply";
    pRedSphere->positionXYZ = location;
    pRedSphere->bIsWireframe = false;
    pRedSphere->rotationEulerXYZ.y = 0.0f;
    pRedSphere->uniformScale = 0.2f;
    pRedSphere->objectColourRGBA = glm::vec4((255 / 255.0f), (0 / 255.0f), (0 / 255.0f), 1.0f);
    pRedSphere->uniqueFriendlyName = "Red_Sphere";
    pRedSphere->growing_shrinking = false;

    ::g_vecMeshesToDraw.push_back(pRedSphere);

    sMesh* pGreySphere = new sMesh();
    pGreySphere->modelFileName = "assets/models/Sphere_radius_1_xyz_N.ply";
    pGreySphere->positionXYZ = location;
    pGreySphere->bIsWireframe = false;
    pGreySphere->rotationEulerXYZ.y = 0.0f;
    pGreySphere->uniformScale = 0.2f;
    pGreySphere->objectColourRGBA = glm::vec4((80 / 255.0f), (80 / 255.0f), (80 / 255.0f), 1.0f);
    pGreySphere->uniqueFriendlyName = "Grey_Sphere";

    ::g_vecMeshesToDraw.push_back(pGreySphere);
}

void ShootRocket()
{
    // Create rocket
    cPhysics::sLine* rocket = new cPhysics::sLine;

    // Draw a bunch of little balls along a line from the camera
    //  to some place in the distance

    // The fly camera is always "looking at" something 1.0 unit away
    rocket->direction = ::g_pFlyCamera->getTargetRelativeToCamera();     //0,0.1,0.9
    glm::vec3 LASERbeam_Offset = glm::vec3(0.0f, -0.5f, 0.0f);

    rocket->startXYZ = ::g_pFlyCamera->getEyeLocation();

    // Move the LASER below the camera
    rocket->startXYZ += LASERbeam_Offset;
    glm::vec3 LASER_ball_location = rocket->startXYZ;

    // Is the LASER less than 500 units long?
    // (is the last LAZER ball we drew beyond 500 units form the camera?)
    while (glm::distance(::g_pFlyCamera->getEyeLocation(), LASER_ball_location) < 150.0f)
    {
        // Move the next ball 0.1 times the normalized camera direction
        LASER_ball_location += (rocket->direction * 0.10f);
        //DrawDebugSphere(LASER_ball_location, glm::vec4(0.0f, 0.0f, 1.0f, 1.0f), 0.05f, program);
    }

    // Set the end of the LASER to the last location of the beam
    rocket->endXYZ = LASER_ball_location;
    //laser->endXYZ.y -= (LASERbeam_Offset.y * 5);

    rocket->timer = 0.0f;
    rocket->isLaser = -1;
    rocket->isRocket = 1;

    // Add laser to vector
    g_vecLasers.push_back(rocket);

    return;
}

void DrawRocketExplosion(glm::vec3 location)
{
    sMesh* pRedSphere = new sMesh();
    pRedSphere->modelFileName = "assets/models/Sphere_radius_1_xyz_N.ply";
    pRedSphere->positionXYZ = location;
    pRedSphere->bIsWireframe = false;
    pRedSphere->rotationEulerXYZ.y = 0.0f;
    pRedSphere->uniformScale = 0.2f;
    pRedSphere->objectColourRGBA = glm::vec4((255 / 255.0f), (0 / 255.0f), (0 / 255.0f), 1.0f);
    pRedSphere->uniqueFriendlyName = "Red_Rocket_Sphere";
    pRedSphere->growing_shrinking = false;

    ::g_vecMeshesToDraw.push_back(pRedSphere);

    sMesh* pGreySphere = new sMesh();
    pGreySphere->modelFileName = "assets/models/Sphere_radius_1_xyz_N.ply";
    pGreySphere->positionXYZ = location;
    pGreySphere->bIsWireframe = false;
    pGreySphere->rotationEulerXYZ.y = 0.0f;
    pGreySphere->uniformScale = 0.2f;
    pGreySphere->objectColourRGBA = glm::vec4((80 / 255.0f), (80 / 255.0f), (80 / 255.0f), 1.0f);
    pGreySphere->uniqueFriendlyName = "Grey_Sphere";

    ::g_vecMeshesToDraw.push_back(pGreySphere);
}

void DrawExhaust(glm::vec3 location)
{
    sMesh* pExhaustSphere = new sMesh();
    pExhaustSphere->modelFileName = "assets/models/Sphere_radius_1_xyz_N.ply";
    pExhaustSphere->positionXYZ = location;
    pExhaustSphere->bIsWireframe = false;
    pExhaustSphere->rotationEulerXYZ.y = 0.0f;
    pExhaustSphere->uniformScale = 0.1f;
    pExhaustSphere->objectColourRGBA = glm::vec4((170 / 255.0f), (170 / 255.0f), (170 / 255.0f), 1.0f);
    pExhaustSphere->uniqueFriendlyName = "Exhaust_Sphere";
    pExhaustSphere->count = 0.0f;

    ::g_vecMeshesToDraw.push_back(pExhaustSphere);
}