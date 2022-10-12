#include "pd_api.h"
#include "playbox2d/playbox2d.h"

int eventHandler(PlaydateAPI* playdate, PDSystemEvent event, uint32_t arg) {
  if(event == kEventInitLua) {
    register_playbox2d(playdate);
  }
  return 0;
}
