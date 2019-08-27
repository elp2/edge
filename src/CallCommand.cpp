#include "CallCommand.hpp"

#include "CommandFactory.hpp"
#include "CPU.hpp"
#include "MMU.hpp"
class CallCommand : public Command {
 public:
    CallCommand(uint8_t opcode, string description, int cycles) {
        this->opcode = opcode;
        this->description = description;
        this->cycles = cycles;
    }

    ~CallCommand() {}

    void Run(CPU *cpu, MMU *mmu) {
        (void)mmu;
        uint16_t nextAddress;
        switch (opcode)
        {
        case 0xCD:
            nextAddress = cpu->Read16Bit(Register_PC) + 1;
            // Presumably, we'll then be able to return by popping and loading.
            cpu->Push16Bit(nextAddress);
            
            cpu->JumpAddress(cpu->Read16Bit(Eat_PC_Word));
            break;
        
        default:
            cout << "No Call opcode for 0x" << hex << unsigned(opcode) << endl;
            assert(false);
            break;
        }
    }
};

void registerCallCommands(AbstractCommandFactory *factory) {
    factory->RegisterCommand(new CallCommand(0xCD, "CALL nn", 12));
//     CALL NZ,nn C4 12
//  CALL Z,nn CC 12
//  CALL NC,nn D4 12
//  CALL C,nn DC 12
}