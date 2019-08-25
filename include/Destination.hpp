#pragma once

enum Destination {
    Unknown = 0,
    Register_A,
    Register_B, 
    Register_C,
    Register_D,
    //5
    Register_E,
    Register_F,
    Register_H,
    Register_L,
    Register_BC,
    // 10
    Register_DE,
    Register_HL,
    Register_SP,
    Register_PC,
    Eat_PC_Byte,
    // 15
    Eat_PC_Word,
    Address_BC,
    Address_DE,
    Address_HL,
    Address_SP,
    // 20
    Address_nn,
};
