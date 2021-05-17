#ifndef _ALARM_H_
#define _ALARM_H_

#include <stdbool.h>

// Init alarm
bool alarm_init();

// Enable or disable alarm
bool alarm_switch(bool state);

// Free alarm
bool alarm_free();

#endif
