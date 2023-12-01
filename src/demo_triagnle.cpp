#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <GL/glut.h>
#include <iostream>

const int textureWidth = 64;
const int textureHeight = 64;
GLuint texture;

float spriteX = 0.0f;
float spriteY = 0.0f;
float velocity = 0.0f;
float angle = 0.0f;
float angularVelocity = 0.0f;
float decayFactor = 0.97f;
float maxVelocity = 0.2f;

void loadTexture() {
    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true); // Flip texture vertically (optional)

    unsigned char *data = stbi_load("texture.png", &width, &height, &nrChannels, STBI_rgb_alpha);

    if (data) {
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        stbi_image_free(data);
    } else {
        std::cout << "Failed to load texture" << std::endl;
    }
}

void drawSprite() {
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texture);

    glPushMatrix();
    glTranslatef(spriteX, spriteY, 0.0f);
    glRotatef(angle, 0.0f, 0.0f, 1.0f);

    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 1.0f);
    glVertex2f(-0.1f, -0.1f); // Bottom-left vertex

    glTexCoord2f(1.0f, 1.0f);
    glVertex2f(0.1f, -0.1f); // Bottom-right vertex

    glTexCoord2f(1.0f, 0.0f);
    glVertex2f(0.1f, 0.1f); // Top-right vertex

    glTexCoord2f(0.0f, 0.0f);
    glVertex2f(-0.1f, 0.1f); // Top-left vertex
    glEnd();

    glPopMatrix();
    glDisable(GL_TEXTURE_2D);
}

void drawGrid() {
    glColor3f(0.8f, 0.8f, 0.8f); // Light gray lines

    glLineWidth(1.0f);
    glBegin(GL_LINES);

    for (float i = -50.0f; i <= 50.0f; i += 1.0f) {
        glVertex2f(-50.0f, i);
        glVertex2f(50.0f, i);
        glVertex2f(i, -50.0f);
        glVertex2f(i, 50.0f);
    }

    glEnd();
}

void update(int) {
    spriteX += velocity * cos(angle * 3.14159 / 180);
    spriteY += velocity * sin(angle * 3.14159 / 180);
    angle += angularVelocity;

    velocity *= decayFactor; // Apply velocity decay

    // Cap the velocity to the maximum limit
    if (velocity > maxVelocity) {
        velocity = maxVelocity;
    } else if (velocity < -maxVelocity) {
        velocity = -maxVelocity;
    }

    // If velocity is very small, consider stopping movement
    if (fabs(velocity) < 0.001f) {
        velocity = 0.0f;
    }

    glutPostRedisplay();
    glutTimerFunc(16, update, 0);
}


void handleKeypress(unsigned char key, int, int) {
    switch (key) {
        case 'w':
            velocity += 0.01f;
            break;
        case 's':
            velocity -= 0.01f;
            break;
        case 'a':
            angularVelocity = 2.0f;
            break;
        case 'd':
            angularVelocity = -2.0f;
            break;
    }
}

void handleKeyUp(unsigned char key, int, int) {
    if (key == 'a' || key == 'd') {
        angularVelocity = 0.0f;
    }

    if (key == 'w' || key == 's') {
        // Decay velocity gradually upon key release
        decayFactor = 0.97f;
    }
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(spriteX, spriteY, 1.0f, spriteX, spriteY, 0.0f, 0.0f, 1.0f, 0.0f);

    drawGrid(); // Draw the grid

    drawSprite(); // Draw the sprite

    glFlush();
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(1000, 1000);
    glutCreateWindow("Square with Texture and Grid");

    loadTexture(); // Load the texture

    glClearColor(0.5f, 0.5f, 0.5f, 1.0f);

    glutDisplayFunc(display);
    glutKeyboardFunc(handleKeypress);
    glutKeyboardUpFunc(handleKeyUp);
    glutTimerFunc(16, update, 0);

    glutMainLoop();
    return 0;
}