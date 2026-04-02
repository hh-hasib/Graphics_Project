
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
void drawCubeProc(unsigned int &v, Shader &s, glm::mat4 pm, glm::vec3 p, glm::vec3 sc);
unsigned int loadTexture(const char *path);
void drawScene(unsigned int &, unsigned int &, Shader &, Shader &, glm::mat4, glm::mat4);

// Settings
unsigned int SCR_WIDTH = 1400, SCR_HEIGHT = 900;
float lastX = 700, lastY = 450;
bool firstMouse = true;
BasicCamera basic_camera(0, 1.7f, 65, 0, 1.7f, 30, glm::vec3(0, 1, 0));
float deltaTime = 0, lastFrame = 0;

// Mall Dimensions — 3-storey, 80x80 footprint
const float FLOOR_H = 7.0f;
const float SLAB = 0.3f;
const float MALL_HALF = 40.0f;
const float ATRIUM_HALF = 10.0f;
const float HOLE_HALF = 6.0f;
const float TOTAL_H = 3*FLOOR_H + 2*SLAB; // ~21.6
const float FLOOR_Y[] = {0.0f, FLOOR_H+SLAB, 2*(FLOOR_H+SLAB)}; // 0, 7.3, 14.6
const float CEIL_Y[] = {FLOOR_H, 2*FLOOR_H+SLAB, 3*FLOOR_H+2*SLAB}; // 7, 14.3, 21.6
const float MH = TOTAL_H;
const float WT = 0.3f;

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
    // N-S corridor
    {0, 4.5f, 18},
    {0, 4.5f, 4},
    // Tech Shop (SW)
    {-14, 4.5f, 16},
    // Clothing Shop (SE)
    {14, 4.5f, 16},
    // Escalator + Staircase area
    {-20, 4.5f, 4},
    {20, 4.5f, 4},
    // Prayer Room
    {-14, 4.5f, -10},
    // Gems Shop
    {14, 4.5f, -10},
    // Wash Room + Lift
    {-14, 4.5f, -3},
    {9, 4.5f, -3},
    // Food court
    {-10, 4.5f, -19},
    {10, 4.5f, -19},
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

// Elevator state machine (3-floor)
enum ElevatorState { ELEV_IDLE, ELEV_DOOR_OPENING, ELEV_DOOR_OPEN, ELEV_DOOR_CLOSING, ELEV_MOVING };
ElevatorState elevatorState = ELEV_DOOR_OPEN;
float elevatorY = 0.0f;
float elevatorDoorOffset = 1.0f;
float elevatorTimer = 0.0f;
int elevatorTargetFloor = 0;
int elevatorCurrentFloor = 0;
const float ELEV_SPEED = 4.0f;
bool lKeyHeld = false;

// Escalator
float escalatorOffset = 0.0f;
int escalatorDir = 1;

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
unsigned int texKitkat = 0, texMovie = 0;

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
    cout << "  E - Call Elevator (when near lift door)" << endl;
    cout << "  L+G/1/2 - Elevator to floor (when inside)" << endl;
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
    texKitkat = loadTexture("kitkat_poster.jpg");
    texMovie = loadTexture("movie_poster.jpg");

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
    glm::vec3 treePositions[] = {{-30, 0, 43}, {30, 0, 43}, {-50, 0, 15}, {-50, 0, -10}, {50, 0, 12}, {50, 0, -15}, {-16, 0, 77}, {16, 0, 77}, {-55, 0, 55}, {55, 0, -25}};
    for (int i = 0; i < NUM_TREES; i++) {
        fractalTrees[i] = new FractalTree();
        float height = 3.5f + (i % 3) * 0.5f;
        float radius = 0.2f + (i % 2) * 0.05f;
        int depth = 3 + (i % 2);
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
        // Elevator state machine update (3-floor)
        elevatorTimer += deltaTime;
        switch (elevatorState) {
        case ELEV_IDLE:
            break;
        case ELEV_DOOR_OPENING:
            elevatorDoorOffset = glm::min(elevatorDoorOffset + deltaTime * 1.0f, 1.0f);
            if (elevatorDoorOffset >= 1.0f) {
                elevatorState = ELEV_DOOR_OPEN;
                elevatorTimer = 0.0f;
            }
            break;
        case ELEV_DOOR_OPEN:
            elevatorTimer += 0; // waiting for button or auto-close after 5s
            if (elevatorTimer > 5.0f) {
                elevatorState = ELEV_IDLE;
                elevatorDoorOffset = 1.0f;
                // close door
                elevatorState = ELEV_DOOR_CLOSING;
            }
            break;
        case ELEV_DOOR_CLOSING:
            elevatorDoorOffset = glm::max(elevatorDoorOffset - deltaTime * 1.0f, 0.0f);
            if (elevatorDoorOffset <= 0.0f) {
                if (elevatorTargetFloor != elevatorCurrentFloor) {
                    elevatorState = ELEV_MOVING;
                } else {
                    elevatorState = ELEV_IDLE;
                }
            }
            break;
        case ELEV_MOVING: {
            float targetY = FLOOR_Y[elevatorTargetFloor];
            float dir = (targetY > elevatorY) ? 1.0f : -1.0f;
            elevatorY += dir * ELEV_SPEED * deltaTime;
            if ((dir > 0 && elevatorY >= targetY) || (dir < 0 && elevatorY <= targetY)) {
                elevatorY = targetY;
                elevatorCurrentFloor = elevatorTargetFloor;
                elevatorState = ELEV_DOOR_OPENING;
                elevatorDoorOffset = 0.0f;
            }
            break;
        }
        }
        lKeyHeld = (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS);

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
        ls.setBool("useProceduralWall", false);

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

// DRAW CUBE WITH PROCEDURAL WALL TEXTURE
void drawCubeProc(unsigned int &v, Shader &s, glm::mat4 pm, glm::vec3 p, glm::vec3 sc)
{
    s.use();
    glm::mat4 m = glm::translate(pm, p);
    m = glm::scale(m, sc);
    s.setVec3("material.ambient", glm::vec3(0.5f));
    s.setVec3("material.diffuse", glm::vec3(0.7f));
    s.setVec3("material.specular", glm::vec3(0.2f));
    s.setFloat("material.shininess", 16.0f);
    s.setBool("useTexture", false);
    s.setBool("useProceduralWall", true);
    s.setMat4("model", m);
    glBindVertexArray(v);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    s.setBool("useProceduralWall", false);
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
    drawCube(V, ls, I, C_ROAD, {0, -.04f, 78}, {130, .06f, 8});
    for (int i = 0; i < 18; i++)
        drawCube(V, ls, I, C_ROAD_YEL, {-64 + i * 7.5f, -.01f, 78}, {4.5f, .02f, .12f});
    drawCube(V, ls, I, C_SIDEWALK, {0, .05f, 83}, {130, .12f, 2});
    drawCube(V, ls, I, C_SIDEWALK, {0, .05f, 73}, {130, .12f, 2});
    // Access road into parking
    drawCube(V, ls, I, C_ROAD, {0, -.04f, 62}, {8, .06f, 24});
    // Sidewalk around building entrance
    drawCube(V, ls, I, C_SIDEWALK, {0, .05f, 42}, {20, .12f, 4});
    // West side road
    drawCube(V, ls, I, C_ROAD, {-48, -.04f, 5}, {6, .06f, 80});
    drawCube(V, ls, I, C_SIDEWALK, {-44.5f, .05f, 5}, {1.5f, .12f, 80});

    // FRACTAL TREES (procedurally generated, replacing old pyramid cones)
    {
        glm::vec3 treeP[] = {{-48, 0, 48}, {48, 0, 48}, {-48, 0, 15}, {-48, 0, -15}, {48, 0, 12}, {48, 0, -20}, {-16, 0, 80}, {16, 0, 80}, {-55, 0, 60}, {55, 0, -30}};
        for (int ti = 0; ti < NUM_TREES; ti++) {
            glm::mat4 treeModel = glm::translate(I, treeP[ti]);
            fractalTrees[ti]->drawBranches(ls, treeModel, texTreeBark, texturesEnabled);
            fractalTrees[ti]->drawLeaves(ls, treeModel, texTreeLeaf, texturesEnabled);
        }
    }

    // PARKING LOT (Z: 27 to 55)

    drawCube(V, ls, I, C_PARKING, {0, .01f, 58}, {50, .05f, 28});
    // Parking lines
    for (int i = 0; i < 9; i++)
    {
        float z = 45 + i * 3.f;
        drawCube(V, ls, I, C_PARK_LINE, {-15, .04f, z}, {.1f, .02f, 2.5f});
        drawCube(V, ls, I, C_PARK_LINE, {15, .04f, z}, {.1f, .02f, 2.5f});
    }
    drawCube(V, ls, I, C_PARK_LINE, {-4, .04f, 58}, {.1f, .02f, 26});
    drawCube(V, ls, I, C_PARK_LINE, {4, .04f, 58}, {.1f, .02f, 26});
    // Arrows
    drawCube(V, ls, I, C_ROAD_WHT, {0, .04f, 52}, {.15f, .02f, 2});
    drawCube(V, ls, I, C_ROAD_WHT, {-.5f, .04f, 53}, {1, .02f, .15f});
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
    drawCar({.82f, .15f, .12f}, {-12, 0, 48}, 90); // Red car
    drawCar({.18f, .35f, .82f}, {-12, 0, 54}, 90); // Blue car
    drawCar({.92f, .92f, .90f}, {12, 0, 51}, -90); // White car
    drawCar({.12f, .52f, .22f}, {12, 0, 60}, -90); // Green car
    // Boom barrier (animated rotation)
    drawCube(V, ls, I, C_LAMP, {3, .5f, 71}, {.2f, 1, .2f}); // post
    {
        glm::mat4 bm = glm::translate(I, {3.0f, 1.1f, 71.0f});
        bm = glm::rotate(bm, glm::radians(-barrierAngle), {0, 0, 1});
        bm = glm::translate(bm, {-3.0f, 0, 0});
        drawCube(V, ls, bm, C_BARRIER_R, {0, 0, 0}, {6, .12f, .12f});
    }
    // Parking lamp posts
    drawCube(V, ls, I, C_LAMP, {-10, 3, 58}, {.15f, 6, .15f});
    drawCube(V, ls, I, C_LAMP, {-10, 6.2f, 58}, {.8f, .15f, .6f});
    drawCube(V, ls, I, C_LAMP, {10, 3, 58}, {.15f, 6, .15f});
    drawCube(V, ls, I, C_LAMP, {10, 6.2f, 58}, {.8f, .15f, .6f});

    // ============================================================
    // 3-STOREY MALL (X:-40 to 40, Z:-40 to 40, Y:0 to ~21.6)
    // ============================================================
    float M = MALL_HALF; // 40

    // ---- EXTERIOR WALLS (procedural brick texture) ----
    drawCubeProc(V, ls, I, {0, WH, -M}, {2*M, MH, WT});           // North
    drawCubeProc(V, ls, I, {-M, WH, 0}, {WT, MH, 2*M});           // West
    drawCubeProc(V, ls, I, {M, WH, 0}, {WT, MH, 2*M});            // East
    // South wall with entrance gap (X:-6 to 6)
    drawCubeProc(V, ls, I, {-23, WH, M}, {34, MH, WT});            // left of entrance
    drawCubeProc(V, ls, I, {23, WH, M}, {34, MH, WT});             // right of entrance
    drawCube(V, ls, I, C_EXT, {0, MH-1, M}, {12, 2, WT});          // strip above entrance

    // ---- FRONT FACADE POSTERS (kitkat + movie) ----
    drawCubeTextured(texCubeVAO, ls, I, glm::vec3(.95f), {-18, 6, M+0.2f}, {8, 6, .12f}, texKitkat, 1, 16);
    drawCubeTextured(texCubeVAO, ls, I, glm::vec3(.95f), {18, 6, M+0.2f}, {8, 6, .12f}, texMovie, 1, 16);

    // ---- ENTRANCE AWNING ----
    drawCube(V, ls, I, C_AWNING, {0, MH+.1f, M+2}, {16, .2f, 4});

    // ---- GLASS PANELS BESIDE ENTRANCE ----
    drawCube(V, ls, I, C_GLASS, {-5.5f, WH-.5f, M+.01f}, {1.5f, MH-1, .08f}, 128, .3f);
    drawCube(V, ls, I, C_GLASS, {5.5f, WH-.5f, M+.01f}, {1.5f, MH-1, .08f}, 128, .3f);

    // ---- GLASS ENTRANCE DOORS (sliding) ----
    {
        float edZ = M;
        float edHW = 5.5f;
        float sl = entranceDoorOffset * edHW * 0.5f;
        float ldX = -edHW*0.25f - sl;
        float rdX = edHW*0.25f + sl;
        drawCube(V, ls, I, C_GLASS*1.05f, {ldX, 3.5f, edZ+.02f}, {edHW*.48f, 7.f*.85f, .1f}, 128, .2f);
        drawCube(V, ls, I, C_GLASS*1.05f, {rdX, 3.5f, edZ+.02f}, {edHW*.48f, 7.f*.85f, .1f}, 128, .2f);
        drawCube(V, ls, I, C_LAMP, {0, FLOOR_H-.1f, edZ+.02f}, {edHW+.4f, .2f, .15f});
        drawCube(V, ls, I, C_LAMP, {-edHW*.5f-.1f, 3.5f, edZ+.02f}, {.15f, 7, .15f});
        drawCube(V, ls, I, C_LAMP, {edHW*.5f+.1f, 3.5f, edZ+.02f}, {.15f, 7, .15f});
        drawCube(V, ls, I, {0.1f,0.9f,0.3f}, {edHW*.5f+1, 1.2f, edZ+.3f}, {.1f,.1f,.03f});
    }

    // ---- WINDOWS ON EAST/WEST (3 levels) ----
    for (int fl = 0; fl < 3; fl++) {
        float wy = FLOOR_Y[fl] + 3.5f;
        for (int i = 0; i < 6; i++) {
            float wz = -35 + i * 12.f;
            drawCube(V, ls, I, C_GLASS, {-M-.01f, wy, wz}, {.08f, 2.5f, 4}, 128, .3f);
            drawCube(V, ls, I, C_GLASS, {M+.01f, wy, wz}, {.08f, 2.5f, 4}, 128, .3f);
        }
    }

    // ============================================================
    // FLOOR SLABS & CEILINGS
    // ============================================================
    // Ground floor
    ls.setBool("blendWithColor", true);
    drawCubeTextured(texCubeVAO, ls, I, C_FLOOR, {0, -.02f, 0}, {79.5f, .08f, 79.5f}, texFloor, 12.0f, 32, .9f);
    ls.setBool("blendWithColor", false);

    // Floor slabs for 1st and 2nd floor (with atrium + staircase + elevator cutouts)
    for (int fl = 1; fl < 3; fl++) {
        float fy = FLOOR_Y[fl] - SLAB/2;
        float hh = HOLE_HALF; // 6
        // Staircase hole bounds for this floor
        float shX1 = (fl==1) ? 16.f : 23.f; // left edge of stair hole
        float shX2 = (fl==1) ? 24.f : 31.f; // right edge of stair hole
        float shZ1 = 7.f, shZ2 = 18.f;      // Z range of stair hole

        ls.setBool("blendWithColor", true);
        // Left strip (X:-40 to -6) — unchanged, no obstructions
        drawCubeTextured(texCubeVAO, ls, I, C_FLOOR, {-(M/2+hh/2), fy, 0}, {M-hh, SLAB, 2*M}, texFloor, 8, 32, .9f);
        // Front strip (X:-6 to 6, Z:6 to 40)
        drawCubeTextured(texCubeVAO, ls, I, C_FLOOR, {0, fy, (M/2+hh/2)}, {2*hh, SLAB, M-hh}, texFloor, 8, 32, .9f);
        // Back strip (X:-6 to 6, Z:-40 to -6) with elevator shaft hole (X:-2 to 2, Z:-15 to -11)
        drawCubeTextured(texCubeVAO, ls, I, C_FLOOR, {0, fy, (-M-15.f)/2.f}, {2*hh, SLAB, M-15.f}, texFloor, 8, 32, .9f);
        drawCubeTextured(texCubeVAO, ls, I, C_FLOOR, {0, fy, (-11.f-hh)/2.f}, {2*hh, SLAB, 11.f-hh}, texFloor, 8, 32, .9f);
        drawCubeTextured(texCubeVAO, ls, I, C_FLOOR, {-4.f, fy, -13}, {4, SLAB, 4}, texFloor, 8, 32, .9f);
        drawCubeTextured(texCubeVAO, ls, I, C_FLOOR, {4.f, fy, -13}, {4, SLAB, 4}, texFloor, 8, 32, .9f);
        // Right strip (X:6 to 40) — split around staircase hole
        float rCX = (hh+M)/2.f, rW = M-hh; // center=23, width=34
        drawCubeTextured(texCubeVAO, ls, I, C_FLOOR, {rCX, fy, (-M+shZ1)/2.f}, {rW, SLAB, shZ1+M}, texFloor, 8, 32, .9f);
        drawCubeTextured(texCubeVAO, ls, I, C_FLOOR, {rCX, fy, (M+shZ2)/2.f}, {rW, SLAB, M-shZ2}, texFloor, 8, 32, .9f);
        drawCubeTextured(texCubeVAO, ls, I, C_FLOOR, {(hh+shX1)/2.f, fy, (shZ1+shZ2)/2.f}, {shX1-hh, SLAB, shZ2-shZ1}, texFloor, 8, 32, .9f);
        drawCubeTextured(texCubeVAO, ls, I, C_FLOOR, {(shX2+M)/2.f, fy, (shZ1+shZ2)/2.f}, {M-shX2, SLAB, shZ2-shZ1}, texFloor, 8, 32, .9f);
        ls.setBool("blendWithColor", false);

        // Glass railing around atrium cutout
        float rH = 1.2f, rY = FLOOR_Y[fl] + rH/2;
        drawCube(V, ls, I, C_GLASS, {-hh, rY, 0}, {.08f, rH, 2*hh}, 128, .3f);
        drawCube(V, ls, I, C_GLASS, {hh, rY, 0}, {.08f, rH, 2*hh}, 128, .3f);
        drawCube(V, ls, I, C_GLASS, {0, rY, -hh}, {2*hh, rH, .08f}, 128, .3f);
        drawCube(V, ls, I, C_GLASS, {0, rY, hh}, {2*hh, rH, .08f}, 128, .3f);
        drawCube(V, ls, I, C_LAMP, {-hh, FLOOR_Y[fl]+rH, 0}, {.1f, .05f, 2*hh});
        drawCube(V, ls, I, C_LAMP, {hh, FLOOR_Y[fl]+rH, 0}, {.1f, .05f, 2*hh});
        drawCube(V, ls, I, C_LAMP, {0, FLOOR_Y[fl]+rH, -hh}, {2*hh, .05f, .1f});
        drawCube(V, ls, I, C_LAMP, {0, FLOOR_Y[fl]+rH, hh}, {2*hh, .05f, .1f});
    }

    // Ceilings for each floor (with matching staircase & elevator holes)
    for (int fl = 0; fl < 3; fl++) {
        float cy = CEIL_Y[fl];
        if (fl == 2) {
            drawCube(V, ls, I, C_CEILING, {0, cy, 0}, {79.5f, .15f, 79.5f}); // roof
        } else {
            // Staircase hole in ceiling — G→1 at X=16-24, 1→2 at X=23-31
            float shX1 = (fl==0) ? 16.f : 23.f;
            float shX2 = (fl==0) ? 24.f : 31.f;
            float shZ1 = 7.f, shZ2 = 18.f;
            float hh = HOLE_HALF;
            // Left ceiling strip
            drawCube(V, ls, I, C_CEILING, {-(M/2+hh/2), cy, 0}, {M-hh, .1f, 2*M});
            // Front ceiling strip
            drawCube(V, ls, I, C_CEILING, {0, cy, (M/2+hh/2)}, {2*hh, .1f, M-hh});
            // Back ceiling strip (with elevator shaft hole)
            drawCube(V, ls, I, C_CEILING, {0, cy, (-M-15.f)/2.f}, {2*hh, .1f, M-15.f});
            drawCube(V, ls, I, C_CEILING, {0, cy, (-11.f-hh)/2.f}, {2*hh, .1f, 11.f-hh});
            drawCube(V, ls, I, C_CEILING, {-4.f, cy, -13}, {4, .1f, 4});
            drawCube(V, ls, I, C_CEILING, {4.f, cy, -13}, {4, .1f, 4});
            // Right ceiling strip (with staircase hole)
            float rCX = (hh+M)/2.f, rW = M-hh;
            drawCube(V, ls, I, C_CEILING, {rCX, cy, (-M+shZ1)/2.f}, {rW, .1f, shZ1+M});
            drawCube(V, ls, I, C_CEILING, {rCX, cy, (M+shZ2)/2.f}, {rW, .1f, M-shZ2});
            drawCube(V, ls, I, C_CEILING, {(hh+shX1)/2.f, cy, (shZ1+shZ2)/2.f}, {shX1-hh, .1f, shZ2-shZ1});
            drawCube(V, ls, I, C_CEILING, {(shX2+M)/2.f, cy, (shZ1+shZ2)/2.f}, {M-shX2, .1f, shZ2-shZ1});
        }
    }

    // ============================================================
    // CENTRAL ATRIUM (X:-10 to 10, Z:-10 to 10) — dramatic open space
    // ============================================================
    // (The atrium is just empty space — no geometry needed except the cutouts above)

    // ============================================================
    // DUAL ESCALATORS (Ground → 1st Floor, in atrium center)
    // Left=UP, Right=DOWN, ~33 degree incline
    // Bottom at Z=5.5, Top at Z=-5.5, Rise: 0 to 7.3
    // ============================================================
    {
        float esBottomZ = 5.5f, esTopZ = -5.5f;
        float esRise = FLOOR_Y[1]; // 7.3
        float esLen = esBottomZ - esTopZ; // 11
        float esAngle = atan2(esRise, esLen); // ~33 deg
        float esInclineLen = sqrt(esRise*esRise + esLen*esLen);
        int numSteps = 24;
        float stepIncline = esInclineLen / numSteps;
        float stepH = esRise / numSteps;
        float stepZ = esLen / numSteps;
        float esW = 2.5f; // width of each escalator
        float esGap = 0.5f;

        // For each escalator (0=left/UP, 1=right/DOWN)
        for (int esc = 0; esc < 2; esc++) {
            float esX = (esc == 0) ? -(esW/2 + esGap/2) : (esW/2 + esGap/2);
            float dir = (esc == 0) ? 1.0f : -1.0f; // UP or DOWN step direction

            // Side panels (glass)
            float panelCX = esX;
            float panelY = esRise/2;
            drawCube(V, ls, I, C_GLASS, {panelCX - esW/2, panelY, 0}, {.08f, esRise+1, esLen+1}, 128, .25f);
            drawCube(V, ls, I, C_GLASS, {panelCX + esW/2, panelY, 0}, {.08f, esRise+1, esLen+1}, 128, .25f);

            // Handrails (black rubber)
            drawCube(V, ls, I, C_TIRE, {panelCX - esW/2, esRise+.5f, 0}, {.1f, .06f, esLen});
            drawCube(V, ls, I, C_TIRE, {panelCX + esW/2, esRise+.5f, 0}, {.1f, .06f, esLen});

            // Inclined truss structure (triangular support under steps)
            for (int ti = 0; ti < 6; ti++) {
                float tt = (float)ti / 5;
                float tZ = esBottomZ - tt * esLen;
                float tY = tt * esRise;
                drawCube(V, ls, I, C_LAMP*.7f, {panelCX, tY*.5f, tZ}, {esW-.3f, tY+.1f, .08f});
            }

            // Animated steps
            float animOffset = fmod((float)glfwGetTime() * 0.3f, 1.0f) * stepH * dir;
            for (int i = 0; i < numSteps; i++) {
                float t = (float)i / numSteps;
                float sZ, sY;
                if (esc == 0) { // UP: bottom(south) to top(north)
                    sZ = esBottomZ - t * esLen + animOffset * (esLen/esRise);
                    sY = t * esRise + animOffset;
                } else { // DOWN: top(north) to bottom(south)
                    sZ = esTopZ + t * esLen - animOffset * (esLen/esRise);
                    sY = esRise - t * esRise - animOffset;
                }
                sY = glm::clamp(sY, 0.0f, esRise);
                sZ = glm::clamp(sZ, esTopZ, esBottomZ);
                // Step surface (dark gray with ribbed texture feel)
                drawCube(V, ls, I, C_STAIR*1.05f, {panelCX, sY+.05f, sZ}, {esW-.4f, .1f, stepZ*.85f});
            }

            // Entry/exit platforms
            drawCube(V, ls, I, C_STAIR, {panelCX, .05f, esBottomZ+1}, {esW, .1f, 2}); // bottom entry
            drawCube(V, ls, I, C_STAIR, {panelCX, esRise+.05f, esTopZ-1}, {esW, .1f, 2}); // top exit
        }

        // Thin metal divider strip between escalators (NOT a solid wall)
        drawCube(V, ls, I, C_LAMP, {0, .5f, 0}, {.08f, 1.0f, esLen}); // lower strip
        drawCube(V, ls, I, C_LAMP, {0, esRise+.3f, 0}, {.08f, .06f, esLen}); // top rail
    }

    // ============================================================
    // ELEVATOR / LIFT (North side of atrium, X=0, Z=-13)
    // 3-floor vertical shaft with glass front
    // ============================================================
    {
        float eX = 0, eZ = -13.f;
        float shaftH = MH;
        // Shaft walls
        drawCube(V, ls, I, C_GLASS, {eX, shaftH/2, eZ+1.75f}, {3.5f, shaftH, .08f}, 128, .2f); // South (glass, facing atrium)
        drawCube(V, ls, I, C_CORR, {eX-1.75f, shaftH/2, eZ}, {.08f, shaftH, 3.5f});   // West
        drawCube(V, ls, I, C_CORR, {eX+1.75f, shaftH/2, eZ}, {.08f, shaftH, 3.5f});   // East
        drawCube(V, ls, I, C_CORR, {eX, shaftH/2, eZ-1.75f}, {3.5f, shaftH, .08f});   // North
        // Shaft corner rails
        drawCube(V, ls, I, C_LAMP, {eX-1.7f, shaftH/2, eZ+1.7f}, {.05f,shaftH,.05f});
        drawCube(V, ls, I, C_LAMP, {eX+1.7f, shaftH/2, eZ+1.7f}, {.05f,shaftH,.05f});
        drawCube(V, ls, I, C_LAMP, {eX-1.7f, shaftH/2, eZ-1.7f}, {.05f,shaftH,.05f});
        drawCube(V, ls, I, C_LAMP, {eX+1.7f, shaftH/2, eZ-1.7f}, {.05f,shaftH,.05f});

        // Cabin (3x3x3 box)
        float cabY = elevatorY;
        drawCube(V, ls, I, C_STAIR, {eX, cabY+.05f, eZ}, {3.f, .1f, 3.f}); // floor
        drawCube(V, ls, I, C_CEILING, {eX, cabY+3.f, eZ}, {3.f, .1f, 3.f}); // ceiling
        drawCube(V, ls, I, C_CORR, {eX-1.45f, cabY+1.5f, eZ}, {.08f, 3, 3}); // left wall
        drawCube(V, ls, I, C_CORR, {eX+1.45f, cabY+1.5f, eZ}, {.08f, 3, 3}); // right wall
        drawCube(V, ls, I, C_CORR, {eX, cabY+1.5f, eZ-1.45f}, {3, 3, .08f}); // back wall

        // Sliding doors on south face (Z = eZ+1.5)
        float doorHalf = 1.4f * (1.f - elevatorDoorOffset);
        if (doorHalf > 0.05f) {
            drawCube(V, ls, I, C_GLASS, {eX-doorHalf*.5f-.05f, cabY+1.5f, eZ+1.5f}, {doorHalf, 2.8f, .06f}, 128, .25f);
            drawCube(V, ls, I, C_GLASS, {eX+doorHalf*.5f+.05f, cabY+1.5f, eZ+1.5f}, {doorHalf, 2.8f, .06f}, 128, .25f);
        }

        // Inside panel (back wall buttons)
        drawCube(V, ls, I, C_LAMP, {eX+.5f, cabY+1.5f, eZ-1.4f}, {.2f, .5f, .05f});
        drawCube(V, ls, I, {.2f,.8f,.2f}, {eX+.5f, cabY+1.7f, eZ-1.38f}, {.08f,.08f,.03f}); // G
        drawCube(V, ls, I, {.8f,.8f,.2f}, {eX+.5f, cabY+1.5f, eZ-1.38f}, {.08f,.08f,.03f}); // 1
        drawCube(V, ls, I, {.8f,.2f,.2f}, {eX+.5f, cabY+1.3f, eZ-1.38f}, {.08f,.08f,.03f}); // 2

        // Call button panels on each floor (outside, south side)
        for (int fl = 0; fl < 3; fl++) {
            float btnY = FLOOR_Y[fl] + 1.2f;
            drawCube(V, ls, I, C_LAMP, {eX+2, btnY, eZ+1.9f}, {.2f, .4f, .1f});
            drawCube(V, ls, I, {.2f,.8f,.2f}, {eX+2, btnY+.1f, eZ+1.96f}, {.06f,.06f,.03f});
            drawCube(V, ls, I, {.8f,.2f,.2f}, {eX+2, btnY-.1f, eZ+1.96f}, {.06f,.06f,.03f});
        }
    }

    // ============================================================
    // STAIRCASE (South side of atrium, X=20, Z=10-15)
    // Switchback: G→1 goes south-to-north, 1→2 goes north-to-south
    // ============================================================
    {
        float stW = 4.0f; // width in X (X=18 to 22)
        // Flight G→1: Z goes from 15(bottom) to 10(top), X=18 to 22
        int ns = 14;
        for (int i = 0; i < ns; i++) {
            float sz = 15.f - i * (5.f/ns);
            float sy = i * (FLOOR_Y[1] / ns);
            drawCube(V, ls, I, C_STAIR, {20, sy + .25f, sz}, {stW, .5f, 5.f/ns * .95f});
        }
        // Landing at top of flight 1
        drawCube(V, ls, I, C_STAIR, {20, FLOOR_Y[1]-.15f, 9}, {stW, SLAB, 2});

        // Flight 1→2: Z goes from 10(bottom) to 15(top), X=25 to 29 (parallel offset)
        for (int i = 0; i < ns; i++) {
            float sz = 10.f + i * (5.f/ns);
            float sy = FLOOR_Y[1] + i * ((FLOOR_Y[2]-FLOOR_Y[1]) / ns);
            drawCube(V, ls, I, C_STAIR, {27, sy + .25f, sz}, {stW, .5f, 5.f/ns * .95f});
        }
        // Landing at top of flight 2
        drawCube(V, ls, I, C_STAIR, {27, FLOOR_Y[2]-.15f, 16}, {stW, SLAB, 2});

        // Railings (along both flights)
        drawCube(V, ls, I, C_LAMP, {18, FLOOR_Y[1]/2+1, 12.5f}, {.08f, FLOOR_Y[1]+2, .08f});
        drawCube(V, ls, I, C_LAMP, {22, FLOOR_Y[1]/2+1, 12.5f}, {.08f, FLOOR_Y[1]+2, .08f});
        drawCube(V, ls, I, C_LAMP, {25, FLOOR_Y[1]+3, 12.5f}, {.08f, FLOOR_H+2, .08f});
        drawCube(V, ls, I, C_LAMP, {29, FLOOR_Y[1]+3, 12.5f}, {.08f, FLOOR_H+2, .08f});
        // Handrail bars
        drawCube(V, ls, I, C_LAMP, {18, FLOOR_Y[1]+.5f, 12.5f}, {.04f, .04f, 6});
        drawCube(V, ls, I, C_LAMP, {22, FLOOR_Y[1]+.5f, 12.5f}, {.04f, .04f, 6});
        drawCube(V, ls, I, C_LAMP, {25, FLOOR_Y[2]+.5f, 12.5f}, {.04f, .04f, 6});
        drawCube(V, ls, I, C_LAMP, {29, FLOOR_Y[2]+.5f, 12.5f}, {.04f, .04f, 6});
    }

    // ============================================================
    // GROUND FLOOR SHOPS (perimeter, 10 shops)
    // ============================================================
    {
        struct ShopDef { float cx, cz, sx, sz; const char* name; glm::vec3 wallCol; unsigned int banner; };
        ShopDef shops[] = {
            {-30, 32, 16, 12, "Tech", C_TECH_WALL, texTech},
            {-14, 32, 8, 12, "Phones", C_TECH_WALL, texTech},
            {14, 32, 8, 12, "Fashion", C_FASH_WALL, texFashion},
            {30, 32, 16, 12, "Shoes", C_FASH_WALL, texFashion},
            {-34, 0, 8, 20, "Books", C_BOOK_WALL, 0},
            {-34, -25, 8, 20, "Prayer", C_PRAY_WALL, 0},
            {34, 0, 8, 20, "Gems", C_GEM_WALL, texGems},
            {34, -25, 8, 20, "Food", C_STALL_RED, texFood},
            {-25, -34, 20, 8, "Cafe", C_STALL_YEL, texFood},
            {25, -34, 20, 8, "Games", C_STALL_GRN, 0},
        };
        int nShops = 10;
        for (int si = 0; si < nShops; si++) {
            auto& sh = shops[si];
            float fy = FLOOR_H / 2;
            // Back wall
            drawCube(V, ls, I, sh.wallCol, {sh.cx, fy, sh.cz}, {sh.sx, FLOOR_H, WT});
            // Side walls (short sides)
            float hw = sh.sx / 2, hd = sh.sz / 2;
            // Floor
            drawCube(V, ls, I, sh.wallCol * .9f, {sh.cx, .02f, sh.cz - hd + sh.sz/2 * (sh.sz > sh.sx ? 0 : 0)}, {sh.sx, .04f, sh.sz});
            // Glass facade (front, with gap for door)
            float facadeZ = (sh.sz > sh.sx) ? sh.cz : sh.cz - hd;
            float glassH = FLOOR_H * .7f;
            // Simple glass front
            if (sh.sx > sh.sz) { // wide shop (N/S wall)
                drawCube(V, ls, I, C_GLASS, {sh.cx - hw*.6f, glassH/2, sh.cz-hd}, {sh.sx*.35f, glassH, .08f}, 128, .3f);
                drawCube(V, ls, I, C_GLASS, {sh.cx + hw*.6f, glassH/2, sh.cz-hd}, {sh.sx*.35f, glassH, .08f}, 128, .3f);
            } else { // deep shop (E/W wall)
                drawCube(V, ls, I, C_GLASS, {sh.cx+(sh.cx>0?-1:1)*hw, glassH/2, sh.cz - hd*.6f}, {.08f, glassH, sh.sz*.35f}, 128, .3f);
                drawCube(V, ls, I, C_GLASS, {sh.cx+(sh.cx>0?-1:1)*hw, glassH/2, sh.cz + hd*.6f}, {.08f, glassH, sh.sz*.35f}, 128, .3f);
            }
            // Shop sign / banner above entrance
            if (sh.banner != 0) {
                if (sh.sx > sh.sz)
                    drawCubeTextured(texCubeVAO, ls, I, glm::vec3(.95f), {sh.cx, FLOOR_H-.5f, sh.cz-hd-.08f}, {sh.sx*.6f, 1.5f, .12f}, sh.banner, 1, 16);
                else
                    drawCubeTextured(texCubeVAO, ls, I, glm::vec3(.95f), {sh.cx+(sh.cx>0?-1:1)*hw-.08f, FLOOR_H-.5f, sh.cz}, {.12f, 1.5f, sh.sz*.6f}, sh.banner, 1, 16);
            }
        }
    }

    // ============================================================
    // WASHROOM (NW area, X:-38 to -28, Z:-8 to 0)
    // ============================================================
    {
        float wX = -33, wZ = -4;
        drawCube(V, ls, I, C_WASH, {wX, .03f, wZ}, {10, .04f, 8}); // floor
        drawCube(V, ls, I, C_WASH*.95f, {wX, FLOOR_H/2, wZ-4}, {10, FLOOR_H, WT}); // back wall
        drawCube(V, ls, I, C_WASH*.95f, {wX, FLOOR_H/2, wZ+4}, {10, FLOOR_H, WT}); // front wall with gap
        // Stall partitions
        for (int i = 0; i < 4; i++) {
            float px = -37 + i * 2.5f;
            drawCube(V, ls, I, C_WASH*.88f, {px, 1.2f, wZ}, {.08f, 2.2f, 5});
            // Stall doors (partially open for visual)
            if (i < 3) {
                float doorAngle = (i == 1) ? 30.f : 0.f;
                glm::mat4 dm = glm::translate(I, {px+1.25f, 1.0f, wZ+2.2f});
                if (doorAngle > 0) dm = glm::rotate(dm, glm::radians(doorAngle), {0,1,0});
                drawCube(V, ls, dm, C_WASH*.85f, {0, 0, 0}, {2.3f, 1.8f, .06f});
                // Door knob
                drawCube(V, ls, dm, C_LAMP, {.9f, 0, .04f}, {.06f, .06f, .04f});
            }
        }
        // Washroom sign
        drawCube(V, ls, I, C_LAMP, {wX, FLOOR_H-.3f, wZ+4.1f}, {2, .6f, .08f});
    }

    // ============================================================
    // 1ST FLOOR — Open floor, glass railing, perimeter (mostly empty)
    // ============================================================
    // (Floor slab and glass railing already drawn in the floor section above)
    // Add some perimeter walls for structure
    for (int fl = 1; fl < 3; fl++) {
        float baseY = FLOOR_Y[fl];
        float midY = baseY + FLOOR_H/2;
        // Interior perimeter walls (lower half only, for visual containment)
        // These are thinner guide walls inside the building at each floor
        if (fl == 1) {
            // Some corridor walls on 1st floor
            drawCube(V, ls, I, C_CORR, {-30, midY, 30}, {16, FLOOR_H, WT});
            drawCube(V, ls, I, C_CORR, {30, midY, 30}, {16, FLOOR_H, WT});
            drawCube(V, ls, I, C_CORR, {-30, midY, -30}, {16, FLOOR_H, WT});
            drawCube(V, ls, I, C_CORR, {30, midY, -30}, {16, FLOOR_H, WT});
        }
    }

    // ============================================================
    // FOOD COURT (Ground floor, north area Z:-38 to -28)
    // ============================================================
    {
        drawCube(V, ls, I, C_FOOD_FLOOR, {0, .03f, -33}, {79.5f, .04f, 14});
        // Food stalls along north wall
        float stallX[] = {-30, -15, 5, 25};
        glm::vec3 stallC[] = {C_STALL_RED, C_STALL_YEL, C_STALL_GRN, C_STALL_ORG};
        for (int i = 0; i < 4; i++) {
            drawCube(V, ls, I, stallC[i], {stallX[i], 3.5f, -39}, {8, FLOOR_H, 1});
            drawCube(V, ls, I, C_COUNTER, {stallX[i], .55f, -37.5f}, {6, 1.1f, 2});
        }
        // Seating tables with bezier tops
        for (int r = 0; r < 2; r++)
            for (int c = 0; c < 5; c++) {
                float tx = -30+c*13.f, tz = -30-r*4.f;
                glm::mat4 ftm = glm::translate(I, {tx, 0.75f, tz});
                bezierTable->draw(ls, ftm, C_TABLE, 64.0f);
                drawCube(V, ls, I, C_TABLE*.7f, {tx,.37f,tz}, {.08f,.72f,.08f});
                drawCube(V, ls, I, C_CHAIR, {tx-.8f,.45f,tz}, {.4f,.05f,.4f});
                drawCube(V, ls, I, C_CHAIR, {tx-.8f,.7f,tz-.18f}, {.4f,.45f,.05f});
                drawCube(V, ls, I, C_CHAIR, {tx+.8f,.45f,tz}, {.4f,.05f,.4f});
                drawCube(V, ls, I, C_CHAIR, {tx+.8f,.7f,tz+.18f}, {.4f,.45f,.05f});
            }
    }

    // ============================================================
    // ROOF PARAPET
    // ============================================================
    float pY = MH + .5f;
    drawCube(V, ls, I, C_PARAPET, {0, pY, -M}, {2*M, 1, .3f});
    drawCube(V, ls, I, C_PARAPET, {0, pY, M}, {2*M, 1, .3f});
    drawCube(V, ls, I, C_PARAPET, {-M, pY, 0}, {.3f, 1, 2*M});
    drawCube(V, ls, I, C_PARAPET, {M, pY, 0}, {.3f, 1, 2*M});

    // LIGHT FIXTURES
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

// (end of old drawScene removed)

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
    // 3-Storey Mall height constraints and overrides
    float x = basic_camera.eye.x, z = basic_camera.eye.z;

    // Escalator ride override
    bool onEscalator = (x >= -22.5f && x <= -17.5f && z >= -6 && z <= 6 && basic_camera.eye.y < FLOOR_Y[1]+1);
    if (!onEscalator) {
        // Normal height constraints
        float targetY = 1.7f;
        // Check which floor we are on
        if (basic_camera.eye.y > FLOOR_Y[2]) targetY = FLOOR_Y[2] + 1.7f;
        else if (basic_camera.eye.y > FLOOR_Y[1]) targetY = FLOOR_Y[1] + 1.7f;

        // Inside Elevator override
        bool inLift = (x >= -2 && x <= 2 && z >= -15 && z <= -11);
        if (inLift) {
            targetY = elevatorY + 1.7f;
        }
        else {
            // Staircase constraints
            // G->1 (X:18-22, Z:10-15) South-to-North
            bool onStairG1 = (x >= 18 && x <= 22 && z >= 10 && z <= 15);
            if (onStairG1) {
                float t = (15.f - z) / 5.f; // 0 at bottom, 1 at top
                t = glm::clamp(t, 0.f, 1.f);
                targetY = FLOOR_Y[0] + t * (FLOOR_Y[1]-FLOOR_Y[0]) + 1.7f;
            }
            // Landing 1
            if (x >= 18 && x <= 22 && z >= 8 && z < 10) targetY = FLOOR_Y[1] + 1.7f;

            // 1->2 (X:25-29, Z:10-15) North-to-South
            bool onStair12 = (x >= 25 && x <= 29 && z >= 10 && z <= 15);
            if (onStair12) {
                float t = (z - 10.f) / 5.f; // 0 at bottom, 1 at top
                t = glm::clamp(t, 0.f, 1.f);
                targetY = FLOOR_Y[1] + t * (FLOOR_Y[2]-FLOOR_Y[1]) + 1.7f;
            }
        }
        // Smooth height transition
        basic_camera.eye.y = glm::mix(basic_camera.eye.y, targetY, deltaTime * 10.0f);
    } else {
        // On escalator: automatic movement logic
        // Use left escalator for UP, right for DOWN
        int esc = (x < -20.0f) ? 0 : 1; // 0=UP, 1=DOWN
        float dir = (esc == 0) ? 1.0f : -1.0f;
        // Esc rise from Z=5.5 to Z=-5.5
        float t = (5.5f - z) / 11.0f;
        t = glm::clamp(t, 0.f, 1.f);
        basic_camera.eye.y = t * FLOOR_Y[1] + 1.7f;
        if (esc == 1) basic_camera.eye.y = (1.0f-t) * FLOOR_Y[1] + 1.7f; // Down goes other way
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
        // Call elevator to nearest floor if player is nearby
        float x = basic_camera.eye.x, z = basic_camera.eye.z, y = basic_camera.eye.y;
        if (x >= -5 && x <= 5 && z >= -16 && z <= -10) {
            int targetFl = 0;
            if (y > FLOOR_Y[2]) targetFl = 2;
            else if (y > FLOOR_Y[1]) targetFl = 1;
            
            if (elevatorState == ELEV_IDLE || elevatorState == ELEV_DOOR_OPEN) {
                elevatorTargetFloor = targetFl;
                if (elevatorCurrentFloor == targetFl) {
                    elevatorState = ELEV_DOOR_OPENING;
                    cout << "Elevator: Opening doors on Floor " << targetFl << endl;
                } else {
                    elevatorState = ELEV_DOOR_CLOSING;
                    cout << "Elevator: Called to Floor " << targetFl << endl;
                }
            }
        }
    }
    // L+Key for elevator floor destination
    if (lKeyHeld) {
        if (key == GLFW_KEY_G) {
            elevatorTargetFloor = 0;
            elevatorState = ELEV_DOOR_CLOSING;
            cout << "Elevator going to Ground Floor" << endl;
        } else if (key == GLFW_KEY_1) {
            elevatorTargetFloor = 1;
            elevatorState = ELEV_DOOR_CLOSING;
            cout << "Elevator going to 1st Floor" << endl;
        } else if (key == GLFW_KEY_2) {
            elevatorTargetFloor = 2;
            elevatorState = ELEV_DOOR_CLOSING;
            cout << "Elevator going to 2nd Floor" << endl;
        }
        return; // Prevent triggering G entrance door
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
    if (key == GLFW_KEY_G && !lKeyHeld)
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
