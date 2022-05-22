#include "array.h"
#include "platform.h"

PBArray* PBArrayCreate(size_t item_size) {
  PBArray* array = (PBArray*)pb_alloc(sizeof(PBArray));
  memset(array, 0, sizeof(PBArray));
  array->item_size = item_size;
  pb_log("PBArray: creating with item size %i", array->item_size);
  return array;
}
  
void PBArrayFree(PBArray* array) {
  if(array->first != NULL) {
    pb_free(array->first);
  }
  pb_free(array);
}
  
void* PBArrayGetItem(PBArray* array, int i) {
  if(i >= array->count) {
    pb_log("PBArray: attempt to fetch item outside of bounds");
    return NULL;
  }
  
  return ((char*)array->first) + (array->item_size * i);
}

void PBArrayRemoveAllItems(PBArray* array) {
  array->count = 0;
  if(array->first != NULL) {
    pb_free(array->first);
    array->first = NULL;
  }
}

void PBArrayRemoveItem(PBArray* array, void* item) {
  int i = PBArrayIndexOfItem(array, item);
  if(i != -1) {
    PBArrayRemoveItemAt(array, i);
  }
  else {
    pb_log("PBArray: couldnt find item %p", item);
  }
}

void PBArrayAppendItem(PBArray* array, void* item) {
  if(item == NULL) {
    pb_log("PBArray: attempt to store NULL item");
    return;
  }
  
  array->count++;
  
  if(array->first == NULL) {
    array->first = pb_alloc(array->item_size);
    memset(array->first, 0, array->item_size);
  }
  else {
    array->first = pb_realloc(array->first, array->item_size * array->count);
    memset( ((char*)array->first) + (array->item_size * (array->count - 1)), 0, array->item_size);
  }
  
  memcpy( ((char*)array->first) + (array->item_size * (array->count - 1)), item, array->item_size);
}
  
void PBArrayRemoveItemAt(PBArray* array, int i) {
  if(array->first == NULL || array->count == 0) {
    pb_log("PBArray: erase item in empty array");
    return;
  }
  
  if(i >= array->count) {
    pb_log("PBArray: attempt to erase item outside of bounds");
    return;
  }
  
  if(i < 0) {
    pb_log("PBArray: attempt to erase item at negative index");
    return;
  }
  
  int items_to_move = array->count - (i + 1);
  array->count--;
  if(items_to_move > 0) {
    memmove( ((char*)array->first) + (array->item_size * i), ((char*)array->first) + (array->item_size * (i + 1)), array->item_size * items_to_move);
  }
  
  if(array->count == 0) {
    pb_free(array->first);
    array->first = NULL;
  }
  else {
    array->first = pb_realloc(array->first, array->item_size * array->count);
  }
}
  
void* PBArrayFindItem(PBArray* array, int (*find_function)(void* item, int i)) {
  for(int i = 0; i < array->count; i++) {
    void* item = PBArrayGetItem(array, i);
    if(find_function(item, i) > 0) {
      return item;
    }
  }
  return NULL;
}

int PBArrayIndexOfItem(PBArray* array, void* item) {
  char first_byte = *(char*)item;
  for(int i = 0; i < array->count; i++) {
    void* found_item = PBArrayGetItem(array, i);
    
    // Quickly compare first bytes.
    if((*(char*)found_item) == first_byte) {
      pb_log("FIRST CHECK PASSED");
      if(memcmp(found_item, item, array->item_size) == 0) {
        return i;
      }
    }
    else {
      pb_log("FAILED FIRST CHECK");
    }
  }
  return -1;
}
