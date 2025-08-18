#include "svg_shape.h"
#include <tinyxml2.h>
#include <iostream>
#include <cmath>
#include <GL/glew.h>

using namespace tinyxml2;

bool SvgShape::loadFromFile(const std::string& filename) {
    XMLDocument doc;

    if (doc.LoadFile(filename.c_str()) != XML_SUCCESS) {
        std::cerr << "Failed to load SVG: " << filename << std::endl;
        return false;
    }

    XMLElement* svg = doc.FirstChildElement("svg");
    if (!svg) {
        std::cerr << "No <svg> element found." << std::endl;
        return false;
    }

    vertices.clear();

    for (XMLElement* elem = svg->FirstChildElement(); elem; elem = elem->NextSiblingElement()) {
        std::string tag = elem->Name();
        std::cout << "Found tag: " << tag << std::endl;

        if (tag == "circle") {
            float cx = elem->FloatAttribute("cx");
            float cy = elem->FloatAttribute("cy");
            float r = elem->FloatAttribute("r");

            const int segments = 64;
            for (int i = 0; i < segments; ++i) {
                float theta = 2.0f * M_PI * i / segments;
                float x = cx + r * std::cos(theta);
                float y = cy + r * std::sin(theta);
                vertices.emplace_back(x, y);
            }

        } else if (tag == "rect") {
            float x = elem->FloatAttribute("x");
            float y = elem->FloatAttribute("y");
            float width = elem->FloatAttribute("width");
            float height = elem->FloatAttribute("height");

            vertices.emplace_back(x, y);
            vertices.emplace_back(x + width, y);
            vertices.emplace_back(x + width, y + height);
            vertices.emplace_back(x, y + height);
        }
    }

    std::cout << "Loaded " << vertices.size() << " vertices\n";

    // Compute center
    glm::vec2 center(0.0f);
    for (const auto& v : vertices) {
        center += v;
    }
    center /= static_cast<float>(vertices.size());

    // Shift vertices so shape is centered at origin
    for (auto& v : vertices) {
        v -= center;
    }

    return true;  // Move return statement to the end
}

glm::vec2 SvgShape::getCenter() const {
    glm::vec2 centroid(0.0f);
    for (const auto& v : vertices) {
        centroid += v;
    }
    centroid /= static_cast<float>(vertices.size());
    return centroid;
}

void SvgShape::inflate(int puffinessLevel) {
    inflatedVertices.clear();
    glm::vec2 center = getCenter();

    float maxRadius = 0.0f;
    for (const auto& v : vertices) {
        float dist = glm::length(v - center);
        if (dist > maxRadius) maxRadius = dist;
    }

    for (const auto& v : vertices) {
        float dist = glm::length(v - center);

        float height = puffinessLevel * std::exp(-(dist * dist) / (2.0f * maxRadius * maxRadius / 3.0f));

        inflatedVertices.push_back(glm::vec3(v.x, v.y, height));
    }
}

