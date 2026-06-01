#include "Day.h"

float Day::TimeLimit(){
    return 180.0f;
}

void Day::ResetShiftStats(ShiftData& shift){
    shift.moneyEarned = 0;
    shift.itemsScanned = 0;
    shift.mistakesMade = 0;
    shift.wasFired = false;
}