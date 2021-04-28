#include "Generator.h"

void Generator::PushItemToRpn(Item* item) {
    buffer_.push_back(item);
}

void Generator::PushItemToProgram(Item* item) {
    if(!should_skip)
        program.push_back(item);
    else {
        should_skip = false;
        delete item;
    }
}

size_t Generator::GetCurrentCursor()
{
    return program.size() - 1;
}

void Generator::ClrBuffer(  ) {
    for (auto& elem : buffer_) {
        delete elem;
    }
    buffer_.clear();
}

void Generator::CompleteRPN() {
    //*TODO*//
    //pushing rpn to programm
}

void Generator::CompleteRPN(int begin, int end)
{
//TODO
    return;
}

void Generator::PushSeparator() {
    //buffer_.push_back(new SeparatorItem());
}

//void Generator::PushFor() {
//   
//}

//void Generator::PushIf() {
//    CompleteRPN(0, buffer_.size());
//    PushItemToProgram(new JumpElseItem());
//}
