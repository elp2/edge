#pragma once

#include <array>
#include <cstdint>
#include <iostream>
#include <string>

#include "command.h"

using namespace std;

class CPU;
class MMU;

class AbstractCommandFactory {
 public:
    AbstractCommandFactory();
    virtual ~AbstractCommandFactory() = default;
    Command *CommandForOpcode(uint8_t opcode);
    void RegisterCommand(Command *command);

 protected: 
    array<Command *, 256> commands;
    string name;
};

class CommandFactory : public AbstractCommandFactory {
    public:
        CommandFactory();
        ~CommandFactory();
};

class CBCommandFactory : public AbstractCommandFactory {
    public:
        CBCommandFactory();
        ~CBCommandFactory();
};