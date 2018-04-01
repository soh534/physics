#include <Common/FileIO.h>

#include <sstream>
#include <fstream>
#include <iostream>

namespace FileIO
{
	int loadFileStream( const char* srcPath, std::string& dstStr )
	{
		std::ifstream fileSrcStream( srcPath, std::ios::in );

		if ( fileSrcStream.is_open() )
		{
			std::string Line = "";

			while ( getline( fileSrcStream, Line ) )
			{
				dstStr += "\n" + Line;
			}

			fileSrcStream.close();

			return 0;
		}
		else
		{
			std::cout << "Warning: failed to read from " << srcPath << std::endl;

			return -1;
		}
	}
}