#ifndef PLAYBOX2D_ARRAY_H
#define PLAYBOX2D_ARRAY_H

#include <stdlib.h>

typedef struct {
  int count;
  size_t item_size;
  void* first;
} PBArray;

extern PBArray* PBArrayCreate(size_t item_size);
extern void PBArrayFree(PBArray* array);
extern void PBArrayRemoveAllItems(PBArray* array);
extern void* PBArrayGetItem(PBArray* array, int i);
extern void PBArrayAppendItem(PBArray* array, void* item);
extern void PBArrayRemoveItemAt(PBArray* array, int i);
extern void* PBArrayFindItem(PBArray* array, int (*find_function)(void* item, int i));
extern int PBArrayIndexOfItem(PBArray* array, void* item);
extern void PBArrayRemoveItem(PBArray* array, void* item);

#endif