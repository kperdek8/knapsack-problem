#ifndef ITEM_H
#define ITEM_H

struct Item {
  int value;
  int weight;
  Item(const int v, const int w) : value(v), weight(w) {}
};

#endif
