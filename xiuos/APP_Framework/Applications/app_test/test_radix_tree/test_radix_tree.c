/**
* @file:    test_radix_tree.c
* @brief:   Implement a simple radix tree
* @version: 1.0
* @date:    2023/5/24
*/

#include <transform.h>
#include "test_radix_tree.h"

/**
 * @description: Create a radix tree node
 * @return node pointer
 */
node* CreateNode()
{
    node* n = (node*)malloc(sizeof(node));
    n->value = NULL;
    for (int i = 0; i < NODE_SIZE; i++) {
        n->next[i] = NULL;
    }
    return n;
}

/**
 * @description: Insert a new node to radix tree
 * @param root - radix tree root
 * @param key - new node key
 * @param value - new node value
 * @return void
 */
void InsertNode(node* root, const char* key, void* value)
{
    if (root == NULL) {
        return;
    }
    node* cur = root;
    size_t len = strlen(key);
    for (size_t i = 0; i < len; i++) {
        uint8_t b = (uint8_t)key[i];
        if (cur->next[b] == NULL) {
            cur->next[b] = CreateNode();
        }
        cur = cur->next[b];
    }
    cur->value = value;
}

/**
 * @description: Delete a node from radix tree
 * @param root - radix tree root
 * @param key - key which is needed to delete
 * @return void
 */
void DeleteNode(node* root, const char* key)
{
    if (root == NULL) {
        return;
    }
    node** cur = &root;
    size_t len = strlen(key);
    for (size_t i = 0; i < len; i++) {
        uint8_t b = (uint8_t)key[i];
        if ((*cur)->next[b] == NULL) {
            return;
        }
        cur = &((*cur)->next[b]);
    }

    if ((*cur)->value == NULL) {
        return;
    }

    (*cur)->value = NULL;

    int has_children = 0;
    for (int i = 0; i < NODE_SIZE; i++) {
        if ((*cur)->next[i] != NULL) {
            has_children = 1;
            break;
        }
    }
    if (!has_children) {
        free(*cur);
        (*cur) = NULL;
    }
}

/**
 * @description: find a node by key
 * @param root - radix tree root
 * @param key - key which is needed to find
 * @return value pointer corresponding to key
 */
void* FindNode(node* root, const char* key)
{
    if (root == NULL) {
        return NULL;
    }
    node* cur = root;
    size_t len = strlen(key);
    for (size_t i = 0; i < len; i++) {
        uint8_t b = (uint8_t)key[i];
        if (cur->next[b] == NULL) {
            return NULL;
        }
        cur = cur->next[b];
    }
    return cur->value;
}

/**
 * @description: Destroy the radix tree
 * @param root - radix tree root
 * @return void
 */
void DestroyTree(node* root)
{
    if (root == NULL) {
        return;
    }
    for (int i = 0; i < NODE_SIZE; i++) {
        DestroyTree(root->next[i]);
    }
    free(root);
}

void TestRadix()
{
    char keys[][MAX_WORD_LEN] = {
        "what",
        "where",
        "why",
        "how",
        "hello!",
        "apple",
        "12345"
    };
    int values[] = {1, 2, 3, 4, 5, 6, 7};

    printf("\nCreate tree and add key & value:\n");
    node* root = CreateNode();
    if (!root) printf("Create node failed.\n");

    int num = sizeof(keys) / sizeof(keys[0]);
    for (int i = 0; i < num - 1; ++i) {
        InsertNode(root, keys[i], &values[i]);
    }

    for (int i = 0; i < num; ++i) {
        int* v = (int*)FindNode(root, keys[i]);
        if (v) printf("keys[%d] \"%s\"'v = %d, values[%d] = %d\n", i, keys[i], *v, i, values[i]);
        else printf("keys[%d] \"%s\" not found\n", i, keys[i]);
    }

    printf("\nDelete \"where\" and \"how\":\n");
    DeleteNode(root, keys[1]);
    DeleteNode(root, keys[3]);
    
    for (int i = 0; i < num; ++i) {
        int* v = (int*)FindNode(root, keys[i]);
        if (v) printf("keys[%d] \"%s\"'v = %d, values[%d] = %d\n", i, keys[i], *v, i, values[i]);
        else printf("keys[%d] \"%s\" not found\n", i, keys[i]);
    }

    printf("\nInsert \"where\" and \"12345\":\n");
    InsertNode(root, keys[1], &values[1]);
    InsertNode(root, keys[6], &values[6]);

    for (int i = 0; i < num; ++i) {
        int* v = (int*)FindNode(root, keys[i]);
        if (v) printf("keys[%d] \"%s\"'v = %d, values[%d] = %d\n", i, keys[i], *v, i, values[i]);
        else printf("keys[%d] \"%s\" not found\n", i, keys[i]);
    }

    printf("\nDestroy tree:\n");
    DestroyTree(root);
    root = NULL;

    for (int i = 0; i < num; ++i) {
        int* v = (int*)FindNode(root, keys[i]);
        if (v) printf("keys[%d] \"%s\"'v = %d, values[%d] = %d\n", i, keys[i], *v, i, values[i]);
        else printf("keys[%d] \"%s\" not found\n", i, keys[i]);
    }
}

PRIV_SHELL_CMD_FUNCTION(TestRadix, Implement a simple radix tree, PRIV_SHELL_CMD_MAIN_ATTR);