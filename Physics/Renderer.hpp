#pragma once

#include <Base.hpp>
#include <string>

// Accuracy for drawing circle
#define STEP_RENDER_CIRCLE 0.05f

#define BLACK (0 | (0 << 8) | (0 << 16) | (255 << 24))
#define RED (255 | (0 << 8) | (0 << 16) | (255 << 24))
#define GREEN (0 | (255 << 8) | (0 << 16) | (255 << 24))
#define BLUE (0 | (0 << 8) | (255 << 16) | (255 << 24))
#define WHITE (255 | (255 << 8) | (255 << 16) | (255 << 24))
#define YELLOW (255 | (255 << 8) | (0 << 16) | (255 << 24))
#define ORANGE (255 | (127 << 8) | (0 << 16) | (255 << 24))
#define PURPLE (255 | (0 << 8) | (255 << 16) | (255 << 24))

int initializeRendering(int width, int height);
int closeRendering();
int step();

void drawLine(const Vector3& p1, const Vector3& p2, unsigned int = BLACK);
void drawCross(const Vector3& pos, const Real rot, const Real len, unsigned int = BLACK);
void drawArrow(const Vector3& pos, const Vector3& dir, unsigned int = BLACK);
void drawBox(const Vector3& max, const Vector3& min, unsigned int = BLACK);
void drawCircle(const Vector3& pos, const Real radius, unsigned int = BLACK);
void renderAxis();

void drawText(const std::string& str, const Vector3& pos, unsigned int = BLACK);
