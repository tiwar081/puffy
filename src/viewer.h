#pragma once
#include "svg_shape.h"

class Viewer {
public:
    Viewer();
    bool loadSVG(const std::string& path, int puffinessLevel = 1);
    void draw(bool isPuffy = false) const;
    void updateCamera(float dx, float dy, float scrollDelta);
    void enter3DMode();

private:
    SvgShape shape;
    SvgShape inflatedShape;   // Puffy version

    bool is3DMode = false;
    float camTheta = 0.0f;
    float camPhi = 0.0f;
    float camRadius = 500.0f;

};