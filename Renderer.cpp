#include <Base.hpp>

#include <Renderer.hpp>
#include <FileIO.hpp>

#define _USE_MATH_DEFINES
#include <math.h>
#include <vector>
#include <sstream>

//#include <ft2build.h>
//#include FT_FREETYPE_H

#define GLEW_STATIC
#include <GL/glew.h>

// Font management
//FT_Face g_face;

std::vector<class Line> g_renderLines;
std::vector<class Text> g_renderTexts;

// For drawing shapes
static GLuint g_programID = 0;
static GLuint g_vertPosID = 0;
static GLuint g_vertColID = 0;
static GLuint g_vertBufferID[2] = { 0 };

// For drawing text
static GLuint g_textProgramID = 0;
static GLuint g_textVertPosID = 0;
static GLuint g_textUVPosID = 0;
static GLuint g_textTextureID = 0;
static GLuint g_textTexture = 0;
static GLuint g_textVertBuffers[2] = { 0 };

static int g_width = 0;
static int g_height = 0;

class Line
{
  public:

	Line(const Point3& pa, const Point3& pb, unsigned int color);
	void render() const;

	Point3 m_pa;
	Point3 m_pb;
	unsigned int m_color;
};

class Text
{
public:
	Text(const std::string& str, const Point3& pos, unsigned int color);
	void render() const;

	std::string m_str;
	Point3 m_pos;
	unsigned int m_color;
};

inline unsigned int SetRGBA(unsigned char r, unsigned char g, unsigned char b, unsigned char a)
{
	return (r) | (g << 8) | (b << 16) | (a << 24);
}

GLuint LoadShaders(const char* vertexShaderPath, const char* fragmentShaderPath)
{
	GLuint vertShaderID = glCreateShader(GL_VERTEX_SHADER);
	GLuint fragShaderID = glCreateShader(GL_FRAGMENT_SHADER);

	std::string vertShaderSrcStr;
	FileIO::loadFileStream(vertexShaderPath, vertShaderSrcStr);

	std::string fragShaderSrcStr;
	FileIO::loadFileStream(fragmentShaderPath, fragShaderSrcStr);

	GLint Result = GL_FALSE;
	int InfoLogLength;

	// Compile vertex shader
	printf("Compiling vertex shader: %s\n", vertexShaderPath);
	const char* vertShaderSrcPtr = vertShaderSrcStr.c_str();
	glShaderSource(vertShaderID, 1, &vertShaderSrcPtr, NULL);
	glCompileShader(vertShaderID);

	glGetShaderiv(vertShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(vertShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0)
	{
		std::vector<char> VertexShaderErrorMessage(InfoLogLength + 1);
		glGetShaderInfoLog(vertShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
		printf("%s\n", &VertexShaderErrorMessage[0]);
	}

	// COMPILE FRAGMENT SHADER
	printf("Compiling fragment shader: %s\n", fragmentShaderPath);
	const char* fragShaderSrcPtr = fragShaderSrcStr.c_str();
	glShaderSource(fragShaderID, 1, &fragShaderSrcPtr, NULL);
	glCompileShader(fragShaderID);

	glGetShaderiv(fragShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(fragShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0)
	{
		std::vector<char> FragmentShaderErrorMessage(InfoLogLength + 1);
		glGetShaderInfoLog(fragShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
		printf("%s\n", &FragmentShaderErrorMessage[0]);
	}

	// LINK PROGRAM
	printf("Linking program\n");
	GLuint programID = glCreateProgram();
	glAttachShader(programID, vertShaderID);
	glAttachShader(programID, fragShaderID);
	glLinkProgram(programID);

	glGetProgramiv(programID, GL_LINK_STATUS, &Result);
	glGetProgramiv(programID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0)
	{
		std::vector<char> ProgramErrorMessage(InfoLogLength + 1);
		glGetProgramInfoLog(programID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
		printf("%s\n", &ProgramErrorMessage[0]);
	}

	glDetachShader(programID, vertShaderID);
	glDetachShader(programID, fragShaderID);

	glDeleteShader(vertShaderID);
	glDeleteShader(fragShaderID);

	return programID;
}

/*
int initializeFreeType()
{
	FT_Library library;

	FT_Error error = FT_Init_FreeType(&library);
	Assert(!error, "failed to initialize freetype");

	error = FT_New_Face(library, "C:\\Windows\\Fonts\\consola.ttf", 0, &g_face);
	Assert(!error, "failed to load font face");

	error = FT_Set_Char_Size(g_face, 0, 16 * 64, g_width, g_height);
	Assert(!error, "failed to set font pixel size");

	return 0;
}
*/

int initializeRendering(int width, int height)
{
	GLenum err = glewInit();
	Assert(err == GLEW_OK, "failed to initialize glew");

	// This part isn't very design-obeying. Change it
	g_width = width;
	g_height = height;

	glGenBuffers(2, g_vertBufferID);
	g_programID = LoadShaders("VertexShader.shader", "FragmentShader.shader");
	g_vertPosID = glGetAttribLocation(g_programID, "pos");
	g_vertColID = glGetAttribLocation(g_programID, "col");

	/*
	initializeFreeType();
	glGenBuffers(3, g_textVertBuffers);
	glGenTextures(1, &g_textTexture);
	glBindTexture(GL_TEXTURE_2D, g_textTexture);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	FT_UInt glyph_index = FT_Get_Char_Index(g_face, 'a');

	FT_Error error = FT_Load_Glyph(g_face, glyph_index, FT_LOAD_DEFAULT);
	ASSERT(!error, "failed to load glyph");

	error = FT_Render_Glyph(g_face->glyph, FT_RENDER_MODE_NORMAL);
	ASSERT(!error, "failed to load glyph bitmap");

	FT_GlyphSlot slot = g_face->glyph;
	ASSERT(slot->format == FT_GLYPH_FORMAT_BITMAP, "glyph slot isnt's bitmap");

	FT_Bitmap bitmap = slot->bitmap;

	glTexImage2D(
		GL_TEXTURE_2D,
		0,
		GL_RED,
		bitmap.width,
		bitmap.rows,
		0,
		GL_RED,
		GL_UNSIGNED_BYTE,
		bitmap.buffer
	);
	g_textProgramID = LoadShaders("TextVertexShader.shader", "TextFragmentShader.shader");

	g_textVertPosID = glGetAttribLocation(g_textProgramID, "pos");
	g_textUVPosID = glGetAttribLocation(g_textProgramID, "uv");
	g_textTextureID = glGetUniformLocation(g_textProgramID, "texture");
	*/

	return 0;
}

int closeRendering()
{
	glDeleteBuffers(2, g_vertBufferID);
	glDeleteProgram(g_programID);

	return 0;
}

int clearDisplays()
{
	for (int i = 0; i < (int)g_renderLines.size(); i++)
	{
		g_renderLines[i].render();
	}

	g_renderLines.clear();

	for (int i = 0; i < (int)g_renderTexts.size(); i++)
	{
		//g_textDisplays[i].render();
	}

	g_renderTexts.clear();

	return 0;
}

Line::Line(const Point3& pa, const Point3& pb, unsigned int color)
    : m_pa(pa), m_pb(pb), m_color(color)
{

}

void Line::render() const
{
	GLfloat verts[] =
	{
		m_pa(0) * 2.0f / g_width,
        m_pa(1) * 2.0f / g_height,
		m_pb(0) * 2.0f / g_width,
		m_pb(1) * 2.0f / g_height
	};

	GLfloat colorf[] =
	{
		(GLfloat)(m_color & 0xff) / 255.f,
		(GLfloat)((m_color >> 8) & 0xff) / 255.f,
		(GLfloat)((m_color >> 16) & 0xff) / 255.f,
		(GLfloat)((m_color >> 24) & 0xff) / 255.f,
		(GLfloat)(m_color & 0xff) / 255.f,
		(GLfloat)((m_color >> 8) & 0xff) / 255.f,
		(GLfloat)((m_color >> 16) & 0xff) / 255.f,
		(GLfloat)((m_color >> 24) & 0xff) / 255.f
	};

	glBindBuffer(GL_ARRAY_BUFFER, g_vertBufferID[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, g_vertBufferID[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(colorf), colorf, GL_STATIC_DRAW);

	glUseProgram(g_programID);

	glEnableVertexAttribArray(g_vertPosID);
	glBindBuffer(GL_ARRAY_BUFFER, g_vertBufferID[0]);
	glVertexAttribPointer(g_vertPosID, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

	glEnableVertexAttribArray(g_vertColID);
	glBindBuffer(GL_ARRAY_BUFFER, g_vertBufferID[1]);
	glVertexAttribPointer(g_vertColID, 4, GL_FLOAT, GL_FALSE, 0, (void*)0);

	// THIS SHOULD BE POSSIBLE TO CHANGE TO GL_LINE BECAUSE ONLY DRAWING A SINGLE LINE
	glDrawArrays(GL_LINE_STRIP, 0, 2); // Number of vertices

	glDisableVertexAttribArray(g_vertPosID);
	glDisableVertexAttribArray(g_vertColID);
}

void addDisplay(const Line& line)
{
    g_renderLines.push_back(line);
}

void drawLine(const Point3& pa, const Point3& pb, unsigned int color)
{
    addDisplay(Line(pa, pb, color));
}

void drawCross(const Point3& pos, const Real rot, const Real len, unsigned int color)
{
	Vector3 needle(len / 2.f, 0.f);
	
	Point3 a, b, c, d;

	a = pos + needle.getRotatedDir(rot);
	b = pos + needle.getRotatedDir(rot + 90.f * g_degToRad);
	c = pos + needle.getRotatedDir(rot + 180.f * g_degToRad);
	d = pos + needle.getRotatedDir(rot + 270.f * g_degToRad);

	drawLine(a, c, color);
	drawLine(b, d, color);
}

void drawArrow(const Point3& pos, const Vector3& dir, unsigned int color)
{
	Point3 dst; dst.setAdd(pos, dir);
	drawLine(pos, dst, color);

	Vector3 dirFrac = dir * .5f;
	Vector3 headLeft = dirFrac.getRotatedDir(135.f * g_degToRad);
	Vector3 headRight = dirFrac.getRotatedDir(-135.f * g_degToRad);
	Vector3 dstToHeadLeft = dst + headLeft;
	Vector3 dstToHeadRight = dst + headRight;

	drawLine(dst, dstToHeadLeft, color);
	drawLine(dst, dstToHeadRight, color);
}

void drawBox(const Vector3& max, const Vector3& min, unsigned int color)
{
	Point3 upperLeft(min(0), max(1));
	Point3 bottomRight(max(0), min(1));

	drawLine(max, upperLeft, color);
	drawLine(max, bottomRight, color);
	drawLine(min, upperLeft, color);
	drawLine(min, bottomRight, color);
}

void drawCircle(const Point3& pos, const Real radius, unsigned int color)
{
	Real step = 2 * (Real)M_PI * STEP_RENDER_CIRCLE;
	
	for (Real i = step; i < 2 * M_PI; i += step)
    {
		Vector3 na, nb;
		na.set(radius * cos(i), radius * sin(i));
		nb.set(radius * cos(i + step), radius * sin(i + step));
		drawLine(pos + na, pos + nb);
    }
}

#define RENDER_AXIS_MARKERS

void renderAxis()
{ // Draw full-length cross in origin
	drawLine(Point3(-g_width / 2.f, 0.f), Point3(g_width / 2.f, 0.f));
	drawLine(Point3(0.f, g_height / 2.f), Point3(0.f, -g_height / 2.f));

#if defined(RENDER_AXIS_MARKERS)
	 int widthLimit = (g_width / 20) * 10;
     int heightLimit = (g_height / 20) * 10;

     const int interval = 50;

	 for (int i = 0; i > -widthLimit; i -= interval)
     {
		 std::stringstream ss;
		 ss << i;
		 drawText(ss.str(), Point3((Real)i, 5.0f));
		 drawLine(Point3((Real)i, 2.5f), Point3((Real)i, -2.5f));
	 }
	 
	 for (int i = interval; i < widthLimit; i += interval)
	 {
		 std::stringstream ss;
		 ss << i;
		 drawText(ss.str(), Point3((Real)i, 5.0f));
		 drawLine(Point3((Real)i, 2.5f), Point3((Real)i, -2.5f));
	 }
	 
	 for (int i = 0; i > -heightLimit; i -= interval)
	 {
		 std::stringstream ss;
		 ss << i;
		 drawText(ss.str(), Point3(5.0f, (Real)i));
		 drawLine(Point3(2.5f, (Real)i), Point3(-2.5f, (Real)i));
	 }
	 
	 for (int i = interval; i < heightLimit; i += interval)
	 {
		 std::stringstream ss;
		 ss << i;
		 drawText(ss.str(), Point3(5.0f, (Real)i));
		 drawLine(Point3(2.5f, (Real)i), Point3(-2.5f, (Real)i));
	 }
#endif
}

Text::Text(const std::string& str, const Point3& pos, unsigned int color)
	: m_str(str), m_pos(pos), m_color(color)
{

}


void Text::render() const
{
	const char* cstr = m_str.c_str();
/*
	for (int i = 0; i < (int)m_str.size(); i++)
	{
		FT_UInt glyph_index = FT_Get_Char_Index(g_face, cstr[i]);
		
		FT_Error error = FT_Load_Glyph(g_face, glyph_index, FT_LOAD_DEFAULT);
		Assert(!error, "failed to load glyph");

		error = FT_Render_Glyph(g_face->glyph, FT_RENDER_MODE_NORMAL);
		Assert(!error, "failed to load glyph bitmap");

		FT_GlyphSlot slot = g_face->glyph;
		Assert(slot->format == FT_GLYPH_FORMAT_BITMAP, "glyph slot isnt's bitmap");

		FT_Bitmap bitmap = slot->bitmap;

		glBindTexture(GL_TEXTURE_2D, g_textTexture);

		glTexImage2D(
			GL_TEXTURE_2D,
			0,
			GL_RED,
			bitmap.width,
			bitmap.rows,
			0,
			GL_RED,
			GL_UNSIGNED_BYTE,
			bitmap.buffer
		);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

		GLfloat vertsScreen[4][2] =
		{
			{(m_pos(0) + 0.f) * 2.0f / g_width, (m_pos(1) + 0.f) * 2.0f / g_height},
			{(m_pos(0) + 16.f) * 2.0f / g_width, (m_pos(1) + 0.f) * 2.0f / g_height},
			{(m_pos(0) + 16.f) * 2.0f / g_width, (m_pos(1) + 16.f) * 2.0f / g_height},
			{(m_pos(0) + 0.f) * 2.0f / g_width, (m_pos(1) + 16.f) * 2.0f / g_height}
		};

		GLfloat vertsUV[4][2] = 
		{
			{0.f, 1.f},
			{1.f, 1.f},
			{1.f, 0.f},
			{0.f, 0.f}
		};

		glEnableVertexAttribArray(g_textVertPosID);
		glBindBuffer(GL_ARRAY_BUFFER, g_textVertBuffers[0]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertsScreen), vertsScreen, GL_STATIC_DRAW);
		glVertexAttribPointer(g_textVertPosID, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

		glEnableVertexAttribArray(g_textUVPosID);
		glBindBuffer(GL_ARRAY_BUFFER, g_textVertBuffers[1]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertsUV), vertsUV, GL_STATIC_DRAW);
		glVertexAttribPointer(g_textUVPosID, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

		glDrawArrays(GL_TRIANGLES, 0, 6);

		glDisableVertexAttribArray(g_textVertPosID);
		glDisableVertexAttribArray(g_textTextureID);
	}
*/
}

void drawText(const std::string& str, const Point3& pos, unsigned int color)
{
    g_renderTexts.push_back(Text(str, pos, color));
}
