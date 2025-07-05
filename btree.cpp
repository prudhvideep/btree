// btree.cpp
#include "btree.h"
#include "btree_unittest_help.h"
#include "catch.hpp"
#include <algorithm>
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
void insert_child_at(shared_ptr<btree> &node, shared_ptr<btree> &child,
                     int idx) {
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
void move_children(shared_ptr<btree> left, shared_ptr<btree> right,
                   int start_idx, int total_children) {

  int idx = 0;
  for (int i = start_idx; i < total_children; i++) {
    right->children[idx++] = left->children[i];
    left->children[i] = nullptr;
  }
}

// Clear keys in a node
void clear_keys(shared_ptr<btree> node, int from_idx) {
  assert(from_idx < BTREE_ORDER + 1);

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
void insert_helper(shared_ptr<btree> &node, int key,
                   shared_ptr<btree> &parent) {

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

bool key_exists(shared_ptr<btree> root, int key) {
  if (root == nullptr)
    return false;

  shared_ptr<btree> temp = root;
  while (temp != nullptr) {

    int insert_pos_idx = find_idx(temp->keys, 0, temp->num_keys - 1, key);
    if (temp->keys[insert_pos_idx] == key) {
      return true;
    }

    temp = temp->children[insert_pos_idx];
  }

  return false;
}

void remove_key_at(shared_ptr<btree> &node, int idx) {
  for (int i = idx + 1; i < node->num_keys; i++) {
    node->keys[i - 1] = node->keys[i];
  }

  node->num_keys--;
}

void remove_child_at(shared_ptr<btree> &node, int idx) {
  for (int i = idx + 1; i < MAX_KEYS + 1; i++) {
    node->children[i - 1] = node->children[i];
  }
}

int get_child_pos(shared_ptr<btree> child, shared_ptr<btree> node) {
  for (int i = 0; i < node->num_keys + 1; i++) {
    if (node->children[i] == child) {
      return i;
    }
  }

  return -1;
}

void merge_leaf_nodes(shared_ptr<btree> from_node, shared_ptr<btree> to_node) {
  assert(from_node->num_keys >= 1);
  assert(to_node->num_keys >= 1);

  if (from_node->keys[0] < to_node->keys[0]) {
    for (int i = from_node->num_keys - 1; i >= 0; i--) {
      insert_key_at(to_node, from_node->keys[i], 0);
    }
  } else {
    for (int i = 0; i < from_node->num_keys; i++) {
      insert_key_at(to_node, from_node->keys[i], to_node->num_keys);
    }
  }

  assert(to_node->num_keys <= MAX_KEYS);
}

void remove_helper(shared_ptr<btree> &node, int key,
                   shared_ptr<btree> &parent) {
  if (node == nullptr)
    return;

  int pos_idx = find_idx(node->keys, 0, node->num_keys - 1, key);
  // Check if the key exists in this node
  // Else do nothing, go to the child
  if (node->keys[pos_idx] == key) {
    if (node->is_leaf) {
      if (node->num_keys > MIN_KEYS) {
        remove_key_at(node, pos_idx);
        return;
      } else if (node->num_keys <= MIN_KEYS) {
        int child_pos = get_child_pos(node, parent);

        shared_ptr<btree> left_sib = nullptr;
        if (child_pos - 1 >= 0) {
          left_sib = parent->children[child_pos - 1];
        }

        shared_ptr<btree> right_sib = nullptr;
        if (child_pos + 1 < BTREE_ORDER + 1) {
          right_sib = parent->children[child_pos + 1];
        }

        if (left_sib != nullptr && left_sib->num_keys > MIN_KEYS) {
          int in_ord_pred = left_sib->keys[left_sib->num_keys - 1];
          remove_key_at(left_sib, left_sib->num_keys - 1);

          int parent_key = parent->keys[child_pos - 1];
          parent->keys[child_pos] = in_ord_pred;

          remove_key_at(node, pos_idx);

          int insert_pos =
              find_idx(node->keys, 0, node->num_keys - 1, parent_key);
          insert_key_at(node, parent_key, insert_pos);

        } else if (right_sib != nullptr && right_sib->num_keys > MIN_KEYS) {

          int in_ord_suc = right_sib->keys[0];
          remove_key_at(right_sib, 0);

          int parent_key = parent->keys[child_pos];
          parent->keys[child_pos] = in_ord_suc;

          remove_key_at(node, pos_idx);

          int insert_pos =
              find_idx(node->keys, 0, node->num_keys - 1, parent_key);
          insert_key_at(node, parent_key, insert_pos);
        } else {
          remove_key_at(node, pos_idx);
          if (left_sib != nullptr) {
            int left_sib_idx = child_pos - 1;

            int root_key = parent->keys[left_sib_idx];
            insert_key_at(left_sib, root_key, left_sib->num_keys);

            remove_key_at(parent, left_sib_idx);
            remove_child_at(parent, left_sib_idx);

            merge_leaf_nodes(left_sib, node);

            if (parent->num_keys == 0) {
              parent = node;
            }
          } else if (right_sib != nullptr) {
            int right_sib_idx = child_pos + 1;

            int root_key = parent->keys[child_pos];

            insert_key_at(right_sib, root_key, 0);

            remove_key_at(parent, child_pos);
            remove_child_at(parent, right_sib_idx);

            merge_leaf_nodes(right_sib, node);

            if (parent->num_keys == 0) {
              parent = node;
            }
          }
        }
      }
    } else {
      // key to delete is in an internal node
    }
  } else {
    remove_helper(node->children[pos_idx], key, node);
  }

  //After visting the children
  //Check if the node has less than min keys
  
}

void remove(shared_ptr<btree> &root, int key) {
  if (root == nullptr)
    return;

  // If key does not exist, do nothing
  if (!key_exists(root, key))
    return;

  shared_ptr<btree> parent = nullptr;
  remove_helper(root, key, parent);
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

// int main(int argc, char **argv) {

//   shared_ptr<btree> root = nullptr;

//   insert(root,  10);

//   for(int i = 20;i <= 35;i++) {
//     insert(root,  i);
//   }

//   print_tree(root);
// }