#include "command_factory.h"

#include <cassert>

#include "bit_command.h"
#include "call_command.h"
#include "cb_command.h"
#include "jump_command.h"
#include "load_command.h"
#include "math_command.h"
#include "misc_command.h"
#include "nop_command.h"
#include "return_command.h"
#include "stack_command.h"
#include "unimplemented_command.h"

AbstractCommandFactory::AbstractCommandFactory() {
  commands = array<Command *, 256>();
}

void AbstractCommandFactory::RegisterCommand(Command *command) {
  Command *existingCommand = commands[command->opcode];
  if (existingCommand != NULL) {
    cout << name << ": Already have an opcode at 0x" << hex
         << unsigned(existingCommand->opcode) << ": "
         << existingCommand->description << " : New = " << command->description
         << endl;
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
  registerNopCommands(this);
  registerJumpCommands(this);
  registerLoadCommands(this);
  registerMiscCommands(this);
  registerCallCommands(this);
  registerReturnCommands(this);
  registerBitCommands(this);
  registerMathCommands(this);
  registerStackCommands(this);
  registerUnimplementedCommands(this);

  for (int i = 0; i < 256; i++) {
    if (commands[i] == NULL && i != 0xCB) {
      cout << "0x" << hex << unsigned(i) << " not implemented" << endl;
      assert(false);
    }
  }
}

CommandFactory::~CommandFactory() {}

CBCommandFactory::CBCommandFactory() {
  name = "CB";

  for (int i = 0; i < 256; i++) {
    RegisterCommand(new CBCommand(i));
  }
}

CBCommandFactory::~CBCommandFactory() {}
