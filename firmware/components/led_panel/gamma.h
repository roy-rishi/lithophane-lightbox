#ifndef GAMMA_TABLE_
#define GAMMA_TABLE_

#include "stdint.h"

// A look-up table of gamma-corrected brightnesses indexed by raw value.
// Eg: Raw brightness 127 -> gamma_table[127]
// Generated with gamma=2.2 in Python.
extern const uint8_t gamma_table[256];

#endif  // GAMMA_TABLE_
