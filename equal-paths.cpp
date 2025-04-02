#ifndef RECCHECK
//if you want to add any #includes like <iostream> you must do them here (before the next endif)

#endif

#include "equal-paths.h"
using namespace std;


// You may add any prototypes of helper functions here

// Prototype of helper
bool depthCheck(Node* node, int depth, int& leafDepth);

// Helper function that returns whether the 2
// lengths to the root are equal or not
bool depthCheck(Node* node, int depth, int& leafDepth) {

    // Base Case
    // If the parameter node is empty then it's true by default
    if (!node) {
        return true;
    }

    // If the node doesnt have any children
    if (!node->left && !node->right) {
        // Record it
        if (leafDepth == -1) {

            // Assign the leaf depth to the new depth
            leafDepth = depth;
            return true;
        }

        // The deepest depth is now the new node
        return depth == leafDepth;
    }

    // If both the left and right node are true then return that
    return depthCheck(node->left, depth + 1, leafDepth) && depthCheck(node->right, depth + 1, leafDepth);
}



bool equalPaths(Node * root)
{
    // Add your code below

    // Set the default depth to be -1 
    int leafDepth = -1;

    // Return the helper function using the root node, and a max depth of 0
    return depthCheck(root, 0, leafDepth);
}

