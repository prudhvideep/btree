# B-Tree Implementation & Testing Playground

This repository is a hands-on environment for learning, implementing, and testing **B-Trees**. A foundational data structure used in storage systems, databases, and file systems. 

## Purpose

B-Trees are core to efficient indexing and data retrieval in large-scale systems. This repo serves as an educational testbed to:

- Implement a B-Tree from scratch
- Understand node splitting, merging, and rebalancing
- Write and run unit tests to validate correctness

## Features

- Starter code for B-Tree node structure and insert/delete operations
- Pre-written unit tests using [Catch2](https://github.com/catchorg/Catch2)
- `print_tree` function for visual debugging
- Graphviz-compatible output for use with the [B-Tree Visualizer](https://www.cs.usfca.edu/~galles/visualization/BTree.html)

## Running Tests

To compile and run all unit tests, simply run:

```bash
make clean
make
./btree_test
```

## Pratice 
You can check out the test branch and implement the umimplemented functions and run the tests to check your implementaion.