#ifndef human_h
#define human_h

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <cstdlib>
#include <cmath>
#include "shader.h"
#include "sphere.h"
#include "cylinder.h"
#include "bezierCurve.h"

extern void drawCube(unsigned int &v, Shader &s, glm::mat4 pm, glm::vec3 c, glm::vec3 p, glm::vec3 sc, float sh, float a);

enum HumanState {
    HS_SPAWNING,
    HS_ROAMING,
    HS_HEADING_TO_SHOP,
    HS_AT_SHOP,
    HS_HEADING_TO_SEAT,
    HS_SITTING,
    HS_SHOPKEEPER
};

class Human {
public:
    glm::vec3 pos;
    float rotY;
    bool isFemale;
    glm::vec3 skinColor;
    glm::vec3 topColor;
    glm::vec3 botColor;

    HumanState state;
    float stateTimer;
    glm::vec3 targetPos;

    float walkCycle;
    bool isMoving;
    float speed;

    Human(glm::vec3 startPos, bool female, HumanState initState) {
        pos = startPos;
        isFemale = female;
        state = initState;
        rotY = 0.0f;
        stateTimer = 0.0f;
        walkCycle = 0.0f;
        isMoving = false;
        speed = 1.3f + ((rand() % 100) / 100.0f); // 1.3 to 2.3 units/sec

        // Skin Colors
        skinColor = glm::vec3(0.9f, 0.75f, 0.6f);
        if(rand() % 2 == 0) skinColor = glm::vec3(0.5f, 0.35f, 0.25f); 
        
        float r = (rand() % 100) / 100.f;
        float g = (rand() % 100) / 100.f;
        float b = (rand() % 100) / 100.f;
        topColor = glm::vec3(r, g, b); // Random shirt
        
        r = (rand() % 100) / 100.f;
        g = (rand() % 100) / 100.f;
        b = (rand() % 100) / 100.f;
        botColor = glm::vec3(r, g, b); // Random pants
        
        if (state != HS_SHOPKEEPER) {
            pickRandomTarget();
            state = HS_ROAMING;
            isMoving = true;
        }
    }

    void pickRandomTarget() {
        // Bounds for open space in Food Court: 
        // Aisle around Z = -25
        float tx = -18.0f + (rand() % 340) / 10.0f;
        float tz = -26.0f + (rand() % 40) / 10.0f; // Strict open corridor
        targetPos = glm::vec3(tx, pos.y, tz);
    }
    
    void update(float dt) {
        if (state == HS_SHOPKEEPER) return; // Shopkeepers stand still
        
        if (state == HS_AT_SHOP || state == HS_SITTING) {
            isMoving = false;
            stateTimer -= dt;
            if (stateTimer <= 0) {
                state = HS_ROAMING;
                pos.y = 14.6f; // Reset height just in case sitting dropped it
                pickRandomTarget();
                isMoving = true;
            }
            return;
        }

        if (isMoving) {
            glm::vec3 dir = targetPos - pos;
            dir.y = 0; // ignore vertical differences
            float dist = glm::length(dir);
            if (dist < 0.2f) {
                isMoving = false;
                if (state == HS_ROAMING) {
                    // Randomly decide next state
                    int roll = rand() % 100;
                    if(roll < 40) { 
                        // Go to a shop
                        state = HS_HEADING_TO_SHOP;
                        int s = rand() % 5;
                        if(s==0) targetPos = glm::vec3(-10, pos.y, -25);
                        else if(s==1) targetPos = glm::vec3(4, pos.y, -25);
                        else if(s==2) targetPos = glm::vec3(18, pos.y, -25);
                        else if(s==3) targetPos = glm::vec3(-24, pos.y, -5);
                        else if(s==4) targetPos = glm::vec3(24, pos.y, -15);
                        isMoving = true;
                    } else if(roll < 70) { 
                        // Sit down at a table group
                        state = HS_HEADING_TO_SEAT;
                        int sc = rand() % 5; int sr = rand() % 3;
                        targetPos = glm::vec3(-18.0f + sc * 8.0f + 1.5f, pos.y, -16.0f + sr * 9.0f);
                        isMoving = true;
                    } else { 
                        // Roam again
                        pickRandomTarget();
                        isMoving = true;
                    }
                } else if (state == HS_HEADING_TO_SHOP) {
                    state = HS_AT_SHOP;
                    stateTimer = 4.0f + (rand() % 4); // Order for 4-8 sec
                } else if (state == HS_HEADING_TO_SEAT) {
                    state = HS_SITTING;
                    stateTimer = 5.0f + (rand() % 10); // Sit 5-15 sec
                    rotY = 0.0f; 
                }
            } else {
                dir = glm::normalize(dir);
                pos += dir * speed * dt;
                
                // Keep bounding box tight just in case
                if (pos.x < -27.0f) pos.x = -27.0f;
                if (pos.x > 27.0f) pos.x = 27.0f;
                if (pos.z < -28.0f) pos.z = -28.0f;
                if (pos.z > 5.0f) pos.z = 5.0f;
                
                rotY = glm::degrees(atan2(dir.x, dir.z));
                walkCycle += dt * 8.0f; 
            }
        }
    }

    void draw(unsigned int &cubeVAO, Shader &s, glm::mat4 pm, Sphere* sphereObj, BezierRevolvedSurface* hairObj = nullptr, Cylinder* cylPtr = nullptr) {
        glm::vec3 rotAxis = {0, 1, 0};
        glm::mat4 root = glm::translate(pm, pos);
        root = glm::rotate(root, glm::radians(rotY), rotAxis);

        bool selling = (state == HS_SHOPKEEPER);
        bool sitting = (state == HS_SITTING);
        float cycle = walkCycle;

        glm::vec3 skinCol = skinColor;
        glm::vec3 shirtColor = topColor;
        glm::vec3 pantsColor = botColor;
        glm::vec3 shoeColor(0.05f);

        if (selling) {
            shirtColor = glm::mix(topColor, glm::vec3(0.85f, 0.15f, 0.15f), 0.5f);
            pantsColor = glm::vec3(0.9f);
        }

        float swing = std::sin(cycle);
        float swingCos = std::cos(cycle);

        float hipL = swing * 30.0f;
        float hipR = -swing * 30.0f;
        float kneeL = std::max(0.0f, -swing) * 40.0f;
        float kneeR = std::max(0.0f, swing) * 40.0f;

        float shoulderL = -swing * 25.0f;
        float shoulderR = swing * 25.0f;
        float elbowL = std::max(0.0f, -swing) * 20.0f + 5.0f;
        float elbowR = std::max(0.0f, swing) * 20.0f + 5.0f;

        float pelvicBounce = std::abs(swingCos) * 0.05f;

        if (selling) {
            hipL = 0; hipR = 0; kneeL = 0; kneeR = 0; pelvicBounce = 0;
            shoulderL = 10.0f + std::sin(cycle * 0.5f) * 15.0f;
            shoulderR = 25.0f + std::cos(cycle) * 20.0f;
            elbowL = 40.0f + std::sin(cycle * 0.5f) * 5.0f;
            elbowR = 30.0f + std::cos(cycle) * 10.0f;
        }

        if (sitting) {
            hipL = -90.0f; hipR = -90.0f;
            kneeL = 90.0f;  kneeR = 90.0f;
            shoulderL = -15.0f; shoulderR = -15.0f;
            elbowL = 60.0f;  elbowR = 60.0f;
            pelvicBounce = 0.0f;
            
            // Adjust sitting position to align with chair
            float sRy = glm::radians(rotY);
            // Move down and backward slightly into the chair
            root = glm::translate(root, glm::vec3(std::cos(sRy)*0.0f + std::sin(sRy)*0.25f, -0.4f, -std::sin(sRy)*0.0f + std::cos(sRy)*0.25f));
        }

        float pelvisY = 0.9f + pelvicBounce;
        glm::mat4 pelvisM = glm::translate(root, glm::vec3(0.0f, pelvisY, 0.0f));

        auto drawLeg = [&](int side, float hipFlex, float kneeFlex) {
            float dirMultiplier = (side == 0) ? -1.0f : 1.0f;

            glm::mat4 thighM = glm::translate(pelvisM, glm::vec3(0.12f * dirMultiplier, -0.05f, 0.0f));
            thighM = glm::rotate(thighM, glm::radians(hipFlex), glm::vec3(1, 0, 0));
            glm::mat4 drawThigh = glm::translate(thighM, glm::vec3(0.0f, -0.2f, 0.0f));
            drawThigh = glm::scale(drawThigh, glm::vec3(0.16f, 0.4f, 0.16f));
            if(cylPtr) cylPtr->draw(s, drawThigh, pantsColor);

            glm::mat4 calfM = glm::translate(thighM, glm::vec3(0.0f, -0.4f, 0.0f));
            calfM = glm::rotate(calfM, glm::radians(kneeFlex), glm::vec3(1, 0, 0));
            glm::mat4 drawCalf = glm::translate(calfM, glm::vec3(0.0f, -0.2f, 0.0f));
            drawCalf = glm::scale(drawCalf, glm::vec3(0.12f, 0.4f, 0.12f));
            if(cylPtr) cylPtr->draw(s, drawCalf, pantsColor);

            glm::mat4 shoeM = glm::translate(calfM, glm::vec3(0.0f, -0.42f, 0.06f));
            drawCube(cubeVAO, s, shoeM, shoeColor, glm::vec3(0), glm::vec3(0.14f, 0.08f, 0.25f), 32.0f, 1.0f);
        };

        if (!isFemale) {
            drawLeg(0, hipL, kneeL);
            drawLeg(1, hipR, kneeR);
        } else {
            drawLeg(0, hipL * 0.8f, kneeL * 0.6f);
            drawLeg(1, hipR * 0.8f, kneeR * 0.6f);
        }

        glm::mat4 torsoM = glm::translate(pelvisM, glm::vec3(0.0f, 0.0f, 0.0f));
        glm::mat4 drawTorso = glm::translate(torsoM, glm::vec3(0.0f, 0.35f, 0.0f));
        glm::vec3 torsoScale = isFemale ? glm::vec3(0.30f, 0.65f, 0.19f) : glm::vec3(0.35f, 0.7f, 0.2f);
        drawCube(cubeVAO, s, drawTorso, shirtColor, glm::vec3(0), torsoScale, 32.0f, 1.0f);

        if (isFemale && cylPtr) {
            for (int seg = 0; seg < 3; ++seg) {
                float t = seg / 2.0f;
                float segY = 0.20f - t * 0.28f;
                float radius = 0.20f + t * 0.12f;
                glm::mat4 dressM = glm::translate(torsoM, glm::vec3(0.0f, segY, 0.0f));
                dressM = glm::scale(dressM, glm::vec3(radius, 0.24f, radius));
                cylPtr->draw(s, dressM, glm::mix(shirtColor, glm::vec3(0.95f), 0.08f));
            }
        }

        auto drawArm = [&](int side, float shoulderFlex, float elbowFlex) {
            float dirMultiplier = (side == 0) ? -1.0f : 1.0f;
            float shoulderWidth = isFemale ? 0.20f : 0.24f;

            glm::mat4 uArmM = glm::translate(torsoM, glm::vec3(shoulderWidth * dirMultiplier, 0.58f, 0.0f));
            uArmM = glm::rotate(uArmM, glm::radians(shoulderFlex), glm::vec3(1, 0, 0));
            glm::mat4 drawUArm = glm::translate(uArmM, glm::vec3(0.0f, -0.15f, 0.0f));
            glm::vec3 uArmScale = isFemale ? glm::vec3(0.10f, 0.28f, 0.10f) : glm::vec3(0.12f, 0.3f, 0.12f);
            drawUArm = glm::scale(drawUArm, uArmScale);
            if(cylPtr) cylPtr->draw(s, drawUArm, shirtColor);

            glm::mat4 lArmM = glm::translate(uArmM, glm::vec3(0.0f, -0.3f, 0.0f));
            lArmM = glm::rotate(lArmM, glm::radians(-elbowFlex), glm::vec3(1, 0, 0));
            glm::mat4 drawLArm = glm::translate(lArmM, glm::vec3(0.0f, -0.15f, 0.0f));
            drawLArm = glm::scale(drawLArm, glm::vec3(0.1f, 0.3f, 0.1f));
            if(cylPtr) cylPtr->draw(s, drawLArm, skinCol);

            glm::mat4 handM = glm::translate(lArmM, glm::vec3(0.0f, -0.35f, 0.0f));
            handM = glm::scale(handM, glm::vec3(0.08f, 0.12f, 0.12f));
            if(sphereObj) sphereObj->drawSphereWithColor(s, handM, skinCol, 32.0f);
        };

        drawArm(0, shoulderL, elbowL);
        drawArm(1, shoulderR, elbowR);

        glm::mat4 neckM = glm::translate(torsoM, glm::vec3(0.0f, 0.75f, 0.0f));
        glm::mat4 drawNeck = glm::scale(neckM, glm::vec3(0.1f, 0.1f, 0.1f));
        if(cylPtr) cylPtr->draw(s, drawNeck, skinCol);

        glm::mat4 headM = glm::translate(neckM, glm::vec3(0.0f, 0.15f, 0.0f));
        // Use sphere for head
        if(sphereObj) {
            glm::mat4 sM = glm::scale(headM, glm::vec3(0.2f, 0.25f, 0.2f));
            sphereObj->drawSphereWithColor(s, sM, skinCol, 32.0f);
        } else {
            drawCube(cubeVAO, s, headM, skinCol, glm::vec3(0), glm::vec3(0.22f, 0.25f, 0.22f), 32.0f, 1.0f);
        }

        if (isFemale) {
            glm::mat4 hairM = glm::translate(neckM, glm::vec3(0.0f, 0.12f, -0.15f));
            hairM = glm::scale(hairM, glm::vec3(0.24f, 0.36f, 0.18f));
            if(sphereObj) sphereObj->drawSphereWithColor(s, hairM, glm::vec3(0.16f, 0.10f, 0.05f), 16.0f);
        } else {
            glm::mat4 hairM = glm::translate(neckM, glm::vec3(0.0f, 0.26f, -0.05f));
            drawCube(cubeVAO, s, hairM, glm::vec3(0.16f, 0.10f, 0.05f), glm::vec3(0), glm::vec3(0.22f, 0.08f, 0.25f), 16.0f, 1.0f);
        }
    }
};

#endif
