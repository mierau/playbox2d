#include "pd_api.h"
#include "playbox.h"

PlaydateAPI* pd = NULL;

int eventHandler(PlaydateAPI* playdate, PDSystemEvent event, uint32_t arg) {
  if(event == kEventInitLua) {
    pd = playdate;
    registerPlaybox();
  }
  return 0;
}