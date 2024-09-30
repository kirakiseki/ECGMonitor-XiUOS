/**
* @file:    test_radix_tree.h
* @brief:   Implement a simple radix tree
* @version: 1.0
* @date:    2023/5/24
*/

#define NODE_SIZE 128
#define MAX_WORD_LEN 128

typedef struct _node {
    void* value;
    struct _node* next[NODE_SIZE];
} node;

node* CreateNode();
void InsertNode(node* root, const char* key, void* value);
void DeleteNode(node* root, const char* key);
void* FindNode(node* root, const char* key);
void DestroyTree(node* root);