#include "prizes.h"
#include "chase.h"
#include "list.h"
#include "server.h"

void updatePrizes(WINDOW *mywin){
    addPrize(init_prize(my_win), head_prizes);
}