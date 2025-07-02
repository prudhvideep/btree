// btree.cpp
#include "btree.h"
#include "btree_unittest_help.h"
#include <array>
#include <cassert>
#include <cstddef>
#include <memory>

using namespace std;

// Initializes a btree node
shared_ptr<btree> init() {
  shared_ptr<btree> node = std::make_shared<btree>();
  return node;
}

// Returns the index where a key can be inserted in a node
int find_idx(array<int, BTREE_ORDER> keys, int l, int h, int target) {
  while (l <= h) {
    int mid = l + (h - l) / 2;

    if (keys[mid] > target) {
      h = mid - 1;
    } else if (keys[mid] < target) {
      l = mid + 1;
    } else {
      return mid;
    }
  }
  return l;
}

// Inserts a key at the index idx in the node
void insert_key_at(shared_ptr<btree> &node, int key, int idx) {
  int no_keys = node->num_keys;

  for (int i = no_keys - 1; i >= idx; i--) {
    node->keys[i + 1] = node->keys[i];
  }

  node->keys[idx] = key;
  node->num_keys++;
}

// Inserts a child at the index
void insert_child_at(shared_ptr<btree> &node, shared_ptr<btree> &child, int idx) {
  int no_children = node->num_keys + 1;

  for (int i = no_children - 1; i >= idx; i--) {
    node->children[i + 1] = node->children[i];
  }

  node->children[idx] = child;
}

// Move keys from the left node to the right node
void move_keys(shared_ptr<btree> left, shared_ptr<btree> right, int start) {
  int idx = 0;

  for (int i = start; i < left->num_keys; i++) {
    right->keys[idx++] = left->keys[i];
    right->num_keys++;
  }
}

// Move children from the left node to the right node
void move_children(shared_ptr<btree> left, shared_ptr<btree> right, int start_idx,
                   int total_children) {

  int idx = 0;
  for (int i = start_idx; i < total_children; i++) {
    right->children[idx++] = left->children[i];
    left->children[i] = nullptr;
  }
}

// Clear keys in a node
void clear_keys(shared_ptr<btree> node, int from_idx) {
  for (int i = from_idx; i < node->num_keys; i++) {
    node->keys[i] = 0;
  }

  node->num_keys = from_idx;
}

// split the leaf node
void split_leaf(shared_ptr<btree> &leaf, shared_ptr<btree> &parent) {
  // This means the leaf node is the root
  bool is_root = (parent == nullptr);
  shared_ptr<btree> left = leaf;

  if (is_root) {
    parent = init();
    parent->is_leaf = false;
    leaf = parent;
  }

  shared_ptr<btree> right = init();

  int mid = (left->num_keys) / 2;

  int key_to_insert = left->keys[mid];
  int idx_parent =
      find_idx(parent->keys, 0, parent->num_keys - 1, key_to_insert);

  insert_key_at(parent, key_to_insert, idx_parent);
  if (is_root) {
    insert_child_at(parent, left, idx_parent);
  }
  insert_child_at(parent, right, idx_parent + 1);

  move_keys(left, right, mid + 1);
  clear_keys(left, mid);
}

// split an internal node
void split_internal(shared_ptr<btree> &node, shared_ptr<btree> &parent) {
  bool is_root = (parent == nullptr);
  shared_ptr<btree> left = node;

  if (is_root) {
    parent = init();
    parent->is_leaf = false;
    node = parent;
  }

  shared_ptr<btree> right = init();
  right->is_leaf = false;

  int mid = left->num_keys / 2;
  int key_to_insert = left->keys[mid];
  int idx_parent =
      find_idx(parent->keys, 0, parent->num_keys - 1, key_to_insert);

  insert_key_at(parent, key_to_insert, idx_parent);

  if (is_root) {
    insert_child_at(parent, left, idx_parent);
  }
  insert_child_at(parent, right, idx_parent + 1);

  int no_of_keys = left->num_keys;
  int no_of_children = no_of_keys + 1;

  move_keys(left, right, mid + 1);
  clear_keys(node, mid);

  int mid_child = no_of_children / 2;
  move_children(left, right, mid_child, no_of_children);
}

// Handles main recursive insert logic
void insert_helper(shared_ptr<btree> &node, int key, shared_ptr<btree> &parent) {

  int pos_idx = find_idx(node->keys, 0, node->num_keys - 1, key);
  if (node->keys[pos_idx] == key) {
    return;
  }

  // If leaf
  if (node->is_leaf) {

    int insert_pos_idx = find_idx(node->keys, 0, node->num_keys - 1, key);
    insert_key_at(node, key, insert_pos_idx);

    if (node->num_keys > MAX_KEYS) {
      split_leaf(node, parent);
    }
  } else {
    insert_helper(node->children[pos_idx], key, node);

    if (node->num_keys > MAX_KEYS) {
      split_internal(node, parent);
    }
  }
}

void insert(shared_ptr<btree> &root, int key) {
  if (root == NULL) {
    root = init();
  }

  shared_ptr<btree> tree_parent = nullptr;

  insert_helper(root, key, tree_parent);
}

void remove(btree *&root, int key) {
  (void)root;
  (void)key;
  // TODO
}

shared_ptr<btree> find(shared_ptr<btree> &root, int key) {
  int pos_idx = find_idx(root->keys, 0, root->num_keys - 1, key);
  if (root->keys[pos_idx] == key) {
    return root;
  } else if (root->is_leaf) {
    return root;
  }

  shared_ptr<btree> child_node = find(root->children[pos_idx], key);
  return child_node;
}

int count_nodes(shared_ptr<btree> &root) {
  if (root == nullptr) {
    return 0;
  }

  int count = 1;
  for (int i = 0; i < root->num_keys + 1; i++) {
    if (root->children[i] != nullptr) {
      count += count_nodes(root->children[i]);
    }
  }
  return count;
}

int count_keys(std::shared_ptr<btree> &root) {
  if (root == nullptr) {
    return 0;
  }

  int count = root->num_keys;
  for (int i = 0; i < root->num_keys + 1; i++) {
    if (root->children[i] != nullptr) {
      count += count_keys(root->children[i]);
    }
  }

  return count;
}