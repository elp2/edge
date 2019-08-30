#include <cstdint>
#include <string>

using namespace std;

enum CartridgeType {CartridgeType_ROM_MBC1, CartridgeType_Unsupported};
enum ROMSizeType {ROMSize_64k, ROMSize_Unsupported};

class ROM {
 public:
   ROM();
   ~ROM();

   bool LoadFile(string filename);
   uint8_t GetByteAt(uint16_t address);

   CartridgeType CartridgeType();
   ROMSizeType ROMSizeType();

   uint32_t Size();

   string GameTitle();

 private:
    uint8_t *rom;
    uint32_t romSize;
};