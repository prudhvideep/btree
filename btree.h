//  .______          .___________..______       _______  _______     _______.
//  |   _  \         |           ||   _  \     |   ____||   ____|   /       |
//  |  |_)  |  ______`---|  |----`|  |_)  |    |  |__   |  |__     |   (----`
//  |   _  <  |______|   |  |     |      /     |   __|  |   __|     \   |
//  |  |_)  |            |  |     |  |\  \----.|  |____ |  |____.----)   |
//  |______/             |__|     | _| `._____||_______||_______|_______/
//
//
//
//
//
//
//

#include <array>
#include <iostream>
#include <memory>

#ifndef btree_h
#define btree_h

// The BTREE_ORDER definition sets the B-tree order (using the Knuth
// definition). This is the number of children the node can have. The
// number of keys is one less than this value.
#define BTREE_ORDER 5
#define MAX_KEYS 4
#define MIN_KEYS 2

#define LOG_INFO(msg) std::cout << "[INFO] " << msg << std::endl;
#define LOG_ERROR(msg) std::cerr << "[ERROR] " << msg << std::endl;

using namespace std;

// Note that the keys and children arrays are OVERSIZED to allow for
// some approaches to work, where nodes are allowed to temporarily
// have too many keys or children. You do not have to use the extra
// slots if you don't want to. Don't take this as a subtle hint that I
// want you to do it this way.
//
// A valid btree node can have at most:
//   BTREE_ORDER-1 keys.
//   BTREE_ORDER children.
struct btree {
  // num_keys is the number of in keys array that are currently valid.
  int num_keys;

  // keys is an array of values. valid indexes are in [0..num_keys)
  array<int, BTREE_ORDER> keys;

  // is_leaf is true if this is a leaf, false otherwise
  bool is_leaf;

  // children is an array of pointers to b-tree subtrees. valid
  // indexes are in [0..num_keys].
  array<shared_ptr<btree>, BTREE_ORDER+1> children;

  btree() : num_keys(0), is_leaf(true) {
    keys.fill(0);
    children.fill(nullptr);
  }
};

// insert adds the given key into a b-tree rooted at 'root'.  If the
// key is already contained in the btree this should do nothing.
//
// On exit:
// -- the 'root' pointer should refer to the root of the
//    tree. (the root may change when we insert or remove)
// -- the btree pointed to by 'root' is valid.
void insert(shared_ptr<btree> &root, int key);

// remove deletes the given key from a b-tree rooted at 'root'. If the
// key is not in the btree this should do nothing.
//
// On exit:
// -- the 'root' pointer should refer to the root of the
//    tree. (the root may change when we insert or delete)
// -- the btree pointed to by 'root' is valid.
void remove(shared_ptr<btree> &root, int key);

// find locates the node that either: (a) currently contains this key,
// or (b) the node that would contain it if we were to try to insert
// it.  Note that this always returns a non-null node.
shared_ptr<btree> find(shared_ptr<btree> &root, int key);

// count_nodes returns the number of nodes referenced by this
// btree. If this node is NULL, count_nodes returns zero; if it is a
// root, it returns 1; otherwise it returns 1 plus however many nodes
// are accessable via any valid child links.
int count_nodes(shared_ptr<btree> &root);

// count_keys returns the total number of keys stored in this
// btree. If the root node is null it returns zero; otherwise it
// returns the number of keys in the root plus however many keys are
// contained in valid child links.
int count_keys(shared_ptr<btree> &root);

#endif
