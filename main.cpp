
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
#include "cylinder.h"
#include "stb_image.h"
#include <iostream>
#include <vector>
#include "cylinder.h"
#include "human.h"
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
void drawCubeProcShop(unsigned int &v, Shader &s, glm::mat4 pm, glm::vec3 c, glm::vec3 p, glm::vec3 sc);
unsigned int loadTexture(const char *path);
void drawScene(unsigned int &, unsigned int &, Shader &, Shader &, glm::mat4, glm::mat4);

// Food Court declarations
void drawFoodShop(unsigned int &v, Shader &s, glm::mat4 pm, glm::vec3 p, glm::vec3 color, float roty, unsigned int shopTex);
void drawChair(unsigned int &v, Shader &s, glm::mat4 pm, glm::vec3 p, float roty);
void drawDiningSet(unsigned int &v, Shader &s, glm::mat4 pm, glm::vec3 p);
void drawWashroom(unsigned int &v, Shader &s, glm::mat4 pm, glm::vec3 p);
void drawFoodCourt(unsigned int &v, Shader &s, glm::mat4 pm);

// 1st Floor Atrium and Retail Space
void drawBench(unsigned int &v, Shader &s, glm::mat4 pm, glm::vec3 p, float roty);
void drawAtrium(unsigned int &v, Shader &s, glm::mat4 pm);
void drawFirstFloorShop(unsigned int &v, Shader &s, glm::mat4 pm, glm::vec3 p, glm::vec3 color, float roty, const char* name);

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
const float TOTAL_H = 3 * FLOOR_H + 2 * SLAB;                                 // ~21.6
const float FLOOR_Y[] = {0.0f, FLOOR_H + SLAB, 2 * (FLOOR_H + SLAB)};         // 0, 7.3, 14.6
const float CEIL_Y[] = {FLOOR_H, 2 * FLOOR_H + SLAB, 3 * FLOOR_H + 2 * SLAB}; // 7, 14.3, 21.6
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
const glm::vec3 C_WOOD(0.5f, 0.35f, 0.2f); // wood furniture
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
    // Central corridor (entrance to atrium)
    {0, 4.5f, 30},
    {0, 4.5f, 5},
    // Tech Shop (NW)
    {-24, 4.5f, -20},
    {-24, 4.5f, -32},
    // Clothing Shop (NE)
    {24, 4.5f, -20},
    {24, 4.5f, -32},
    // Escalator area
    {-5, 4.5f, 20},
    {5, 4.5f, 20},
    // Atrium
    {0, 4.5f, -14},
    // Staircases + Lift + Washroom
    {-18, 4.5f, 16},
    {18, 4.5f, 16},
    {-32, 4.5f, 14},
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
float timeOfDay = 10.0f;    // 0-24 hours, start at 10 AM
float dayNightSpeed = 0.0f; // hours per second (0 = paused, press N to cycle)
bool dayNightCycleActive = false;

// Spot Lights (store display highlights)
const int NUM_SPOT_LIGHTS = 4;
SpotLight *spotLights_arr[NUM_SPOT_LIGHTS] = {};

// Elevator state machine (3-floor)
enum ElevatorState
{
    ELEV_IDLE,
    ELEV_DOOR_OPENING,
    ELEV_DOOR_OPEN,
    ELEV_DOOR_CLOSING,
    ELEV_MOVING
};
ElevatorState elevatorState = ELEV_IDLE;
float elevatorY = 0.0f;
float elevatorDoorOffset = 0.0f; // 0=closed, 1=open
float elevatorTimer = 0.0f;
int elevatorTargetFloor = 0;
int elevatorCurrentFloor = 0;
const float ELEV_SPEED = 3.0f;
bool playerInsideLift = false;
// Lift bounds: X:-37..-31, Z:22..28, center (-34,25)
const float LIFT_X = -34.0f, LIFT_Z = 25.0f;

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

// Prayer room glass door
bool prayerDoorOpen = false;
float prayerDoorOffset = 0.0f; // 0=closed, 1=open

// Texture IDs
unsigned int texCubeVAO = 0;
unsigned int texFloor = 0, texFashion = 0, texTech = 0, texGems = 0, texFood = 0, texTreeLeaf = 0, texTreeBark = 0, texGrass = 0;
unsigned int texKitkat = 0, texMovie = 0, texIceCream = 0, texCoke = 0, texColdDrink = 0;
unsigned int texBiriyani = 0, texCake = 0, texCoffee = 0, texMcdonalds = 0, texPizza = 0, texWood = 0, texPrayerTiles = 0, texMahrib = 0;

// Spheres
Sphere *sphWheel = nullptr;
Sphere *sphHub = nullptr;
Sphere *sphLeaf[5] = {};

// Pyramid
Pyramid *treeCone = nullptr;

// Fractal Trees
const int NUM_TREES = 10;
FractalTree *fractalTrees[NUM_TREES] = {};

// Bezier furniture objects
BezierRevolvedSurface *bezierTable = nullptr;
BezierArch *bezierArch = nullptr;
BezierRevolvedSurface *hairSurface = nullptr;

// Humans
std::vector<Human> patrons;
std::vector<Human> shopkeepers;
std::vector<Human> f1Patrons;
std::vector<Human> f1Shopkeepers;

// 1st Floor Atrium Tree
FractalTree* atriumTree = nullptr;

// Cylinder
Cylinder *cylinderObj = nullptr;

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
    cout << "  E - Call Elevator / Open door (when near lift)" << endl;
    cout << "  Shift+0 / Shift+1 / Shift+2 - Lift to G/1st/2nd floor" << endl;
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
    texIceCream = loadTexture("icecream.jpg");
    texCoke = loadTexture("cocacola.png");
    texColdDrink = loadTexture("colddrink.jpg");
    texBiriyani = loadTexture("biriyani.jpg");
    texCake = loadTexture("cake.jpg");
    texCoffee = loadTexture("coffee.jpg");
    texMcdonalds = loadTexture("mcdonalds.jpg");
    texPizza = loadTexture("pizza.jpg");
    texWood = loadTexture("woodenTexture.jpg");
    texPrayerTiles = loadTexture("prayerTiles.jpg");
    texMahrib = loadTexture("mahrib.jpg");

    for (int i = 0; i < NUM_POINT_LIGHTS; i++)
    {
        glm::vec3 a(0.1f), d(0.6f, 0.58f, 0.55f), sp(0.5f);
        pointLights[i] = new PointLight(plPos[i].x, plPos[i].y, plPos[i].z, a.x, a.y, a.z, d.x, d.y, d.z, sp.x, sp.y, sp.z, 1, .09f, .032f, i);
    }

    // Create Spot Lights (store display highlights)
    // Tech store (NW) - 2 spots
    spotLights_arr[0] = new SpotLight(
        glm::vec3(-20.0f, 4.5f, -20), glm::vec3(0, -1, 0),
        glm::vec3(0.05f), glm::vec3(0.9f, 0.85f, 0.8f), glm::vec3(0.8f),
        15.0f, 25.0f, 1.0f, 0.09f, 0.032f, 0);
    spotLights_arr[1] = new SpotLight(
        glm::vec3(-28.0f, 4.5f, -28), glm::vec3(0, -1, 0),
        glm::vec3(0.05f), glm::vec3(0.9f, 0.85f, 0.8f), glm::vec3(0.8f),
        15.0f, 25.0f, 1.0f, 0.09f, 0.032f, 1);
    // Clothing store (NE) - 2 spots
    spotLights_arr[2] = new SpotLight(
        glm::vec3(20.0f, 4.5f, -20), glm::vec3(0, -1, 0),
        glm::vec3(0.05f), glm::vec3(1.0f, 0.95f, 0.7f), glm::vec3(0.9f),
        12.0f, 22.0f, 1.0f, 0.09f, 0.032f, 2);
    spotLights_arr[3] = new SpotLight(
        glm::vec3(28.0f, 4.5f, -28), glm::vec3(0, -1, 0.1f),
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
    for (int i = 0; i < NUM_TREES; i++)
    {
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
            {0.0f, 0.0f, 0.0f},  // center
            {0.3f, 0.02f, 0.0f}, // slight curve up
            {0.5f, 0.03f, 0.0f}, // mid
            {0.55f, 0.0f, 0.0f}  // edge curves down
        };
        bezierTable->generate(tableProfile, 12, 16);
    }
    
    // Create Atrium Fractal Tree
    atriumTree = new FractalTree();
    atriumTree->generate(glm::vec3(0), 1.8f, 0.15f, 4, 16.0f);

    // Create Bezier arch for doorways
    bezierArch = new BezierArch();
    {
        std::vector<glm::vec3> archProfile = {
            {-1.5f, 0.0f, 0.0f}, // left base
            {-1.5f, 3.5f, 0.0f}, // left rise
            {0.0f, 4.5f, 0.0f},  // top peak
            {1.5f, 3.5f, 0.0f},  // right rise
            {1.5f, 0.0f, 0.0f}   // right base
        };
        bezierArch->generate(archProfile, 20, 0.3f);
    }

    // Create Cylinder for cans
    cylinderObj = new Cylinder(36, 0.5f, 1.0f);

    // Create Hair surface
    hairSurface = new BezierRevolvedSurface();
    {
        std::vector<glm::vec3> hairProfile = {
            {0.0f, 0.4f, 0.0f},
            {0.5f, 0.3f, 0.0f},
            {0.6f, 0.0f, 0.0f},
            {0.65f, -0.3f, 0.0f},
            {0.5f, -0.7f, 0.0f}
        };
        hairSurface->generate(hairProfile, 16, 16);
    }
    
    // Initialize Humans
    float fY = FLOOR_Y[2];
    shopkeepers.push_back(Human({-10, fY, -33}, false, HS_SHOPKEEPER)); // McDonald's
    shopkeepers.back().rotY = 0.0f; 
    shopkeepers.push_back(Human({4, fY, -33}, true, HS_SHOPKEEPER)); // Biriyani
    shopkeepers.back().rotY = 0.0f;
    shopkeepers.push_back(Human({18, fY, -33}, false, HS_SHOPKEEPER)); // Pizza
    shopkeepers.back().rotY = 0.0f;
    shopkeepers.push_back(Human({-28, fY, -5}, true, HS_SHOPKEEPER)); // Cake
    shopkeepers.back().rotY = 90.0f;
    shopkeepers.push_back(Human({28, fY, -15}, false, HS_SHOPKEEPER)); // Coffee
    shopkeepers.back().rotY = -90.0f;

    for(int i=0; i<25; i++) {
        bool isFem = (rand()%2 == 0);
        float randX = -20.0f + (rand() % 400) / 10.0f;
        float randZ = -25.0f + (rand() % 200) / 10.0f;
        patrons.push_back(Human({randX, fY, randZ}, isFem, HS_ROAMING));
    }
    
    // 1st Floor Humans
    float f1Y = FLOOR_Y[1];
    f1Shopkeepers.push_back(Human({-30, f1Y, -33}, true, HS_SHOPKEEPER)); // Bookstore
    f1Shopkeepers.back().rotY = 0.0f;
    f1Shopkeepers.push_back(Human({30, f1Y, -33}, false, HS_SHOPKEEPER)); // Gems
    f1Shopkeepers.back().rotY = 0.0f;
    for(int i=0; i<8; i++) {
        bool isFem = (rand()%2 == 0);
        float randX = -25.0f + (rand() % 500) / 10.0f;
        float randZ = -25.0f + (rand() % 400) / 10.0f;
        f1Patrons.push_back(Human({randX, f1Y, randZ}, isFem, HS_ROAMING));
    }

    while (!glfwWindowShouldClose(window))
    {
        float t = (float)glfwGetTime();
        deltaTime = t - lastFrame;
        lastFrame = t;
        processInput(window);

        // ---- Day/Night Cycle Update ----
        if (dayNightCycleActive)
        {
            timeOfDay += dayNightSpeed * deltaTime;
            if (timeOfDay >= 24.0f)
                timeOfDay -= 24.0f;
            if (timeOfDay < 0.0f)
                timeOfDay += 24.0f;
        }
        // Compute dayNightMix: 0 = full day (noon), 1 = full night (midnight)
        // Smooth transition: day 6-18, night 20-4, transitions at 4-6 and 18-20
        float dnMix = 0.0f;
        if (timeOfDay >= 6.0f && timeOfDay < 18.0f)
        {
            dnMix = 0.0f; // full day
        }
        else if (timeOfDay >= 20.0f || timeOfDay < 4.0f)
        {
            dnMix = 1.0f; // full night
        }
        else if (timeOfDay >= 18.0f && timeOfDay < 20.0f)
        {
            dnMix = (timeOfDay - 18.0f) / 2.0f; // dusk transition
        }
        else
        {                                             // 4 to 6
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
        if (wheelRotation > 360.0f)
            wheelRotation -= 360.0f;

        // Escalator step movement
        escalatorOffset += escalatorDir * deltaTime * 2.0f;
        if (escalatorOffset > 10.0f)
            escalatorOffset -= 10.0f;
        if (escalatorOffset < 0.0f)
            escalatorOffset += 10.0f;

        // Boom barrier animation
        float targetAngle = barrierOpen ? 90.0f : 0.0f;
        if (barrierAngle < targetAngle)
            barrierAngle = glm::min(barrierAngle + deltaTime * 90.0f, targetAngle);
        if (barrierAngle > targetAngle)
            barrierAngle = glm::max(barrierAngle - deltaTime * 90.0f, targetAngle);

        // Entrance door animation
        float doorTarget = entranceDoorOpen ? 1.0f : 0.0f;
        if (entranceDoorOffset < doorTarget)
            entranceDoorOffset = glm::min(entranceDoorOffset + deltaTime * 1.2f, doorTarget);
        if (entranceDoorOffset > doorTarget)
            entranceDoorOffset = glm::max(entranceDoorOffset - deltaTime * 1.2f, doorTarget);

        // Prayer room door animation
        float prayerTarget = prayerDoorOpen ? 1.0f : 0.0f;
        if (prayerDoorOffset < prayerTarget)
            prayerDoorOffset = glm::min(prayerDoorOffset + deltaTime * 1.5f, prayerTarget);
        if (prayerDoorOffset > prayerTarget)
            prayerDoorOffset = glm::max(prayerDoorOffset - deltaTime * 1.5f, prayerTarget);
        // Elevator state machine update (3-floor)
        // Check if player is inside lift (X:-37..-31, Z:22..28)
        {
            float px = basic_camera.eye.x, pz = basic_camera.eye.z;
            float py = basic_camera.eye.y;
            float liftFloorY = elevatorY;
            playerInsideLift = (px >= -37 && px <= -31 && pz >= 22 && pz <= 28 &&
                                py >= liftFloorY + 0.5f && py <= liftFloorY + 4.0f);
        }
        elevatorTimer += deltaTime;
        switch (elevatorState)
        {
        case ELEV_IDLE:
            break;
        case ELEV_DOOR_OPENING:
            elevatorDoorOffset = glm::min(elevatorDoorOffset + deltaTime * 1.2f, 1.0f);
            if (elevatorDoorOffset >= 1.0f)
            {
                elevatorState = ELEV_DOOR_OPEN;
                elevatorTimer = 0.0f;
            }
            break;
        case ELEV_DOOR_OPEN:
            // Auto-close: after player enters, wait 2s then close
            // Or auto-close after 8s if nobody enters
            if (playerInsideLift && elevatorTimer > 2.0f)
            {
                elevatorState = ELEV_DOOR_CLOSING;
                elevatorTimer = 0.0f;
            }
            else if (!playerInsideLift && elevatorTimer > 8.0f)
            {
                elevatorState = ELEV_DOOR_CLOSING;
                elevatorTimer = 0.0f;
            }
            break;
        case ELEV_DOOR_CLOSING:
            elevatorDoorOffset = glm::max(elevatorDoorOffset - deltaTime * 1.2f, 0.0f);
            if (elevatorDoorOffset <= 0.0f)
            {
                if (elevatorTargetFloor != elevatorCurrentFloor)
                {
                    elevatorState = ELEV_MOVING;
                }
                else
                {
                    elevatorState = ELEV_IDLE;
                }
            }
            break;
        case ELEV_MOVING:
        {
            float targetY = FLOOR_Y[elevatorTargetFloor];
            float dir = (targetY > elevatorY) ? 1.0f : -1.0f;
            elevatorY += dir * ELEV_SPEED * deltaTime;
            if ((dir > 0 && elevatorY >= targetY) || (dir < 0 && elevatorY <= targetY))
            {
                elevatorY = targetY;
                elevatorCurrentFloor = elevatorTargetFloor;
                elevatorState = ELEV_DOOR_OPENING;
                elevatorDoorOffset = 0.0f;
                elevatorTimer = 0.0f;
            }
            break;
        }
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
        if (spotLightsEnabled)
        {
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
    delete cylinderObj;
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
    s.setFloat("alpha", a);
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
        GLenum fmt;
        if (ch == 1) fmt = GL_RED;
        else if (ch == 3) fmt = GL_RGB;
        else if (ch == 4) fmt = GL_RGBA;
        else fmt = GL_RGB;
        
        glBindTexture(GL_TEXTURE_2D, id);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
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
    s.setFloat("alpha", a);
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
    s.setFloat("alpha", 1.0f);
    s.setBool("useTexture", false);
    s.setBool("useProceduralWall", true);
    s.setMat4("model", m);
    glBindVertexArray(v);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    s.setBool("useProceduralWall", false);
}

// DRAW CUBE WITH PROCEDURAL SHOP TEXTURE
void drawCubeProcShop(unsigned int &v, Shader &s, glm::mat4 pm, glm::vec3 c, glm::vec3 p, glm::vec3 sc)
{
    s.use();
    glm::mat4 m = glm::translate(pm, p);
    m = glm::scale(m, sc);
    s.setVec3("material.ambient", c * 1.0f);
    s.setVec3("material.diffuse", c);
    s.setVec3("material.specular", glm::vec3(0.2f));
    s.setFloat("material.shininess", 16.0f);
    s.setFloat("alpha", 1.0f);
    s.setBool("useTexture", false);
    s.setBool("useProceduralWall", false);
    s.setBool("useProceduralShop", true);
    s.setMat4("model", m);
    glBindVertexArray(v);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    s.setBool("useProceduralShop", false);
}

// ============================================================
// FOOD COURT FUNCTIONS (2ND FLOOR)
// ============================================================

void drawFoodShop(unsigned int &v, Shader &s, glm::mat4 pm, glm::vec3 p, glm::vec3 color, float roty, unsigned int shopTex)
{
    glm::mat4 m = glm::translate(pm, p);
    m = glm::rotate(m, glm::radians(roty), {0, 1, 0});
    
    // Front counter
    drawCubeTextured(texCubeVAO, s, m, glm::vec3(0.9f), {0, 0.45f, 0}, {12.0f, 0.9f, 2.0f}, texWood, 1.0f, 32.0f);
    drawCube(v, s, m, color, {0, 0.9f, 0}, {12.2f, 0.1f, 2.2f}); // Counter top
    
    // Back wall
    drawCubeProcShop(v, s, m, color * 0.8f, {0, 3.5f, -9.8f}, {12.0f, 7.0f, 0.4f});
    // Poster on the back wall
    drawCubeTextured(texCubeVAO, s, m, glm::vec3(1.0f), {0, 3.5f, -9.58f}, {10.0f, 6.0f, 0.04f}, shopTex, 1.0f, 32.0f);
    
    // Side walls
    drawCubeProcShop(v, s, m, color * 0.9f, {-5.8f, 3.5f, -4.8f}, {0.4f, 7.0f, 9.6f});
    drawCubeProcShop(v, s, m, color * 0.9f, {5.8f, 3.5f, -4.8f}, {0.4f, 7.0f, 9.6f});

    // Top awning / Menu board (angled)
    glm::mat4 awk = glm::translate(m, {0, 6.2f, 0});
    awk = glm::rotate(awk, glm::radians(20.0f), {1, 0, 0});
    drawCubeTextured(texCubeVAO, s, awk, glm::vec3(0.95f), {0, 0, 0}, {12.0f, 1.5f, 0.2f}, shopTex, 1.0f, 16.0f);

    // Racks on right wall (X = 5.8)
    drawCube(v, s, m, C_WOOD, {4.6f, 2.0f, -6.0f}, {2.0f, 0.1f, 4.0f}); // rack 1
    drawCube(v, s, m, C_WOOD, {4.6f, 3.5f, -6.0f}, {2.0f, 0.1f, 4.0f}); // rack 2
    drawCube(v, s, m, C_WOOD, {4.6f, 5.0f, -6.0f}, {2.0f, 0.1f, 4.0f}); // rack 3

    // Items on right racks (Cans)
    if(cylinderObj) {
        for(int cy = 0; cy < 3; cy++) {
            float ry = 2.2f + cy * 1.5f;
            for(int cx = 0; cx < 4; cx++) {
                glm::mat4 canM = glm::translate(m, {4.6f, ry, -7.0f + cx * 0.6f});
                canM = glm::scale(canM, glm::vec3(0.3f, 0.4f, 0.3f));
                cylinderObj->draw(s, canM, glm::vec3(1.0f), shopTex, 1.0f);
            }
        }
    }

    // Cash Register on counter (Right side)
    drawCube(v, s, m, glm::vec3(0.1f), {3.0f, 1.25f, 0.5f}, {1.0f, 0.6f, 0.8f}); // base
    drawCube(v, s, m, glm::vec3(0.3f), {3.0f, 1.6f, 0.6f}, {0.8f, 0.5f, 0.1f}); // screen
    
    // Coffee Maker / Machine (Left side)
    drawCube(v, s, m, glm::vec3(0.05f), {-3.5f, 1.4f, 0.5f}, {1.5f, 0.9f, 1.0f}); // machine body
    drawCube(v, s, m, glm::vec3(0.6f), {-3.5f, 1.15f, 0.5f}, {0.9f, 0.4f, 0.8f}); // tray

    // Box stack on the counter
    drawCubeTextured(texCubeVAO, s, m, glm::vec3(0.9f), {-1.0f, 1.02f, 0.5f}, {1.2f, 0.08f, 1.2f}, shopTex, 1.0f, 32.0f);
    drawCubeTextured(texCubeVAO, s, m, glm::vec3(0.9f), {-1.0f, 1.1f, 0.5f}, {1.2f, 0.08f, 1.2f}, shopTex, 1.0f, 32.0f);
}

void drawChair(unsigned int &v, Shader &s, glm::mat4 pm, glm::vec3 p, float roty)
{
    glm::mat4 m = glm::translate(pm, p);
    m = glm::rotate(m, glm::radians(roty), {0, 1, 0});
    
    // Seat
    drawCube(v, s, m, C_CHAIR, {0, 0.5f, 0}, {0.6f, 0.05f, 0.6f});
    
    // Legs
    float lOff = 0.25f;
    drawCube(v, s, m, C_CHAIR * 0.6f, {-lOff, 0.25f, -lOff}, {0.05f, 0.5f, 0.05f});
    drawCube(v, s, m, C_CHAIR * 0.6f, {lOff, 0.25f, -lOff}, {0.05f, 0.5f, 0.05f});
    drawCube(v, s, m, C_CHAIR * 0.6f, {-lOff, 0.25f, lOff}, {0.05f, 0.5f, 0.05f});
    drawCube(v, s, m, C_CHAIR * 0.6f, {lOff, 0.25f, lOff}, {0.05f, 0.5f, 0.05f});
    
    // Backrest (rear legs extended, local +Z is "back")
    drawCube(v, s, m, C_CHAIR * 0.6f, {-lOff, 0.75f, lOff}, {0.05f, 0.5f, 0.05f});
    drawCube(v, s, m, C_CHAIR * 0.6f, {lOff, 0.75f, lOff}, {0.05f, 0.5f, 0.05f});
    drawCube(v, s, m, C_CHAIR, {0, 0.95f, lOff}, {0.6f, 0.2f, 0.05f});
}

void drawDiningSet(unsigned int &v, Shader &s, glm::mat4 pm, glm::vec3 p)
{
    glm::mat4 m = glm::translate(pm, p);
    
    // Central Table leg
    drawCube(v, s, m, C_TABLE * 0.6f, {0, 0.45f, 0}, {0.2f, 0.9f, 0.2f});
    drawCube(v, s, m, C_TABLE * 0.5f, {0, 0.02f, 0}, {0.8f, 0.04f, 0.8f}); // Base plate

    // Table top using bezierTable
    if (bezierTable != nullptr) {
        glm::mat4 tm = glm::translate(m, {0, 0.9f, 0});
        tm = glm::scale(tm, glm::vec3(2.0f, 1.0f, 2.0f)); 
        bezierTable->draw(s, tm, C_TABLE);
    } else {
        drawCube(v, s, m, C_TABLE, {0, 0.9f, 0}, {2.2f, 0.05f, 2.2f});
    }

    // 4 Chairs arranged radially
    drawChair(v, s, m, {0, 0, 1.4f}, 0);     // South chair (faces North)
    drawChair(v, s, m, {0, 0, -1.4f}, 180);  // North chair (faces South)
    drawChair(v, s, m, {-1.4f, 0, 0}, -90);  // West chair (faces East)
    drawChair(v, s, m, {1.4f, 0, 0}, 90);    // East chair (faces West)
}

void drawWashroom(unsigned int &v, Shader &s, glm::mat4 pm, glm::vec3 p)
{
    glm::mat4 m = glm::translate(pm, p);
    drawCube(v, s, m, C_WASH, {0, 0.03f, 0}, {14.0f, 0.04f, 18.0f}); // floor
    
    // East wall (X = 7.0)
    drawCube(v, s, m, C_WASH * 0.95f, {7.0f, 3.5f, -1.0f}, {0.3f, 7.0f, 20.0f});
    // South wall (Z = 9.0) with a door gap in the middle
    drawCube(v, s, m, C_WASH * 0.95f, {-3.5f, 3.5f, 9.0f}, {7.0f, 7.0f, 0.3f});  // left of door
    drawCube(v, s, m, C_WASH * 0.95f, {5.5f, 3.5f, 9.0f}, {3.0f, 7.0f, 0.3f});   // right of door
    // Door header
    drawCube(v, s, m, C_WASH * 0.95f, {2.0f, 5.5f, 9.0f}, {4.0f, 3.0f, 0.3f});
    
    // Inside partitions for stalls
    for (int i = 0; i < 3; i++) {
        float px = -5.0f + i * 3.5f;
        drawCube(v, s, m, C_WASH * 0.88f, {px, 2.0f, -6.0f}, {0.1f, 4.0f, 6.0f});
        if (i < 3) {
            drawCube(v, s, m, C_WASH * 0.85f, {px + 1.75f, 2.0f, -3.0f}, {3.4f, 3.8f, 0.1f}); // door
        }
    }
    // Sinks against the East wall
    drawCube(v, s, m, glm::vec3(0.9f), {6.0f, 0.9f, 2.0f}, {1.5f, 0.1f, 8.0f});
    // Mirror
    drawCube(v, s, m, C_MIRROR, {6.8f, 2.0f, 2.0f}, {0.1f, 1.5f, 8.0f}, 128, 0.3f);
    // Washroom sign
    drawCube(v, s, m, C_LAMP, {4.1f, FLOOR_H - 0.3f, 9.2f}, {0.6f, 0.6f, 0.1f});
}

void drawIceCreamFreezer(unsigned int &v, unsigned int &texCube, Shader &s, glm::mat4 pm, glm::vec3 p, float roty)
{
    glm::mat4 m = glm::translate(pm, p);
    m = glm::rotate(m, glm::radians(roty), {0, 1, 0});
    
    // Base unit
    drawCube(v, s, m, glm::vec3(0.85f), {0, 0.6f, 0}, {4.0f, 1.2f, 2.0f});
    // Curved/slanted glass front approximation using an angled transparent cube
    glm::mat4 g = glm::translate(m, {0, 1.5f, 0.2f});
    g = glm::rotate(g, glm::radians(-25.0f), {1, 0, 0});
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDepthMask(GL_FALSE);
    
    drawCube(v, s, g, C_GLASS, {0, 0, 0}, {4.0f, 1.2f, 0.05f}, 128, 0.3f);
    
    // Glass sides and top
    drawCube(v, s, m, C_GLASS, {-1.95f, 1.6f, -0.4f}, {0.05f, 1.0f, 1.2f}, 128, 0.3f);
    drawCube(v, s, m, C_GLASS, {1.95f, 1.6f, -0.4f}, {0.05f, 1.0f, 1.2f}, 128, 0.3f);
    drawCube(v, s, m, C_GLASS, {0, 2.1f, -0.4f}, {4.0f, 0.05f, 1.2f}, 128, 0.3f);

    glDepthMask(GL_TRUE);
    glDisable(GL_BLEND);

    // Textured panel at lower front
    drawCubeTextured(texCube, s, m, glm::vec3(0.9f), {0, 0.6f, 1.02f}, {3.5f, 0.8f, 0.02f}, texIceCream, 1.0f, 32.0f);
}

void drawDrinksFreezer(unsigned int &v, unsigned int &texCube, Shader &s, glm::mat4 pm, glm::vec3 p, float roty)
{
    glm::mat4 m = glm::translate(pm, p);
    m = glm::rotate(m, glm::radians(roty), {0, 1, 0});
    
    // Main body (Hollowed out)
    // Back wall
    drawCube(v, s, m, glm::vec3(0.1f), {0, 1.5f, -0.9f}, {3.0f, 3.0f, 0.2f});
    // Side walls
    drawCube(v, s, m, glm::vec3(0.1f), {-1.4f, 1.5f, 0.0f}, {0.2f, 3.0f, 2.0f});
    drawCube(v, s, m, glm::vec3(0.1f), {1.4f, 1.5f, 0.0f}, {0.2f, 3.0f, 2.0f});
    // Top wall
    drawCube(v, s, m, glm::vec3(0.1f), {0, 2.9f, 0.0f}, {3.0f, 0.2f, 2.0f});
    // Bottom wall
    drawCube(v, s, m, glm::vec3(0.1f), {0, 0.1f, 0.0f}, {3.0f, 0.2f, 2.0f});

    // Top marquee texture
    drawCubeTextured(texCube, s, m, glm::vec3(1.0f), {0, 3.3f, 1.01f}, {3.0f, 0.6f, 0.02f}, texColdDrink, 1.0f, 64.0f);

    // Middle divider
    drawCube(v, s, m, glm::vec3(0.05f), {0, 1.5f, 1.03f}, {0.1f, 2.9f, 0.05f});

    // Populate interior shelves with bottles/cans using Cylinder
    for(int sh = 0; sh < 4; sh++) {
        float shy = 0.5f + sh * 0.6f;
        drawCube(v, s, m, glm::vec3(0.8f), {0, shy, 0.5f}, {2.8f, 0.05f, 0.8f}); // shelf
        
        // Add cans if cylinderObj exists
        if(cylinderObj) {
            for(int cx = 0; cx < 4; cx++) {
                glm::mat4 canM = glm::translate(m, {-1.0f + cx * 0.6f, shy + 0.15f, 0.6f});
                canM = glm::scale(canM, glm::vec3(0.2f, 0.3f, 0.2f));
                cylinderObj->draw(s, canM, glm::vec3(1.0f), texCoke, 1.0f);
            }
        }
    }

    // Glass doors
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDepthMask(GL_FALSE);

    drawCube(v, s, m, C_GLASS, {-0.8f, 1.5f, 1.01f}, {1.35f, 2.9f, 0.05f}, 128, 0.3f);
    drawCube(v, s, m, C_GLASS, {0.8f, 1.5f, 1.01f}, {1.35f, 2.9f, 0.05f}, 128, 0.3f);

    glDepthMask(GL_TRUE);
    glDisable(GL_BLEND);
}

void drawFoodCourt(unsigned int &v, Shader &s, glm::mat4 pm)
{
    float fY = FLOOR_Y[2];
    
    // 1. Washroom (NW Corner: Center at X = -31, Z = -29)
    drawWashroom(v, s, pm, {-31.0f, fY, -29.0f});
    
    glm::vec3 stallColors[5] = {C_STALL_RED, C_STALL_YEL, C_STALL_GRN, C_STALL_ORG, C_STALL_PUR};
    unsigned int shopTextures[5] = {texMcdonalds, texBiriyani, texPizza, texCake, texCoffee};
    
    // 2. Food Shops (3 sides: North, East, West)
    // --- North Wall (Facing South, Z = -28 bounding to -38) ---
    // Place 3 stalls strictly between washroom edge (X = -24) and East wall edge (X = 38).
    // Width points: X = -12, X = 4, X = 20.
    for(int i = 0; i < 3; i++) {
        float sx = -10.0f + i * 14.0f;
        drawFoodShop(v, s, pm, {sx, fY, -28.0f}, stallColors[i], 0.0f, shopTextures[i]);
    }
    
    // --- West Wall (Facing East, X = -28 bounding to -38) ---
    // Placed south of Washroom (which ends at Z = -20).
    // Available free West wall: Z = -18 to Z = 5. We'll place 1 stall at Z=-8.
    drawFoodShop(v, s, pm, {-27.0f, fY, -5.0f}, stallColors[3], 90.0f, shopTextures[3]);

    // --- East Wall (Facing West, X = 28 bounding to 38) ---
    // Only 1 stall (Purple) as requested.
    drawFoodShop(v, s, pm, {27.0f, fY, -15.0f}, stallColors[4], -90.0f, shopTextures[4]);

    // Freezers (Right side / East Wall)
    drawDrinksFreezer(v, texCubeVAO, s, pm, {27.0f, fY, -6.0f}, -90.0f);
    drawIceCreamFreezer(v, texCubeVAO, s, pm, {27.0f, fY, -0.0f}, -90.0f);

    // 3. Dining Sets (Center Area)
    // Grid bounded to X: -20 to 16, Z: -18 to 2
    for(int r = 0; r < 3; r++) {
        for(int c = 0; c < 5; c++) {
            float dx = -18.0f + c * 8.0f;
            float dz = -16.0f + r * 9.0f;
            if (c == 2 && r == 1) continue; // skip center
            drawDiningSet(v, s, pm, {dx, fY, dz});
        }
    }

    // Process and Draw Humans
    for(auto &h : shopkeepers) {
        h.draw(v, s, pm, sphWheel, hairSurface, cylinderObj);
    }
    for(auto &h : patrons) {
        h.update(deltaTime);
        h.draw(v, s, pm, sphWheel, hairSurface, cylinderObj);
    }
}

// 1st Floor Atrium & Retail space functions

void drawBench(unsigned int &v, Shader &s, glm::mat4 pm, glm::vec3 p, float roty)
{
    glm::mat4 m = glm::translate(pm, p);
    m = glm::rotate(m, glm::radians(roty), {0, 1, 0});

    // Seat
    drawCube(v, s, m, C_WOOD, {0, 0.4f, 0}, {2.0f, 0.1f, 0.8f});
    // Legs
    drawCube(v, s, m, C_WOOD * 0.7f, {-0.9f, 0.2f, -0.3f}, {0.1f, 0.4f, 0.1f});
    drawCube(v, s, m, C_WOOD * 0.7f, {0.9f, 0.2f, -0.3f}, {0.1f, 0.4f, 0.1f});
    drawCube(v, s, m, C_WOOD * 0.7f, {-0.9f, 0.2f, 0.3f}, {0.1f, 0.4f, 0.1f});
    drawCube(v, s, m, C_WOOD * 0.7f, {0.9f, 0.2f, 0.3f}, {0.1f, 0.4f, 0.1f});
    // Backrest
    drawCube(v, s, m, C_WOOD, {0, 0.8f, -0.35f}, {2.0f, 0.6f, 0.1f});
}

void drawFirstFloorShop(unsigned int &v, Shader &s, glm::mat4 pm, glm::vec3 p, glm::vec3 color, float roty, const char* name)
{
    glm::mat4 m = glm::translate(pm, p);
    m = glm::rotate(m, glm::radians(roty), {0, 1, 0});

    // Back wall
    drawCube(v, s, m, color, {0, 3.5f, -7.8f}, {12, 7, 0.4f});
    // Side walls
    drawCube(v, s, m, color * 0.95f, {-5.8f, 3.5f, -3.8f}, {0.4f, 7, 8});
    drawCube(v, s, m, color * 0.95f, {5.8f, 3.5f, -3.8f}, {0.4f, 7, 8});
    
    // Front top facade
    drawCube(v, s, m, color * 0.8f, {0, 6.5f, 0}, {12, 1.0f, 0.5f});
    
    // Counter
    drawCube(v, s, m, glm::vec3(0.9f), {0, 0.5f, 0}, {10, 1, 1.5f});
    
    // Theme details
    if (std::string(name) == "Bookstore") {
        for(int h=0; h<3; h++) {
            drawCube(v, s, m, C_BOOKSHELF, {0, 1.5f + h*1.8f, -7.0f}, {11, 0.1f, 1.2f});
            for(int b=0; b<10; b++) {
                glm::vec3 bCol = glm::vec3((b%3==0?0.8f:0.2f), (b%2==0?0.7f:0.3f), (b%5==0?0.9f:0.1f));
                drawCube(v, s, m, bCol, {-5.0f + b*1.1f, 1.9f+h*1.8f, -7.0f}, {0.15f, 0.7f, 0.9f});
            }
        }
    } else {
        // Gems Shop
        for(int i=0; i<3; i++) {
            drawCube(v, s, m, glm::vec3(0.85f), {-3.0f + i*3.0f, 1.2f, 0}, {1.5f, 0.4f, 1.0f});
            drawCube(v, s, m, C_MIRROR, {-3.0f + i*3.0f, 1.45f, 0}, {1.3f, 0.1f, 0.8f});
        }
    }
}

void drawAtrium(unsigned int &v, Shader &s, glm::mat4 pm)
{
    float f1Y = FLOOR_Y[1];
    glm::vec3 atriumCenter(0, f1Y, -14);
    glm::mat4 m = glm::translate(pm, atriumCenter);

    // 1. Centerpiece: Fractal Tree
    if(atriumTree) {
        atriumTree->drawBranches(s, m, texTreeBark, texturesEnabled);
        atriumTree->drawLeaves(s, m, texTreeLeaf, texturesEnabled);
    }
    
    // Planter base
    drawCube(v, s, m, glm::vec3(0.2f), {0, 0.25f, 0}, {2.5f, 0.5f, 2.5f});

    // 2. Benches
    drawBench(v, s, m, {0, 0, -4.5f}, 0);
    drawBench(v, s, m, {0, 0, 4.5f}, 180);
    drawBench(v, s, m, {-4.5f, 0, 0}, -90);
    drawBench(v, s, m, {4.5f, 0, 0}, 90);

    // 3. Curved Glass Border (DRAW LAST FOR TRANSPARENCY)
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    int segments = 24;
    float radius = 8.0f;
    for(int i=0; i<segments; i++) {
        float angle = (float)i / segments * 360.0f;
        if (angle > 150 && angle < 210) continue; // ENTRANCE GAP

        float rad = glm::radians(angle);
        float px = radius * sin(rad);
        float pz = radius * cos(rad);
        
        glm::mat4 glassM = glm::translate(m, {px, 0.75f, pz});
        glassM = glm::rotate(glassM, rad, {0, 1, 0});
        drawCube(v, s, glassM, C_GLASS, {0, 0, 0}, {2.2f, 1.5f, 0.1f}, 128, 0.3f);
        // rails
        drawCube(v, s, glassM, glm::vec3(0.8f), {0, 0.75f, 0}, {2.3f, 0.1f, 0.15f});
        drawCube(v, s, glassM, glm::vec3(0.8f), {0, -0.75f, 0}, {2.3f, 0.1f, 0.15f});
    }
    glDisable(GL_BLEND);
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
        for (int ti = 0; ti < NUM_TREES; ti++)
        {
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
        drawCube(V, ls, cb, bc, {0, .35f, 0}, {2.2f, .5f, 4.4f}, 64, .85f);         // base slab
        drawCube(V, ls, cb, bc * 1.02f, {0, .55f, 0}, {2.1f, .2f, 4.3f}, 64, .85f); // upper body strip
        // Front bumper (rounded profile: two layers)
        drawCube(V, ls, cb, dk, {0, .28f, 2.25f}, {2.1f, .3f, .35f}, 64, .7f);
        drawCube(V, ls, cb, dk * .95f, {0, .22f, 2.32f}, {1.8f, .18f, .2f}, 64, .7f); // lower lip
        // Rear bumper
        drawCube(V, ls, cb, dk, {0, .28f, -2.25f}, {2.1f, .3f, .35f}, 64, .7f);
        drawCube(V, ls, cb, dk * .95f, {0, .22f, -2.32f}, {1.8f, .18f, .2f}, 64, .7f);
        // Hood (sloped via angled cube layers)
        drawCube(V, ls, cb, lt, {0, .72f, 1.5f}, {1.95f, .12f, 1.2f}, 64, .9f);        // flat top
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
            wm = glm::scale(wm, glm::vec3(.05f, .24f, .24f));             // VERY flat on X axis = disc
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
    drawCubeProc(V, ls, I, {0, WH, -M}, {2 * M, MH, WT}); // North
    drawCubeProc(V, ls, I, {-M, WH, 0}, {WT, MH, 2 * M}); // West
    drawCubeProc(V, ls, I, {M, WH, 0}, {WT, MH, 2 * M});  // East
    // South wall with entrance gap (X:-6 to 6)
    drawCubeProc(V, ls, I, {-23, WH, M}, {34, MH, WT});     // left of entrance
    drawCubeProc(V, ls, I, {23, WH, M}, {34, MH, WT});      // right of entrance
    drawCube(V, ls, I, C_EXT, {0, MH - 1, M}, {12, 2, WT}); // strip above entrance

    // ---- FRONT FACADE POSTERS (kitkat + movie) ----
    drawCubeTextured(texCubeVAO, ls, I, glm::vec3(.95f), {-18, 6, M + 0.2f}, {8, 6, .12f}, texKitkat, 1, 16);
    drawCubeTextured(texCubeVAO, ls, I, glm::vec3(.95f), {18, 6, M + 0.2f}, {8, 6, .12f}, texMovie, 1, 16);

    // ---- ENTRANCE AWNING ----
    drawCube(V, ls, I, C_AWNING, {0, MH + .1f, M + 2}, {16, .2f, 4});

    // ---- GLASS PANELS BESIDE ENTRANCE ----
    drawCube(V, ls, I, C_GLASS, {-5.5f, WH - .5f, M + .01f}, {1.5f, MH - 1, .08f}, 128, .3f);
    drawCube(V, ls, I, C_GLASS, {5.5f, WH - .5f, M + .01f}, {1.5f, MH - 1, .08f}, 128, .3f);

    // ---- GLASS ENTRANCE DOORS (sliding) ----
    {
        float edZ = M;
        float edHW = 5.5f;
        float sl = entranceDoorOffset * edHW * 0.5f;
        float ldX = -edHW * 0.25f - sl;
        float rdX = edHW * 0.25f + sl;
        drawCube(V, ls, I, C_GLASS * 1.05f, {ldX, 3.5f, edZ + .02f}, {edHW * .48f, 7.f * .85f, .1f}, 128, .2f);
        drawCube(V, ls, I, C_GLASS * 1.05f, {rdX, 3.5f, edZ + .02f}, {edHW * .48f, 7.f * .85f, .1f}, 128, .2f);
        drawCube(V, ls, I, C_LAMP, {0, FLOOR_H - .1f, edZ + .02f}, {edHW + .4f, .2f, .15f});
        drawCube(V, ls, I, C_LAMP, {-edHW * .5f - .1f, 3.5f, edZ + .02f}, {.15f, 7, .15f});
        drawCube(V, ls, I, C_LAMP, {edHW * .5f + .1f, 3.5f, edZ + .02f}, {.15f, 7, .15f});
        drawCube(V, ls, I, {0.1f, 0.9f, 0.3f}, {edHW * .5f + 1, 1.2f, edZ + .3f}, {.1f, .1f, .03f});
    }

    // ---- WINDOWS ON EAST/WEST (3 levels) ----
    for (int fl = 0; fl < 3; fl++)
    {
        float wy = FLOOR_Y[fl] + 3.5f;
        for (int i = 0; i < 6; i++)
        {
            float wz = -35 + i * 12.f;
            drawCube(V, ls, I, C_GLASS, {-M - .01f, wy, wz}, {.08f, 2.5f, 4}, 128, .3f);
            drawCube(V, ls, I, C_GLASS, {M + .01f, wy, wz}, {.08f, 2.5f, 4}, 128, .3f);
        }
    }

    // ============================================================
    // FLOOR SLABS & CEILINGS
    // ============================================================
    // Ground floor
    ls.setBool("blendWithColor", true);
    drawCubeTextured(texCubeVAO, ls, I, C_FLOOR, {0, -.02f, 0}, {79.5f, .08f, 79.5f}, texFloor, 12.0f, 32, .9f);
    ls.setBool("blendWithColor", false);

    // Floor slabs for 1st and 2nd floor
    // Holes: Escalator X:-5..5 Z:10..32 (floor1 only)
    //        L-Stair X:-22..-14 Z:10..24 (both floors)
    //        R-Stair X:14..22 Z:10..24 (both floors)
    //        Elevator X:-34..-28 Z:12..18 (both floors)
    //        Atrium X:-10..10 Z:-20..-8 (floor1 only)
    for (int fl = 1; fl < 3; fl++)
    {
        float fy = FLOOR_Y[fl] - SLAB / 2;
        ls.setBool("blendWithColor", true);

        if (fl == 1)
        {
            // FLOOR 1 — all cutouts
            // North band Z:-40 to -20 (full width, no holes)
            drawCubeTextured(texCubeVAO, ls, I, C_FLOOR, {0, fy, -30}, {80, SLAB, 20}, texFloor, 8, 32, .9f);
            // Z:-20 to -8 (Solid floor, atrium hole removed)
            drawCubeTextured(texCubeVAO, ls, I, C_FLOOR, {0, fy, -14}, {80, SLAB, 12}, texFloor, 8, 32, .9f);
            // Z:-8 to 10 (full width, below escalator/stair holes)
            drawCubeTextured(texCubeVAO, ls, I, C_FLOOR, {0, fy, 1}, {80, SLAB, 18}, texFloor, 8, 32, .9f);
            // Z:10 to 40 — strips around holes
            // Far west X:-40..-37 (narrow strip beside elevator)
            drawCubeTextured(texCubeVAO, ls, I, C_FLOOR, {-38.5f, fy, 25}, {3, SLAB, 30}, texFloor, 8, 32, .9f);
            // Elev shaft col X:-37..-31 (hole Z:22..28)
            drawCubeTextured(texCubeVAO, ls, I, C_FLOOR, {-34, fy, 16}, {6, SLAB, 12}, texFloor, 8, 32, .9f);
            drawCubeTextured(texCubeVAO, ls, I, C_FLOOR, {-34, fy, 34}, {6, SLAB, 12}, texFloor, 8, 32, .9f);
            // X:-31..-5 (combined strip: L-stair removed)
            drawCubeTextured(texCubeVAO, ls, I, C_FLOOR, {-18, fy, 25}, {26, SLAB, 30}, texFloor, 8, 32, .9f);
            // Escalator col X:-5..5 (hole Z:10..32)
            drawCubeTextured(texCubeVAO, ls, I, C_FLOOR, {0, fy, 36}, {10, SLAB, 8}, texFloor, 8, 32, .9f);
            // X:5..24 (Solid corridor)
            drawCubeTextured(texCubeVAO, ls, I, C_FLOOR, {14.5f, fy, 25}, {19, SLAB, 30}, texFloor, 8, 32, .9f);
            // New right staircase G->1 hole X:24..36, Z:22..26
            drawCubeTextured(texCubeVAO, ls, I, C_FLOOR, {30.0f, fy, 16}, {12, SLAB, 12}, texFloor, 8, 32, .9f); // Z:10..22
            drawCubeTextured(texCubeVAO, ls, I, C_FLOOR, {30.0f, fy, 33}, {12, SLAB, 14}, texFloor, 8, 32, .9f); // Z:26..40
            // East edge X:36..40
            drawCubeTextured(texCubeVAO, ls, I, C_FLOOR, {38.0f, fy, 25}, {4.0f, SLAB, 30}, texFloor, 8, 32, .9f);
        }
        else
        {
            // FLOOR 2 — only staircase + elevator holes (no escalator, no atrium)
            // North Z:-40 to 10 (full width)
            drawCubeTextured(texCubeVAO, ls, I, C_FLOOR, {0, fy, -15}, {80, SLAB, 50}, texFloor, 8, 32, .9f);
            // South Z:10 to 40 — strips around stair + elevator holes
            drawCubeTextured(texCubeVAO, ls, I, C_FLOOR, {-38.5f, fy, 25}, {3, SLAB, 30}, texFloor, 8, 32, .9f);
            drawCubeTextured(texCubeVAO, ls, I, C_FLOOR, {-34, fy, 16}, {6, SLAB, 12}, texFloor, 8, 32, .9f);
            drawCubeTextured(texCubeVAO, ls, I, C_FLOOR, {-34, fy, 34}, {6, SLAB, 12}, texFloor, 8, 32, .9f);
            // X:-31..24 (combined floor 2: L-stair removed)
            drawCubeTextured(texCubeVAO, ls, I, C_FLOOR, {-3.5f, fy, 25}, {55, SLAB, 30}, texFloor, 8, 32, .9f);
            // New right staircase 1->2 hole X:24..36, Z:26..30
            drawCubeTextured(texCubeVAO, ls, I, C_FLOOR, {30.0f, fy, 18}, {12, SLAB, 16}, texFloor, 8, 32, .9f); // Z:10..26
            drawCubeTextured(texCubeVAO, ls, I, C_FLOOR, {30.0f, fy, 35}, {12, SLAB, 10}, texFloor, 8, 32, .9f); // Z:30..40
            // East edge X:36..40
            drawCubeTextured(texCubeVAO, ls, I, C_FLOOR, {38.0f, fy, 25}, {4.0f, SLAB, 30}, texFloor, 8, 32, .9f);
        }
        ls.setBool("blendWithColor", false);

    }

    // Ceilings for each floor
    for (int fl = 0; fl < 3; fl++)
    {
        float cy = CEIL_Y[fl];
        if (fl == 2)
        {
            // Roof with only the stair hole for 2->Roof (X:24..36, Z:22..26)
            drawCube(V, ls, I, C_CEILING, {-8.0f, cy, 0}, {64.0f, .15f, 80.0f}); // West: X:-40..24
            drawCube(V, ls, I, C_CEILING, {30.0f, cy, -9.0f}, {12.0f, .15f, 62.0f}); // North: Z:-40..22 (X:24..36)
            drawCube(V, ls, I, C_CEILING, {30.0f, cy, 33.0f}, {12.0f, .15f, 14.0f}); // South: Z:26..40 (X:24..36)
            drawCube(V, ls, I, C_CEILING, {38.0f, cy, 0}, {4.0f, .15f, 80.0f}); // East: X:36..40
        }
        else if (fl == 0)
        {
            // Ground floor ceiling — same holes as floor 1 slab
            // North Z:-40..-20 full
            drawCube(V, ls, I, C_CEILING, {0, cy, -30}, {80, .1f, 20});
            // Z:-20..-8 (Solid ceiling, atrium hole removed)
            drawCube(V, ls, I, C_CEILING, {0, cy, -14}, {80, .1f, 12});
            // Z:-8..10 full
            drawCube(V, ls, I, C_CEILING, {0, cy, 1}, {80, .1f, 18});
            // Z:10..40 strips (same pattern as floor 1 slab)
            drawCube(V, ls, I, C_CEILING, {-38.5f, cy, 25}, {3, .1f, 30});
            drawCube(V, ls, I, C_CEILING, {-34, cy, 16}, {6, .1f, 12});
            drawCube(V, ls, I, C_CEILING, {-34, cy, 34}, {6, .1f, 12});
            // X:-31..-5 (combined ceiling: L-stair removed)
            drawCube(V, ls, I, C_CEILING, {-18, cy, 25}, {26, .1f, 30});
            drawCube(V, ls, I, C_CEILING, {0, cy, 36}, {10, .1f, 8});
            drawCube(V, ls, I, C_CEILING, {14.5f, cy, 25}, {19, .1f, 30});
            drawCube(V, ls, I, C_CEILING, {30.0f, cy, 16}, {12, .1f, 12});
            drawCube(V, ls, I, C_CEILING, {30.0f, cy, 33}, {12, .1f, 14});
            drawCube(V, ls, I, C_CEILING, {38.0f, cy, 25}, {4.0f, .1f, 30});
        }
        else
        {
            // 1st floor ceiling — only staircase + elevator holes
            drawCube(V, ls, I, C_CEILING, {0, cy, -15}, {80, .1f, 50});
            drawCube(V, ls, I, C_CEILING, {-38.5f, cy, 25}, {3, .1f, 30});
            drawCube(V, ls, I, C_CEILING, {-34, cy, 16}, {6, .1f, 12});
            drawCube(V, ls, I, C_CEILING, {-34, cy, 34}, {6, .1f, 12});
            // X:-31..24 (combined 1st floor ceiling: L-stair removed)
            drawCube(V, ls, I, C_CEILING, {-3.5f, cy, 25}, {55, .1f, 30});
            drawCube(V, ls, I, C_CEILING, {30.0f, cy, 18}, {12, .1f, 16});
            drawCube(V, ls, I, C_CEILING, {30.0f, cy, 35}, {12, .1f, 10});
            drawCube(V, ls, I, C_CEILING, {38.0f, cy, 25}, {4.0f, .1f, 30});
        }
    }

    // ============================================================
    // DUAL ESCALATORS (Ground → 1st Floor, center near entrance)
    // Bottom at Z=30 (near entrance), Top at Z=12, Rise: 0 to 7.3
    // ============================================================
    {
        float esBottomZ = 30.0f, esTopZ = 12.0f;
        float esRise = FLOOR_Y[1];        // 7.3
        float esLen = esBottomZ - esTopZ; // 18
        int numSteps = 30;
        float stepH = esRise / numSteps;
        float stepZ = esLen / numSteps;
        float esW = 2.5f;
        float esGap = 0.5f;
        float esCenterZ = (esBottomZ + esTopZ) / 2.0f; // 21

        for (int esc = 0; esc < 2; esc++)
        {
            float esX = (esc == 0) ? -(esW / 2 + esGap / 2) : (esW / 2 + esGap / 2);

            // Side panels (glass) - angled for realism
            float pitch = atan2(esRise, esLen);
            float hypot = sqrt(esRise * esRise + esLen * esLen);
            glm::mat4 panelM = glm::translate(I, {0, esRise / 2.0f, esCenterZ});
            
            glm::mat4 leftPanelM = glm::translate(panelM, {esX - esW / 2, 0.5f, 0});
            leftPanelM = glm::rotate(leftPanelM, pitch, {1, 0, 0});
            drawCube(V, ls, leftPanelM, C_GLASS, {0, 0, 0}, {.08f, 1.2f, hypot + 2.0f}, 128, .2f);
            
            glm::mat4 rightPanelM = glm::translate(panelM, {esX + esW / 2, 0.5f, 0});
            rightPanelM = glm::rotate(rightPanelM, pitch, {1, 0, 0});
            drawCube(V, ls, rightPanelM, C_GLASS, {0, 0, 0}, {.08f, 1.2f, hypot + 2.0f}, 128, .2f);

            // Handrails
            glm::mat4 leftRailM = glm::translate(leftPanelM, {0, 0.6f, 0});
            drawCube(V, ls, leftRailM, C_TIRE, {0, 0, 0}, {.12f, .06f, hypot + 2.0f});
            
            glm::mat4 rightRailM = glm::translate(rightPanelM, {0, 0.6f, 0});
            drawCube(V, ls, rightRailM, C_TIRE, {0, 0, 0}, {.12f, .06f, hypot + 2.0f});

            // Truss structure (slanted metal base)
            glm::mat4 baseM = glm::translate(panelM, {esX, -0.2f, 0});
            baseM = glm::rotate(baseM, pitch, {1, 0, 0});
            drawCube(V, ls, baseM, C_LAMP * .7f, {0, 0, 0}, {esW - .2f, 0.4f, hypot});

            // Animated steps
            // Right (esc=1) goes UP, Left (esc=0) goes DOWN
            float dir = (esc == 1) ? 1.0f : -1.0f;
            for (int i = 0; i < numSteps; i++)
            {
                float rawT = (float)i / numSteps;
                float currentT = fmod(rawT + (dir * escalatorOffset * 0.1f), 1.0f);
                if (currentT < 0) currentT += 1.0f;

                float sZ = esBottomZ - currentT * esLen;
                float sY = currentT * esRise;
                
                // Flat part of the step
                drawCube(V, ls, I, C_STAIR * 1.05f, {esX, sY + .05f, sZ}, {esW - .4f, .1f, stepZ * 1.0f});
                // Vertical part of the step (riser)
                drawCube(V, ls, I, C_STAIR * 0.9f, {esX, sY - stepH / 2 + .1f, sZ + stepZ / 2}, {esW - .4f, stepH, .1f});
            }

            // Entry/exit platforms
            drawCube(V, ls, I, C_STAIR, {esX, .05f, esBottomZ + 1}, {esW, .1f, 2});
            drawCube(V, ls, I, C_STAIR, {esX, esRise + .05f, esTopZ - 1}, {esW, .1f, 2});
        }

        // Metal divider between escalators
        drawCube(V, ls, I, C_LAMP, {0, .5f, esCenterZ}, {.08f, 1.0f, esLen});
        drawCube(V, ls, I, C_LAMP, {0, esRise + .3f, esCenterZ}, {.08f, .06f, esLen});
    }

    // ============================================================
    // ELEVATOR / LIFT (West side, beside left staircase, X=-31, Z=15)
    // ============================================================
    {
        float eX = -34, eZ = 25.f;
        float shaftH = MH;
        // Shaft walls (South, West, North are solid)
        drawCube(V, ls, I, C_CORR, {eX, shaftH / 2, eZ + 3}, {6, shaftH, WT}); // South
        drawCube(V, ls, I, C_CORR, {eX - 3, shaftH / 2, eZ}, {WT, shaftH, 6}); // West
        drawCube(V, ls, I, C_CORR, {eX, shaftH / 2, eZ - 3}, {6, shaftH, WT}); // North

        // East wall (facing corridor) - solid with holes for doors
        // The door gap is 4.0 units wide (from eZ-2.0 to eZ+2.0)
        drawCube(V, ls, I, C_CORR, {eX + 3, shaftH / 2, eZ - 2.5f}, {WT, shaftH, 1.f}); // North strip
        drawCube(V, ls, I, C_CORR, {eX + 3, shaftH / 2, eZ + 2.5f}, {WT, shaftH, 1.f}); // South strip

        for (int fl = 0; fl < 3; fl++)
        {
            // Header wall above door on each floor (door height is 4.0)
            float headerH = FLOOR_H - 4.0f;
            float headerY = FLOOR_Y[fl] + 4.0f + headerH / 2;
            drawCube(V, ls, I, C_CORR, {eX + 3, headerY, eZ}, {WT, headerH, 4.f});

            // Outer sliding doors on each floor
            float slide = 0.0f; // 0 = closed, up to 2.0 = fully open (shifted into walls)
            if (fl == elevatorTargetFloor && elevatorState != ELEV_MOVING)
            {
                slide = elevatorDoorOffset * 2.0f; // synchronized with inner doors
            }
            // North door (slides negative Z)
            drawCube(V, ls, I, C_GLASS, {eX + 2.95f, FLOOR_Y[fl] + 2.0f, eZ - 1.0f - slide}, {.08f, 4.0f, 2.0f}, 128, .25f);
            // South door (slides positive Z)
            drawCube(V, ls, I, C_GLASS, {eX + 2.95f, FLOOR_Y[fl] + 2.0f, eZ + 1.0f + slide}, {.08f, 4.0f, 2.0f}, 128, .25f);
        }

        // Corner rails
        drawCube(V, ls, I, C_LAMP, {eX - 2.9f, shaftH / 2, eZ + 2.9f}, {.05f, shaftH, .05f});
        drawCube(V, ls, I, C_LAMP, {eX + 2.9f, shaftH / 2, eZ + 2.9f}, {.05f, shaftH, .05f});
        drawCube(V, ls, I, C_LAMP, {eX - 2.9f, shaftH / 2, eZ - 2.9f}, {.05f, shaftH, .05f});
        drawCube(V, ls, I, C_LAMP, {eX + 2.9f, shaftH / 2, eZ - 2.9f}, {.05f, shaftH, .05f});

        // Cabin
        float cabY = elevatorY;
        drawCube(V, ls, I, C_STAIR, {eX, cabY + .05f, eZ}, {5.f, .1f, 5.f});
        drawCube(V, ls, I, C_CEILING, {eX, cabY + 4.f, eZ}, {5.f, .1f, 5.f});
        drawCube(V, ls, I, C_CORR, {eX - 2.45f, cabY + 2.0f, eZ}, {.08f, 4.0f, 5.0f}); // Left wall
        drawCube(V, ls, I, C_CORR, {eX, cabY + 2.0f, eZ - 2.45f}, {5.0f, 4.0f, .08f}); // Back wall
        drawCube(V, ls, I, C_CORR, {eX, cabY + 2.0f, eZ + 2.45f}, {5.0f, 4.0f, .08f}); // Right wall

        // Inner sliding doors (east face, attached to cabin)
        float slide = elevatorDoorOffset * 2.0f;
        drawCube(V, ls, I, C_GLASS, {eX + 2.45f, cabY + 2.0f, eZ - 1.0f - slide}, {.08f, 4.0f, 2.0f}, 128, .25f);
        drawCube(V, ls, I, C_GLASS, {eX + 2.45f, cabY + 2.0f, eZ + 1.0f + slide}, {.08f, 4.0f, 2.0f}, 128, .25f);

        // Inside panel buttons
        drawCube(V, ls, I, C_LAMP, {eX - 2.3f, cabY + 2.0f, eZ + .5f}, {.05f, .6f, .2f});             // panel body
        drawCube(V, ls, I, {.2f, .8f, .2f}, {eX - 2.28f, cabY + 2.2f, eZ + .5f}, {.03f, .08f, .08f}); // top button
        drawCube(V, ls, I, {.8f, .8f, .2f}, {eX - 2.28f, cabY + 2.0f, eZ + .5f}, {.03f, .08f, .08f}); // mid button
        drawCube(V, ls, I, {.8f, .2f, .2f}, {eX - 2.28f, cabY + 1.8f, eZ + .5f}, {.03f, .08f, .08f}); // bottom button

        // Call buttons on each floor (outside, east side)
        for (int fl = 0; fl < 3; fl++)
        {
            float btnY = FLOOR_Y[fl] + 1.2f;
            drawCube(V, ls, I, C_LAMP, {eX + 3.2f, btnY, eZ}, {.1f, .4f, .2f});
            drawCube(V, ls, I, {.2f, .8f, .2f}, {eX + 3.26f, btnY + .1f, eZ}, {.03f, .06f, .06f});
            drawCube(V, ls, I, {.8f, .2f, .2f}, {eX + 3.26f, btnY - .1f, eZ}, {.03f, .06f, .06f});
        }
    }

    // ============================================================
    // DUAL STAIRCASES (Both sides, all floors)
    // Left: X=-22..-14, Z=10..24   Right: X=14..22, Z=10..24
    // Switchback: G→1 goes south-north, 1→2 goes north-south (offset in X)
    // ============================================================
    {
        int ns = 14;
        float stW = 4.0f; // Width of stairs in Z
        float stepRun = 10.0f / ns; // X length = 10 (24 to 34)
        
        // Flight G→1: Starts at X=24, ends at X=34. Moves +X (East). Z = 24.
        for (int i = 0; i < ns; i++)
        {
            float sx = 24.0f + i * stepRun;
            float sy = i * (FLOOR_Y[1] / ns);
            drawCube(V, ls, I, C_STAIR, {sx + stepRun / 2, sy + .25f, 24.0f}, {stepRun * .95f, .5f, stW});
        }
        // Landing at top of G→1 (X=35, Z=24) connecting to Flight 1->2
        drawCube(V, ls, I, C_STAIR, {35.0f, FLOOR_Y[1] - .15f, 26.0f}, {2.0f, SLAB, stW * 2});

        // Flight 1→2: Starts at X=34, ends at X=24. Moves -X (West). Z = 28.
        for (int i = 0; i < ns; i++)
        {
            float sx = 34.0f - i * stepRun;
            float sy = FLOOR_Y[1] + i * ((FLOOR_Y[2] - FLOOR_Y[1]) / ns);
            drawCube(V, ls, I, C_STAIR, {sx - stepRun / 2, sy + .25f, 28.0f}, {stepRun * .95f, .5f, stW});
        }
        // Landing at top of 1→2 (X=23, Z=28) - integrates to corridor
        drawCube(V, ls, I, C_STAIR, {23.0f, FLOOR_Y[2] - .15f, 28.0f}, {2.0f, SLAB, stW});

        // Open staircases (Railings)
        
        // Vertical Posts
        drawCube(V, ls, I, C_LAMP, {24.5f, FLOOR_Y[1] / 2 + 1, 22.0f}, {.08f, FLOOR_Y[1] + 2, .08f}); // start post G->1
        drawCube(V, ls, I, C_LAMP, {34.0f, FLOOR_Y[1] / 2 + 1, 22.0f}, {.08f, FLOOR_Y[1] + 2, .08f}); // end post G->1
        drawCube(V, ls, I, C_LAMP, {24.5f, FLOOR_Y[1] / 2 + 1, 26.0f}, {.08f, FLOOR_Y[1] + 2, .08f});
        drawCube(V, ls, I, C_LAMP, {34.0f, FLOOR_Y[1] / 2 + 1, 26.0f}, {.08f, FLOOR_Y[1] + 2, .08f});

        drawCube(V, ls, I, C_LAMP, {24.5f, FLOOR_Y[1] + FLOOR_H / 2 + 1, 26.0f}, {.08f, FLOOR_H + 2, .08f}); // end post 1->2
        drawCube(V, ls, I, C_LAMP, {34.0f, FLOOR_Y[1] + FLOOR_H / 2 + 1, 26.0f}, {.08f, FLOOR_H + 2, .08f}); // start post 1->2
        drawCube(V, ls, I, C_LAMP, {24.5f, FLOOR_Y[1] + FLOOR_H / 2 + 1, 30.0f}, {.08f, FLOOR_H + 2, .08f});
        drawCube(V, ls, I, C_LAMP, {34.0f, FLOOR_Y[1] + FLOOR_H / 2 + 1, 30.0f}, {.08f, FLOOR_H + 2, .08f});

        // Handrail bars (angled)
        glm::mat4 m1 = glm::translate(I, {29.0f, FLOOR_Y[1] / 2 + 1.2f, 22.05f});
        m1 = glm::rotate(m1, atan2(FLOOR_Y[1], 10.0f), {0, 0, 1});
        drawCube(V, ls, m1, C_LAMP, {0, 0, 0}, {12.5f, .06f, .06f}); // left rail G->1
        glm::mat4 m2 = glm::translate(I, {29.0f, FLOOR_Y[1] / 2 + 1.2f, 25.95f});
        m2 = glm::rotate(m2, atan2(FLOOR_Y[1], 10.0f), {0, 0, 1});
        drawCube(V, ls, m2, C_LAMP, {0, 0, 0}, {12.5f, .06f, .06f}); // right rail G->1

        glm::mat4 m3 = glm::translate(I, {29.0f, FLOOR_Y[1] + FLOOR_H / 2 + 1.2f, 26.05f});
        m3 = glm::rotate(m3, atan2(-FLOOR_Y[1], 10.0f), {0, 0, 1}); 
        drawCube(V, ls, m3, C_LAMP, {0, 0, 0}, {12.5f, .06f, .06f}); // left rail 1->2
        glm::mat4 m4 = glm::translate(I, {29.0f, FLOOR_Y[1] + FLOOR_H / 2 + 1.2f, 29.95f});
        m4 = glm::rotate(m4, atan2(-FLOOR_Y[1], 10.0f), {0, 0, 1}); 
        drawCube(V, ls, m4, C_LAMP, {0, 0, 0}, {12.5f, .06f, .06f}); // right rail 1->2

        // Flight 2→Roof: Starts at X=24, ends at X=34. Moves +X (East). Z = 24.
        for (int i = 0; i < ns; i++)
        {
            float sx = 24.0f + i * stepRun;
            float sy = FLOOR_Y[2] + i * ((CEIL_Y[2] - FLOOR_Y[2]) / ns);
            drawCube(V, ls, I, C_STAIR, {sx + stepRun / 2, sy + .25f, 24.0f}, {stepRun * .95f, .5f, stW});
        }
        // Landing at top of 2→Roof (X=35, Z=24) connecting to Roof
        drawCube(V, ls, I, C_STAIR, {35.0f, CEIL_Y[2] - .15f, 26.0f}, {2.0f, SLAB, stW * 2});

        // Vertical Posts for 2->Roof
        drawCube(V, ls, I, C_LAMP, {24.5f, FLOOR_Y[2] + FLOOR_H / 2 + 1, 22.0f}, {.08f, FLOOR_H + 2, .08f}); 
        drawCube(V, ls, I, C_LAMP, {34.0f, FLOOR_Y[2] + FLOOR_H / 2 + 1, 22.0f}, {.08f, FLOOR_H + 2, .08f}); 
        drawCube(V, ls, I, C_LAMP, {24.5f, FLOOR_Y[2] + FLOOR_H / 2 + 1, 26.0f}, {.08f, FLOOR_H + 2, .08f});
        drawCube(V, ls, I, C_LAMP, {34.0f, FLOOR_Y[2] + FLOOR_H / 2 + 1, 26.0f}, {.08f, FLOOR_H + 2, .08f});

        // Handrail bars for 2->Roof (angled)
        glm::mat4 m5 = glm::translate(I, {29.0f, FLOOR_Y[2] + FLOOR_H / 2 + 1.2f, 22.05f});
        m5 = glm::rotate(m5, atan2(CEIL_Y[2] - FLOOR_Y[2], 10.0f), {0, 0, 1});
        drawCube(V, ls, m5, C_LAMP, {0, 0, 0}, {12.5f, .06f, .06f}); // left rail 2->Roof
        glm::mat4 m6 = glm::translate(I, {29.0f, FLOOR_Y[2] + FLOOR_H / 2 + 1.2f, 25.95f});
        m6 = glm::rotate(m6, atan2(CEIL_Y[2] - FLOOR_Y[2], 10.0f), {0, 0, 1});
        drawCube(V, ls, m6, C_LAMP, {0, 0, 0}, {12.5f, .06f, .06f}); // right rail 2->Roof
    }

    // ============================================================
    // ROOFTOP STAIRCASE SHED (Headhouse)
    // ============================================================
    {
        float bY = CEIL_Y[2]; // Base Y
        float sH = 3.5f; // Shed Height
        float wT = 0.2f; // Wall thickness
        
        // West Wall (solid behind stairs)
        drawCube(V, ls, I, C_EXT, {23.9f, bY + sH/2, 24.0f}, {wT, sH, 4.4f});
        
        // North Wall (solid with glass insert)
        drawCube(V, ls, I, C_EXT, {30.0f, bY + sH/2, 21.9f}, {12.0f, sH, wT});
        // South Wall (solid with glass insert)
        drawCube(V, ls, I, C_EXT, {30.0f, bY + sH/2, 26.1f}, {12.0f, sH, wT});

        // East Face (Open doorway frames)
        drawCube(V, ls, I, C_EXT, {36.1f, bY + sH/2, 21.9f}, {wT, sH, wT}); // NE Pillar
        drawCube(V, ls, I, C_EXT, {36.1f, bY + sH/2, 26.1f}, {wT, sH, wT}); // SE Pillar
        drawCube(V, ls, I, C_EXT, {36.1f, bY + sH - 0.4f, 24.0f}, {wT, 0.8f, 4.4f}); // Top Header

        // Roof Slab (Slight overhang)
        drawCube(V, ls, I, C_EXT * 0.8f, {30.0f, bY + sH + 0.1f, 24.0f}, {12.8f, 0.2f, 4.8f});

        // Glass panel inserts for North/South walls
        drawCube(V, ls, I, C_GLASS, {30.0f, bY + sH/2, 21.9f}, {8.0f, 2.0f, wT + 0.05f}, 128, 0.4f);
        drawCube(V, ls, I, C_GLASS, {30.0f, bY + sH/2, 26.1f}, {8.0f, 2.0f, wT + 0.05f}, 128, 0.4f);
    }

    // ============================================================
    // GROUND FLOOR — ONLY 2 SHOPS (Tech NW, Clothing NE)
    // ============================================================
    {
        float shopH = FLOOR_H / 2;
        float shopDepth = 28.0f; // Z: -38 to -10
        float shopWidth = 26.0f; // X: ±12 to ±38

        // === SHOP 1: TECH SHOP (NW corner) ===
        // Bounds: X=-38 to -12, Z=-38 to -10
        {
            float cx = -25, cz = -24;
            // Floor
            drawCube(V, ls, I, C_TECH_FLOOR, {cx, .02f, cz}, {shopWidth, .04f, shopDepth});
            // Back wall (north, Z=-38)
            drawCube(V, ls, I, C_TECH_WALL, {cx, shopH, -38}, {shopWidth, FLOOR_H, WT});
            // West wall (X=-38, already exterior)
            // East wall (X=-12)
            drawCube(V, ls, I, C_TECH_WALL, {-12, shopH, cz}, {WT, FLOOR_H, shopDepth});
            // Front wall (south, Z=-10) with glass facade
            drawCube(V, ls, I, C_TECH_WALL, {cx - 8, shopH, -10}, {10, FLOOR_H, WT}); // left section
            drawCube(V, ls, I, C_TECH_WALL, {cx + 8, shopH, -10}, {10, FLOOR_H, WT}); // right section
            // Glass facade (center gap for entrance)
            float glassH = FLOOR_H * .75f;
            drawCube(V, ls, I, C_GLASS, {cx - 4, glassH / 2, -10}, {6, glassH, .08f}, 128, .3f);
            drawCube(V, ls, I, C_GLASS, {cx + 4, glassH / 2, -10}, {6, glassH, .08f}, 128, .3f);
            // Banner
            drawCubeTextured(texCubeVAO, ls, I, glm::vec3(.95f), {cx, FLOOR_H - .5f, -10.1f}, {14, 1.5f, .12f}, texTech, 1, 16);
            // Display tables (tech products)
            for (int r = 0; r < 2; r++)
                for (int c = 0; c < 3; c++)
                {
                    float tx = -34 + c * 8.f, tz = -32 + r * 12.f;
                    drawCube(V, ls, I, C_DISP_TABLE, {tx, .45f, tz}, {3, .9f, 2});
                    drawCube(V, ls, I, C_DISP_BASE, {tx, .02f, tz}, {2.5f, .04f, 1.5f});
                    drawCube(V, ls, I, C_PHONE, {tx - .5f, .95f, tz}, {.4f, .02f, .7f});
                    drawCube(V, ls, I, C_LAPTOP, {tx + .5f, .95f, tz}, {.8f, .03f, .5f});
                    drawCube(V, ls, I, C_SCREEN, {tx + .5f, 1.1f, tz - .2f}, {.78f, .3f, .02f});
                }
        }

        // === SHOP 2: CLOTHING SHOP (NE corner) ===
        // Bounds: X=12 to 38, Z=-38 to -10
        {
            float cx = 25, cz = -24;
            // Floor
            drawCube(V, ls, I, C_FASH_FLOOR, {cx, .02f, cz}, {shopWidth, .04f, shopDepth});
            // Back wall (north, Z=-38)
            drawCube(V, ls, I, C_FASH_WALL, {cx, shopH, -38}, {shopWidth, FLOOR_H, WT});
            // East wall (X=38, already exterior)
            // West wall (X=12)
            drawCube(V, ls, I, C_FASH_WALL, {12, shopH, cz}, {WT, FLOOR_H, shopDepth});
            // Front wall (south, Z=-10) with glass facade
            drawCube(V, ls, I, C_FASH_WALL, {cx - 8, shopH, -10}, {10, FLOOR_H, WT});
            drawCube(V, ls, I, C_FASH_WALL, {cx + 8, shopH, -10}, {10, FLOOR_H, WT});
            // Glass facade
            float glassH = FLOOR_H * .75f;
            drawCube(V, ls, I, C_GLASS, {cx - 4, glassH / 2, -10}, {6, glassH, .08f}, 128, .3f);
            drawCube(V, ls, I, C_GLASS, {cx + 4, glassH / 2, -10}, {6, glassH, .08f}, 128, .3f);
            // Banner
            drawCubeTextured(texCubeVAO, ls, I, glm::vec3(.95f), {cx, FLOOR_H - .5f, -10.1f}, {14, 1.5f, .12f}, texFashion, 1, 16);
            // Clothing racks and mannequins
            for (int r = 0; r < 3; r++)
            {
                float rz = -34 + r * 10.f;
                // Clothing rack (metal bar with hangers)
                drawCube(V, ls, I, C_RACK, {cx - 6, 2.0f, rz}, {.08f, 4, .08f});
                drawCube(V, ls, I, C_RACK, {cx - 6, 3.8f, rz}, {4, .08f, .08f});
                // Mannequin
                drawCube(V, ls, I, C_MANNEQUIN, {cx + 4, 1.5f, rz}, {.6f, 3, .3f});
                drawCube(V, ls, I, C_MANNEQUIN, {cx + 4, 3.2f, rz}, {.3f, .3f, .3f}); // head
            }
            // Cash desk
            drawCube(V, ls, I, C_CASH_DESK, {cx + 8, .55f, -14}, {3, 1.1f, 2});
            // Mirror
            drawCube(V, ls, I, C_MIRROR, {37.8f, 2.5f, -28}, {.06f, 4, 3}, 128, .2f);
        }
    }

    // ============================================================
    // PRAYER ROOM (Ground Floor, X:-10 to 10, Z:-20 to -8)
    // ============================================================
    {
        ls.setBool("blendWithColor", true);
        glm::mat4 mFloor = glm::translate(I, {0.0f, 0.01f, -14.0f});
        mFloor = glm::rotate(mFloor, glm::radians(180.0f), {0.0f, 1.0f, 0.0f});
        drawCubeTextured(texCubeVAO, ls, mFloor, C_PRAY_FLOOR, {0, 0, 0}, {20.0f, 0.04f, 12.0f}, texPrayerTiles, 8.0f, 32, 1.0f);
        ls.setBool("blendWithColor", false);

        // Walls (X:-10..10, Z:-20..-8)
        drawCube(V, ls, I, C_PRAY_WALL, {0, FLOOR_H / 2, -19.9f}, {20, FLOOR_H, 0.2f}); // Back wall
        drawCube(V, ls, I, C_PRAY_WALL, {-9.9f, FLOOR_H / 2, -14}, {0.2f, FLOOR_H, 12}); // Left wall
        drawCube(V, ls, I, C_PRAY_WALL, {9.9f, FLOOR_H / 2, -14}, {0.2f, FLOOR_H, 12});  // Right wall
        
        // Front wall with glass door gap
        drawCube(V, ls, I, C_PRAY_WALL, {-7.5f, FLOOR_H / 2, -8.1f}, {5, FLOOR_H, 0.2f}); // Left of door
        drawCube(V, ls, I, C_PRAY_WALL, {7.5f, FLOOR_H / 2, -8.1f}, {5, FLOOR_H, 0.2f});  // Right of door
        drawCube(V, ls, I, C_PRAY_WALL, {0, FLOOR_H - 1.0f, -8.1f}, {10, 2.0f, 0.2f}); // Above door
        
        // Glass Doors (Double doors sliding)
        {
            float doorW = 4.8f;
            float slide = prayerDoorOffset * doorW * 0.9f;
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            glDepthMask(GL_FALSE);
            
            drawCube(V, ls, I, C_GLASS, {-2.5f - slide, (FLOOR_H - 2.0f) / 2, -8.1f}, {doorW, FLOOR_H - 2.0f, 0.1f}, 128, 0.4f);
            drawCube(V, ls, I, C_GLASS, {2.5f + slide, (FLOOR_H - 2.0f) / 2, -8.1f}, {doorW, FLOOR_H - 2.0f, 0.1f}, 128, 0.4f);
            
            glDepthMask(GL_TRUE);
            glDisable(GL_BLEND);
        }
        
        // Mihrab Decoration on back wall 
        // 1. Arch outline
        if (bezierArch) {
            glm::mat4 mArch = glm::translate(I, {0, 0, -19.6f});
            mArch = glm::scale(mArch, {2.5f, 1.5f, 1.0f});
            bezierArch->draw(ls, mArch, glm::vec3(0.95f, 0.90f, 0.75f), 16.0f);
            ls.setBool("useTexture", true); // Reset if needed
        }
        
        // 2. Circular Calligraphy Frame (flattened cylinder)
        if (cylinderObj) {
            glm::mat4 mCyl = glm::translate(I, {0, 3.5f, -19.7f});
            mCyl = glm::rotate(mCyl, glm::radians(90.0f), {1.0f, 0.0f, 0.0f});
            mCyl = glm::scale(mCyl, {1.5f, 0.1f, 1.5f});
            cylinderObj->draw(ls, mCyl, glm::vec3(0.15f, 0.15f, 0.15f));
            
            // Gold border
            mCyl = glm::translate(I, {0, 3.5f, -19.75f});
            mCyl = glm::rotate(mCyl, glm::radians(90.0f), {1.0f, 0.0f, 0.0f});
            mCyl = glm::scale(mCyl, {1.6f, 0.05f, 1.6f});
            cylinderObj->draw(ls, mCyl, glm::vec3(0.85f, 0.65f, 0.15f));
        }

        // 3. Flanking textured columns
        for (int side = -1; side <= 1; side += 2) {
            float sx = side * 5.0f;
            ls.setBool("blendWithColor", true);
            // Split column into two 3x3 cubes to prevent vertical stretching of the square aspect ratio texture
            drawCubeTextured(texCubeVAO, ls, I, glm::vec3(1.0f), {sx, 1.5f, -19.6f}, {3.0f, 3.0f, 0.6f}, texMahrib, 1.0f, 32, 1.0f); // Bottom half
            drawCubeTextured(texCubeVAO, ls, I, glm::vec3(1.0f), {sx, 4.5f, -19.6f}, {3.0f, 3.0f, 0.6f}, texMahrib, 1.0f, 32, 1.0f); // Top half
            ls.setBool("blendWithColor", false);
        }
        
        // 4. Quran Stand (Rehal) - X shape
        ls.setBool("blendWithColor", true);
        glm::mat4 mR1 = glm::translate(I, {0, 0.6f, -17.0f});
        mR1 = glm::rotate(mR1, glm::radians(45.0f), {0.0f, 0.0f, 1.0f});
        drawCubeTextured(texCubeVAO, ls, mR1, C_WOOD, {0, 0, 0}, {0.1f, 1.2f, 1.0f}, texWood, 1.0f, 64, 1.0f);
        
        glm::mat4 mR2 = glm::translate(I, {0, 0.6f, -17.0f});
        mR2 = glm::rotate(mR2, glm::radians(-45.0f), {0.0f, 0.0f, 1.0f});
        drawCubeTextured(texCubeVAO, ls, mR2, C_WOOD, {0, 0, 0}, {0.1f, 1.2f, 1.0f}, texWood, 1.0f, 64, 1.0f);
        ls.setBool("blendWithColor", false);
    }

    // ============================================================
    // (Old ATRIUM SEATING SPACE from ground floor was here, now replaced)
    // ============================================================

    // ============================================================
    // WASHROOM (East side, beside right staircase, X=24..38, Z=8..20)
    // ============================================================
    {
        float wX = 31, wZ = 14;
        drawCube(V, ls, I, C_WASH, {wX, .03f, wZ}, {14, .04f, 12}); // floor
        // Walls (enclosed room)
        drawCube(V, ls, I, C_WASH * .95f, {wX, FLOOR_H / 2, 8}, {14, FLOOR_H, WT});  // north wall
        drawCube(V, ls, I, C_WASH * .95f, {wX, FLOOR_H / 2, 20}, {14, FLOOR_H, WT}); // south wall
        // West wall (facing corridor, with door gap)
        drawCube(V, ls, I, C_WASH * .95f, {24, FLOOR_H / 2, 11}, {WT, FLOOR_H, 6}); // left of door
        drawCube(V, ls, I, C_WASH * .95f, {24, FLOOR_H / 2, 17}, {WT, FLOOR_H, 6}); // right of door
        // East wall (X=38, exterior already drawn)
        // Stall partitions
        for (int i = 0; i < 4; i++)
        {
            float pz = 10 + i * 2.5f;
            drawCube(V, ls, I, C_WASH * .88f, {wX, 1.2f, pz}, {8, 2.2f, .08f});
            if (i < 3)
            {
                float doorAngle = (i == 1) ? 30.f : 0.f;
                glm::mat4 dm = glm::translate(I, {wX - 2, 1.0f, pz + 1.25f});
                if (doorAngle > 0)
                    dm = glm::rotate(dm, glm::radians(doorAngle), {0, 1, 0});
                drawCube(V, ls, dm, C_WASH * .85f, {0, 0, 0}, {1.8f, 1.8f, .06f});
                drawCube(V, ls, dm, C_LAMP, {0, 0, .04f}, {.06f, .06f, .04f});
            }
        }
        // Washroom sign
        drawCube(V, ls, I, C_LAMP, {24.1f, FLOOR_H - .3f, 14}, {.08f, .6f, 2});
    }

    // ============================================================
    // INTERIOR DIVIDING WALLS (ground floor corridor structure)
    // ============================================================
    {
        float midY = FLOOR_H / 2;
        // Wall between corridor and shop area (Z=-8 line, center gap for atrium)
        // Already drawn as shop front walls above
        // Corridor side walls guiding traffic flow
        // West corridor wall (between lift area and shop1)
        drawCube(V, ls, I, C_CORR, {-12, midY, 1}, {WT, FLOOR_H, 18}); // X=-12 from Z=-8 to Z=10
        // East corridor wall (between washroom area and shop2)
        drawCube(V, ls, I, C_CORR, {12, midY, 1}, {WT, FLOOR_H, 18}); // X=12 from Z=-8 to Z=10
    }

    // ============================================================
    // UPPER FLOOR WALLS (1st & 2nd floor structure)
    // ============================================================
    for (int fl = 1; fl < 3; fl++)
    {
        float midY = FLOOR_Y[fl] + FLOOR_H / 2;
        if (fl == 1)
        {
            // 1. Atrium & Benches
            drawAtrium(V, ls, I);
            
            // 2. Retail Shops (NW & NE Corners)
            // Bookstore at NW
            drawFirstFloorShop(V, ls, I, {-30, FLOOR_Y[1], -30}, C_BOOK_WALL, 0.0f, "Bookstore");
            // Gems at NE (using Gem Wall color)
            drawFirstFloorShop(V, ls, I, {30, FLOOR_Y[1], -30}, C_GEM_WALL, 0.0f, "Gems");

            // 3. 1st Floor Humans (Updating and Drawing)
            for(auto &h : f1Shopkeepers) {
                h.draw(V, ls, I, sphWheel, hairSurface, cylinderObj);
            }
            for(auto &h : f1Patrons) {
                h.update(deltaTime);
                h.draw(V, ls, I, sphWheel, hairSurface, cylinderObj);
            }
        }
        else if (fl == 2)
        {
            // 2nd floor Food Court
            drawFoodCourt(V, ls, I);
        }
    }

    // ============================================================
    // ROOF PARAPET
    // ============================================================
    float pY = MH + .5f;
    drawCube(V, ls, I, C_PARAPET, {0, pY, -M}, {2 * M, 1, .3f});
    drawCube(V, ls, I, C_PARAPET, {0, pY, M}, {2 * M, 1, .3f});
    drawCube(V, ls, I, C_PARAPET, {-M, pY, 0}, {.3f, 1, 2 * M});
    drawCube(V, ls, I, C_PARAPET, {M, pY, 0}, {.3f, 1, 2 * M});

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

    // Escalator ride override (escalator is at X:-4..4, Z:12..30)
    // Height depends on position, not direction — both escalators have same height mapping
    bool onEscalator = (x >= -4.f && x <= 4.f && z >= 12 && z <= 30 && basic_camera.eye.y < FLOOR_Y[2]);
    if (!onEscalator)
    {
        // Normal height constraints
        float targetY = 1.7f;
        // Check which floor we are on
        if (basic_camera.eye.y > CEIL_Y[2])
            targetY = CEIL_Y[2] + 1.7f;
        else if (basic_camera.eye.y > FLOOR_Y[2])
            targetY = FLOOR_Y[2] + 1.7f;
        else if (basic_camera.eye.y > FLOOR_Y[1])
            targetY = FLOOR_Y[1] + 1.7f;

        // Inside Elevator override (lift at X:-37..-31, Z:22..28)
        bool inLift = (x >= -37 && x <= -31 && z >= 22 && z <= 28);
        if (inLift)
        {
            targetY = elevatorY + 1.7f;
        }
        else
        {
            // Right staircase G->1 (X:24..34, Z:22..26, UP is +X)
            bool onRStairG1 = (x >= 24 && x <= 34 && z >= 22 && z <= 26);
            if (onRStairG1)
            {
                float t = (x - 24.f) / 10.f;
                t = glm::clamp(t, 0.f, 1.f);
                targetY = FLOOR_Y[0] + t * (FLOOR_Y[1] - FLOOR_Y[0]) + 1.7f;
            }
            // Landing at top of G->1
            if (x >= 34 && x <= 38 && z >= 22 && z <= 30 && basic_camera.eye.y < FLOOR_Y[2] - 1)
                targetY = FLOOR_Y[1] + 1.7f;

            // Right staircase 1->2 (X:24..34, Z:26..30, UP is -X)
            bool onRStair12 = (x >= 24 && x <= 34 && z >= 26 && z <= 30 && basic_camera.eye.y > FLOOR_Y[1] - 1);
            if (onRStair12)
            {
                float t = (34.f - x) / 10.f;
                t = glm::clamp(t, 0.f, 1.f);
                targetY = FLOOR_Y[1] + t * (FLOOR_Y[2] - FLOOR_Y[1]) + 1.7f;
            }
            // Landing at top of 1->2 (corridor integration)
            if (x >= 22 && x <= 26 && z >= 26 && z <= 30 && basic_camera.eye.y > FLOOR_Y[2] - 1 && basic_camera.eye.y < CEIL_Y[2] - 1)
                targetY = FLOOR_Y[2] + 1.7f;

            // Right staircase 2->Roof (X:24..34, Z:22..26, UP is +X)
            bool onRStair2R = (x >= 24 && x <= 34 && z >= 22 && z <= 26 && basic_camera.eye.y > FLOOR_Y[2] - 1);
            if (onRStair2R)
            {
                float t = (x - 24.f) / 10.f;
                t = glm::clamp(t, 0.f, 1.f);
                targetY = FLOOR_Y[2] + t * (CEIL_Y[2] - FLOOR_Y[2]) + 1.7f;
            }
            // Landing at top of 2->Roof
            if (x >= 34 && x <= 38 && z >= 22 && z <= 30 && basic_camera.eye.y > CEIL_Y[2] - 1)
                targetY = CEIL_Y[2] + 1.7f;
        }
        // Smooth height transition
        basic_camera.eye.y = glm::mix(basic_camera.eye.y, targetY, deltaTime * 10.0f);
    }
    else
    {
        // On escalator: automatically move Z
        // 18 units in 5 seconds = 3.6 units/sec * escalatorDir
        float esSpeed = 3.6f * deltaTime * escalatorDir; 
        if (x > 0.f) {
            // Right escalator: UP (Z goes towards 12)
            basic_camera.eye.z -= esSpeed;
        } else {
            // Left escalator: DOWN (Z goes towards 30)
            basic_camera.eye.z += esSpeed;
        }
        z = basic_camera.eye.z;
        
        // Z=30 (near entrance) = ground level, Z=12 (far end) = 1st floor level
        float t = (30.f - z) / 18.0f;
        t = glm::clamp(t, 0.f, 1.f);
        basic_camera.eye.y = t * FLOOR_Y[1] + 1.7f;
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
    if (key == GLFW_KEY_1 && !(mods & GLFW_MOD_SHIFT))
    {
        ambientEnabled = !ambientEnabled;
        cout << "Ambient: " << (ambientEnabled ? "ON" : "OFF") << endl;
    }
    if (key == GLFW_KEY_2 && !(mods & GLFW_MOD_SHIFT))
    {
        pointLightsEnabled = !pointLightsEnabled;
        cout << "Point Lights: " << (pointLightsEnabled ? "ON" : "OFF") << endl;
    }
    if (key == GLFW_KEY_3 && !(mods & GLFW_MOD_SHIFT))
    {
        dirLightEnabled = !dirLightEnabled;
        cout << "Dir Light: " << (dirLightEnabled ? "ON" : "OFF") << endl;
    }
    if (key == GLFW_KEY_5 && !(mods & GLFW_MOD_SHIFT))
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
        // Call elevator / open door when near lift (X:-39..-29, Z:20..30)
        float x = basic_camera.eye.x, z = basic_camera.eye.z, y = basic_camera.eye.y;
        bool nearLift = (x >= -39 && x <= -29 && z >= 20 && z <= 30);
        if (nearLift)
        {
            // Determine which floor player is on
            int playerFloor = 0;
            if (y > FLOOR_Y[2] - 1)
                playerFloor = 2;
            else if (y > FLOOR_Y[1] - 1)
                playerFloor = 1;

            if (elevatorState == ELEV_IDLE)
            {
                if (elevatorCurrentFloor == playerFloor)
                {
                    // Lift is here, just open the door
                    elevatorState = ELEV_DOOR_OPENING;
                    elevatorDoorOffset = 0.0f;
                    elevatorTimer = 0.0f;
                    cout << "Elevator: Opening doors on Floor " << playerFloor << endl;
                }
                else
                {
                    // Call lift to this floor
                    elevatorTargetFloor = playerFloor;
                    elevatorState = ELEV_MOVING;
                    cout << "Elevator: Called to Floor " << playerFloor << endl;
                }
            }
            else if (elevatorState == ELEV_DOOR_OPEN || elevatorState == ELEV_DOOR_CLOSING)
            {
                // Re-open if closing
                elevatorState = ELEV_DOOR_OPENING;
                elevatorTimer = 0.0f;
                cout << "Elevator: Re-opening doors" << endl;
            }
        }
    }
    // Shift+0/1/2 for elevator floor selection (when inside lift)
    if (mods & GLFW_MOD_SHIFT)
    {
        int targetFloor = -1;
        if (key == GLFW_KEY_0)
        {
            targetFloor = 0;
        } // Shift+0 = Ground
        else if (key == GLFW_KEY_1)
        {
            targetFloor = 1;
        } // Shift+1 = 1st
        else if (key == GLFW_KEY_2)
        {
            targetFloor = 2;
        } // Shift+2 = 2nd

        if (targetFloor >= 0 && playerInsideLift)
        {
            elevatorTargetFloor = targetFloor;
            if (elevatorState == ELEV_DOOR_OPEN || elevatorState == ELEV_IDLE)
            {
                elevatorState = ELEV_DOOR_CLOSING;
                elevatorTimer = 0.0f;
            }
            const char *floorNames[] = {"Ground", "1st", "2nd"};
            cout << "Elevator going to " << floorNames[targetFloor] << " Floor" << endl;
            return; // Prevent other Shift+key actions
        }
    }
    if (key == GLFW_KEY_R)
    {
        escalatorDir = (escalatorDir + 2) % 3 - 1; // cycle: 1 -> 0 -> -1 -> 1
        const char *dirs[] = {"DOWN", "PAUSED", "UP"};
        cout << "Escalator: " << dirs[escalatorDir + 1] << endl;
    }
    if (key == GLFW_KEY_B)
    {
        barrierOpen = !barrierOpen;
        cout << "Boom Barrier: " << (barrierOpen ? "OPEN" : "CLOSED") << endl;
    }
    if (key == GLFW_KEY_P)
    {
        prayerDoorOpen = !prayerDoorOpen;
        cout << "Prayer Room Door: " << (prayerDoorOpen ? "OPEN" : "CLOSED") << endl;
    }
    if (key == GLFW_KEY_C)
    {
        cursorCaptured = !cursorCaptured;
        glfwSetInputMode(w, GLFW_CURSOR, cursorCaptured ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
        if (cursorCaptured)
            firstMouse = true; // avoid jump when re-capturing
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
    if (!cursorCaptured)
        return; // don't move camera when cursor is free
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
