#include "Pad.h"
 
std::map<KEYTYPE, bool> Pad::keys;
std::map<KEYTYPE, bool> Pad::kpress;
Uint32 Pad::t_up = 0;
Uint32 Pad::t_down = 0;