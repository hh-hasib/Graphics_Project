//
//  cylinder.h
//  Procedural cylinder generation
//

#ifndef cylinder_h
#define cylinder_h

#include <glad/glad.h>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "shader.h"

using namespace std;

class Cylinder {
public:
    unsigned int VAO, VBO, EBO;
    int indexCount;

    Cylinder(int sectorCount = 36, float radius = 0.5f, float height = 1.0f) {
        buildVerticesSmooth(sectorCount, radius, height);
    }

    ~Cylinder() {
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
        glDeleteBuffers(1, &EBO);
    }

    void draw(Shader& shader, glm::mat4 model, glm::vec3 color, unsigned int tex = 0, float tiling = 1.0f) {
        shader.use();
        shader.setVec3("material.ambient", color * 0.8f);
        shader.setVec3("material.diffuse", color);
        shader.setVec3("material.specular", glm::vec3(0.5f));
        shader.setFloat("material.shininess", 32.0f);
        shader.setFloat("alpha", 1.0f);
        if (tex != 0) {
            shader.setBool("useTexture", true);
            shader.setFloat("texTiling", tiling);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, tex);
            shader.setInt("texture1", 0);
        } else {
            shader.setBool("useTexture", false);
        }
        shader.setMat4("model", model);

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
        shader.setBool("useTexture", false);
    }

private:
    void buildVerticesSmooth(int sectorCount, float radius, float height) {
        std::vector<float> vertices;
        std::vector<unsigned int> indices;

        float sectorStep = 2 * 3.1415926f / sectorCount;
        float sectorAngle;  

        // SIDE WALL vertices
        for(int i = 0; i <= sectorCount; ++i) {
            sectorAngle = i * sectorStep;
            float x = radius * cosf(sectorAngle);
            float z = radius * sinf(sectorAngle);
            float nx = cosf(sectorAngle);
            float nz = sinf(sectorAngle);
            float u = (float)i / sectorCount;

            // Bottom vertex (y = -height/2)
            vertices.push_back(x); vertices.push_back(-height / 2); vertices.push_back(z);
            vertices.push_back(nx); vertices.push_back(0.0f); vertices.push_back(nz);
            vertices.push_back(u); vertices.push_back(0.0f);

            // Top vertex (y = height/2)
            vertices.push_back(x); vertices.push_back(height / 2); vertices.push_back(z);
            vertices.push_back(nx); vertices.push_back(0.0f); vertices.push_back(nz);
            vertices.push_back(u); vertices.push_back(1.0f);
        }

        // TOP/BOTTOM Caps
        int topCenterIdx = (vertices.size() / 8);
        vertices.push_back(0); vertices.push_back(height / 2); vertices.push_back(0);
        vertices.push_back(0); vertices.push_back(1.0f); vertices.push_back(0);
        vertices.push_back(0.5f); vertices.push_back(0.5f);

        int bottomCenterIdx = topCenterIdx + 1;
        vertices.push_back(0); vertices.push_back(-height / 2); vertices.push_back(0);
        vertices.push_back(0); vertices.push_back(-1.0f); vertices.push_back(0);
        vertices.push_back(0.5f); vertices.push_back(0.5f);

        int capStartIdx = bottomCenterIdx + 1;
        for(int i = 0; i <= sectorCount; ++i) {
            sectorAngle = i * sectorStep;
            float x = radius * cosf(sectorAngle);
            float z = radius * sinf(sectorAngle);
            float u = 0.5f + 0.5f * cosf(sectorAngle);
            float v = 0.5f + 0.5f * sinf(sectorAngle);

            // top cap edge
            vertices.push_back(x); vertices.push_back(height / 2); vertices.push_back(z);
            vertices.push_back(0); vertices.push_back(1.0f); vertices.push_back(0);
            vertices.push_back(u); vertices.push_back(v);

            // bottom cap edge
            vertices.push_back(x); vertices.push_back(-height / 2); vertices.push_back(z);
            vertices.push_back(0); vertices.push_back(-1.0f); vertices.push_back(0);
            vertices.push_back(u); vertices.push_back(v);
        }

        // INDICES
        int k1, k2;
        // Side wall
        for(int i = 0; i < sectorCount; ++i) {
            k1 = i * 2;
            k2 = k1 + 1;
            int k1_next = (i + 1) * 2;
            int k2_next = k1_next + 1;

            indices.push_back(k1); indices.push_back(k2); indices.push_back(k1_next);
            indices.push_back(k1_next); indices.push_back(k2); indices.push_back(k2_next);
        }

        // Top and Bottom Caps
        for(int i = 0; i < sectorCount; ++i) {
            int topEdge1 = capStartIdx + i * 2;
            int topEdge2 = capStartIdx + (i + 1) * 2;
            indices.push_back(topCenterIdx); indices.push_back(topEdge1); indices.push_back(topEdge2);

            int botEdge1 = capStartIdx + i * 2 + 1;
            int botEdge2 = capStartIdx + (i + 1) * 2 + 1;
            indices.push_back(bottomCenterIdx); indices.push_back(botEdge2); indices.push_back(botEdge1);
        }

        indexCount = (int)indices.size();

        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
        glEnableVertexAttribArray(2);

        glBindVertexArray(0);
    }
};

#endif
