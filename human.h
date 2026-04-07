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
#include <algorithm>

extern std::vector<glm::vec3> g_occupiedTargets;

extern void drawCube(unsigned int &v, Shader &s, glm::mat4 pm, glm::vec3 c, glm::vec3 p, glm::vec3 sc, float sh, float a);

enum HumanState
{
    HS_ROAMING,
    HS_HEADING_TO_SHOP,
    HS_AT_SHOP,
    HS_HEADING_TO_SEAT,
    HS_SITTING,
    HS_SHOPKEEPER
};

class Human
{
public:
    glm::vec3 pos;
    float rotY;
    bool isFemale;
    glm::vec3 skinColor, topColor, botColor;

    HumanState state;
    float stateTimer;
    glm::vec3 targetPos;

    float walkCycle;
    bool isMoving;
    float speed;
    bool hasOccupiedSeat;
    float stuckTimer;
    glm::vec3 benchFinalTarget;
    bool hasBenchWaypoint;

    Human(glm::vec3 startPos, bool female, HumanState initState)
    {
        pos = startPos;
        isFemale = female;
        state = initState;
        rotY = 0.0f;
        walkCycle = 0.0f;
        isMoving = false;
        hasOccupiedSeat = false;
        stuckTimer = 0.0f;
        hasBenchWaypoint = false;
        benchFinalTarget = glm::vec3(0);
        speed = 1.3f + ((rand() % 100) / 100.0f);

        skinColor = glm::vec3(0.9f, 0.75f, 0.6f);
        if (rand() % 2 == 0)
            skinColor = glm::vec3(0.5f, 0.35f, 0.25f);

        topColor = glm::vec3((rand() % 100) / 100.f, (rand() % 100) / 100.f, (rand() % 100) / 100.f);
        botColor = glm::vec3((rand() % 100) / 100.f, (rand() % 100) / 100.f, (rand() % 100) / 100.f);

        if (state != HS_SHOPKEEPER)
        {
            pickRandomTarget();
            state = HS_ROAMING;
            isMoving = true;
        }
    }

    // Release any seat reservation this human holds
    void releaseSeat()
    {
        if (hasOccupiedSeat)
        {
            auto it = std::find(g_occupiedTargets.begin(), g_occupiedTargets.end(), targetPos);
            if (it != g_occupiedTargets.end()) g_occupiedTargets.erase(it);
            auto it2 = std::find(g_occupiedTargets.begin(), g_occupiedTargets.end(), benchFinalTarget);
            if (it2 != g_occupiedTargets.end()) g_occupiedTargets.erase(it2);
            hasOccupiedSeat = false;
        }
        hasBenchWaypoint = false;
    }

    // Try to reserve a seat. Returns true if successful.
    bool tryReserveSeat(glm::vec3 seatPos)
    {
        auto it = std::find(g_occupiedTargets.begin(), g_occupiedTargets.end(), seatPos);
        if (it != g_occupiedTargets.end()) return false; // occupied
        g_occupiedTargets.push_back(seatPos);
        hasOccupiedSeat = true;
        return true;
    }

    // Pick a random walkable target on the current floor
    void pickRandomTarget()
    {
        releaseSeat();
        float tx = 0, tz = 0;

        if (pos.y > 10.0f)
        {
            tx = -18.0f + (rand() % 360) / 10.0f;
            tz = -26.0f + (rand() % 260) / 10.0f;
        }
        else if (pos.y > 5.0f)
        {
            float myR = sqrt(pos.x * pos.x + (pos.z + 14.0f) * (pos.z + 14.0f));
            if (myR < 11.0f)
            {
                // Inside atrium ring - stay inside
                for (int a = 0; a < 20; a++) {
                    tx = -9.0f + (rand() % 180) / 10.0f;
                    tz = -23.0f + (rand() % 180) / 10.0f;
                    float dr = sqrt(tx * tx + (tz + 14.0f) * (tz + 14.0f));
                    if (dr > 2.5f && dr < 10.5f) break;
                }
            }
            else
            {
                // Outside atrium ring - stay outside
                for (int a = 0; a < 20; a++) {
                    tx = -16.0f + (rand() % 320) / 10.0f;
                    tz = -38.0f + (rand() % 700) / 10.0f;
                    float dr = sqrt(tx * tx + (tz + 14.0f) * (tz + 14.0f));
                    if (dr > 13.5f && !(tx > -5.5f && tx < 5.5f && tz > 9.5f && tz < 32.5f)) break;
                }
            }
        }
        else
        {
            tx = -35.0f + (rand() % 700) / 10.0f;
            tz = -35.0f + (rand() % 700) / 10.0f;
        }

        targetPos = glm::vec3(tx, pos.y, tz);
    }

    // Check if a position collides with walls/obstacles on the current floor
    bool checkCollision(glm::vec3 p)
    {
        if (p.y > 5.0f && p.y < 10.0f)
        {
            float dx = p.x, dz = p.z + 14.0f;
            float r = sqrt(dx * dx + dz * dz);

            if (r < 2.5f) return true; // Tree planter

            // Benches (skip for humans heading to or sitting on them)
            if (state != HS_HEADING_TO_SEAT && state != HS_SITTING)
            {
                if (abs(p.x) < 1.2f && abs(p.z - (-21.5f)) < 0.6f) return true;
                if (abs(p.x) < 1.2f && abs(p.z - (-6.5f)) < 0.6f) return true;
                if (abs(p.x - (-7.5f)) < 0.6f && abs(p.z - (-14.0f)) < 1.2f) return true;
                if (abs(p.x - 7.5f) < 0.6f && abs(p.z - (-14.0f)) < 1.2f) return true;
            }

            // Glass ring (with entrance gaps)
            if (r > 11.2f && r < 12.8f)
            {
                float deg = glm::degrees(atan2(dx, dz));
                if (deg < 0) deg += 360.0f;
                if (!((deg > 155.0f && deg < 205.0f) || (deg > 335.0f || deg < 25.0f)))
                    return true;
            }

            if (p.x < -17.0f || p.x > 17.0f) return true;  // Shop walls
            if (p.x > -5.5f && p.x < 5.5f && p.z > 9.5f && p.z < 32.5f) return true; // Escalator
            if (p.x < -38.0f || p.x > 38.0f || p.z < -38.0f || p.z > 38.0f) return true;
        }
        else if (p.y > 10.0f)
        {
            if (p.x < -27.0f || p.x > 27.0f || p.z < -28.0f || p.z > 5.0f) return true;
        }
        else
        {
            if (p.x < -38.0f || p.x > 38.0f || p.z < -38.0f || p.z > 38.0f) return true;
        }
        return false;
    }

    void update(float dt)
    {
        if (state == HS_SHOPKEEPER) return;

        // Waiting states (sitting / at shop counter)
        if (state == HS_AT_SHOP || state == HS_SITTING)
        {
            isMoving = false;
            walkCycle = 0.0f;
            stateTimer -= dt;
            if (stateTimer <= 0)
            {
                state = HS_ROAMING;
                pos.y = (pos.y < 5.0f) ? 0.0f : ((pos.y < 12.0f) ? 7.3f : 14.6f);
                pickRandomTarget(); // also releases seat
                isMoving = true;
            }
            return;
        }

        if (!isMoving) return;

        glm::vec3 dir = targetPos - pos;
        dir.y = 0;
        float dist = glm::length(dir);

        // === ARRIVED AT TARGET ===
        if (dist < 0.2f)
        {
            isMoving = false;
            pos.x = targetPos.x;
            pos.z = targetPos.z;

            if (state == HS_ROAMING)
            {
                int roll = rand() % 100;
                if (roll < 10)
                {
                    // Head to a shop counter
                    state = HS_HEADING_TO_SHOP;
                    if (pos.y > 10.0f)
                    {
                        int s = rand() % 5;
                        glm::vec3 shops[] = {{-10, pos.y, -25}, {4, pos.y, -25}, {18, pos.y, -25}, {-24, pos.y, -5}, {24, pos.y, -15}};
                        targetPos = shops[s];
                    }
                    else if (pos.y > 5.0f)
                    {
                        int s = rand() % 4;
                        glm::vec3 shops[] = {{-16, pos.y, -25}, {16, pos.y, -25}, {-16, pos.y, 5}, {16, pos.y, 5}};
                        targetPos = shops[s];
                    }
                    isMoving = true;
                }
                else if (roll < 95)
                {
                    // Head to a seat
                    state = HS_HEADING_TO_SEAT;

                    if (pos.y > 10.0f)
                    {
                        // 2nd floor dining chair
                        int sc = rand() % 5, sr = rand() % 3;
                        if (sc == 2 && sr == 1) sc = 3;
                        float cx = -18.0f + sc * 8.0f, cz = -16.0f + sr * 9.0f;
                        int ch = rand() % 4;
                        if (ch == 0) cz += 1.4f; else if (ch == 1) cz -= 1.4f;
                        else if (ch == 2) cx -= 1.4f; else cx += 1.4f;
                        targetPos = glm::vec3(cx, pos.y, cz);

                        if (!tryReserveSeat(targetPos)) { state = HS_ROAMING; pickRandomTarget(); }
                    }
                    else if (pos.y > 5.0f)
                    {
                        // 1st floor atrium bench
                        int bi = rand() % 4;
                        glm::vec3 benches[] = {{0, pos.y, -21.5f}, {0, pos.y, -6.5f}, {-7.5f, pos.y, -14.0f}, {7.5f, pos.y, -14.0f}};
                        glm::vec3 benchTarget = benches[bi];

                        if (!tryReserveSeat(benchTarget))
                        {
                            state = HS_ROAMING;
                            pickRandomTarget();
                        }
                        else
                        {
                            // Route through entrance if outside the ring
                            float myR = sqrt(pos.x * pos.x + (pos.z + 14.0f) * (pos.z + 14.0f));
                            if (myR < 11.0f)
                            {
                                targetPos = benchTarget;
                                hasBenchWaypoint = false;
                            }
                            else
                            {
                                float dN = abs(pos.z + 26.0f), dS = abs(pos.z + 2.0f);
                                targetPos = (dN < dS) ? glm::vec3(0, pos.y, -26.0f) : glm::vec3(0, pos.y, -2.0f);
                                benchFinalTarget = benchTarget;
                                hasBenchWaypoint = true;
                            }
                        }
                    }
                    isMoving = true;
                }
                else
                {
                    pickRandomTarget();
                    isMoving = true;
                }
            }
            else if (state == HS_HEADING_TO_SHOP)
            {
                state = HS_AT_SHOP;
                stateTimer = 5.0f + (rand() % 10);
                walkCycle = 0.0f;

                if (pos.y > 10.0f)
                {
                    if (targetPos.x == -10 || targetPos.x == 4 || targetPos.x == 18) rotY = 180.0f;
                    else if (targetPos.x == -24) rotY = -90.0f;
                    else if (targetPos.x == 24) rotY = 90.0f;
                }
                else if (pos.y > 5.0f)
                {
                    rotY = (targetPos.x < 0) ? -90.0f : 90.0f;
                }
            }
            else if (state == HS_HEADING_TO_SEAT)
            {
                if (hasBenchWaypoint && pos.y > 5.0f && pos.y < 10.0f)
                {
                    // Arrived at entrance waypoint, now head to actual bench
                    targetPos = benchFinalTarget;
                    hasBenchWaypoint = false;
                    isMoving = true;
                }
                else
                {
                    // Arrived at seat
                    state = HS_SITTING;
                    stateTimer = 30.0f + (rand() % 60);
                    walkCycle = 0.0f;

                    if (pos.y > 10.0f)
                    {
                        float tableX = std::round((targetPos.x + 18.0f) / 8.0f) * 8.0f - 18.0f;
                        float tableZ = std::round((targetPos.z + 16.0f) / 9.0f) * 9.0f - 16.0f;
                        rotY = glm::degrees(atan2(tableX - targetPos.x, tableZ - targetPos.z));
                    }
                    else if (pos.y > 5.0f)
                    {
                        // Hardcoded rotY matching bench orientation
                        if (targetPos.z < -20.0f) rotY = 0.0f;
                        else if (targetPos.z > -8.0f) rotY = 180.0f;
                        else if (targetPos.x < -5.0f) rotY = -90.0f;
                        else rotY = 90.0f;
                    }
                }
            }
        }
        // === MOVING TOWARD TARGET ===
        else
        {
            dir = glm::normalize(dir);
            float sx = dir.x * speed * dt, sz = dir.z * speed * dt;

            bool movedX = !checkCollision(pos + glm::vec3(sx, 0, 0));
            bool movedZ = !checkCollision(glm::vec3(pos.x, pos.y, pos.z + sz));

            if (movedX) pos.x += sx;
            if (movedZ) pos.z += sz;

            if (movedX || movedZ)
            {
                stuckTimer = 0.0f;
                rotY = glm::degrees(atan2(dir.x, dir.z));
                walkCycle += dt * 8.0f;
            }
            else
            {
                walkCycle = 0.0f; // Stop animation when stuck
                stuckTimer += dt;

                if (stuckTimer > 0.5f)
                {
                    stuckTimer = 0.0f;
                    // Push backward and reroute
                    float rad = glm::radians(rotY);
                    glm::vec3 push(pos.x - sin(rad) * 1.5f, pos.y, pos.z - cos(rad) * 1.5f);
                    if (!checkCollision(push)) { pos.x = push.x; pos.z = push.z; }
                    state = HS_ROAMING;
                    pickRandomTarget(); // also releases seat
                }
            }
        }
    }

    void draw(unsigned int &cubeVAO, Shader &s, glm::mat4 pm, Sphere *sphereObj, BezierRevolvedSurface *hairObj = nullptr, Cylinder *cylPtr = nullptr)
    {
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

        if (selling)
        {
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

        if (selling)
        {
            hipL = 0; hipR = 0; kneeL = 0; kneeR = 0; pelvicBounce = 0;
            shoulderL = 10.0f + std::sin(cycle * 0.5f) * 15.0f;
            shoulderR = 25.0f + std::cos(cycle) * 20.0f;
            elbowL = 40.0f + std::sin(cycle * 0.5f) * 5.0f;
            elbowR = 30.0f + std::cos(cycle) * 10.0f;
        }

        if (sitting)
        {
            hipL = -90.0f; hipR = -90.0f; kneeL = 90.0f; kneeR = 90.0f;
            shoulderL = -15.0f; shoulderR = -15.0f;
            elbowL = 60.0f; elbowR = 60.0f;
            pelvicBounce = 0.0f;

            float sRy = glm::radians(rotY);
            root = glm::translate(root, glm::vec3(std::sin(sRy) * 0.25f, -0.4f, std::cos(sRy) * 0.25f));
        }

        float pelvisY = 0.9f + pelvicBounce;
        glm::mat4 pelvisM = glm::translate(root, glm::vec3(0.0f, pelvisY, 0.0f));

        auto drawLeg = [&](int side, float hipFlex, float kneeFlex)
        {
            float dir = (side == 0) ? -1.0f : 1.0f;

            glm::mat4 thighM = glm::translate(pelvisM, glm::vec3(0.12f * dir, -0.05f, 0.0f));
            thighM = glm::rotate(thighM, glm::radians(hipFlex), glm::vec3(1, 0, 0));
            glm::mat4 drawThigh = glm::scale(glm::translate(thighM, glm::vec3(0, -0.2f, 0)), glm::vec3(0.16f, 0.4f, 0.16f));
            if (cylPtr) cylPtr->draw(s, drawThigh, pantsColor);

            glm::mat4 calfM = glm::translate(thighM, glm::vec3(0, -0.4f, 0));
            calfM = glm::rotate(calfM, glm::radians(kneeFlex), glm::vec3(1, 0, 0));
            glm::mat4 drawCalf = glm::scale(glm::translate(calfM, glm::vec3(0, -0.2f, 0)), glm::vec3(0.12f, 0.4f, 0.12f));
            if (cylPtr) cylPtr->draw(s, drawCalf, pantsColor);

            glm::mat4 shoeM = glm::translate(calfM, glm::vec3(0, -0.42f, 0.06f));
            drawCube(cubeVAO, s, shoeM, shoeColor, glm::vec3(0), glm::vec3(0.14f, 0.08f, 0.25f), 32.0f, 1.0f);
        };

        if (!isFemale) { drawLeg(0, hipL, kneeL); drawLeg(1, hipR, kneeR); }
        else { drawLeg(0, hipL * 0.8f, kneeL * 0.6f); drawLeg(1, hipR * 0.8f, kneeR * 0.6f); }

        glm::mat4 torsoM = glm::translate(pelvisM, glm::vec3(0));
        glm::mat4 drawTorso = glm::translate(torsoM, glm::vec3(0, 0.35f, 0));
        glm::vec3 torsoScale = isFemale ? glm::vec3(0.30f, 0.65f, 0.19f) : glm::vec3(0.35f, 0.7f, 0.2f);
        drawCube(cubeVAO, s, drawTorso, shirtColor, glm::vec3(0), torsoScale, 32.0f, 1.0f);

        if (isFemale && cylPtr)
        {
            for (int seg = 0; seg < 3; ++seg)
            {
                float t = seg / 2.0f;
                float segY = 0.20f - t * 0.28f;
                float radius = 0.20f + t * 0.12f;
                glm::mat4 dressM = glm::scale(glm::translate(torsoM, glm::vec3(0, segY, 0)), glm::vec3(radius, 0.24f, radius));
                cylPtr->draw(s, dressM, glm::mix(shirtColor, glm::vec3(0.95f), 0.08f));
            }
        }

        auto drawArm = [&](int side, float shoulderFlex, float elbowFlex)
        {
            float dir = (side == 0) ? -1.0f : 1.0f;
            float sw = isFemale ? 0.20f : 0.24f;

            glm::mat4 uArmM = glm::translate(torsoM, glm::vec3(sw * dir, 0.58f, 0));
            uArmM = glm::rotate(uArmM, glm::radians(shoulderFlex), glm::vec3(1, 0, 0));
            glm::vec3 uS = isFemale ? glm::vec3(0.10f, 0.28f, 0.10f) : glm::vec3(0.12f, 0.3f, 0.12f);
            if (cylPtr) cylPtr->draw(s, glm::scale(glm::translate(uArmM, glm::vec3(0, -0.15f, 0)), uS), shirtColor);

            glm::mat4 lArmM = glm::translate(uArmM, glm::vec3(0, -0.3f, 0));
            lArmM = glm::rotate(lArmM, glm::radians(-elbowFlex), glm::vec3(1, 0, 0));
            if (cylPtr) cylPtr->draw(s, glm::scale(glm::translate(lArmM, glm::vec3(0, -0.15f, 0)), glm::vec3(0.1f, 0.3f, 0.1f)), skinCol);

            glm::mat4 handM = glm::scale(glm::translate(lArmM, glm::vec3(0, -0.35f, 0)), glm::vec3(0.08f, 0.12f, 0.12f));
            if (sphereObj) sphereObj->drawSphereWithColor(s, handM, skinCol, 32.0f);
        };

        drawArm(0, shoulderL, elbowL);
        drawArm(1, shoulderR, elbowR);

        glm::mat4 neckM = glm::translate(torsoM, glm::vec3(0, 0.75f, 0));
        if (cylPtr) cylPtr->draw(s, glm::scale(neckM, glm::vec3(0.1f)), skinCol);

        glm::mat4 headM = glm::translate(neckM, glm::vec3(0, 0.15f, 0));
        if (sphereObj)
            sphereObj->drawSphereWithColor(s, glm::scale(headM, glm::vec3(0.2f, 0.25f, 0.2f)), skinCol, 32.0f);
        else
            drawCube(cubeVAO, s, headM, skinCol, glm::vec3(0), glm::vec3(0.22f, 0.25f, 0.22f), 32.0f, 1.0f);

        if (isFemale)
        {
            glm::mat4 hairM = glm::scale(glm::translate(neckM, glm::vec3(0, 0.12f, -0.15f)), glm::vec3(0.24f, 0.36f, 0.18f));
            if (sphereObj) sphereObj->drawSphereWithColor(s, hairM, glm::vec3(0.16f, 0.10f, 0.05f), 16.0f);
        }
        else
        {
            glm::mat4 hairM = glm::translate(neckM, glm::vec3(0, 0.26f, -0.05f));
            drawCube(cubeVAO, s, hairM, glm::vec3(0.16f, 0.10f, 0.05f), glm::vec3(0), glm::vec3(0.22f, 0.08f, 0.25f), 16.0f, 1.0f);
        }
    }
};

#endif
