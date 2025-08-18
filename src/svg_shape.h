#pragma once
#include <vector>
#include <string>
#include <glm/glm.hpp> // Use GLM for vector math

class SvgShape {
public:
    bool loadFromFile(const std::string& filename);

    const std::vector<glm::vec2>& getVertices() const { return vertices; }
    const std::vector<glm::vec3>& getInflatedVertices() const { return inflatedVertices; }

    void draw() const;  // OpenGL draw function
    void inflate(int puffinessLevel = 0);
    glm::vec2 getCenter() const;




private:
    std::vector<glm::vec2> vertices;
    std::vector<glm::vec3> inflatedVertices;

};
