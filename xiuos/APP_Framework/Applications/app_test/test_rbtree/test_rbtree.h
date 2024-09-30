/*
* Copyright (c) 2023 AIIT XUOS Lab
* XiUOS is licensed under Mulan PSL v2.
* You can use this software according to the terms and conditions of the Mulan PSL v2.
* You may obtain a copy of Mulan PSL v2 at:
*        http://license.coscl.org.cn/MulanPSL2
* THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
* EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
* MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
* See the Mulan PSL v2 for more details.
*/

/**
* @file:    test_rbtree.h
* @brief:   a head file of red-black tree structure
* @version: 1.0
* @author:  AIIT XUOS Lab
* @date:    2023/6/23
*/
#ifndef REDBLACKTREE_H_
#define REDBLACKTREE_H_
#include <stdbool.h>
#include <stdio.h>

typedef struct RedBlackNode
{
    int key;
    struct RedBlackNode *left_child;
    struct RedBlackNode *right_child;
    struct RedBlackNode *parent;
    bool is_red;
} RBNodeType;

typedef struct RedBlackTree
{
    RBNodeType *root;
    RBNodeType *leaf;
} RBTreeType;

void TestRBTree(void);

void RBTreeTraversal(RBTreeType *tree, RBNodeType *node);

void RBTreeLeftRotate(RBTreeType *tree, RBNodeType *current_node);

void RBTreeRightRotate(RBTreeType *tree, RBNodeType* current_node);

void InsertFixup(RBTreeType *tree, RBNodeType* current_node);

void RBTreeInsert(RBTreeType *tree, RBNodeType* new_node);

void DeleteFixup(RBTreeType *tree, RBNodeType* current_node);

void RBTreeDelete(RBTreeType *tree, RBNodeType* target_node);

RBNodeType* FindSuccessor(RBTreeType *tree, RBNodeType* current_node);

RBNodeType* RBTreeSearch(RBTreeType *tree, int key);

#endif