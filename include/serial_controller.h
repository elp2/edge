#pragma once

#include <cstdint>
#include <string>

// Simple Serial Controller capable of outputting Test ROM.

class SerialController {
 public:
  SerialController();
  ~SerialController() = default;

  uint8_t GetByteAt(uint16_t address);
  void SetByteAt(uint16_t address, uint8_t byte);

  void set_sb(uint8_t byte) { sb_ = byte; };
  void set_sc(uint8_t byte);

  uint8_t sb() { return sb_; };
  uint8_t sc() { return sc_; };

 private:
  std::string *line = new std::string();
  uint8_t sb_ = 0;
  uint8_t sc_ = 0;
};