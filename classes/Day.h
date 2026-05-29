#ifndef DAY_H
#define DAY_H

#include "../scenes/game_main.h"

class Day {
public:
    static float TimeLimit();
    static void ResetShiftStats(ShiftData& shift);
};

#endif
