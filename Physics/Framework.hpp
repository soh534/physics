#ifndef FRAMEWORK_HPP
#define FRAMEWORK_HPP

class physicsWorld;

struct WindowCinfo
{
	unsigned int widthWindow;
	unsigned int heightWindow;
	unsigned int framesPerSecond;
	physicsWorld* world;
};

void BeginGraphics(const WindowCinfo& cinfo);

#endif