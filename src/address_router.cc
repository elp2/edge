#include "address_router.h"

#include <cassert>
#include <iostream>

#include "input_controller.h"
#include "interrupt_controller.h"
#include "mmu.h"
#include "ppu.h"
#include "serial_controller.h"
#include "sound_controller.h"
#include "state.h"
#include "timer_controller.h"
#include "utils.h"

using namespace std;

const uint16_t DMA_ADDRESS = 0xFF46;
const uint16_t VIDEO_RAM_START_ADDRESS = 0x8000;

const uint16_t OAM_RAM_ADDRESS = 0xFE00;
const int NUM_OAM_SPRITES = 40;
const int OAM_SPRITE_BYTES = 4;  // Technically only uses the first 28 bits.

AddressRouter::AddressRouter(MMU *mmu, PPU *ppu,
                             SerialController *serial_controller,
                             InterruptController *interrupt_controller,
                             InputController *input_controller,
                             TimerController *timer_controller,
                             SoundController *sound_controller) {
  mmu_ = mmu;
  ppu_ = ppu;
  serial_controller_ = serial_controller;
  interrupt_controller_ = interrupt_controller;
  input_controller_ = input_controller;
  timer_controller_ = timer_controller;
  sound_controller_ = sound_controller;
}

AddressOwner ownerForIOAddress(uint16_t address) {
  // cout << "IO @ " << hex << unsigned(address) << endl;
  switch (address) {
    case 0xFF00:
      // P0 Joy Pad.
      return AddressOwner_Input;
    case 0xFF01:
    case 0xFF02:
      // Serials.
      return AddressOwner_Serial;
    case 0xFF04:
      // Divider.
      return AddressOwner_Timer;
    case 0xFF05:
      // Timer Counter TIMA.
      return AddressOwner_Timer;
    case 0xFF06:
      // TMA Timer Modulo.
      return AddressOwner_Timer;
    case 0xFF07:
      // TAC Timer Control.
      return AddressOwner_Timer;
    case 0xFF0F:
      return AddressOwner_Interrupt;
    case 0xFF10:
    case 0xFF11:
    case 0xFF12:
    case 0xFF13:
    case 0xFF14:
    case 0xFF15:
    case 0xFF16:
    case 0xFF17:
    case 0xFF18:
    case 0xFF19:
    case 0xFF1A:
    case 0xFF1B:
    case 0xFF1C:
    case 0xFF1D:
    case 0xFF1E:
    case 0xFF1F:
    case 0xFF20:
    case 0xFF21:
    case 0xFF22:
    case 0xFF23:
    case 0xFF24:
    case 0xFF25:
    case 0xFF26:
      // Nothng until FF30.

    case 0xFF27:
    case 0xFF28:
    case 0xFF29:
    case 0xFF2A:
    case 0xFF2B:
    case 0xFF2C:
    case 0xFF2D:
    case 0xFF2E:
    case 0xFF2F:
      // Sound voices.
      return AddressOwner_Sound;
    case 0xFF30:
    case 0xFF31:
    case 0xFF32:
    case 0xFF33:
    case 0xFF34:
    case 0xFF35:
    case 0xFF36:
    case 0xFF37:
    case 0xFF38:
    case 0xFF39:
    case 0xFF3A:
    case 0xFF3B:
    case 0xFF3C:
    case 0xFF3D:
    case 0xFF3E:
    case 0xFF3F:
      // Waveform.
      return AddressOwner_Sound;
    case 0xFF40:
    case 0xFF41:
    case 0xFF42:
    case 0xFF43:
    case 0xFF44:
    case 0xFF45:
    // case 0xFF46: DMA is handled by the Address Router itself.
    case 0xFF47:
    case 0xFF48:
    case 0xFF49:
    case 0xFF4A:
    case 0xFF4B:
      return AddressOwner_PPU;
    case 0xFFFF:
      // Interrupt Enable.
      return AddressOwner_Interrupt;
    default:
      // Unknown IO, but we should just ignore since we encounter this when saving all bytes.
      return AddressOwner_MMU;
  }
}

AddressOwner ownerForAddress(uint16_t address) {
  if (address < VIDEO_RAM_START_ADDRESS) {
    return AddressOwner_MMU;
  } else if (address < 0xa000) {
    return AddressOwner_PPU;
  } else if (address < 0xc000) {
    return AddressOwner_MMU;
  } else if (address < 0xe000) {
    return AddressOwner_MMU;
  } else if (address < 0xfe00) {
    return AddressOwner_MMU;
  } else if (address < 0xfea0) {
    return AddressOwner_PPU;  // OAM.
  } else if (address < 0xff00) {
    return AddressOwner_MMU;  // Empty i/o.
  } else if (address == DMA_ADDRESS) {
    return AddressOwner_DMA;
  } else if (address < 0xFF4C || address == 0xFFFF) {
    return ownerForIOAddress(address);
  } else if (address < 0xFF80) {
    return AddressOwner_MMU;  // Empty i/o (2).
  } else if (address < 0xFFFF) {
    return AddressOwner_MMU;  // Internal RAM.
  } else {
    return AddressOwner_MMU;  // Interrupt Enable Register.
  }
}

uint8_t AddressRouter::GetByteAtAddressFromOwner(AddressOwner owner,
                                                 uint16_t address) {
  switch (owner) {
    case AddressOwner_MMU:
      return mmu_->GetByteAt(address);
    case AddressOwner_PPU:
      return ppu_->GetByteAt(address);
    case AddressOwner_Serial:
      return serial_controller_->GetByteAt(address);
    case AddressOwner_Interrupt:
      return interrupt_controller_->GetByteAt(address);
    case AddressOwner_Input:
      return input_controller_->GetByteAt(address);
    case AddressOwner_DMA:
      return dma_base_;
    case AddressOwner_Timer:
      return timer_controller_->GetByteAt(address);
    case AddressOwner_Sound:
      return sound_controller_->GetByteAt(address);
    default:
      assert(false);
      return 0x00;
  }
}

void AddressRouter::EnableDisassemblerMode(bool disassemblerMode) {
  disassemblerMode_ = disassemblerMode;
  mmu_->EnableDisassemblerMode(disassemblerMode);
}

void AddressRouter::SetByteAtAddressInOwner(AddressOwner owner,
                                            uint16_t address, uint8_t byte) {
  switch (owner) {
    case AddressOwner_MMU:
      return mmu_->SetByteAt(address, byte);
    case AddressOwner_PPU:
      return ppu_->SetByteAt(address, byte);
    case AddressOwner_Serial:
      return serial_controller_->SetByteAt(address, byte);
    case AddressOwner_Interrupt:
      return interrupt_controller_->SetByteAt(address, byte);
    case AddressOwner_Input:
      return input_controller_->SetByteAt(address, byte);
    case AddressOwner_DMA:
      return PerformDMA(byte);
    case AddressOwner_Timer:
      return timer_controller_->SetByteAt(address, byte);
    case AddressOwner_Sound:
      return sound_controller_->SetByteAt(address, byte);
    default:
      assert(false);
      break;
  }
}

uint8_t AddressRouter::GetByteAt(uint16_t address) {
  AddressOwner owner = ownerForAddress(address);
  return GetByteAtAddressFromOwner(owner, address);
}

void AddressRouter::SetByteAt(uint16_t address, uint8_t byte) {
  AddressOwner owner = ownerForAddress(address);
  SetByteAtAddressInOwner(owner, address, byte);
}

uint16_t AddressRouter::GetWordAt(uint16_t address) {
  AddressOwner owner_lsb = ownerForAddress(address);
  AddressOwner owner_msb = ownerForAddress(address + 1);
  if (owner_lsb != owner_msb) {
    cout << "Unexpected cross owner get at 0x" << hex << unsigned(address)
         << " +1" << endl;
    assert(false);
  }

  uint8_t lsb = GetByteAtAddressFromOwner(owner_lsb, address);
  uint8_t msb = GetByteAtAddressFromOwner(owner_msb, address + 1);

  uint16_t result = (msb << 8) | lsb;
  return result;
}

void AddressRouter::SetWordAt(uint16_t address, uint16_t word) {
  AddressOwner owner_lsb = ownerForAddress(address);
  AddressOwner owner_msb = ownerForAddress(address + 1);
  if (owner_lsb != owner_msb) {
    cout << "Unexpected cross owner set at 0x" << hex << unsigned(address)
         << " +1" << endl;
    assert(false);
  }

  SetByteAtAddressInOwner(owner_lsb, address, LOWER8(word));
  SetByteAtAddressInOwner(owner_msb, address + 1, HIGHER8(word));
}

void AddressRouter::PerformDMA(uint8_t dma_base) {
  dma_base_ = dma_base;

  uint16_t dma_address = dma_base_;
  dma_address <<= 8;
  // This takes 160 cycles, but we're assuming the CPU is doing its loop.
  for (int i = 0; i < NUM_OAM_SPRITES * OAM_SPRITE_BYTES; i++) {
    SetByteAt(OAM_RAM_ADDRESS + i, GetByteAt(dma_address + i));
  }
}

void AddressRouter::SaveState(struct MemorySaveState &state) {
  for (int i = 0x8000; i <= 0xFFFF; i++) {
    AddressOwner owner = ownerForAddress(i);
    if (owner != AdressOwner_Unknown) {
      state.ram[i - 0x8000] = GetByteAtAddressFromOwner(owner, i);
    }
  }
}

void AddressRouter::LoadState(const struct MemorySaveState &state) {
  for (int i = 0x8000; i <= 0xFFFF; i++) {
    AddressOwner owner = ownerForAddress(i);
    if (owner != AdressOwner_Unknown) {
      SetByteAtAddressInOwner(owner, i, state.ram[i - 0x8000]);
    }
  }
}

void AddressRouter::SkipBootROM() {
  struct MemorySaveState ss = {};

  ss.ram[0xFF00 - 0x8000] = 0xCF;
  ss.ram[0xFF01 - 0x8000] = 0x00;
  ss.ram[0xFF02 - 0x8000] = 0x7E;
  ss.ram[0xFF04 - 0x8000] = 0xAB;
  ss.ram[0xFF05 - 0x8000] = 0x00;
  ss.ram[0xFF06 - 0x8000] = 0x00;
  ss.ram[0xFF07 - 0x8000] = 0xF8;
  ss.ram[0xFF0F - 0x8000] = 0xE1;
  ss.ram[0xFF10 - 0x8000] = 0x80;
  /* 
  Do not play any sounds from the boot ROM.
  ss.ram[0xFF11 - 0x8000] = 0xBF;
  ss.ram[0xFF12 - 0x8000] = 0xF3;
  ss.ram[0xFF13 - 0x8000] = 0xFF;
  ss.ram[0xFF14 - 0x8000] = 0xBF;
  ss.ram[0xFF16 - 0x8000] = 0x3F;
  ss.ram[0xFF17 - 0x8000] = 0x00;
  ss.ram[0xFF18 - 0x8000] = 0xFF;
  ss.ram[0xFF19 - 0x8000] = 0xBF;
  ss.ram[0xFF1A - 0x8000] = 0x7F;
  ss.ram[0xFF1B - 0x8000] = 0xFF;
  ss.ram[0xFF1C - 0x8000] = 0x9F;
  ss.ram[0xFF1D] = 0xFF;
  ss.ram[0xFF1E - 0x8000] = 0xBF;
  ss.ram[0xFF20 - 0x8000] = 0xFF;
  ss.ram[0xFF21 - 0x8000] = 0x00;
  ss.ram[0xFF22 - 0x8000] = 0x00;
  */
  ss.ram[0xFF23 - 0x8000] = 0xBF;
  ss.ram[0xFF24 - 0x8000] = 0x77;
  ss.ram[0xFF25 - 0x8000] = 0xF3;
  ss.ram[0xFF26 - 0x8000] = 0xF1;
  ss.ram[0xFFFF - 0x8000] = 0x00;


  ss.ram[0xFF40 - 0x8000] = 0x91;
  ss.ram[0xFF41 - 0x8000] = 0x85;
  ss.ram[0xFF42 - 0x8000] = 0x00;
  ss.ram[0xFF43 - 0x8000] = 0x00;
  ss.ram[0xFF44 - 0x8000] = 0x00;
  ss.ram[0xFF45 - 0x8000] = 0x00;
  ss.ram[0xFF46 - 0x8000] = 0xFF;
  ss.ram[0xFF47 - 0x8000] = 0xFC;
  ss.ram[0xFF4A - 0x8000] = 0x00;
  ss.ram[0xFF4B - 0x8000] = 0x00;

  LoadState(ss);
}
