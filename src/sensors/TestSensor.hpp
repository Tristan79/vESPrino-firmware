#ifndef TestSensor_h
#define TestSensor_h

#include "interfaces/Sensor.hpp"
#include <LinkedList.h>
#include "interfaces/Pair.h"

class TestSensor : public Sensor {
public:
  TestSensor();
  bool setup(MenuHandler *handler);
  void getData(LinkedList<Pair*> *data);
  const char* getName() {
    return "TEST";
  }
  static void toggle(const char *line);
  void toggleInst();

private:
  bool enabled = false;
};
#endif
