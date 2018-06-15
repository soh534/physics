#pragma once

#include <map>

class TemplTest
{
    // This is not enough for std::map<int, TestStruct> m_map;
	struct TestStruct;

    // Need to be defined for std::map<int, TestStruct> m_map;
    struct TestStruct
    {
        std::string s1, s2;
    };

public:

    // Takeaway: template non-ptr members require template objects be defined beforehand
	std::map<int, TestStruct> m_map;
};

void templTest()
{
    TemplTest tt;
}