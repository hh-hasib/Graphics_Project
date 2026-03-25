
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "shader.h"
#include "basic_camera.h"
#include "pointLight.h"
#include "directionalLight.h"
#include "spotLight.h"
#include "sphere.h"
#include "pyramid.h"
#include "bezierCurve.h"
#include "fractalTree.h"
#include "stb_image.h"
#include <iostream>
#include <vector>
using namespace std;

// ---- Forward Declarations ----
void framebuffer_size_callback(GLFWwindow *, int, int);
void key_callback(GLFWwindow *, int, int, int, int);
void mouse_callback(GLFWwindow *, double, double);
void scroll_callback(GLFWwindow *, double, double);
void processInput(GLFWwindow *);
void drawCube(unsigned int &v, Shader &s, glm::mat4 pm, glm::vec3 c, glm::vec3 p, glm::vec3 sc, float sh = 32, float a = 1.0f);
void drawCubeTextured(unsigned int &v, Shader &s, glm::mat4 pm, glm::vec3 c, glm::vec3 p, glm::vec3 sc, unsigned int tex, float tiling = 1.0f, float sh = 32, float a = 1.0f);
unsigned int loadTexture(const char *path);
void drawScene(unsigned int &, unsigned int &, Shader &, Shader &, glm::mat4, glm::mat4);

// Settings
unsigned int SCR_WIDTH = 1400, SCR_HEIGHT = 900;
float lastX = 700, lastY = 450;
bool firstMouse = true;
BasicCamera basic_camera(0, 1.7f, 50, 0, 1.7f, 30, glm::vec3(0, 1, 0));
float deltaTime = 0, lastFrame = 0;

// Mall Dimensions (from map)
//  Mall: X: -25 to 25, Z: -25 to 25, Y: 0 to 5
//  N-S corridor: X: -3 to 3
//  E-W corridor: Z: 10 to 16 (divides shops from back areas)
//  Entrance: south wall Z=25, gap X:-4 to 4
const float MH = 5.0f; // mall height
const float WT = 0.3f; // wall thickness

// Colors
//  Exterior
const glm::vec3 C_GRASS(0.22f, 0.55f, 0.18f);
const glm::vec3 C_GRASS2(0.18f, 0.48f, 0.15f);
const glm::vec3 C_ROAD(0.25f, 0.25f, 0.27f);
const glm::vec3 C_SIDEWALK(0.68f, 0.66f, 0.62f);
const glm::vec3 C_ROAD_YEL(0.95f, 0.82f, 0.12f);
const glm::vec3 C_ROAD_WHT(0.92f, 0.92f, 0.90f);
const glm::vec3 C_PARKING(0.42f, 0.42f, 0.44f);
const glm::vec3 C_PARK_LINE(0.95f, 0.85f, 0.15f);
const glm::vec3 C_EXT(0.80f, 0.78f, 0.74f); // exterior wall
const glm::vec3 C_GLASS(0.58f, 0.80f, 0.93f);
const glm::vec3 C_AWNING(0.38f, 0.38f, 0.42f);
const glm::vec3 C_TRUNK(0.42f, 0.30f, 0.16f);
const glm::vec3 C_CROWN(0.22f, 0.58f, 0.20f);
const glm::vec3 C_LAMP(0.32f, 0.32f, 0.34f);
const glm::vec3 C_TIRE(0.12f, 0.12f, 0.12f);
const glm::vec3 C_BARRIER_R(0.90f, 0.15f, 0.10f);
// Interior
const glm::vec3 C_FLOOR(0.82f, 0.78f, 0.72f); // mall floor
const glm::vec3 C_CEILING(0.93f, 0.91f, 0.89f);
const glm::vec3 C_CORR(0.85f, 0.83f, 0.78f); // corridor wall
// Fashion shop - PINK walls (from reference)
const glm::vec3 C_FASH_WALL(0.95f, 0.68f, 0.72f);
const glm::vec3 C_FASH_FLOOR(0.78f, 0.65f, 0.55f);
const glm::vec3 C_RACK(0.70f, 0.70f, 0.72f); // clothing rack metal
const glm::vec3 C_MIRROR(0.75f, 0.88f, 0.95f);
const glm::vec3 C_MANNEQUIN(0.85f, 0.78f, 0.65f);
const glm::vec3 C_CASH_DESK(0.60f, 0.45f, 0.30f);
// Tech shop - BLUE-GREY walls (from reference)
const glm::vec3 C_TECH_WALL(0.68f, 0.75f, 0.85f);
const glm::vec3 C_TECH_FLOOR(0.65f, 0.65f, 0.68f);
const glm::vec3 C_DISP_TABLE(0.72f, 0.60f, 0.45f); // display table wood
const glm::vec3 C_DISP_BASE(0.50f, 0.50f, 0.52f);  // table base grey
const glm::vec3 C_PHONE(0.15f, 0.15f, 0.18f);      // phone/device
const glm::vec3 C_SCREEN(0.25f, 0.62f, 0.45f);     // screen green-ish
const glm::vec3 C_LAPTOP(0.55f, 0.55f, 0.58f);
// Gems shop
const glm::vec3 C_GEM_WALL(0.92f, 0.85f, 0.65f);
const glm::vec3 C_GEM_CASE(0.70f, 0.68f, 0.65f);
// Book shop
const glm::vec3 C_BOOK_WALL(0.72f, 0.58f, 0.42f);
const glm::vec3 C_BOOKSHELF(0.50f, 0.35f, 0.22f);
// Food court (from reference - colorful stalls)
const glm::vec3 C_FOOD_FLOOR(0.72f, 0.70f, 0.66f);
const glm::vec3 C_STALL_RED(0.88f, 0.22f, 0.15f);
const glm::vec3 C_STALL_YEL(0.92f, 0.82f, 0.18f);
const glm::vec3 C_STALL_GRN(0.20f, 0.72f, 0.30f);
const glm::vec3 C_STALL_ORG(0.95f, 0.55f, 0.12f);
const glm::vec3 C_STALL_PUR(0.60f, 0.30f, 0.70f);
const glm::vec3 C_TABLE(0.62f, 0.48f, 0.30f);
const glm::vec3 C_CHAIR(0.55f, 0.40f, 0.25f);
const glm::vec3 C_COUNTER(0.65f, 0.60f, 0.55f);
// Prayer room (from reference - calm beige/cream)
const glm::vec3 C_PRAY_WALL(0.90f, 0.87f, 0.80f);
const glm::vec3 C_PRAY_FLOOR(0.78f, 0.68f, 0.55f);
const glm::vec3 C_PRAY_MAT(0.55f, 0.72f, 0.58f);
const glm::vec3 C_PRAY_LAMP(0.92f, 0.88f, 0.75f);
const glm::vec3 C_PRAY_SHELF(0.65f, 0.55f, 0.42f);
// Washroom / Staircase
const glm::vec3 C_WASH(0.88f, 0.88f, 0.86f);
const glm::vec3 C_STAIR(0.58f, 0.56f, 0.52f);
const glm::vec3 C_PARAPET(0.72f, 0.70f, 0.66f);

//  Lights
const int NUM_POINT_LIGHTS = 14;
glm::vec3 plPos[NUM_POINT_LIGHTS] = {
    // N-S corridor (pathway)
    {0, 4.5f, 22},
    {0, 4.5f, 5},
    {0, 4.5f, -10},
    // E-W corridor
    {-12, 4.5f, 13},
    {12, 4.5f, 13},
    // One light per shop
    {-18, 4.5f, 20},
    {-8, 4.5f, 20},
    {9, 4.5f, 20},
    {20, 4.5f, 20},
    // Food court
    {-14, 4.5f, -10},
    // Prayer room
    {9, 4.5f, -15},
    // Washroom
    {20, 4.5f, -11},
    // Parking lot
    {-10, 6.2f, 42},
    {10, 6.2f, 42}};
PointLight *pointLights[NUM_POINT_LIGHTS];
DirectionalLight dirLight(
    glm::vec3(-10, 6.2f, 42), glm::vec3(0, -1, 0),
    glm::vec3(0.08f, 0.07f, 0.03f), glm::vec3(0.9f, 0.75f, 0.4f),
    glm::vec3(0.7f, 0.55f, 0.3f), 35.f);
DirectionalLight dirLight2(
    glm::vec3(10, 6.2f, 42), glm::vec3(0, -1, 0),
    glm::vec3(0.08f, 0.07f, 0.03f), glm::vec3(0.9f, 0.75f, 0.4f),
    glm::vec3(0.7f, 0.55f, 0.3f), 35.f);
int activeViewport = 0;

// Toggle flags
bool texturesEnabled = true;
bool ambientEnabled = true;
bool pointLightsEnabled = true;
bool dirLightEnabled = true;
bool spotLightsEnabled = true;

// Day/Night cycle
float timeOfDay = 10.0f;  // 0-24 hours, start at 10 AM
float dayNightSpeed = 0.0f; // hours per second (0 = paused, press N to cycle)
bool dayNightCycleActive = false;

// Spot Lights (store display highlights)
const int NUM_SPOT_LIGHTS = 4;
SpotLight* spotLights_arr[NUM_SPOT_LIGHTS] = {};

// Elevator state machine
enum ElevatorState { ELEV_STOPPED_BOTTOM, ELEV_DOOR_CLOSING_UP, ELEV_MOVING_UP, ELEV_STOPPED_TOP, ELEV_DOOR_CLOSING_DOWN, ELEV_MOVING_DOWN, ELEV_DOOR_OPENING };
ElevatorState elevatorState = ELEV_STOPPED_BOTTOM;
float elevatorY = 0.0f;
float elevatorDoorOffset = 0.0f; // 0 = closed, 1 = open
float elevatorTimer = 0.0f;

// Escalator
float escalatorOffset = 0.0f;
int escalatorDir = 1; // 1=up, -1=down, 0=paused

// Boom barrier
float barrierAngle = 0.0f; // 0=closed, 90=open
bool barrierOpen = false;

// Wheel rotation for cars
float wheelRotation = 0.0f;

// Cursor capture toggle
bool cursorCaptured = true;

// Entrance glass door
bool entranceDoorOpen = false;
float entranceDoorOffset = 0.0f; // 0=closed, 1=open

// Texture IDs
unsigned int texCubeVAO = 0;
unsigned int texFloor = 0, texFashion = 0, texTech = 0, texGems = 0, texFood = 0, texTreeLeaf = 0, texTreeBark = 0, texGrass = 0;

// Spheres
Sphere *sphWheel = nullptr;
Sphere *sphHub = nullptr;
Sphere *sphLeaf[5] = {};

// Pyramid
Pyramid *treeCone = nullptr;

// Fractal Trees
const int NUM_TREES = 10;
FractalTree* fractalTrees[NUM_TREES] = {};

// Bezier furniture objects
BezierRevolvedSurface* bezierTable = nullptr;
BezierArch* bezierArch = nullptr;

// MAIN
int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "3D Shopping Mall", NULL, NULL);
    if (!window)
    {
        cout << "Window failed" << endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetKeyCallback(window, key_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        cout << "GLAD failed" << endl;
        return -1;
    }
    glEnable(GL_DEPTH_TEST);

    cout << "  3D MODERN SHOPPING MALL SIMULATION" << endl;
    cout << "  W/A/S/D - Move | Mouse - Look | ESC - Exit" << endl;
    cout << "  C - Toggle Cursor (Mouse Lock)" << endl;
    cout << "  F1-F4 - Fullscreen viewport toggle" << endl;
    cout << "  1 - Toggle Ambient Light" << endl;
    cout << "  2 - Toggle Point Lights" << endl;
    cout << "  3 - Toggle Directional Light (Lamppost)" << endl;
    cout << "  5 - Toggle Spot Lights (Store displays)" << endl;
    cout << "  T - Toggle Textures" << endl;
    cout << "  N - Toggle Day/Night Cycle" << endl;
    cout << "  E - Elevator Up/Down" << endl;
    cout << "  R - Escalator Direction (Up/Down/Pause)" << endl;
    cout << "  B - Boom Barrier Open/Close" << endl;
    cout << "  G - Open/Close Entrance Door" << endl;

    Shader ls("vertexShaderForPhongShading.vs", "fragmentShaderForPhongShading.fs");
    Shader fs("vertexShader.vs", "fragmentShader.fs");

    float cv[] = {
        -.5f, -.5f, -.5f, 0, 0, -1, .5f, -.5f, -.5f, 0, 0, -1, .5f, .5f, -.5f, 0, 0, -1, -.5f, .5f, -.5f, 0, 0, -1,
        .5f, -.5f, -.5f, 1, 0, 0, .5f, .5f, -.5f, 1, 0, 0, .5f, -.5f, .5f, 1, 0, 0, .5f, .5f, .5f, 1, 0, 0,
        -.5f, -.5f, .5f, 0, 0, 1, .5f, -.5f, .5f, 0, 0, 1, .5f, .5f, .5f, 0, 0, 1, -.5f, .5f, .5f, 0, 0, 1,
        -.5f, -.5f, .5f, -1, 0, 0, -.5f, .5f, .5f, -1, 0, 0, -.5f, .5f, -.5f, -1, 0, 0, -.5f, -.5f, -.5f, -1, 0, 0,
        .5f, .5f, .5f, 0, 1, 0, .5f, .5f, -.5f, 0, 1, 0, -.5f, .5f, -.5f, 0, 1, 0, -.5f, .5f, .5f, 0, 1, 0,
        -.5f, -.5f, -.5f, 0, -1, 0, .5f, -.5f, -.5f, 0, -1, 0, .5f, -.5f, .5f, 0, -1, 0, -.5f, -.5f, .5f, 0, -1, 0};
    unsigned int ci[] = {0, 3, 2, 2, 1, 0, 4, 5, 7, 7, 6, 4, 8, 9, 10, 10, 11, 8, 12, 13, 14, 14, 15, 12, 16, 17, 18, 18, 19, 16, 20, 21, 22, 22, 23, 20};
    unsigned int cubeVAO, cubeVBO, cubeEBO;
    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &cubeVBO);
    glGenBuffers(1, &cubeEBO);
    glBindVertexArray(cubeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cv), cv, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cubeEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(ci), ci, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 24, (void *)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 24, (void *)12);
    glEnableVertexAttribArray(1);
    unsigned int lightVAO;
    glGenVertexArrays(1, &lightVAO);
    glBindVertexArray(lightVAO);
    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cubeEBO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 24, (void *)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 24, (void *)12);
    glEnableVertexAttribArray(1);

    // Textured Cube VAO
    float tcv[] = {
        -.5f, -.5f, -.5f, 0, 0, -1, 0, 0, .5f, -.5f, -.5f, 0, 0, -1, 1, 0, .5f, .5f, -.5f, 0, 0, -1, 1, 1, -.5f, .5f, -.5f, 0, 0, -1, 0, 1,
        .5f, -.5f, -.5f, 1, 0, 0, 0, 0, .5f, .5f, -.5f, 1, 0, 0, 0, 1, .5f, -.5f, .5f, 1, 0, 0, 1, 0, .5f, .5f, .5f, 1, 0, 0, 1, 1,
        -.5f, -.5f, .5f, 0, 0, 1, 0, 0, .5f, -.5f, .5f, 0, 0, 1, 1, 0, .5f, .5f, .5f, 0, 0, 1, 1, 1, -.5f, .5f, .5f, 0, 0, 1, 0, 1,
        -.5f, -.5f, .5f, -1, 0, 0, 0, 0, -.5f, .5f, .5f, -1, 0, 0, 0, 1, -.5f, .5f, -.5f, -1, 0, 0, 1, 1, -.5f, -.5f, -.5f, -1, 0, 0, 1, 0,
        .5f, .5f, .5f, 0, 1, 0, 1, 1, .5f, .5f, -.5f, 0, 1, 0, 1, 0, -.5f, .5f, -.5f, 0, 1, 0, 0, 0, -.5f, .5f, .5f, 0, 1, 0, 0, 1,
        -.5f, -.5f, -.5f, 0, -1, 0, 0, 0, .5f, -.5f, -.5f, 0, -1, 0, 1, 0, .5f, -.5f, .5f, 0, -1, 0, 1, 1, -.5f, -.5f, .5f, 0, -1, 0, 0, 1};
    unsigned int tcVBO, tcEBO;
    glGenVertexArrays(1, &texCubeVAO);
    glGenBuffers(1, &tcVBO);
    glGenBuffers(1, &tcEBO);
    glBindVertexArray(texCubeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, tcVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(tcv), tcv, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, tcEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(ci), ci, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 32, (void *)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 32, (void *)12);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 32, (void *)24);
    glEnableVertexAttribArray(2);
    glBindVertexArray(0);

    // Load Textures
    stbi_set_flip_vertically_on_load(true);
    texFloor = loadTexture("floor_tiles_texture.jpg");
    texFashion = loadTexture("fashion_shop.png");
    texTech = loadTexture("tech_shop.png");
    texGems = loadTexture("gems_shop.jpg");
    texFood = loadTexture("food_court.png");
    texTreeLeaf = loadTexture("tree_leaves_texture.png");
    texTreeBark = loadTexture("tree_texture.jpg");
    texGrass = loadTexture("grass.jpg");

    for (int i = 0; i < NUM_POINT_LIGHTS; i++)
    {
        glm::vec3 a(0.1f), d(0.6f, 0.58f, 0.55f), sp(0.5f);
        pointLights[i] = new PointLight(plPos[i].x, plPos[i].y, plPos[i].z, a.x, a.y, a.z, d.x, d.y, d.z, sp.x, sp.y, sp.z, 1, .09f, .032f, i);
    }

    // Create Spot Lights (store display highlights)
    // Tech store - 2 spots aimed at display tables
    spotLights_arr[0] = new SpotLight(
        glm::vec3(-10.5f, 4.5f, 19), glm::vec3(0, -1, 0),
        glm::vec3(0.05f), glm::vec3(0.9f, 0.85f, 0.8f), glm::vec3(0.8f),
        15.0f, 25.0f, 1.0f, 0.09f, 0.032f, 0);
    spotLights_arr[1] = new SpotLight(
        glm::vec3(-5.5f, 4.5f, 22), glm::vec3(0, -1, 0),
        glm::vec3(0.05f), glm::vec3(0.9f, 0.85f, 0.8f), glm::vec3(0.8f),
        15.0f, 25.0f, 1.0f, 0.09f, 0.032f, 1);
    // Gems store - spot on display cases
    spotLights_arr[2] = new SpotLight(
        glm::vec3(9, 4.5f, 20), glm::vec3(0, -1, 0),
        glm::vec3(0.05f), glm::vec3(1.0f, 0.95f, 0.7f), glm::vec3(0.9f),
        12.0f, 22.0f, 1.0f, 0.09f, 0.032f, 2);
    // Bookstore reading area
    spotLights_arr[3] = new SpotLight(
        glm::vec3(20, 4.5f, 23), glm::vec3(0, -1, 0.1f),
        glm::vec3(0.04f), glm::vec3(0.85f, 0.8f, 0.65f), glm::vec3(0.5f),
        18.0f, 28.0f, 1.0f, 0.09f, 0.032f, 3);

    // Create sphere objects for wheels and leaves
    sphWheel = new Sphere(1.0f, 18, 9, glm::vec3(.12f), glm::vec3(.12f), glm::vec3(.2f), 16);
    sphHub = new Sphere(1.0f, 12, 6, glm::vec3(.7f), glm::vec3(.72f), glm::vec3(.9f), 64);
    glm::vec3 leafG[] = {{.22f, .55f, .18f}, {.18f, .48f, .15f}, {.25f, .58f, .20f}, {.20f, .50f, .16f}, {.15f, .45f, .14f}};
    for (int i = 0; i < 5; i++)
        sphLeaf[i] = new Sphere(1.0f, 12, 8, leafG[i], leafG[i], glm::vec3(.15f), 8);

    // Create tree cone pyramid
    treeCone = new Pyramid(texTreeLeaf, glm::vec3(.22f, .55f, .18f), glm::vec3(.22f, .55f, .18f), glm::vec3(.1f), 8.f);

    // Create Fractal Trees for outdoor planters
    glm::vec3 treePositions[] = {{-20, 0, 28}, {22, 0, 28}, {-36, 0, 15}, {-36, 0, -10}, {36, 0, 12}, {36, 0, -15}, {-16, 0, 62}, {16, 0, 62}, {-45, 0, 40}, {45, 0, -18}};
    for (int i = 0; i < NUM_TREES; i++) {
        fractalTrees[i] = new FractalTree();
        // Vary tree parameters slightly for natural look
        float height = 3.5f + (i % 3) * 0.5f;
        float radius = 0.2f + (i % 2) * 0.05f;
        int depth = 3 + (i % 2); // depth 3 or 4
        float angle = 28.0f + (i % 3) * 4.0f;
        fractalTrees[i]->generate(glm::vec3(0), height, radius, depth, angle);
    }

    // Create Bezier rounded table top (profile: flat disk shape)
    bezierTable = new BezierRevolvedSurface();
    {
        std::vector<glm::vec3> tableProfile = {
            {0.0f, 0.0f, 0.0f},   // center
            {0.3f, 0.02f, 0.0f},  // slight curve up
            {0.5f, 0.03f, 0.0f},  // mid
            {0.55f, 0.0f, 0.0f}   // edge curves down
        };
        bezierTable->generate(tableProfile, 12, 16);
    }

    // Create Bezier arch for doorways
    bezierArch = new BezierArch();
    {
        std::vector<glm::vec3> archProfile = {
            {-1.5f, 0.0f, 0.0f},  // left base
            {-1.5f, 3.5f, 0.0f},  // left rise
            {0.0f, 4.5f, 0.0f},   // top peak
            {1.5f, 3.5f, 0.0f},   // right rise
            {1.5f, 0.0f, 0.0f}    // right base
        };
        bezierArch->generate(archProfile, 20, 0.3f);
    }

    while (!glfwWindowShouldClose(window))
    {
        float t = (float)glfwGetTime();
        deltaTime = t - lastFrame;
        lastFrame = t;
        processInput(window);

        // ---- Day/Night Cycle Update ----
        if (dayNightCycleActive) {
            timeOfDay += dayNightSpeed * deltaTime;
            if (timeOfDay >= 24.0f) timeOfDay -= 24.0f;
            if (timeOfDay < 0.0f) timeOfDay += 24.0f;
        }
        // Compute dayNightMix: 0 = full day (noon), 1 = full night (midnight)
        // Smooth transition: day 6-18, night 20-4, transitions at 4-6 and 18-20
        float dnMix = 0.0f;
        if (timeOfDay >= 6.0f && timeOfDay < 18.0f) {
            dnMix = 0.0f; // full day
        } else if (timeOfDay >= 20.0f || timeOfDay < 4.0f) {
            dnMix = 1.0f; // full night
        } else if (timeOfDay >= 18.0f && timeOfDay < 20.0f) {
            dnMix = (timeOfDay - 18.0f) / 2.0f; // dusk transition
        } else { // 4 to 6
            dnMix = 1.0f - (timeOfDay - 4.0f) / 2.0f; // dawn transition
        }

        // Sky color interpolation
        glm::vec3 daySky(0.53f, 0.81f, 0.98f);
        glm::vec3 nightSky(0.02f, 0.02f, 0.08f);
        glm::vec3 skyColor = glm::mix(daySky, nightSky, dnMix);
        glClearColor(skyColor.r, skyColor.g, skyColor.b, 1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // ---- Animation Updates ----
        wheelRotation += deltaTime * 45.0f; // slow spin
        if (wheelRotation > 360.0f) wheelRotation -= 360.0f;

        // Escalator step movement
        escalatorOffset += escalatorDir * deltaTime * 2.0f;
        if (escalatorOffset > 10.0f) escalatorOffset -= 10.0f;
        if (escalatorOffset < 0.0f) escalatorOffset += 10.0f;

        // Boom barrier animation
        float targetAngle = barrierOpen ? 90.0f : 0.0f;
        if (barrierAngle < targetAngle) barrierAngle = glm::min(barrierAngle + deltaTime * 90.0f, targetAngle);
        if (barrierAngle > targetAngle) barrierAngle = glm::max(barrierAngle - deltaTime * 90.0f, targetAngle);

        // Entrance door animation
        float doorTarget = entranceDoorOpen ? 1.0f : 0.0f;
        if (entranceDoorOffset < doorTarget) entranceDoorOffset = glm::min(entranceDoorOffset + deltaTime * 1.2f, doorTarget);
        if (entranceDoorOffset > doorTarget) entranceDoorOffset = glm::max(entranceDoorOffset - deltaTime * 1.2f, doorTarget);

        // Elevator state machine update
        elevatorTimer += deltaTime;
        switch (elevatorState) {
        case ELEV_STOPPED_BOTTOM:
        case ELEV_STOPPED_TOP:
            break; // waiting for key press
        case ELEV_DOOR_CLOSING_UP:
        case ELEV_DOOR_CLOSING_DOWN:
            elevatorDoorOffset = glm::max(elevatorDoorOffset - deltaTime * 1.5f, 0.0f);
            if (elevatorDoorOffset <= 0.0f) {
                elevatorState = (elevatorState == ELEV_DOOR_CLOSING_UP) ? ELEV_MOVING_UP : ELEV_MOVING_DOWN;
                elevatorTimer = 0.0f;
            }
            break;
        case ELEV_MOVING_UP:
            elevatorY = glm::min(elevatorY + deltaTime * 1.5f, MH);
            if (elevatorY >= MH) {
                elevatorState = ELEV_STOPPED_TOP;
                elevatorDoorOffset = 0.0f;
                elevatorTimer = 0.0f;
                // Auto-open door at top
                elevatorState = ELEV_DOOR_OPENING;
            }
            break;
        case ELEV_MOVING_DOWN:
            elevatorY = glm::max(elevatorY - deltaTime * 1.5f, 0.0f);
            if (elevatorY <= 0.0f) {
                elevatorState = ELEV_STOPPED_BOTTOM;
                elevatorDoorOffset = 0.0f;
                elevatorTimer = 0.0f;
                elevatorState = ELEV_DOOR_OPENING;
            }
            break;
        case ELEV_DOOR_OPENING:
            elevatorDoorOffset = glm::min(elevatorDoorOffset + deltaTime * 1.5f, 1.0f);
            if (elevatorDoorOffset >= 1.0f) {
                elevatorState = (elevatorY >= MH) ? ELEV_STOPPED_TOP : ELEV_STOPPED_BOTTOM;
            }
            break;
        }

        // ---- Shader Setup ----
        ls.use();
        ls.setVec3("viewPos", basic_camera.eye);
        ls.setVec3("globalAmbient", ambientEnabled ? glm::vec3(0.25f, 0.25f, 0.30f) : glm::vec3(0));
        ls.setFloat("dayNightMix", dnMix);
        ls.setInt("numPointLights", pointLightsEnabled ? NUM_POINT_LIGHTS : 0);
        if (pointLightsEnabled)
        {
            for (int i = 0; i < NUM_POINT_LIGHTS; i++)
                pointLights[i]->setUpPointLight(ls);
        }

        // Sun directional light interpolation (warm day → cool night)
        glm::vec3 sunDiffDay(0.9f, 0.75f, 0.4f);
        glm::vec3 sunDiffNight(0.05f, 0.05f, 0.12f);
        glm::vec3 sunSpecDay(0.7f, 0.55f, 0.3f);
        glm::vec3 sunSpecNight(0.02f, 0.02f, 0.05f);
        dirLight.diffuse = glm::mix(sunDiffDay, sunDiffNight, dnMix);
        dirLight.specular = glm::mix(sunSpecDay, sunSpecNight, dnMix);
        dirLight2.diffuse = dirLight.diffuse;
        dirLight2.specular = dirLight.specular;

        if (dirLightEnabled)
            dirLight.turnOn();
        else
            dirLight.turnOff();
        dirLight.setUpDirectionalLight(ls);
        if (dirLightEnabled)
            dirLight2.turnOn();
        else
            dirLight2.turnOff();
        dirLight2.setUpDirectionalLight(ls, "dirLight2");

        // Spot lights
        ls.setInt("numSpotLights", spotLightsEnabled ? NUM_SPOT_LIGHTS : 0);
        if (spotLightsEnabled) {
            for (int i = 0; i < NUM_SPOT_LIGHTS; i++)
                spotLights_arr[i]->setUpSpotLight(ls);
        }

        ls.setBool("useTexture", false);
        ls.setBool("blendWithColor", false);
        ls.setFloat("texTiling", 1.0f);

        int w = SCR_WIDTH, h = SCR_HEIGHT, hw = w / 2, hh = h / 2;
        float os = 40.f;
        glm::mat4 oP = glm::ortho(-os, os, -os * ((float)hh / hw), os * ((float)hh / hw), 0.1f, 300.f);
        glm::mat4 pP = glm::perspective(glm::radians(basic_camera.Zoom), (float)hw / (float)hh, 0.1f, 300.f);
        glm::vec3 gc(0, MH * .5f, 15);
        glm::mat4 vF = glm::lookAt(glm::vec3(0, MH * .5f, 90), gc, {0, 1, 0});
        glm::mat4 vS = glm::lookAt(glm::vec3(90, MH * .5f, 15), gc, {0, 1, 0});
        glm::mat4 vT = glm::lookAt(glm::vec3(0, 90, 15.01f), gc, {0, 0, -1});
        glm::mat4 vP = basic_camera.createViewMatrix();

        glEnable(GL_SCISSOR_TEST);
        if (activeViewport == 0)
        {
            glViewport(0, hh, hw, hh);
            glScissor(0, hh, hw, hh);
            glClear(GL_DEPTH_BUFFER_BIT);
            drawScene(cubeVAO, lightVAO, ls, fs, vF, oP);
            glViewport(hw, hh, hw, hh);
            glScissor(hw, hh, hw, hh);
            glClear(GL_DEPTH_BUFFER_BIT);
            drawScene(cubeVAO, lightVAO, ls, fs, vS, oP);
            glViewport(0, 0, hw, hh);
            glScissor(0, 0, hw, hh);
            glClear(GL_DEPTH_BUFFER_BIT);
            drawScene(cubeVAO, lightVAO, ls, fs, vT, oP);
            glViewport(hw, 0, hw, hh);
            glScissor(hw, 0, hw, hh);
            glClear(GL_DEPTH_BUFFER_BIT);
            drawScene(cubeVAO, lightVAO, ls, fs, vP, pP);
            glDisable(GL_SCISSOR_TEST);
            fs.use();
            fs.setMat4("projection", glm::ortho(0.f, (float)w, 0.f, (float)h, -1.f, 1.f));
            fs.setMat4("view", glm::mat4(1));
            fs.setVec3("color", glm::vec3(0.3f));
            glViewport(0, 0, w, h);
            glBindVertexArray(lightVAO);
            glm::mat4 m = glm::translate(glm::mat4(1), {w * .5f, (float)hh, 0});
            m = glm::scale(m, {(float)w, 2, 1});
            fs.setMat4("model", m);
            glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
            m = glm::translate(glm::mat4(1), {(float)hw, h * .5f, 0});
            m = glm::scale(m, {2, (float)h, 1});
            fs.setMat4("model", m);
            glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
        }
        else
        {
            float fa = (float)w / h;
            glm::mat4 sv, sp;
            glm::mat4 fO = glm::ortho(-os, os, -os / fa, os / fa, .1f, 300.f);
            glm::mat4 fP = glm::perspective(glm::radians(basic_camera.Zoom), fa, .1f, 300.f);
            switch (activeViewport)
            {
            case 1:
                sv = vF;
                sp = fO;
                break;
            case 2:
                sv = vS;
                sp = fO;
                break;
            case 3:
                sv = vT;
                sp = fO;
                break;
            default:
                sv = vP;
                sp = fP;
            }
            glViewport(0, 0, w, h);
            glScissor(0, 0, w, h);
            glClear(GL_DEPTH_BUFFER_BIT);
            drawScene(cubeVAO, lightVAO, ls, fs, sv, sp);
            glDisable(GL_SCISSOR_TEST);
        }
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    for (int i = 0; i < NUM_POINT_LIGHTS; i++)
        delete pointLights[i];
    for (int i = 0; i < NUM_SPOT_LIGHTS; i++)
        delete spotLights_arr[i];
    delete sphWheel;
    delete sphHub;
    for (int i = 0; i < 5; i++)
        delete sphLeaf[i];
    delete treeCone;
    glDeleteVertexArrays(1, &cubeVAO);
    glDeleteVertexArrays(1, &lightVAO);
    glDeleteVertexArrays(1, &texCubeVAO);
    glDeleteBuffers(1, &cubeVBO);
    glDeleteBuffers(1, &cubeEBO);
    glfwTerminate();
    return 0;
}

// DRAW CUBE
void drawCube(unsigned int &v, Shader &s, glm::mat4 pm, glm::vec3 c, glm::vec3 p, glm::vec3 sc, float sh, float a)
{
    s.use();
    glm::mat4 m = glm::translate(pm, p);
    m = glm::scale(m, sc);
    s.setVec3("material.ambient", c * a);
    s.setVec3("material.diffuse", c);
    s.setVec3("material.specular", glm::vec3(0.3f));
    s.setFloat("material.shininess", sh);
    s.setBool("useTexture", false);
    s.setMat4("model", m);
    glBindVertexArray(v);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
}

// LOAD TEXTURE
unsigned int loadTexture(const char *path)
{
    unsigned int id;
    glGenTextures(1, &id);
    int w, h, ch;
    unsigned char *data = stbi_load(path, &w, &h, &ch, 0);
    if (data)
    {
        GLenum fmt = ch == 4 ? GL_RGBA : GL_RGB;
        glBindTexture(GL_TEXTURE_2D, id);
        glTexImage2D(GL_TEXTURE_2D, 0, fmt, w, h, 0, fmt, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }
    else
    {
        cout << "Failed to load texture: " << path << endl;
    }
    stbi_image_free(data);
    return id;
}

// DRAW CUBE TEXTURED
void drawCubeTextured(unsigned int &v, Shader &s, glm::mat4 pm, glm::vec3 c, glm::vec3 p, glm::vec3 sc, unsigned int tex, float tiling, float sh, float a)
{
    s.use();
    glm::mat4 m = glm::translate(pm, p);
    m = glm::scale(m, sc);
    s.setVec3("material.ambient", c * a);
    s.setVec3("material.diffuse", c);
    s.setVec3("material.specular", glm::vec3(0.3f));
    s.setFloat("material.shininess", sh);
    if (texturesEnabled && tex != 0)
    {
        s.setBool("useTexture", true);
        s.setFloat("texTiling", tiling);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, tex);
        s.setInt("texture1", 0);
    }
    else
    {
        s.setBool("useTexture", false);
    }
    s.setMat4("model", m);
    glBindVertexArray(v);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    s.setBool("useTexture", false);
}

// DRAW SCENE

void drawScene(unsigned int &V, unsigned int &LV, Shader &ls, Shader &fs, glm::mat4 view, glm::mat4 proj)
{
    ls.use();
    ls.setMat4("view", view);
    ls.setMat4("projection", proj);
    ls.setVec3("viewPos", basic_camera.eye);
    glm::mat4 I(1);
    float WH = MH / 2;

    // OUTDOOR ENVIRONMENT

    // Grass ground (blended texture with surface color)
    ls.setBool("blendWithColor", true);
    drawCubeTextured(texCubeVAO, ls, I, C_GRASS, {0, -.1f, 15}, {200, .12f, 160}, texGrass, 12, 8);
    drawCubeTextured(texCubeVAO, ls, I, C_GRASS2, {-40, -.08f, 20}, {25, .1f, 20}, texGrass, 4, 8);
    drawCubeTextured(texCubeVAO, ls, I, C_GRASS2, {42, -.08f, -5}, {18, .1f, 14}, texGrass, 4, 8);
    ls.setBool("blendWithColor", false);

    // Main E-W road south of parking
    drawCube(V, ls, I, C_ROAD, {0, -.04f, 60}, {130, .06f, 8});
    for (int i = 0; i < 18; i++)
        drawCube(V, ls, I, C_ROAD_YEL, {-64 + i * 7.5f, -.01f, 60}, {4.5f, .02f, .12f});
    drawCube(V, ls, I, C_SIDEWALK, {0, .05f, 65}, {130, .12f, 2});
    drawCube(V, ls, I, C_SIDEWALK, {0, .05f, 55}, {130, .12f, 2});
    // Access road into parking
    drawCube(V, ls, I, C_ROAD, {0, -.04f, 52}, {8, .06f, 16});
    // West side road
    drawCube(V, ls, I, C_ROAD, {-30, -.04f, 5}, {6, .06f, 60});
    drawCube(V, ls, I, C_SIDEWALK, {-26.5f, .05f, 5}, {1.5f, .12f, 60});

    // FRACTAL TREES (procedurally generated, replacing old pyramid cones)
    {
        glm::vec3 treeP[] = {{-20, 0, 28}, {22, 0, 28}, {-36, 0, 15}, {-36, 0, -10}, {36, 0, 12}, {36, 0, -15}, {-16, 0, 62}, {16, 0, 62}, {-45, 0, 40}, {45, 0, -18}};
        for (int ti = 0; ti < NUM_TREES; ti++) {
            glm::mat4 treeModel = glm::translate(I, treeP[ti]);
            fractalTrees[ti]->drawBranches(ls, treeModel, texTreeBark, texturesEnabled);
            fractalTrees[ti]->drawLeaves(ls, treeModel, texTreeLeaf, texturesEnabled);
        }
    }

    // PARKING LOT (Z: 27 to 55)

    drawCube(V, ls, I, C_PARKING, {0, .01f, 41}, {50, .05f, 28});
    // Parking lines
    for (int i = 0; i < 9; i++)
    {
        float z = 28 + i * 3.f;
        drawCube(V, ls, I, C_PARK_LINE, {-15, .04f, z}, {.1f, .02f, 2.5f});
        drawCube(V, ls, I, C_PARK_LINE, {15, .04f, z}, {.1f, .02f, 2.5f});
    }
    drawCube(V, ls, I, C_PARK_LINE, {-4, .04f, 41}, {.1f, .02f, 26});
    drawCube(V, ls, I, C_PARK_LINE, {4, .04f, 41}, {.1f, .02f, 26});
    // Arrows
    drawCube(V, ls, I, C_ROAD_WHT, {0, .04f, 36}, {.15f, .02f, 2});
    drawCube(V, ls, I, C_ROAD_WHT, {-.5f, .04f, 37}, {1, .02f, .15f});
    // Cars (detailed low-poly sedan)
    auto drawCar = [&](glm::vec3 bc, glm::vec3 p, float ry)
    {
        glm::mat4 cb = glm::translate(I, p);
        if (ry != 0)
            cb = glm::rotate(cb, glm::radians(ry), {0, 1, 0});
        glm::vec3 dk = bc * 0.75f;                         // darker shade
        glm::vec3 lt = glm::min(bc * 1.15f, glm::vec3(1)); // lighter shade
        // --- Lower body (wider at bottom, slightly rounded look via layered cubes) ---
        drawCube(V, ls, cb, bc, {0, .35f, 0}, {2.2f, .5f, 4.4f}, 64, .85f);        // base slab
        drawCube(V, ls, cb, bc * 1.02f, {0, .55f, 0}, {2.1f, .2f, 4.3f}, 64, .85f); // upper body strip
        // Front bumper (rounded profile: two layers)
        drawCube(V, ls, cb, dk, {0, .28f, 2.25f}, {2.1f, .3f, .35f}, 64, .7f);
        drawCube(V, ls, cb, dk * .95f, {0, .22f, 2.32f}, {1.8f, .18f, .2f}, 64, .7f); // lower lip
        // Rear bumper
        drawCube(V, ls, cb, dk, {0, .28f, -2.25f}, {2.1f, .3f, .35f}, 64, .7f);
        drawCube(V, ls, cb, dk * .95f, {0, .22f, -2.32f}, {1.8f, .18f, .2f}, 64, .7f);
        // Hood (sloped via angled cube layers)
        drawCube(V, ls, cb, lt, {0, .72f, 1.5f}, {1.95f, .12f, 1.2f}, 64, .9f);       // flat top
        drawCube(V, ls, cb, lt * .97f, {0, .66f, 1.85f}, {1.85f, .08f, .5f}, 64, .9f); // front slope
        // Trunk (slightly lower than hood)
        drawCube(V, ls, cb, lt, {0, .68f, -1.6f}, {1.9f, .1f, 1.0f}, 64, .9f);
        // Cabin (narrower than body = trapezoidal feel)
        drawCube(V, ls, cb, bc * .95f, {0, 1.0f, -.2f}, {1.6f, .55f, 2.0f}, 64, .85f);
        // Roof (even narrower)
        drawCube(V, ls, cb, lt, {0, 1.3f, -.2f}, {1.5f, .06f, 1.7f}, 64, .9f);
        // A-pillar / windshield (angled look via thin tilted cubes)
        drawCube(V, ls, cb, C_GLASS, {0, 1.0f, .85f}, {1.42f, .48f, .08f}, 128, .25f);
        drawCube(V, ls, cb, dk * .6f, {-.72f, 1.0f, .75f}, {.04f, .48f, .28f}, 64, .7f); // A-pillar L
        drawCube(V, ls, cb, dk * .6f, {.72f, 1.0f, .75f}, {.04f, .48f, .28f}, 64, .7f);  // A-pillar R
        // Rear window
        drawCube(V, ls, cb, C_GLASS, {0, 1.0f, -1.25f}, {1.4f, .42f, .08f}, 128, .25f);
        // Side windows left
        drawCube(V, ls, cb, C_GLASS, {-.81f, 1.0f, -.2f}, {.06f, .42f, 1.6f}, 128, .25f);
        // Side windows right
        drawCube(V, ls, cb, C_GLASS, {.81f, 1.0f, -.2f}, {.06f, .42f, 1.6f}, 128, .25f);
        // C-pillar (rear side pillars for shape)
        drawCube(V, ls, cb, dk * .7f, {-.72f, 1.0f, -1.15f}, {.04f, .48f, .25f}, 64, .7f);
        drawCube(V, ls, cb, dk * .7f, {.72f, 1.0f, -1.15f}, {.04f, .48f, .25f}, 64, .7f);
        // Wheel wells (curved arches via layered cubes)
        drawCube(V, ls, cb, dk * .7f, {-.98f, .32f, 1.3f}, {.28f, .45f, .7f}, 32, .7f);
        drawCube(V, ls, cb, dk * .7f, {.98f, .32f, 1.3f}, {.28f, .45f, .7f}, 32, .7f);
        drawCube(V, ls, cb, dk * .7f, {-.98f, .32f, -1.3f}, {.28f, .45f, .7f}, 32, .7f);
        drawCube(V, ls, cb, dk * .7f, {.98f, .32f, -1.3f}, {.28f, .45f, .7f}, 32, .7f);
        // --- Wheels: flat disc-style (heavily squashed sphere = circle, NOT 3D ball) ---
        glm::vec3 wheelPos[] = {{-.98f, .22f, 1.3f}, {.98f, .22f, 1.3f}, {-.98f, .22f, -1.3f}, {.98f, .22f, -1.3f}};
        for (int wi = 0; wi < 4; wi++)
        {
            // Tire: very flat along axle (X) so it looks like a circle/disc, not a ball
            glm::mat4 wm = glm::translate(cb, wheelPos[wi]);
            wm = glm::rotate(wm, glm::radians(wheelRotation), {1, 0, 0}); // rotate around axle
            wm = glm::scale(wm, glm::vec3(.05f, .24f, .24f)); // VERY flat on X axis = disc
            sphWheel->drawSphere(ls, wm);
            // Hub: even flatter, silver disc on outer face
            glm::mat4 hm = glm::translate(cb, {wheelPos[wi].x + (wheelPos[wi].x < 0 ? -.04f : .04f), wheelPos[wi].y, wheelPos[wi].z});
            hm = glm::rotate(hm, glm::radians(wheelRotation), {1, 0, 0});
            hm = glm::scale(hm, glm::vec3(.02f, .14f, .14f)); // very flat hub
            sphHub->drawSphere(ls, hm);
        }
        // Headlights (slightly recessed look)
        drawCube(V, ls, cb, {1, .95f, .7f}, {-.65f, .48f, 2.28f}, {.38f, .2f, .06f}, 128);
        drawCube(V, ls, cb, {1, .95f, .7f}, {.65f, .48f, 2.28f}, {.38f, .2f, .06f}, 128);
        // Taillights
        drawCube(V, ls, cb, {.9f, .1f, .05f}, {-.65f, .48f, -2.28f}, {.38f, .18f, .06f}, 128);
        drawCube(V, ls, cb, {.9f, .1f, .05f}, {.65f, .48f, -2.28f}, {.38f, .18f, .06f}, 128);
        // Grille
        drawCube(V, ls, cb, dk * .5f, {0, .42f, 2.3f}, {.9f, .22f, .05f}, 32, .5f);
        drawCube(V, ls, cb, dk * .4f, {0, .42f, 2.32f}, {.7f, .15f, .02f}, 32, .4f); // inner grille
        // Side mirrors
        drawCube(V, ls, cb, bc, {-1.18f, .9f, .5f}, {.15f, .12f, .2f}, 64, .8f);
        drawCube(V, ls, cb, bc, {1.18f, .9f, .5f}, {.15f, .12f, .2f}, 64, .8f);
        // Door lines (subtle)
        drawCube(V, ls, cb, dk * .85f, {-1.11f, .45f, 0}, {.02f, .15f, 1.8f}, 32, .6f);
        drawCube(V, ls, cb, dk * .85f, {1.11f, .45f, 0}, {.02f, .15f, 1.8f}, 32, .6f);
        // Door handles
        drawCube(V, ls, cb, dk * .5f, {-1.12f, .5f, .3f}, {.03f, .04f, .2f}, 64, .5f);
        drawCube(V, ls, cb, dk * .5f, {1.12f, .5f, .3f}, {.03f, .04f, .2f}, 64, .5f);
        // Side skirts (lower body trim)
        drawCube(V, ls, cb, dk * .65f, {-1.1f, .14f, 0}, {.04f, .1f, 3.8f}, 32, .6f);
        drawCube(V, ls, cb, dk * .65f, {1.1f, .14f, 0}, {.04f, .1f, 3.8f}, 32, .6f);
    };
    drawCar({.82f, .15f, .12f}, {-12, 0, 31}, 90); // Red car
    drawCar({.18f, .35f, .82f}, {-12, 0, 37}, 90); // Blue car
    drawCar({.92f, .92f, .90f}, {12, 0, 34}, -90); // White car
    drawCar({.12f, .52f, .22f}, {12, 0, 43}, -90); // Green car
    // Boom barrier (animated rotation)
    drawCube(V, ls, I, C_LAMP, {3, .5f, 54}, {.2f, 1, .2f}); // post
    {
        // Pivot at post top, rotate the arm around Z-axis at pivot point
        glm::mat4 bm = glm::translate(I, {3.0f, 1.1f, 54.0f}); // move to pivot
        bm = glm::rotate(bm, glm::radians(-barrierAngle), {0, 0, 1}); // rotate
        bm = glm::translate(bm, {-3.0f, 0, 0}); // offset arm from pivot
        drawCube(V, ls, bm, C_BARRIER_R, {0, 0, 0}, {6, .12f, .12f});
    }
    // Parking lamp posts
    drawCube(V, ls, I, C_LAMP, {-10, 3, 42}, {.15f, 6, .15f});
    drawCube(V, ls, I, C_LAMP, {-10, 6.2f, 42}, {.8f, .15f, .6f});
    drawCube(V, ls, I, C_LAMP, {10, 3, 42}, {.15f, 6, .15f});
    drawCube(V, ls, I, C_LAMP, {10, 6.2f, 42}, {.8f, .15f, .6f});

    // MALL EXTERIOR (X:-25 to 25, Z:-25 to 25)

    // North wall
    drawCube(V, ls, I, C_EXT, {0, WH, -25}, {50, MH, WT});
    // South wall (gap X:-4 to 4 for entrance)
    drawCube(V, ls, I, C_EXT, {-14.5f, WH, 25}, {21, MH, WT});
    drawCube(V, ls, I, C_EXT, {14.5f, WH, 25}, {21, MH, WT});
    drawCube(V, ls, I, C_EXT, {0, MH - .5f, 25}, {8, 1, WT}); // strip above entrance
    // West wall
    drawCube(V, ls, I, C_EXT, {-25, WH, 0}, {WT, MH, 50});
    // East wall
    drawCube(V, ls, I, C_EXT, {25, WH, 0}, {WT, MH, 50});
    // Entrance awning
    drawCube(V, ls, I, C_AWNING, {0, MH + .1f, 26.5f}, {12, .2f, 3.5f});
    // Glass panels beside entrance
    drawCube(V, ls, I, C_GLASS, {-3.5f, WH - .5f, 25.01f}, {1.5f, MH - 1, .08f}, 128, .3f);
    drawCube(V, ls, I, C_GLASS, {3.5f, WH - .5f, 25.01f}, {1.5f, MH - 1, .08f}, 128, .3f);
    // GLASS ENTRANCE DOORS (sliding, press G to open/close)
    {
        float edX = 0.0f, edZ = 25.0f;
        float edHalfW = 3.8f; // total opening width
        float slideAmt = entranceDoorOffset * edHalfW * 0.5f; // how far each door slides
        // Left door panel (slides left when open)
        float ldX = edX - edHalfW * 0.25f - slideAmt;
        drawCube(V, ls, I, C_GLASS * 1.05f, {ldX, WH * 0.9f, edZ + 0.02f}, {edHalfW * 0.48f, MH * 0.85f, .1f}, 128, .2f);
        // Right door panel (slides right when open)
        float rdX = edX + edHalfW * 0.25f + slideAmt;
        drawCube(V, ls, I, C_GLASS * 1.05f, {rdX, WH * 0.9f, edZ + 0.02f}, {edHalfW * 0.48f, MH * 0.85f, .1f}, 128, .2f);
        // Door frame (metal surround)
        drawCube(V, ls, I, C_LAMP, {edX, MH - 0.1f, edZ + 0.02f}, {edHalfW + 0.4f, .2f, .15f}); // top bar
        drawCube(V, ls, I, C_LAMP, {edX - edHalfW * 0.5f - 0.1f, WH * 0.9f, edZ + 0.02f}, {.15f, MH * 0.85f, .15f}); // left post
        drawCube(V, ls, I, C_LAMP, {edX + edHalfW * 0.5f + 0.1f, WH * 0.9f, edZ + 0.02f}, {.15f, MH * 0.85f, .15f}); // right post
        // Door handle marks (faint)
        drawCube(V, ls, I, C_LAMP * 1.3f, {ldX + edHalfW * 0.15f, 1.2f, edZ + 0.08f}, {.04f, .35f, .04f}, 64);
        drawCube(V, ls, I, C_LAMP * 1.3f, {rdX - edHalfW * 0.15f, 1.2f, edZ + 0.08f}, {.04f, .35f, .04f}, 64);
        // Button panel beside entrance (outside)
        drawCube(V, ls, I, C_LAMP, {edX + edHalfW * 0.5f + 0.8f, 1.2f, edZ + 0.3f}, {.2f, .3f, .1f});
        drawCube(V, ls, I, {0.1f, 0.9f, 0.3f}, {edX + edHalfW * 0.5f + 0.8f, 1.2f, edZ + 0.36f}, {.1f, .1f, .03f}); // green button
    }
    // Windows on east/west
    for (int i = 0; i < 4; i++)
    {
        float wz = -20 + i * 12.f;
        drawCube(V, ls, I, C_GLASS, {-25.01f, 2.5f, wz}, {.08f, 2, 4}, 128, .3f);
        drawCube(V, ls, I, C_GLASS, {25.01f, 2.5f, wz}, {.08f, 2, 4}, 128, .3f);
    }

    // MALL FLOOR & CEILING
    ls.setBool("blendWithColor", true);
    drawCubeTextured(texCubeVAO, ls, I, C_FLOOR, {0, -.02f, 0}, {49.5f, .08f, 49.5f}, texFloor, 8.0f, 32, .9f);
    ls.setBool("blendWithColor", false);
    // Ceiling (hole at staircase corner X:15-25, Z:-25 to -16)
    drawCube(V, ls, I, C_CEILING, {-5, MH, 0}, {40, .15f, 49.5f});
    drawCube(V, ls, I, C_CEILING, {20, MH, 4.5f}, {10, .15f, 40});
    // Roof floor
    drawCube(V, ls, I, C_PARKING, {0, MH + .05f, 0}, {49.5f, .1f, 49.5f}, 16, .8f);

    // CORRIDORS
    // N-S corridor: X:-3 to 3, full Z
    // E-W corridor: Z:10 to 16, full X
    //  Left N-S wall (X=-3)
    // Below E-W corridor (shops side, Z:16 to 25)
    //   Shop 1 entrance gap Z:20 to 24, Shop 2 gap Z:17 to 21 => simplify: just corner pieces
    drawCube(V, ls, I, C_CORR, {-3, WH, 25}, {WT, MH, WT}); // corner at Z=25
    drawCube(V, ls, I, C_CORR, {-3, WH, 16}, {WT, MH, WT}); // corner at E-W junction
    // Above E-W corridor (food court side, Z:-25 to 10)
    //   Food court wide opening: gap Z:-18 to -2
    drawCube(V, ls, I, C_CORR, {-3, WH, -21.5f}, {WT, MH, 7}); // Z:-25 to -18
    drawCube(V, ls, I, C_CORR, {-3, WH, 4}, {WT, MH, 12});     // Z:-2 to 10

    // --- Right N-S wall (X=3) ---
    drawCube(V, ls, I, C_CORR, {3, WH, 25}, {WT, MH, WT});
    drawCube(V, ls, I, C_CORR, {3, WH, 16}, {WT, MH, WT});
    // Prayer room entrance: gap Z:-16 to -6
    drawCube(V, ls, I, C_CORR, {3, WH, -21}, {WT, MH, 8}); // Z:-25 to -17
    drawCube(V, ls, I, C_CORR, {3, WH, 2}, {WT, MH, 16});  // Z:-6 to 10

    // E-W corridor walls
    // South wall of E-W corridor (Z=16), west side
    drawCube(V, ls, I, C_CORR, {-14, WH, 16}, {22, MH, WT});
    // South wall of E-W corridor (Z=16), east side
    drawCube(V, ls, I, C_CORR, {14, WH, 16}, {22, MH, WT});
    // North wall of E-W corridor (Z=10), west side (food court opening Z=5 to 10 area is open)
    drawCube(V, ls, I, C_CORR, {-14, WH, 10}, {22, MH, WT});
    // North wall of E-W corridor (Z=10), east side
    drawCube(V, ls, I, C_CORR, {14, WH, 10}, {22, MH, WT});

    // ------ SHOP BANNERS (textured signs above entrances, on corridor-facing wall) ------
    // Fashion shop banner (above E-W corridor south wall, facing corridor)
    drawCubeTextured(texCubeVAO, ls, I, glm::vec3(.95f), {-19.5f, 4.2f, 15.7f}, {10, 1.5f, .1f}, texFashion, 1, 16);
    // Tech shop banner
    drawCubeTextured(texCubeVAO, ls, I, glm::vec3(.95f), {-8.5f, 4.2f, 15.7f}, {9, 1.5f, .1f}, texTech, 1, 16);
    // Gems shop banner
    drawCubeTextured(texCubeVAO, ls, I, glm::vec3(.95f), {9.f, 4.2f, 15.7f}, {10, 1.5f, .1f}, texGems, 1, 16);
    // Food court banner (on N-S corridor wall, facing corridor)
    drawCubeTextured(texCubeVAO, ls, I, glm::vec3(.95f), {-2.85f, 4.2f, -10}, {.1f, 1.5f, 14}, texFood, 1, 16);

    // SHOP 1 - FASHION (X:-25 to -3, Z:16 to 25)
    {
        float sx1 = -25, sx2 = -3, sz1 = 16, sz2 = 25;
        float cx = (sx1 + sx2) / 2, cz = (sz1 + sz2) / 2;
        // Walls - pink
        drawCube(V, ls, I, C_FASH_WALL, {sx1 + WT / 2, WH, cz}, {WT, MH, sz2 - sz1}); // back (west)
        // Floor tint
        drawCube(V, ls, I, C_FASH_FLOOR, {cx, .03f, cz}, {sx2 - sx1, .04f, sz2 - sz1});
        // Divider wall between shop 1 and shop 2 at X=-14
        drawCube(V, ls, I, C_FASH_WALL, {-14, WH, cz}, {WT, MH, sz2 - sz1});

        // Clothing racks (horizontal bars on stands)
        // Rack 1: along back wall
        drawCube(V, ls, I, C_RACK, {-23, 1.5f, 19}, {.1f, 2.8f, .1f});     // left post
        drawCube(V, ls, I, C_RACK, {-23, 1.5f, 22}, {.1f, 2.8f, .1f});     // right post
        drawCube(V, ls, I, C_RACK, {-23, 2.9f, 20.5f}, {.1f, .08f, 3.5f}); // bar
        // Hanging clothes (colorful thin cubes)
        glm::vec3 clothes[] = {{.9f, .2f, .3f}, {.2f, .5f, .9f}, {.9f, .8f, .1f}, {.3f, .8f, .3f}, {.8f, .4f, .9f}};
        for (int i = 0; i < 5; i++)
            drawCube(V, ls, I, clothes[i], {-23, 2.2f, 19.2f + i * .7f}, {.05f, 1.2f, .5f});

        // Rack 2: center of shop
        drawCube(V, ls, I, C_RACK, {-10, 1.3f, 19}, {.1f, 2.4f, .1f});
        drawCube(V, ls, I, C_RACK, {-10, 1.3f, 23}, {.1f, 2.4f, .1f});
        drawCube(V, ls, I, C_RACK, {-10, 2.5f, 21}, {.1f, .08f, 4});
        for (int i = 0; i < 5; i++)
            drawCube(V, ls, I, clothes[(i + 2) % 5], {-10, 1.8f, 19.5f + i * .8f}, {.05f, 1.1f, .5f});

        // Mannequins (simple: base + body + head) ===
        auto drawMannequin = [&](float mx, float mz, glm::vec3 shirtCol)
        {
            drawCube(V, ls, I, C_MANNEQUIN * .8f, {mx, .15f, mz}, {.8f, .3f, .8f}); // base
            drawCube(V, ls, I, shirtCol, {mx, 1.2f, mz}, {.5f, 1.5f, .3f});         // body/shirt
            drawCube(V, ls, I, C_MANNEQUIN, {mx, 2.15f, mz}, {.3f, .35f, .3f});     // head
        };
        drawMannequin(-20, 21, {.9f, .3f, .4f});
        drawMannequin(-18, 21, {.3f, .5f, .9f});
        drawMannequin(-7, 21, {.2f, .7f, .3f});

        // Mirrors (tall glass panels)
        drawCube(V, ls, I, C_MIRROR, {-23, 2, 17}, {.05f, 3, 2}, 128, .3f);
        drawCube(V, ls, I, C_MIRROR, {-17, 2, 24.5f}, {3, 3, .05f}, 128, .3f);

        // Cash register desk
        drawCube(V, ls, I, C_CASH_DESK, {-5, 0.5f, 23}, {2, 1, 1.5f});
        drawCube(V, ls, I, C_CASH_DESK * 0.8f, {-5, 1.1f, 23}, {1, .3f, .8f}); // register
    }

    // SHOP 2 - TECH (X:-14 to -3, Z:16 to 25)
    {
        float sx1 = -14, sx2 = -3, sz1 = 16, sz2 = 25;
        float cx = (sx1 + sx2) / 2, cz = (sz1 + sz2) / 2;
        // Floor tint
        drawCube(V, ls, I, C_TECH_FLOOR, {cx, .03f, cz}, {sx2 - sx1, .04f, sz2 - sz1});

        // Display tables with devices (Apple Store style)
        auto drawDispTable = [&](float tx, float tz)
        {
            drawCube(V, ls, I, C_DISP_BASE, {tx, .45f, tz}, {2, .9f, 1.2f});     // base
            drawCube(V, ls, I, C_DISP_TABLE, {tx, .95f, tz}, {2.1f, .1f, 1.3f}); // top
            // Devices on table
            drawCube(V, ls, I, C_PHONE, {tx - .5f, 1.1f, tz}, {.15f, .02f, .3f}); // phone
            drawCube(V, ls, I, C_PHONE, {tx, 1.1f, tz}, {.15f, .02f, .3f});
            drawCube(V, ls, I, C_PHONE, {tx + .5f, 1.1f, tz}, {.15f, .02f, .3f});
        };
        drawDispTable(-10.5f, 19);
        drawDispTable(-10.5f, 22);
        drawDispTable(-5.5f, 19);
        drawDispTable(-5.5f, 22);

        // Large screen on wall
        drawCube(V, ls, I, {.15f, .15f, .18f}, {sx1 + .2f, 3, 20.5f}, {.08f, 1.8f, 5}); // frame
        drawCube(V, ls, I, C_SCREEN, {sx1 + .22f, 3, 20.5f}, {.04f, 1.6f, 4.6f});       // screen

        // Laptops on right table
        drawCube(V, ls, I, C_LAPTOP, {-5.5f, 1.05f, 22.3f}, {.6f, .02f, .4f});      // base
        drawCube(V, ls, I, C_LAPTOP * .9f, {-5.5f, 1.3f, 22.5f}, {.6f, .4f, .02f}); // screen
    }

    // SHOP 3 - GEMS/JEWELRY (X:3 to 25, Z:16 to 25) - GOLD WALLS
    {
        float sx1 = 3, sx2 = 25, sz1 = 16, sz2 = 25;
        float cx = (sx1 + sx2) / 2, cz = (sz1 + sz2) / 2;
        drawCube(V, ls, I, C_GEM_WALL, {sx2 - WT / 2, WH, cz}, {WT, MH, sz2 - sz1}); // back (east)
        drawCube(V, ls, I, C_GEM_WALL, {cx, .03f, cz}, {sx2 - sx1, .04f, sz2 - sz1});
        // Divider at X=15
        drawCube(V, ls, I, C_GEM_WALL, {15, WH, cz}, {WT, MH, sz2 - sz1});

        // Glass display cases
        for (int i = 0; i < 3; i++)
        {
            float gx = 7 + i * 4.f;
            drawCube(V, ls, I, C_GEM_CASE, {gx, .5f, 20}, {2, 1, 1.2f});
            drawCube(V, ls, I, C_GLASS, {gx, 1.15f, 20}, {2.1f, .3f, 1.3f}, 128, .3f); // glass top
            // Gems inside
            drawCube(V, ls, I, {.9f, .2f, .2f}, {gx - .3f, .75f, 20}, {.2f, .15f, .2f}, 128); // ruby
            drawCube(V, ls, I, {.2f, .3f, .9f}, {gx + .3f, .75f, 20}, {.2f, .15f, .2f}, 128); // sapphire
        }
    }

    // SHOP 4 - BOOKSTORE (X:15 to 25, Z:16 to 25) -
    {
        drawCube(V, ls, I, C_BOOK_WALL * 1.1f, {20, .03f, 20.5f}, {10, .04f, 9});
        // Tall bookshelves along walls
        for (int i = 0; i < 3; i++)
        {
            drawCube(V, ls, I, C_BOOKSHELF, {17 + i * 3.f, 1.8f, 18}, {.4f, 3.4f, 3});
            // Book rows (colorful)
            glm::vec3 bc[] = {{.8f, .2f, .1f}, {.1f, .3f, .7f}, {.2f, .6f, .2f}, {.7f, .6f, .1f}};
            for (int j = 0; j < 4; j++)
                drawCube(V, ls, I, bc[j], {17 + i * 3.f, .5f + j * .8f, 17.9f}, {.35f, .5f, .6f});
        }
        // Reading table
        drawCube(V, ls, I, C_TABLE, {20, .5f, 23}, {3, .06f, 2});
        drawCube(V, ls, I, C_TABLE * .8f, {20, .25f, 23}, {.1f, .48f, .1f});
    }

    // FOOD COURT (X:-25 to -3, Z:-25 to 10) - COLORFUL STALLS
    {
        drawCube(V, ls, I, C_FOOD_FLOOR, {-14, .03f, -7.5f}, {22, .04f, 35});

        // Food stalls along west wall
        // Stall 1 - Burger (red)
        drawCube(V, ls, I, C_STALL_RED, {-24, WH, -20}, {2, MH, 6});
        drawCube(V, ls, I, C_COUNTER, {-22, .55f, -20}, {2, 1.1f, 5});
        // Stall 2 - Pizza (yellow)
        drawCube(V, ls, I, C_STALL_YEL, {-24, WH, -13}, {2, MH, 6});
        drawCube(V, ls, I, C_COUNTER, {-22, .55f, -13}, {2, 1.1f, 5});
        // Stall 3 - Chicken (green)
        drawCube(V, ls, I, C_STALL_GRN, {-24, WH, -6}, {2, MH, 6});
        drawCube(V, ls, I, C_COUNTER, {-22, .55f, -6}, {2, 1.1f, 5});

        //  Food stalls along north wall
        // Stall 4 - Tacos (orange)
        drawCube(V, ls, I, C_STALL_ORG, {-18, WH, -24}, {10, MH, 2});
        drawCube(V, ls, I, C_COUNTER, {-18, .55f, -22}, {9, 1.1f, 2});
        // Stall 5 - Drinks (purple)
        drawCube(V, ls, I, C_STALL_PUR, {-8, WH, -24}, {6, MH, 2});
        drawCube(V, ls, I, C_COUNTER, {-8, .55f, -22}, {5, 1.1f, 2});

        // Tables and chairs (grid) - NOW WITH BEZIER ROUNDED TABLE TOPS
        for (int r = 0; r < 3; r++)
            for (int c = 0; c < 3; c++)
            {
                float tx = -18 + c * 5.f, tz = -4 - r * 6.f;
                // Bezier rounded table top (revolved surface)
                glm::mat4 tm = glm::translate(I, {tx, 0.75f, tz});
                tm = glm::scale(tm, glm::vec3(1.2f, 1.0f, 1.2f));
                bezierTable->draw(ls, tm, C_TABLE, 64.0f);
                // Table leg
                drawCube(V, ls, I, C_TABLE * .7f, {tx, .37f, tz}, {.08f, .72f, .08f});
                // 4 chairs (curved backs via slight rotation gives illusion of curves)
                drawCube(V, ls, I, C_CHAIR, {tx - .8f, .45f, tz}, {.4f, .05f, .4f});
                drawCube(V, ls, I, C_CHAIR, {tx - .8f, .7f, tz - .18f}, {.4f, .45f, .05f});
                drawCube(V, ls, I, C_CHAIR, {tx + .8f, .45f, tz}, {.4f, .05f, .4f});
                drawCube(V, ls, I, C_CHAIR, {tx + .8f, .7f, tz + .18f}, {.4f, .45f, .05f});
                drawCube(V, ls, I, C_CHAIR, {tx, .45f, tz - .8f}, {.4f, .05f, .4f});
                drawCube(V, ls, I, C_CHAIR, {tx, .7f, tz - .8f}, {.05f, .45f, .4f});
                drawCube(V, ls, I, C_CHAIR, {tx, .45f, tz + .8f}, {.4f, .05f, .4f});
                drawCube(V, ls, I, C_CHAIR, {tx, .7f, tz + .8f}, {.05f, .45f, .4f});
            }
    }

    // PRAYER ROOM (X:3 to 15, Z:-25 to -6) - CALM BEIGE
    {
        drawCube(V, ls, I, C_PRAY_WALL, {15, WH, -15.5f}, {WT, MH, 19}); // east wall (divider)
        drawCube(V, ls, I, C_PRAY_FLOOR, {9, .03f, -15.5f}, {12, .04f, 19});
        // Prayer mat
        drawCube(V, ls, I, C_PRAY_MAT, {9, .06f, -16}, {4, .04f, 2.5f});
        drawCube(V, ls, I, C_PRAY_MAT * .9f, {9, .07f, -16}, {3, .02f, 1.8f}); // inner pattern

        // Standing lamp (from reference)
        drawCube(V, ls, I, C_PRAY_LAMP * .6f, {12, .05f, -20}, {.5f, .1f, .5f});    // base
        drawCube(V, ls, I, C_PRAY_LAMP * .5f, {12, 1.2f, -20}, {.08f, 2.3f, .08f}); // pole
        drawCube(V, ls, I, C_PRAY_LAMP, {12, 2.6f, -20}, {.5f, .8f, .5f});          // lampshade

        // Small bookshelf (from reference)
        drawCube(V, ls, I, C_PRAY_SHELF, {13, .5f, -13}, {1.5f, 1, .8f});         // shelf body
        drawCube(V, ls, I, C_PRAY_SHELF * .8f, {13, .7f, -13}, {1.2f, .3f, .6f}); // books
    }

    // WASHROOM (X:15 to 25, Z:-16 to -6)
    {
        drawCube(V, ls, I, C_WASH, {20, .03f, -11}, {10, .04f, 10});
        drawCube(V, ls, I, C_CORR, {20, WH, -6}, {10, MH, WT});  // south wall
        drawCube(V, ls, I, C_CORR, {15, WH, -11}, {WT, MH, 10}); // west wall (shared with prayer room)
        // Stall partitions
        for (int i = 0; i < 3; i++)
            drawCube(V, ls, I, C_WASH * .9f, {18 + i * 2.5f, 1.2f, -11}, {.08f, 2.2f, 6});
    }

    // STAIRCASE (X:15 to 25, Z:-25 to -16)
    {
        drawCube(V, ls, I, C_CORR, {20, WH, -16}, {10, MH, WT}); // divider above washroom
        float stW = 9, stepD = 9.f / 10;
        for (int i = 0; i < 10; i++)
        {
            float sy = i * .5f + .25f, sz = -16.5f - i * stepD - stepD / 2;
            drawCube(V, ls, I, C_STAIR, {20, sy, sz}, {stW, .5f, stepD});
        }
        // Railings
        drawCube(V, ls, I, C_LAMP, {15.5f, 3, -20.5f}, {.1f, MH + 1, 9});
        drawCube(V, ls, I, C_LAMP, {24.5f, 3, -20.5f}, {.1f, MH + 1, 9});
    }

    // ROOF PARAPET
    float pY = MH + .5f;
    drawCube(V, ls, I, C_PARAPET, {0, pY, -25}, {50, 1, .3f});
    drawCube(V, ls, I, C_PARAPET, {0, pY, 25}, {50, 1, .3f});
    drawCube(V, ls, I, C_PARAPET, {-25, pY, 0}, {.3f, 1, 50});
    drawCube(V, ls, I, C_PARAPET, {25, pY, 0}, {.3f, 1, 50});

    // GLASS ELEVATOR (moved to side of corridor, near E-W junction)
    {
        float eX = -2.0f, eZ = 13.0f;
        // Shaft frame (glass walls)
        drawCube(V, ls, I, C_GLASS, {eX - 1.2f, WH, eZ}, {.08f, MH, 2}, 128, .2f); // left wall
        drawCube(V, ls, I, C_GLASS, {eX + 1.2f, WH, eZ}, {.08f, MH, 2}, 128, .2f); // right wall
        drawCube(V, ls, I, C_GLASS, {eX, WH, eZ - 1.0f}, {2.4f, MH, .08f}, 128, .2f); // back wall
        // Shaft rails (metal)
        drawCube(V, ls, I, C_LAMP, {eX - 1.15f, WH, eZ - .95f}, {.05f, MH, .05f});
        drawCube(V, ls, I, C_LAMP, {eX + 1.15f, WH, eZ - .95f}, {.05f, MH, .05f});
        drawCube(V, ls, I, C_LAMP, {eX - 1.15f, WH, eZ + .95f}, {.05f, MH, .05f});
        drawCube(V, ls, I, C_LAMP, {eX + 1.15f, WH, eZ + .95f}, {.05f, MH, .05f});
        // Moving platform
        drawCube(V, ls, I, C_STAIR, {eX, elevatorY + .05f, eZ}, {2.2f, .1f, 1.8f});
        // Sliding doors (split at front, Z=eZ+1)
        float doorHalf = 1.1f * (1.0f - elevatorDoorOffset); // doors slide apart
        if (doorHalf > 0.05f) {
            drawCube(V, ls, I, C_GLASS, {eX - doorHalf * 0.5f - 0.05f, elevatorY + 0.7f, eZ + 1.0f}, {doorHalf, 1.2f, .06f}, 128, .25f);
            drawCube(V, ls, I, C_GLASS, {eX + doorHalf * 0.5f + 0.05f, elevatorY + 0.7f, eZ + 1.0f}, {doorHalf, 1.2f, .06f}, 128, .25f);
        }
        // Call button panel (small box beside elevator)
        drawCube(V, ls, I, C_LAMP, {eX + 1.5f, 1.2f, eZ + 0.8f}, {.15f, .25f, .1f}); // panel
        drawCube(V, ls, I, {0.2f, 0.8f, 0.2f}, {eX + 1.5f, 1.28f, eZ + 0.82f}, {.08f, .08f, .03f}); // up button
        drawCube(V, ls, I, {0.8f, 0.2f, 0.2f}, {eX + 1.5f, 1.12f, eZ + 0.82f}, {.08f, .08f, .03f}); // down button
    }

    // ESCALATOR (near staircase area, X=15 corridor, Z=-6 to -16 area)
    {
        float esX = 13.0f, esZ1 = -6.0f, esZ2 = -14.0f;
        float esLen = esZ1 - esZ2; // 8 units long
        float esH = 4.0f; // height rise
        // Side rails
        drawCube(V, ls, I, C_LAMP, {esX - 0.6f, esH / 2, (esZ1 + esZ2) / 2}, {.05f, esH + .5f, esLen});
        drawCube(V, ls, I, C_LAMP, {esX + 0.6f, esH / 2, (esZ1 + esZ2) / 2}, {.05f, esH + .5f, esLen});
        // Handrails (top)
        drawCube(V, ls, I, C_LAMP * 1.2f, {esX - 0.6f, esH + .3f, (esZ1 + esZ2) / 2}, {.08f, .06f, esLen});
        drawCube(V, ls, I, C_LAMP * 1.2f, {esX + 0.6f, esH + .3f, (esZ1 + esZ2) / 2}, {.08f, .06f, esLen});
        // Moving steps
        int numSteps = 16;
        float stepLen = esLen / numSteps;
        for (int i = 0; i < numSteps; i++) {
            float rawZ = esZ2 + (i + escalatorOffset / (esLen / numSteps)) * stepLen;
            float frac = fmod(rawZ - esZ2, esLen);
            if (frac < 0) frac += esLen;
            float sZ = esZ2 + frac;
            float t = (sZ - esZ2) / esLen; // 0 (bottom) to 1 (top)
            float sY = t * esH;
            drawCube(V, ls, I, C_STAIR * 1.05f, {esX, sY + .05f, sZ}, {1.0f, .1f, stepLen * .85f});
        }
    }

    // LIGHT FIXTURE GEOMETRY (visual only, lights off)
    fs.use();
    fs.setMat4("projection", proj);
    fs.setMat4("view", view);
    glBindVertexArray(LV);
    for (int i = 0; i < NUM_POINT_LIGHTS; i++)
    {
        glm::mat4 lm = glm::translate(I, plPos[i]);
        lm = glm::scale(lm, i >= 12 ? glm::vec3(.8f, .15f, .6f) : glm::vec3(1, .08f, .4f));
        fs.setVec3("color", glm::vec3(.9f, .9f, .85f));
        fs.setMat4("model", lm);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    }
}

// INPUT
void processInput(GLFWwindow *w)
{
    if (glfwGetKey(w, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(w, true);
    if (glfwGetKey(w, GLFW_KEY_W) == GLFW_PRESS)
        basic_camera.move(FORWARD, deltaTime);
    if (glfwGetKey(w, GLFW_KEY_S) == GLFW_PRESS)
        basic_camera.move(BACKWARD, deltaTime);
    if (glfwGetKey(w, GLFW_KEY_A) == GLFW_PRESS)
        basic_camera.move(LEFT, deltaTime);
    if (glfwGetKey(w, GLFW_KEY_D) == GLFW_PRESS)
        basic_camera.move(RIGHT, deltaTime);
    // Staircase height (X:15-25, Z:-25 to -16)
    float x = basic_camera.eye.x, z = basic_camera.eye.z;
    if (x >= 15 && x <= 25 && z >= -25 && z <= -16)
    {
        float t = (-16.f - z) / 9.f;
        t = glm::clamp(t, 0.f, 1.f);
        basic_camera.eye.y = 1.7f + t * MH;
    }
    else if (basic_camera.eye.y > 3 && x >= -25 && x <= 25 && z >= -25 && z <= 25)
    {
        basic_camera.eye.y = MH + 1.7f;
    }
    else
    {
        basic_camera.eye.y = 1.7f;
    }
}
void key_callback(GLFWwindow *w, int key, int sc, int action, int mods)
{
    if (action != GLFW_PRESS)
        return;
    if (key == GLFW_KEY_F1)
        activeViewport = (activeViewport == 1) ? 0 : 1;
    if (key == GLFW_KEY_F2)
        activeViewport = (activeViewport == 2) ? 0 : 2;
    if (key == GLFW_KEY_F3)
        activeViewport = (activeViewport == 3) ? 0 : 3;
    if (key == GLFW_KEY_F4)
        activeViewport = (activeViewport == 4) ? 0 : 4;
    if (key == GLFW_KEY_1)
    {
        ambientEnabled = !ambientEnabled;
        cout << "Ambient: " << (ambientEnabled ? "ON" : "OFF") << endl;
    }
    if (key == GLFW_KEY_2)
    {
        pointLightsEnabled = !pointLightsEnabled;
        cout << "Point Lights: " << (pointLightsEnabled ? "ON" : "OFF") << endl;
    }
    if (key == GLFW_KEY_3)
    {
        dirLightEnabled = !dirLightEnabled;
        cout << "Dir Light: " << (dirLightEnabled ? "ON" : "OFF") << endl;
    }
    if (key == GLFW_KEY_5)
    {
        spotLightsEnabled = !spotLightsEnabled;
        cout << "Spot Lights: " << (spotLightsEnabled ? "ON" : "OFF") << endl;
    }
    if (key == GLFW_KEY_T)
    {
        texturesEnabled = !texturesEnabled;
        cout << "Textures: " << (texturesEnabled ? "ON" : "OFF") << endl;
    }
    if (key == GLFW_KEY_N)
    {
        dayNightCycleActive = !dayNightCycleActive;
        dayNightSpeed = dayNightCycleActive ? 3.0f : 0.0f; // 3 hours per second for demo
        cout << "Day/Night Cycle: " << (dayNightCycleActive ? "ON" : "OFF")
             << " (Time: " << (int)timeOfDay << ":00)" << endl;
    }
    if (key == GLFW_KEY_E)
    {
        // Elevator toggle
        if (elevatorState == ELEV_STOPPED_BOTTOM) {
            elevatorState = ELEV_DOOR_CLOSING_UP;
            elevatorDoorOffset = 1.0f; // start with door open -> closing
            cout << "Elevator: Going UP" << endl;
        } else if (elevatorState == ELEV_STOPPED_TOP) {
            elevatorState = ELEV_DOOR_CLOSING_DOWN;
            elevatorDoorOffset = 1.0f;
            cout << "Elevator: Going DOWN" << endl;
        } else {
            cout << "Elevator: In motion..." << endl;
        }
    }
    if (key == GLFW_KEY_R)
    {
        escalatorDir = (escalatorDir + 2) % 3 - 1; // cycle: 1 -> 0 -> -1 -> 1
        const char* dirs[] = {"DOWN", "PAUSED", "UP"};
        cout << "Escalator: " << dirs[escalatorDir + 1] << endl;
    }
    if (key == GLFW_KEY_B)
    {
        barrierOpen = !barrierOpen;
        cout << "Boom Barrier: " << (barrierOpen ? "OPENING" : "CLOSING") << endl;
    }
    if (key == GLFW_KEY_C)
    {
        cursorCaptured = !cursorCaptured;
        glfwSetInputMode(w, GLFW_CURSOR, cursorCaptured ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
        if (cursorCaptured) firstMouse = true; // avoid jump when re-capturing
        cout << "Cursor: " << (cursorCaptured ? "CAPTURED" : "FREE") << endl;
    }
    if (key == GLFW_KEY_G)
    {
        entranceDoorOpen = !entranceDoorOpen;
        cout << "Entrance Door: " << (entranceDoorOpen ? "OPENING" : "CLOSING") << endl;
    }
}
void mouse_callback(GLFWwindow *w, double xp, double yp)
{
    if (!cursorCaptured) return; // don't move camera when cursor is free
    float x = (float)xp, y = (float)yp;
    if (firstMouse)
    {
        lastX = x;
        lastY = y;
        firstMouse = false;
    }
    basic_camera.ProcessMouseMovement(x - lastX, lastY - y);
    lastX = x;
    lastY = y;
}
void scroll_callback(GLFWwindow *w, double xo, double yo) { basic_camera.ProcessMouseScroll((float)yo); }
void framebuffer_size_callback(GLFWwindow *w, int width, int height)
{
    SCR_WIDTH = width;
    SCR_HEIGHT = height;
}
