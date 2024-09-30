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
* @file:    test_rbtree.c
* @brief:   a application of red-black tree function
* @version: 1.0
* @author:  AIIT XUOS Lab
* @date:    2023/6/23
*/
#include<string.h>
#include <transform.h>
#include"test_rbtree.h"
#ifdef ADD_XIZI_FEATURES

void RBTreeTraversal(RBTreeType *tree, RBNodeType *node) 
{
	if (node != tree->leaf) {
		RBTreeTraversal(tree, node->left_child);
		printf("key:%d, color:%s\n", node->key, (node->is_red ? "Red" : "Black"));
		RBTreeTraversal(tree, node->right_child);
	}
}

RBNodeType* RBTreeSearch(RBTreeType *tree, int key)
{
    RBNodeType* current_node = tree->root;
    while (current_node != tree->leaf){
        if (key < current_node->key)
            current_node = current_node->left_child;
        else if (key > current_node->key)
            current_node = current_node->right_child;
        else
            return current_node;
    }

    return tree->leaf;
}

void RBTreeLeftRotate(RBTreeType *tree, RBNodeType *current_node)
{
    RBNodeType* child_node = current_node->right_child;

    current_node->right_child = child_node->left_child;
    if (child_node->left_child != tree->leaf)
        child_node->left_child->parent = current_node;

    child_node->parent = current_node->parent;
    if (current_node->parent == tree->leaf)
        tree->root = child_node;
    else if (current_node == current_node->parent->left_child)
        current_node->parent->left_child = child_node;
    else
        current_node->parent->right_child = child_node;
    
    child_node->left_child = current_node;
    current_node->parent = child_node;
}

void RBTreeRightRotate(RBTreeType *tree, RBNodeType* current_node)
{
    RBNodeType* child_node = current_node->left_child;

    current_node->left_child = child_node->right_child;
    if (child_node->right_child != tree->leaf)
        child_node->right_child->parent = current_node;

    child_node->parent = current_node->parent;
    if (current_node->parent == tree->leaf)
        tree->root = child_node;
    else if (current_node == current_node->parent->right_child)
        current_node->parent->right_child = child_node;
    else
        current_node->parent->left_child = child_node;
    
    child_node->right_child = current_node;
    current_node->parent = child_node;
}

void InsertFixup(RBTreeType *tree, RBNodeType* current_node)
{
    while (current_node->parent->is_red){
        /* The parent of current_node is the left subtree of the grandfather */
        if (current_node->parent == current_node->parent->parent->left_child){
            RBNodeType * uncle_node = current_node->parent->parent->right_child;
            if (uncle_node->is_red){    /* case1:red uncle and red parent, change color */
                uncle_node->is_red = false;
                current_node->parent->is_red = false;
                current_node->parent->parent->is_red = true;

                current_node = current_node->parent->parent;
            }else{                      /* case2:black uncle and red parent, need rotation */
                if (current_node->parent->right_child == current_node){
                    current_node = current_node->parent;
                    RBTreeLeftRotate(tree, current_node);
                }

                current_node->parent->is_red = false;
                current_node->parent->parent->is_red = true;
                RBTreeRightRotate(tree, current_node->parent->parent);
            }
        /* The parent of current_node is the right subtree of the grandfather, same with left subtree */
        }else{
            RBNodeType * uncle_node = current_node->parent->parent->left_child;
            if (uncle_node->is_red){
                uncle_node->is_red = false;
                current_node->parent->is_red = false;
                current_node->parent->parent->is_red = true;

                current_node = current_node->parent->parent;
            }else{
                if (current_node->parent->left_child == current_node){
                    current_node = current_node->parent;
                    RBTreeRightRotate(tree, current_node);
                }

                current_node->parent->is_red = false;
                current_node->parent->parent->is_red = true;
                RBTreeLeftRotate(tree, current_node->parent->parent);
            }
        }
    }
    tree->root->is_red = false;
}

void RBTreeInsert(RBTreeType *tree, RBNodeType* new_node)
{
    RBNodeType* previous_node = tree->root;
    RBNodeType* current_node = tree->root;

    while (current_node != tree->leaf){
        previous_node = current_node;
        if (new_node->key > current_node->key)
            current_node = current_node->right_child;
        else if (new_node->key < current_node->key)
            current_node = current_node->left_child;
        else
            return;
    }

    if (previous_node == tree->leaf){
        tree->root = new_node;
        tree->root->parent = tree->leaf;
    }else{
        new_node->parent = previous_node;

        if (previous_node->key > new_node->key)
            previous_node->left_child = new_node;
        else
            previous_node->right_child = new_node;
    }

    InsertFixup(tree, new_node);
}

RBNodeType* FindSuccessor(RBTreeType *tree, RBNodeType* current_node)
{
    RBNodeType* parent_node = current_node->parent;
    if (current_node->right_child != tree->leaf){
        current_node = current_node->right_child;
        while (current_node->left_child != tree->leaf)
            current_node = current_node->left_child;
        return current_node;
    }

    while ((parent_node != tree->leaf) && (current_node == parent_node->right_child)){
        current_node = parent_node;
        parent_node = parent_node->parent;
    }
    return parent_node;
}

void DeleteFixup(RBTreeType *tree, RBNodeType* current_node)
{
    while ((current_node != tree->root) && (current_node->is_red == false)){
        if (current_node == current_node->parent->left_child){
            
            RBNodeType* brother_node = current_node->parent->right_child;
            if (brother_node->is_red){
                brother_node->is_red = false;
                current_node->parent->is_red = true;
                RBTreeLeftRotate(tree, current_node->parent);
                brother_node = current_node->parent->right_child;
            }

            if ((brother_node->left_child->is_red == false) && (brother_node->right_child->is_red == false)){
                brother_node->is_red = true;
                current_node = current_node->parent;
            }else{
                if (brother_node->right_child->is_red == false){
                    brother_node->left_child->is_red = false;
                    brother_node->is_red = true;
                    RBTreeRightRotate(tree, brother_node);
                    brother_node = current_node->parent->right_child;
                }

                brother_node->is_red = current_node->parent->is_red;
                current_node->parent->is_red = false;
                brother_node->right_child->is_red = false;
                RBTreeLeftRotate(tree, current_node->parent);
                current_node = tree->root;
            }
        }else{
            RBNodeType* brother_node = current_node->parent->left_child;
            if (brother_node->is_red){
                brother_node->is_red = false;
                current_node->parent->is_red = true;
                RBTreeRightRotate(tree, current_node->parent);
                brother_node = current_node->parent->left_child;
            }

            if ((brother_node->left_child->is_red == false) && (brother_node->right_child->is_red == false)){
                brother_node->is_red = true;
                current_node = current_node->parent;
            }else{
                if (brother_node->left_child->is_red == false){
                    brother_node->right_child->is_red = false;
                    brother_node->is_red = true;
                    RBTreeLeftRotate(tree, brother_node);
                    brother_node = current_node->parent->left_child;
                }

                brother_node->is_red = current_node->parent->is_red;
                current_node->parent->is_red = false;
                brother_node->left_child->is_red = false;
                RBTreeRightRotate(tree, current_node->parent);
                current_node = tree->root;
            }
        }
    }
    current_node->is_red = false;
}

void RBTreeDelete(RBTreeType *tree, RBNodeType* target_node)
{
    RBNodeType* delete_node = tree->leaf;
    RBNodeType* replace_node = tree->leaf;

    if ((target_node->left_child == tree->leaf) || (target_node->right_child == tree->leaf))
        delete_node = target_node;
    else
        delete_node = FindSuccessor(tree, target_node);
    
    if (delete_node->left_child != tree->leaf) /* successor still has subtree */
        replace_node = delete_node->left_child;
    else if (delete_node->right_child != tree->leaf)
        replace_node = delete_node->right_child;
    
    replace_node->parent = delete_node->parent;

    if (delete_node->parent == tree->leaf) /* delete a root node */
        tree->root = replace_node;
    else if (delete_node == delete_node->parent->left_child)
        delete_node->parent->left_child = replace_node;
    else
        delete_node->parent->right_child = replace_node;

    if (delete_node != target_node)
        target_node->key = delete_node->key;
    
    if (delete_node->is_red == false)
        DeleteFixup(tree, replace_node);

    free(delete_node);
}


void TestRBTree(void)
{
    int default_key[] = { 16, 25, 23, 5, 2, 6, 17, 37, 38, 98, 20, 19, 47, 49, 12, 21, 9, 18, 14, 15 };
    int array_size = sizeof(default_key) / sizeof(default_key[0]);
    
    printf("Test Red Black Tree\n");
    printf("default_key array: ");
    for (int i = 0; i < array_size; i++)
        printf("%d  ", default_key[i]);
    printf("\n%d elements\n", array_size);

	RBTreeType *tree = (RBTreeType *)malloc(sizeof(RBTreeType));
	if (tree == NULL) {
		printf("malloc failed\n");
        return;
	}

	tree->leaf = (RBNodeType*)malloc(sizeof(RBNodeType));
    tree->leaf->left_child = NULL;
    tree->leaf->right_child = NULL;
    tree->leaf->parent = NULL;
    tree->leaf->is_red = false;
    tree->leaf->key = -1;
	tree->root = tree->leaf;

	RBNodeType *node = tree->leaf;

	for (int i = 0; i < array_size; i++) {
		node = (RBNodeType*)malloc(sizeof(RBNodeType));
        node->left_child = tree->leaf;
        node->right_child = tree->leaf;
        node->parent = NULL;
        node->is_red = true;
        node->key = default_key[i];
		printf("insert key[%d]=%d\n",i,default_key[i]);
		RBTreeInsert(tree, node);
	}

	printf("------------------Inorder Traversal------------------\n");
	RBTreeTraversal(tree, tree->root);

	for (int i = 0; i < array_size; i++) {
		printf("search key = %d\n", default_key[i]);
		node = RBTreeSearch(tree, default_key[i]);
        printf("search succeeded, parent node: %d, left-child: %d, right-child: %d\n", node->parent->key, node->left_child->key, node->right_child->key);

    	printf("delete key = %d\n", node->key);
        RBTreeDelete(tree, node);
		
		printf("Show current tree?Y/N \n");
        char ch;
        scanf("%c", &ch);
         if (ch == 'Y' || ch == 'y') {
            printf("------------------Inorder Traversal Tree After Deletion------------------\n");
            if (tree->root != tree->leaf)
	            RBTreeTraversal(tree, tree->root);
            else
                printf("the tree is empty.\n");
        }
	}
}

PRIV_SHELL_CMD_FUNCTION(TestRBTree, a red-black tree test sample, PRIV_SHELL_CMD_MAIN_ATTR);

#endif