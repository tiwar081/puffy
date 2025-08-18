#include "viewer.h"
#include <GL/glew.h>
#include <iostream>

Viewer::Viewer() 
    : camTheta(1.57f), camPhi(1.57f), camRadius(200.0f), is3DMode(false) {}


bool Viewer::loadSVG(const std::string& path, int puffinessLevel) {
    bool success = shape.loadFromFile(path);

    if (!success) return false;

    inflatedShape = shape;        // Copy original
    inflatedShape.inflate(puffinessLevel);  // Pass the puffiness level

    return true;
}

void Viewer::enter3DMode() {
    is3DMode = true;
}

void Viewer::updateCamera(float dx, float dy, float scrollDelta) {
    camTheta -= dx * 0.01f;
    camPhi   += dy * 0.01f;
    camRadius -= scrollDelta * 5.0f;
    camRadius = std::max(camRadius, 10.0f);
}

void Viewer::draw(bool puffy) const {
    if (is3DMode) {
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_LIGHTING);
        glEnable(GL_LIGHT0);

        GLfloat light_pos[] = { 0.0f, 0.0f, 10.0f, 1.0f };
        glLightfv(GL_LIGHT0, GL_POSITION, light_pos);

        GLfloat light_diffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
        glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);

        GLfloat light_ambient[] = { 0.3f, 0.3f, 0.3f, 1.0f };
        glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        gluPerspective(45.0, 800.0 / 600.0, 1.0, 1000.0);

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        // Camera orbits around origin
        float camX = camRadius * sin(camPhi) * cos(camTheta);
        float camY = camRadius * cos(camPhi);
        float camZ = camRadius * sin(camPhi) * sin(camTheta);

        gluLookAt(
            camX, camY, camZ,     // Camera position
            0.0f, 0.0f, 0.0f,    // Look at origin
            0.0f, 1.0f, 0.0f     // Up vector
        );

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Get shape center and translate so shape center is at origin
        glm::vec2 center2D = shape.getCenter();
        glTranslatef(-center2D.x, -center2D.y, 0.0f);
        glScalef(0.28f, 0.28f, 0.28f); //scale!!!


            if (puffy) {
                const std::vector<glm::vec3>& verts = inflatedShape.getInflatedVertices();
                GLfloat diffuseColor[] = { 0.6f, 0.8f, 0.6f, 1.0f }; //sage green !!!
                GLfloat ambientColor[] = { 0.8f, 0.8f, 0.8f, 0.8f };
                glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuseColor);
                glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambientColor);

                glBegin(GL_TRIANGLE_FAN);
                for (const auto& v : verts) {
                    glNormal3f(0, 0, 1);
                    glVertex3f(v.x , v.y , v.z);
                }
                glEnd();
            } 
        }
    else {
        glDisable(GL_DEPTH_TEST);
            glClear(GL_COLOR_BUFFER_BIT);

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(0.0, 800.0, 0.0, 600.0, -1.0, 1.0);
        glMatrixMode(GL_MODELVIEW);

        
        glm::vec2 center2D = shape.getCenter();
        float screenCenterX = 400.0f;
        float screenCenterY = 300.0f;

        const std::vector<glm::vec2>& verts = shape.getVertices();
        glBegin(GL_LINE_LOOP);
        for (const auto& v : verts) {
            // Translate shape so its center is at screen center
            glVertex2f((v.x - center2D.x) + screenCenterX,
                    (v.y - center2D.y) + screenCenterY);
        }
        glEnd();
    }
}
