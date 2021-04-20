#pragma once
#include <stack>
#include <iostream>
#include <map>
#include <vector>

class Item;

class Generator
{
public:
    void PushItemToRpn(Item* item);
    void ClrBuffer();
    void CompleteRPN();
    void PushSeparator();
    void PushFor();
    void PushWhile();
    void PushDoWhile();
    void PushFunc();


private:
    friend Item;
    std::vector<Item*> buffer_;
    std::vector<Item*> programm;


};

