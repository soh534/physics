#include "imgui.h"
#include "imgui_impl_glfw_gl3.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <Common/Base.h>

void drawString( const ImVec2& pos, const ImColor& color, const char* string, ... )
{
	va_list arg;
	va_start( arg, string );
	ImGui::Begin( "Overlay", NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoScrollbar );
	ImGui::SetCursorPos( pos ); // Window-coordinate system (pixel-based, top-left origin)
	ImGui::TextColoredV( color, string, arg );
	ImGui::End();
	va_end( arg );
}

int main( int argc, char* argv[] )
{
	GLFWwindow* window;

	Assert( glfwInit() == GLFW_TRUE, "failed to initialize GLFW" );

	int width = 1024;
	int height = 768;

	window = glfwCreateWindow( width, height, "Test", nullptr, nullptr );
	glfwMakeContextCurrent( window );

	Assert( glewInit() == GLEW_OK, "failed to initialize GLEW" );

	ImGui::CreateContext();
	ImGui_ImplGlfwGL3_Init( window, true );

	while ( !glfwWindowShouldClose( window ) )
	{
		glfwPollEvents();
		ImGui_ImplGlfwGL3_NewFrame();

		// Not frame-persistent, must be called every frame
		ImGui::SetNextWindowPos( ImVec2( 0, 0 ) );
		ImGui::SetNextWindowSize( ImVec2( (float)width, (float)height ) );
		ImGui::SetNextWindowBgAlpha( 0.f );

		glClearColor( 1.0f, 1.0f, 1.0f, 1.0f ); // White, full opacity
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

		// Draw coordinates @ coordinate
		drawString( ImVec2( (float)width / 2, (float)height / 2 ), ImColor( 0, 0, 0 ), "%d, %d", width / 2, height / 2 );
		drawString( ImVec2( (float) width / 3, (float) height / 3 ), ImColor( 255, 0, 0 ), "%d, %d", width / 3, height / 3 );
		drawString( ImVec2( (float) width / 4, (float) height / 4 ), ImColor( 255, 255, 0 ), "%d, %d", width / 4, height / 4 );
		drawString( ImVec2( (float) width / 5, (float) height / 5 ), ImColor( 0, 255, 255 ), "%d, %d", width / 5, height / 5 );

		ImGui::Render();
		ImGui_ImplGlfwGL3_RenderDrawData( ImGui::GetDrawData() );
		glfwSwapBuffers( window );
	}

	ImGui_ImplGlfwGL3_Shutdown();
	ImGui::DestroyContext();

	glfwDestroyWindow( window );
	glfwTerminate();

	return 0;
}
