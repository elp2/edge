#include "CommandFactory.hpp"

#include "BitCommand.hpp"
#include "CallCommand.hpp"
#include "CBCommand.hpp"
#include "JumpCommand.hpp"
#include "LoadCommand.hpp"
#include "MathCommand.hpp"
#include "MiscCommand.hpp"
#include "NopCommand.hpp"
#include "ReturnCommand.hpp"
#include "StackCommand.hpp"

AbstractCommandFactory::AbstractCommandFactory() {
    commands = array<Command *, 256>();
}

void AbstractCommandFactory::RegisterCommand(Command *command) {
    Command *existingCommand = commands[command->opcode];
    if (existingCommand != NULL) {
        cout << name << ": Already have an opcode at 0x" << hex << unsigned(existingCommand->opcode) << ": " << existingCommand->description << " : New = " << command->description << endl;
        assert(false);
    }
    commands[command->opcode] = command;
}

Command *AbstractCommandFactory::CommandForOpcode(uint8_t opcode) {
    if (commands[opcode] == NULL) {
        cout << name << ": No opcode for " << unsigned(opcode) << endl;
        assert(false);
    }
    return commands[opcode];
}

CommandFactory::CommandFactory() {
    name = "Base";
    cout << name << " created!" << endl;
    registerNopCommands(this);
    registerJumpCommands(this);
    registerLoadCommands(this);
    registerMiscCommands(this);
    registerCallCommands(this);
    registerReturnCommands(this);
    registerBitCommands(this);
    registerMathCommands(this);
    registerStackCommands(this);

    int implementedCommands = 0;
    for (int i = 0; i < 256; i++) {
        if (commands[i] == NULL) {
            cout << "0x" << hex << unsigned(i) << " not implemented" << endl;
        } else {
            implementedCommands++;
        }
    }
    cout << implementedCommands << " / " << "256 commands implemented!" << endl;
}

CommandFactory::~CommandFactory() {

}

CBCommandFactory::CBCommandFactory() {
    name = "CB";
    cout << name << " created!" << endl;

    for (int i = 0; i < 256; i++) {
        RegisterCommand(new CBCommand(i));
    }
}

CBCommandFactory::~CBCommandFactory() {

}
