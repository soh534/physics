#pragma once

#include <Common/Base.h>
#include <string>
#include <vector>

#include <GL/glew.h>
#include <glm/glm.hpp>

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

class Shader;

class Renderer
{
public:

	struct Renderable
	{
		Renderable( unsigned int color = BLACK );
		virtual void render() = 0;

		unsigned int m_color;
	};

	struct Line : Renderable
	{
		Line( const Vector4& a, const Vector4& b, unsigned int color = BLACK );

		Vector4 m_a;
		Vector4 m_b;
	};

	struct Triangle : Renderable
	{
		Triangle( const Vector4& a, const Vector4& b, const Vector4& c, unsigned int color = BLACK );

		Vector4 m_a;
		Vector4 m_b;
		Vector4 m_c;
	};

	struct Text : Renderable
	{
		Text( const std::string& str, const Vector4& pos, const Real scale = 1.f, unsigned int color = BLACK );

		std::string m_str;
		Vector4 m_pos;
		Real m_scale;
	};

	Renderer();
	~Renderer();

	int init( int width, int height );
	int step();

	void getDimensions( float& left, float& right, float& bottom, float& top );
	void drawLine( const Vector4& p1, const Vector4& p2, unsigned int color = BLACK );
	void drawTriangle( const Vector4& a, const Vector4& b, const Vector4& c, unsigned int color = BLACK );
	void drawText( const std::string& str, const Vector4& pos, const Real scale = 1.f, unsigned int color = BLACK );

	// TODO: move these under RenderUtils
	void drawCross( const Vector4& pos, const Real rot, const Real len, unsigned int = BLACK );
	void drawArrow( const Vector4& pos, const Vector4& dir, unsigned int = BLACK );
	void drawBox( const Vector4& max, const Vector4& min, unsigned int = BLACK );
	void drawCircle( const Vector4& pos, const Real radius, unsigned int = BLACK );

private:

	int initializeFreeType();

	// Make these virtual
	void renderLine( const Line& line ) const;
	void renderTriangle( const Triangle& tri ) const;
	void renderText( const Text& text ) const;

	int m_left;
	int m_right;
	int m_bottom;
	int m_top;
	glm::mat4 m_projection;

	// For drawing lines
	GLuint m_lineVAO;
	GLuint m_lineVBO[2];

	// For drawing tris
	GLuint m_triVAO;
	GLuint m_triVBO[2];

	// For drawing text
	GLuint m_textVAO;
	GLuint m_textVBO;

	Shader* m_lineShader;
	Shader* m_triShader;
	Shader* m_textShader;

	Shader* m_geometryShader;

	std::vector<Line> m_displayLines;
	std::vector<Triangle> m_displayTris;
	std::vector<Text> m_displayTexts;
};

