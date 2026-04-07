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
    HS_SPAWNING,
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
    glm::vec3 skinColor;
    glm::vec3 topColor;
    glm::vec3 botColor;

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
        speed = 1.3f + ((rand() % 100) / 100.0f); // 1.3 to 2.3 units/sec

        // Skin Colors
        skinColor = glm::vec3(0.9f, 0.75f, 0.6f);
        if (rand() % 2 == 0)
            skinColor = glm::vec3(0.5f, 0.35f, 0.25f);

        float r = (rand() % 100) / 100.f;
        float g = (rand() % 100) / 100.f;
        float b = (rand() % 100) / 100.f;
        topColor = glm::vec3(r, g, b); // Random shirt

        r = (rand() % 100) / 100.f;
        g = (rand() % 100) / 100.f;
        b = (rand() % 100) / 100.f;
        botColor = glm::vec3(r, g, b); // Random pants

        if (state != HS_SHOPKEEPER)
        {
            pickRandomTarget();
            state = HS_ROAMING;
            isMoving = true;
        }
    }

    void releaseSeat()
    {
        if (hasOccupiedSeat)
        {
            // Try removing the actual targetPos
            auto it = std::find(g_occupiedTargets.begin(), g_occupiedTargets.end(), targetPos);
            if (it != g_occupiedTargets.end())
                g_occupiedTargets.erase(it);
            // Also try removing the benchFinalTarget (if we were en-route via waypoint)
            auto it2 = std::find(g_occupiedTargets.begin(), g_occupiedTargets.end(), benchFinalTarget);
            if (it2 != g_occupiedTargets.end())
                g_occupiedTargets.erase(it2);
            hasOccupiedSeat = false;
        }
        hasBenchWaypoint = false;
    }

    void pickRandomTarget()
    {
        releaseSeat();
        float tx, tz;
        
        if (pos.y > 10.0f) {
            // 2nd floor food court
            tx = -18.0f + (rand() % 360) / 10.0f;
            tz = -26.0f + (rand() % 260) / 10.0f;
        } else if (pos.y > 5.0f) {
            // 1st floor - check if we're inside or outside the atrium ring
            float myDx = pos.x;
            float myDz = pos.z + 14.0f;
            float myR = sqrt(myDx * myDx + myDz * myDz);
            bool insideAtrium = (myR < 11.0f);
            
            if (insideAtrium) {
                // Stay inside the ring - pick a point within radius 10
                for (int attempt = 0; attempt < 20; attempt++) {
                    tx = -9.0f + (rand() % 180) / 10.0f;  // -9 to 9
                    tz = -23.0f + (rand() % 180) / 10.0f;  // -23 to -5
                    float dr = sqrt(tx * tx + (tz + 14.0f) * (tz + 14.0f));
                    if (dr > 2.0f && dr < 10.5f) break;
                }
            } else {
                // Stay outside the ring - pick walkway points
                for (int attempt = 0; attempt < 20; attempt++) {
                    tx = -16.0f + (rand() % 320) / 10.0f;  // -16 to 16
                    tz = -38.0f + (rand() % 700) / 10.0f;  // -38 to 32
                    float dr = sqrt(tx * tx + (tz + 14.0f) * (tz + 14.0f));
                    // Must be outside the ring AND not in escalator hole
                    if (dr > 13.5f && !(tx > -5.5f && tx < 5.5f && tz > 9.5f && tz < 32.5f))
                        break;
                }
            }
        } else {
            // Ground floor
            tx = -35.0f + (rand() % 700) / 10.0f;
            tz = -35.0f + (rand() % 700) / 10.0f;
        }
        
        targetPos = glm::vec3(tx, pos.y, tz);
    }

    bool checkCollision(glm::vec3 testPos)
    {
        if (testPos.y > 5.0f && testPos.y < 10.0f)
        {
            // Atrium Glass Ring and Tree
            float dx = testPos.x;
            float dz = testPos.z + 14.0f;
            float r = sqrt(dx * dx + dz * dz);
            
            if (r < 2.5f) return true; // Tree planter collision (larger zone)
            
            // Bench collision zones (skip if heading to sit on a bench)
            if (state != HS_HEADING_TO_SEAT && state != HS_SITTING)
            {
                // North bench at (0, -21.5)
                if (abs(testPos.x) < 1.2f && abs(testPos.z - (-21.5f)) < 0.6f) return true;
                // South bench at (0, -6.5)
                if (abs(testPos.x) < 1.2f && abs(testPos.z - (-6.5f)) < 0.6f) return true;
                // West bench at (-7.5, -14)
                if (abs(testPos.x - (-7.5f)) < 0.6f && abs(testPos.z - (-14.0f)) < 1.2f) return true;
                // East bench at (7.5, -14)
                if (abs(testPos.x - 7.5f) < 0.6f && abs(testPos.z - (-14.0f)) < 1.2f) return true;
            }
            
            // Glass ring is at radius 12.0f
            if (r > 11.2f && r < 12.8f)
            {
                float deg = glm::degrees(atan2(dx, dz));
                if (deg < 0) deg += 360.0f;
                // Entrances are (160..200) and (340..360, 0..20)
                if (!((deg > 155.0f && deg < 205.0f) || (deg > 335.0f || deg < 25.0f)))
                {
                    return true;
                }
            }
            
            // Shops (Customers completely stay in the main corridor)
            if (testPos.x < -17.0f || testPos.x > 17.0f) return true;
            
            // Escalator hole south
            if (testPos.x > -5.5f && testPos.x < 5.5f && testPos.z > 9.5f && testPos.z < 32.5f) return true;
            
            // Outer limits
            if (testPos.x < -38.0f || testPos.x > 38.0f || testPos.z < -38.0f || testPos.z > 38.0f) return true;
        }
        else if (testPos.y > 10.0f)
        {
            if (testPos.x < -27.0f || testPos.x > 27.0f || testPos.z < -28.0f || testPos.z > 5.0f) return true;
        }
        else
        {
            if (testPos.x < -38.0f || testPos.x > 38.0f || testPos.z < -38.0f || testPos.z > 38.0f) return true;
        }
        return false;
    }

    void update(float dt)
    {
        if (state == HS_SHOPKEEPER)
            return; // Shopkeepers stand still

        if (state == HS_AT_SHOP || state == HS_SITTING)
        {
            isMoving = false;
            walkCycle = 0.0f; // Ensure walk posture is completely reset
            stateTimer -= dt;
            if (stateTimer <= 0)
            {
                releaseSeat();
                state = HS_ROAMING;
                // Determine reset height based on where they were sitting
                float resetY = (pos.y < 5.0f) ? 0.0f : ((pos.y < 12.0f) ? 7.3f : 14.6f);
                pos.y = resetY;
                pickRandomTarget();
                isMoving = true;
            }
            return;
        }

        if (isMoving)
        {
            glm::vec3 dir = targetPos - pos;
            dir.y = 0; // ignore vertical differences
            float dist = glm::length(dir);
            if (dist < 0.2f)
            {
                isMoving = false;
                pos.x = targetPos.x; // Snap precisely to target to avoid drift
                pos.z = targetPos.z;

                if (state == HS_ROAMING)
                {
                    // Randomly decide next state
                    int roll = rand() % 100;
                    if (roll < 10)
                    {
                        // Go to a shop (10% chance)
                        state = HS_HEADING_TO_SHOP;
                        if (pos.y > 10.0f)
                        {
                            // 2nd floor shops
                            int s = rand() % 5;
                            if (s == 0)
                                targetPos = glm::vec3(-10, pos.y, -25);
                            else if (s == 1)
                                targetPos = glm::vec3(4, pos.y, -25);
                            else if (s == 2)
                                targetPos = glm::vec3(18, pos.y, -25);
                            else if (s == 3)
                                targetPos = glm::vec3(-24, pos.y, -5);
                            else if (s == 4)
                                targetPos = glm::vec3(24, pos.y, -15);
                        }
                        else if (pos.y > 5.0f)
                        {
                            // 1st floor shops
                            int s = rand() % 4;
                            if (s == 0)
                                targetPos = glm::vec3(-16, pos.y, -25); // Bookstore front
                            else if (s == 1)
                                targetPos = glm::vec3(16, pos.y, -25); // Gems front
                            else if (s == 2)
                                targetPos = glm::vec3(-16, pos.y, 5); // Clothing front
                            else if (s == 3)
                                targetPos = glm::vec3(16, pos.y, 5); // Tech front
                        }
                        isMoving = true;
                    }
                    else if (roll < 95)
                    {
                        // Sit down at a table group
                        state = HS_HEADING_TO_SEAT;
                        bool seatHandled = false;
                        // Determine which floor we are on
                        if (pos.y > 10.0f)
                        {
                            // 2nd Floor Dining Logic
                            int sc = rand() % 5;
                            int sr = rand() % 3;
                            if (sc == 2 && sr == 1)
                                sc = 3;
                            float basex = -18.0f + sc * 8.0f;
                            float basez = -16.0f + sr * 9.0f;
                            int chair = rand() % 4;
                            float cx = basex, cz = basez;
                            if (chair == 0)
                                cz += 1.4f;
                            else if (chair == 1)
                                cz -= 1.4f;
                            else if (chair == 2)
                                cx -= 1.4f;
                            else if (chair == 3)
                                cx += 1.4f;
                            targetPos = glm::vec3(cx, pos.y, cz);
                        }
                        else if (pos.y > 5.0f)
                        {
                            // 1st Floor Atrium Benches Logic
                            int benchIdx = rand() % 4;
                            float bx = 0, bz = 0;
                            if (benchIdx == 0) { bx = 0; bz = -21.5f; }
                            else if (benchIdx == 1) { bx = 0; bz = -6.5f; }
                            else if (benchIdx == 2) { bx = -7.5f; bz = -14.0f; }
                            else if (benchIdx == 3) { bx = 7.5f; bz = -14.0f; }
                            
                            glm::vec3 benchTarget(bx, pos.y, bz);
                            auto it = std::find(g_occupiedTargets.begin(), g_occupiedTargets.end(), benchTarget);
                            if (it != g_occupiedTargets.end())
                            {
                                // Already occupied, just roam instead
                                state = HS_ROAMING;
                                pickRandomTarget();
                            }
                            else
                            {
                                g_occupiedTargets.push_back(benchTarget);
                                hasOccupiedSeat = true;
                                
                                // Check if we're already inside the atrium
                                float myDx = pos.x;
                                float myDz = pos.z + 14.0f;
                                float myR = sqrt(myDx * myDx + myDz * myDz);
                                
                                if (myR < 11.0f) {
                                    targetPos = benchTarget;
                                    hasBenchWaypoint = false;
                                } else {
                                    // Route through nearest entrance first
                                    float distToNorth = abs(pos.z - (-26.0f));
                                    float distToSouth = abs(pos.z - (-2.0f));
                                    if (distToNorth < distToSouth)
                                        targetPos = glm::vec3(0, pos.y, -26.0f);
                                    else
                                        targetPos = glm::vec3(0, pos.y, -2.0f);
                                    benchFinalTarget = benchTarget;
                                    hasBenchWaypoint = true;
                                }
                            }
                            seatHandled = true;
                        }

                        // Check if seat is occupied (for 2nd floor only)
                        if (!seatHandled)
                        {
                            auto it = std::find(g_occupiedTargets.begin(), g_occupiedTargets.end(), targetPos);
                            if (it != g_occupiedTargets.end())
                            {
                                state = HS_ROAMING;
                                pickRandomTarget();
                            }
                            else
                            {
                                g_occupiedTargets.push_back(targetPos);
                                hasOccupiedSeat = true;
                            }
                        }
                        isMoving = true;
                    }
                    else
                    {
                        // Roam again (5% chance)
                        pickRandomTarget();
                        isMoving = true;
                    }
                }
                else if (state == HS_HEADING_TO_SHOP)
                {
                    state = HS_AT_SHOP;
                    stateTimer = 5.0f + (rand() % 10); // Order for 5-15 sec
                    walkCycle = 0.0f;                  // Stop walking animation

                    if (pos.y > 10.0f)
                    {
                        // Face the shopkeeper perfectly (2nd floor Food Court)
                        if (targetPos.x == -10 || targetPos.x == 4 || targetPos.x == 18)
                        {
                            rotY = 180.0f; // Facing North
                        }
                        else if (targetPos.x == -24)
                        {
                            rotY = -90.0f; // Facing West
                        }
                        else if (targetPos.x == 24)
                        {
                            rotY = 90.0f; // Facing East
                        }
                    }
                    else if (pos.y > 5.0f)
                    {
                        // Face the shopkeeper perfectly (1st floor Shops)
                        if (targetPos.x == -16)
                        {
                            rotY = -90.0f; // Facing West Wall
                        }
                        else if (targetPos.x == 16)
                        {
                            rotY = 90.0f; // Facing East Wall
                        }
                    }
                }
                else if (state == HS_HEADING_TO_SEAT)
                {
                    // Check if this was a waypoint (entrance) arrival for 1st floor bench
                    if (hasBenchWaypoint && pos.y > 5.0f && pos.y < 10.0f)
                    {
                        // We arrived at the entrance, now head to the actual bench
                        targetPos = benchFinalTarget;
                        hasBenchWaypoint = false;
                        isMoving = true;
                    }
                    else
                    {
                        state = HS_SITTING;
                        stateTimer = 30.0f + (rand() % 60); // Sit 30-90 sec
                        walkCycle = 0.0f;                   // Stop walking animation

                        if (pos.y > 10.0f)
                        {
                            // Face the table center
                            float tableX = std::round((targetPos.x + 18.0f) / 8.0f) * 8.0f - 18.0f;
                            float tableZ = std::round((targetPos.z + 16.0f) / 9.0f) * 9.0f - 16.0f;
                            rotY = glm::degrees(atan2(tableX - targetPos.x, tableZ - targetPos.z));
                        }
                        else if (pos.y > 5.0f)
                        {
                            // Atrium benches - hardcoded rotY matching bench orientation
                            // Bench backrest is at local -Z, person faces local +Z
                            if (targetPos.z < -20.0f) // North bench (0, -21.5)
                                rotY = 0.0f;  // bench rot=0, face +Z toward tree
                            else if (targetPos.z > -8.0f) // South bench (0, -6.5)
                                rotY = 180.0f; // bench rot=180, face -Z toward tree
                            else if (targetPos.x < -5.0f) // West bench (-7.5, -14)
                                rotY = -90.0f; // bench rot=-90, face +X toward tree
                            else // East bench (7.5, -14)
                                rotY = 90.0f; // bench rot=90, face -X toward tree
                        }
                    }
                }
            }
            else
            {
                dir = glm::normalize(dir);
                float stepX = dir.x * speed * dt;
                float stepZ = dir.z * speed * dt;
                
                glm::vec3 nextX = pos + glm::vec3(stepX, 0, 0);
                glm::vec3 nextZ = glm::vec3(pos.x, pos.y, pos.z + stepZ);
                
                bool movedX = false, movedZ = false;
                if (!checkCollision(nextX))
                {
                    pos.x = nextX.x;
                    movedX = true;
                }
                if (!checkCollision(nextZ))
                {
                    pos.z = nextZ.z;
                    movedZ = true;
                }
                
                if (movedX || movedZ)
                {
                    stuckTimer = 0.0f;
                    rotY = glm::degrees(atan2(dir.x, dir.z));
                    walkCycle += dt * 8.0f;
                }
                else
                {
                    // Not moving at all - stop walk animation immediately
                    walkCycle = 0.0f;
                    stuckTimer += dt;
                    
                    if (stuckTimer > 0.5f)
                    {
                        // Push away from obstacle and pick new target
                        stuckTimer = 0.0f;
                        releaseSeat();
                        state = HS_ROAMING;
                        
                        // Push backward (opposite of facing direction)
                        float pushDist = 1.5f;
                        float rad = glm::radians(rotY);
                        float pushX = pos.x - sin(rad) * pushDist;
                        float pushZ = pos.z - cos(rad) * pushDist;
                        glm::vec3 pushPos(pushX, pos.y, pushZ);
                        if (!checkCollision(pushPos))
                        {
                            pos.x = pushX;
                            pos.z = pushZ;
                        }
                        
                        pickRandomTarget();
                    }
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
            hipL = 0;
            hipR = 0;
            kneeL = 0;
            kneeR = 0;
            pelvicBounce = 0;
            shoulderL = 10.0f + std::sin(cycle * 0.5f) * 15.0f;
            shoulderR = 25.0f + std::cos(cycle) * 20.0f;
            elbowL = 40.0f + std::sin(cycle * 0.5f) * 5.0f;
            elbowR = 30.0f + std::cos(cycle) * 10.0f;
        }

        if (sitting)
        {
            hipL = -90.0f;
            hipR = -90.0f;
            kneeL = 90.0f;
            kneeR = 90.0f;
            shoulderL = -15.0f;
            shoulderR = -15.0f;
            elbowL = 60.0f;
            elbowR = 60.0f;
            pelvicBounce = 0.0f;

            // Adjust sitting position to align with chair
            float sRy = glm::radians(rotY);
            // Move down and backward slightly into the chair
            root = glm::translate(root, glm::vec3(std::cos(sRy) * 0.0f + std::sin(sRy) * 0.25f, -0.4f, -std::sin(sRy) * 0.0f + std::cos(sRy) * 0.25f));
        }

        float pelvisY = 0.9f + pelvicBounce;
        glm::mat4 pelvisM = glm::translate(root, glm::vec3(0.0f, pelvisY, 0.0f));

        auto drawLeg = [&](int side, float hipFlex, float kneeFlex)
        {
            float dirMultiplier = (side == 0) ? -1.0f : 1.0f;

            glm::mat4 thighM = glm::translate(pelvisM, glm::vec3(0.12f * dirMultiplier, -0.05f, 0.0f));
            thighM = glm::rotate(thighM, glm::radians(hipFlex), glm::vec3(1, 0, 0));
            glm::mat4 drawThigh = glm::translate(thighM, glm::vec3(0.0f, -0.2f, 0.0f));
            drawThigh = glm::scale(drawThigh, glm::vec3(0.16f, 0.4f, 0.16f));
            if (cylPtr)
                cylPtr->draw(s, drawThigh, pantsColor);

            glm::mat4 calfM = glm::translate(thighM, glm::vec3(0.0f, -0.4f, 0.0f));
            calfM = glm::rotate(calfM, glm::radians(kneeFlex), glm::vec3(1, 0, 0));
            glm::mat4 drawCalf = glm::translate(calfM, glm::vec3(0.0f, -0.2f, 0.0f));
            drawCalf = glm::scale(drawCalf, glm::vec3(0.12f, 0.4f, 0.12f));
            if (cylPtr)
                cylPtr->draw(s, drawCalf, pantsColor);

            glm::mat4 shoeM = glm::translate(calfM, glm::vec3(0.0f, -0.42f, 0.06f));
            drawCube(cubeVAO, s, shoeM, shoeColor, glm::vec3(0), glm::vec3(0.14f, 0.08f, 0.25f), 32.0f, 1.0f);
        };

        if (!isFemale)
        {
            drawLeg(0, hipL, kneeL);
            drawLeg(1, hipR, kneeR);
        }
        else
        {
            drawLeg(0, hipL * 0.8f, kneeL * 0.6f);
            drawLeg(1, hipR * 0.8f, kneeR * 0.6f);
        }

        glm::mat4 torsoM = glm::translate(pelvisM, glm::vec3(0.0f, 0.0f, 0.0f));
        glm::mat4 drawTorso = glm::translate(torsoM, glm::vec3(0.0f, 0.35f, 0.0f));
        glm::vec3 torsoScale = isFemale ? glm::vec3(0.30f, 0.65f, 0.19f) : glm::vec3(0.35f, 0.7f, 0.2f);
        drawCube(cubeVAO, s, drawTorso, shirtColor, glm::vec3(0), torsoScale, 32.0f, 1.0f);

        if (isFemale && cylPtr)
        {
            for (int seg = 0; seg < 3; ++seg)
            {
                float t = seg / 2.0f;
                float segY = 0.20f - t * 0.28f;
                float radius = 0.20f + t * 0.12f;
                glm::mat4 dressM = glm::translate(torsoM, glm::vec3(0.0f, segY, 0.0f));
                dressM = glm::scale(dressM, glm::vec3(radius, 0.24f, radius));
                cylPtr->draw(s, dressM, glm::mix(shirtColor, glm::vec3(0.95f), 0.08f));
            }
        }

        auto drawArm = [&](int side, float shoulderFlex, float elbowFlex)
        {
            float dirMultiplier = (side == 0) ? -1.0f : 1.0f;
            float shoulderWidth = isFemale ? 0.20f : 0.24f;

            glm::mat4 uArmM = glm::translate(torsoM, glm::vec3(shoulderWidth * dirMultiplier, 0.58f, 0.0f));
            uArmM = glm::rotate(uArmM, glm::radians(shoulderFlex), glm::vec3(1, 0, 0));
            glm::mat4 drawUArm = glm::translate(uArmM, glm::vec3(0.0f, -0.15f, 0.0f));
            glm::vec3 uArmScale = isFemale ? glm::vec3(0.10f, 0.28f, 0.10f) : glm::vec3(0.12f, 0.3f, 0.12f);
            drawUArm = glm::scale(drawUArm, uArmScale);
            if (cylPtr)
                cylPtr->draw(s, drawUArm, shirtColor);

            glm::mat4 lArmM = glm::translate(uArmM, glm::vec3(0.0f, -0.3f, 0.0f));
            lArmM = glm::rotate(lArmM, glm::radians(-elbowFlex), glm::vec3(1, 0, 0));
            glm::mat4 drawLArm = glm::translate(lArmM, glm::vec3(0.0f, -0.15f, 0.0f));
            drawLArm = glm::scale(drawLArm, glm::vec3(0.1f, 0.3f, 0.1f));
            if (cylPtr)
                cylPtr->draw(s, drawLArm, skinCol);

            glm::mat4 handM = glm::translate(lArmM, glm::vec3(0.0f, -0.35f, 0.0f));
            handM = glm::scale(handM, glm::vec3(0.08f, 0.12f, 0.12f));
            if (sphereObj)
                sphereObj->drawSphereWithColor(s, handM, skinCol, 32.0f);
        };

        drawArm(0, shoulderL, elbowL);
        drawArm(1, shoulderR, elbowR);

        glm::mat4 neckM = glm::translate(torsoM, glm::vec3(0.0f, 0.75f, 0.0f));
        glm::mat4 drawNeck = glm::scale(neckM, glm::vec3(0.1f, 0.1f, 0.1f));
        if (cylPtr)
            cylPtr->draw(s, drawNeck, skinCol);

        glm::mat4 headM = glm::translate(neckM, glm::vec3(0.0f, 0.15f, 0.0f));
        // Use sphere for head
        if (sphereObj)
        {
            glm::mat4 sM = glm::scale(headM, glm::vec3(0.2f, 0.25f, 0.2f));
            sphereObj->drawSphereWithColor(s, sM, skinCol, 32.0f);
        }
        else
        {
            drawCube(cubeVAO, s, headM, skinCol, glm::vec3(0), glm::vec3(0.22f, 0.25f, 0.22f), 32.0f, 1.0f);
        }

        if (isFemale)
        {
            glm::mat4 hairM = glm::translate(neckM, glm::vec3(0.0f, 0.12f, -0.15f));
            hairM = glm::scale(hairM, glm::vec3(0.24f, 0.36f, 0.18f));
            if (sphereObj)
                sphereObj->drawSphereWithColor(s, hairM, glm::vec3(0.16f, 0.10f, 0.05f), 16.0f);
        }
        else
        {
            glm::mat4 hairM = glm::translate(neckM, glm::vec3(0.0f, 0.26f, -0.05f));
            drawCube(cubeVAO, s, hairM, glm::vec3(0.16f, 0.10f, 0.05f), glm::vec3(0), glm::vec3(0.22f, 0.08f, 0.25f), 16.0f, 1.0f);
        }
    }
};

#endif
