// btree.cpp
#include "btree.h"
#include "btree_unittest_help.h"
#include "catch.hpp"
#include <algorithm>
#include <array>
#include <atomic>
#include <cassert>
#include <cstddef>
#include <locale>
#include <memory>

using namespace std;

// Initializes a btree node
shared_ptr<btree> init() {
  shared_ptr<btree> node = std::make_shared<btree>();
  return node;
}

void insert(shared_ptr<btree> &root, int key) {
  //TODO
}


void remove(shared_ptr<btree> &root, int key) {
  //TODO
}

shared_ptr<btree> find(shared_ptr<btree> &root, int key) {
  //TODO
}

int count_nodes(shared_ptr<btree> &root) {
  //TODO
}

int count_keys(std::shared_ptr<btree> &root) {
  //TODO
}