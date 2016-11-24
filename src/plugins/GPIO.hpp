#ifndef GPIO_H
#define GPIO_H
#include "common.hpp"
#include "PropertyList.hpp"
#include "interfaces/Plugin.hpp"
#include <Timer.h>

class GPIOClass : public Plugin {
public:
  GPIOClass() ;
  void setup(MenuHandler *handler);
  const char* getName() {
    return "GPIO";
  }
  void loop();
  static void setBit(uint32_t &val, int bit, int state);
  static bool isBitSet(uint32_t val, int bit);
private:
  uint32_t ports = 0;
  uint32_t modes = 0;
  uint32_t oldState = 0;
  uint32_t doubleCheck = 0;
  void initPortsFromString(const char *s);
  uint32_t getGPIOState();
  static void cmdGPIOSet(const char *line);
};
#endif
