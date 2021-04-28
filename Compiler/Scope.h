#pragma once
#include <map>
#include <string>


class Scope
{
    bool bar;
    Scope* parrent;
    Scope* child;
    std::map<std::wstring, int> offsets;
};

