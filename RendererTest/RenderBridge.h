#pragma once


// Accuracy for drawing circle
#define STEP_RENDER_CIRCLE 0.05f

#define COLOR_IN_RGB(R, G, B) (R | (G << 8) | (B << 16) | (255 << 24))

// Color tables obtained from
// www.rapidtables.com/web/color/RGB_Color.htm
#define BLACK COLOR_IN_RGB(0, 0, 0)
#define WHITE COLOR_IN_RGB(255, 255, 255)
#define RED COLOR_IN_RGB(255, 0, 0)
#define LIME COLOR_IN_RGB(0, 255, 0)
#define BLUE COLOR_IN_RGB(0, 0, 255)
#define YELLOW COLOR_IN_RGB(255, 255, 0)
#define CYAN COLOR_IN_RGB(0, 255, 255)
#define MAGENTA COLOR_IN_RGB(255, 0, 255)
#define SILVER COLOR_IN_RGB(192, 192, 192)
#define GRAY COLOR_IN_RGB(128, 128, 128)
#define MAROON COLOR_IN_RGB(128, 0, 0)
#define OLIVE COLOR_IN_RGB(128, 128, 0)
#define GREEN COLOR_IN_RGB(0, 128, 0)
#define PURPLE COLOR_IN_RGB(128, 0, 128)
#define TEAL COLOR_IN_RGB(0, 128, 128)
#define NAVY COLOR_IN_RGB(0, 0, 128)

/*
void drawText( const std::string& str, const Vector4& pos, const Real scale = 1.f, unsigned int color = BLACK );

// TODO: move these under RenderUtils
void drawCross( const Vector4& pos, const Real rot, const Real len, unsigned int = BLACK );
void drawArrow( const Vector4& pos, const Vector4& dir, unsigned int = BLACK );
void drawBox( const Vector4& max, const Vector4& min, unsigned int = BLACK );
void drawCircle( const Vector4& pos, const Real radius, unsigned int = BLACK );
*/