#ifndef AVLBST_H
#define AVLBST_H


#include <iostream>
#include <exception>
#include <cstdlib>
#include <cstdint>
#include <algorithm>
#include "bst.h"

struct KeyError { };

/**
* A special kind of node for an AVL tree, which adds the balance as a data member, plus
* other additional helper functions. You do NOT need to implement any functionality or
* add additional data members or helper functions.
*/
template <typename Key, typename Value>
class AVLNode : public Node<Key, Value>
{
public:
    // Constructor/destructor.
    AVLNode(const Key& key, const Value& value, AVLNode<Key, Value>* parent);
    virtual ~AVLNode();

    // Getter/setter for the node's height.
    int8_t getBalance () const;
    void setBalance (int8_t balance);
    void updateBalance(int8_t diff);

    // Getters for parent, left, and right. These need to be redefined since they
    // return pointers to AVLNodes - not plain Nodes. See the Node class in bst.h
    // for more information.
    virtual AVLNode<Key, Value>* getParent() const override;
    virtual AVLNode<Key, Value>* getLeft() const override;
    virtual AVLNode<Key, Value>* getRight() const override;

protected:
    int8_t balance_;    // effectively a signed char

    
};

/*
  -------------------------------------------------
  Begin implementations for the AVLNode class.
  -------------------------------------------------
*/

/**
* An explicit constructor to initialize the elements by calling the base class constructor
*/
template<class Key, class Value>
AVLNode<Key, Value>::AVLNode(const Key& key, const Value& value, AVLNode<Key, Value> *parent) :
    Node<Key, Value>(key, value, parent), balance_(0)
{

  

}

/**
* A destructor which does nothing.
*/
template<class Key, class Value>
AVLNode<Key, Value>::~AVLNode()
{

}

/**
* A getter for the balance of a AVLNode.
*/
template<class Key, class Value>
int8_t AVLNode<Key, Value>::getBalance() const
{
    return balance_;
}

/**
* A setter for the balance of a AVLNode.
*/
template<class Key, class Value>
void AVLNode<Key, Value>::setBalance(int8_t balance)
{
    balance_ = balance;
}

/**
* Adds diff to the balance of a AVLNode.
*/
template<class Key, class Value>
void AVLNode<Key, Value>::updateBalance(int8_t diff)
{
    balance_ += diff;
}

/**
* An overridden function for getting the parent since a static_cast is necessary to make sure
* that our node is a AVLNode.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getParent() const
{
    return static_cast<AVLNode<Key, Value>*>(this->parent_);
}

/**
* Overridden for the same reasons as above.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getLeft() const
{
    return static_cast<AVLNode<Key, Value>*>(this->left_);
}

/**
* Overridden for the same reasons as above.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getRight() const
{
    return static_cast<AVLNode<Key, Value>*>(this->right_);
}


/*
  -----------------------------------------------
  End implementations for the AVLNode class.
  -----------------------------------------------
*/


template <class Key, class Value>
class AVLTree : public BinarySearchTree<Key, Value>
{
public:
    virtual void insert (const std::pair<const Key, Value> &new_item); // TODO
    virtual void remove(const Key& key);  // TODO
protected:
    virtual void nodeSwap( AVLNode<Key,Value>* n1, AVLNode<Key,Value>* n2);

    // Add helper functions here

    // Helper for the insert function 
    AVLNode<Key, Value>* insertHelper(AVLNode<Key, Value>* current, AVLNode<Key, Value>* parentNode, const std::pair<const Key, Value>& item);

    // Rebalancing overarching class with help from rotate left and rotate right
    void rebalance(AVLNode<Key, Value>* current);

    // Calculation methods to reduce code lines for height and the balance
    int calculateHeight(AVLNode<Key, Value>* node) const;
    int calculateBalance(AVLNode<Key, Value>* node) const;

    // Rotate helper functions
    void rotateLeft(AVLNode<Key, Value>* node);
    void rotateRight(AVLNode<Key, Value>* node);
};

/*
 * Recall: If key is already in the tree, you should 
 * overwrite the current value with the updated value.
 */
template<class Key, class Value>
void AVLTree<Key, Value>::insert (const std::pair<const Key, Value>& item)
{
    // If there is no existing root of the tree, make it the root, with no parent
    if (this->root_ == nullptr) {

        this->root_ = new AVLNode<Key, Value>(item.first, item.second, nullptr);

    }
    else {
        
        // Use the recursive helper to find where to put it
        insertHelper(static_cast<AVLNode<Key, Value>*>(this->root_), nullptr, item);

    }


}

/*
 * Recall: The writeup specifies that if a node has 2 children you
 * should swap with the predecessor and then remove.
 */
template<class Key, class Value>
void AVLTree<Key, Value>:: remove(const Key& key)
{
    // TODO

    // Use internal find to find which node it is based on the key value pair
    AVLNode<Key, Value>* nodeToRemove = static_cast<AVLNode<Key, Value>*>(this->internalFind(key));

    if (nodeToRemove == nullptr) {

        return;
    }

    // The child of the new node
    AVLNode<Key, Value>* x = nullptr;

    // if they have both children, swap im
    if (nodeToRemove->getLeft() && nodeToRemove->getRight()) {

        AVLNode<Key, Value>* pred = static_cast<AVLNode<Key, Value>*>(this->predecessor(nodeToRemove));
        nodeSwap(nodeToRemove, pred);

    }

    // Picks the child that is non-null if there is only one, or just null otherwise
    AVLNode<Key, Value>* child = nodeToRemove->getLeft() ? nodeToRemove->getLeft() : nodeToRemove->getRight();


    if (child) {

        // If the node has a child, set its parent to the removed nodes parent
        child->setParent(nodeToRemove->getParent());

        // if its the root, make the child the new root
        if (!nodeToRemove->getParent()) {

            this->root_ = child;
        }

        else if (nodeToRemove == nodeToRemove->getParent()->getLeft()) {

            // If the node is the left chid, set it to be left child
            nodeToRemove->getParent()->setLeft(child);
        }
        else {
            // If the node is the right chid, set it to be right child
            nodeToRemove->getParent()->setRight(child);
        }

         x = child->getParent();
    }

    // If its a leaf node (no children)
    else {
    
        // The fixed node becomes the parent of the old node
        x = nodeToRemove->getParent();

        // If the parent is null then its the root so make it root
        if(!nodeToRemove->getParent()){

            this->root_=nullptr;
        }

        // if its a left child then delete the left child of its parent (the node to remove)
        else if (nodeToRemove == nodeToRemove->getParent()->getLeft()){
            nodeToRemove->getParent()->setLeft(nullptr);
        }

        // if its a right child then delete the right child of its parent (the node to remove)
        else {
            nodeToRemove->getParent()->setRight(nullptr);
        }




    }

       
    // delete it
    delete nodeToRemove;

    // Still need to rebalance the x node
    while(x) {

        rebalance(x);
        x = x->getParent();

    }


}


template<class Key, class Value>
void AVLTree<Key, Value>::nodeSwap( AVLNode<Key,Value>* n1, AVLNode<Key,Value>* n2)
{
    BinarySearchTree<Key, Value>::nodeSwap(n1, n2);
    int8_t tempB = n1->getBalance();
    n1->setBalance(n2->getBalance());
    n2->setBalance(tempB);
}

template<class Key, class Value>
int AVLTree<Key, Value>::calculateHeight(AVLNode<Key, Value>* node) const {

  // If there is no node left (base case) return 0
  if (!node) {
    return 0;
  }


  // Uses recursion to find the max height of the tree from the given node, adding 1 for the current node
  return 1 + std::max(calculateHeight(node->getLeft()), calculateHeight(node->getRight()));

}

template<class Key, class Value>
int AVLTree<Key, Value>::calculateBalance(AVLNode<Key, Value>* node) const {

  // If there is no node left (base case) return 0
  if (!node) {
    return 0;
  }

  // Balance is equal to left node - right node
  return calculateHeight(node->getLeft()) - calculateHeight(node->getRight());

}

template<class Key, class Value>
void AVLTree<Key, Value>::rotateLeft(AVLNode<Key, Value>* oldRoot) {

    // Since it's left rotation, grab the right child node
    AVLNode<Key, Value>* newRoot = oldRoot->getRight();

    // The left subtree of m becomes the new right subtree of x
    oldRoot->setRight(newRoot->getLeft());

    // If the new Root has a left subtree, the old root becomes the parent of it
    if (newRoot->getLeft()) {

        newRoot->getLeft()->setParent(oldRoot);
    }

    // Make m and n have the same parent so we do not lose track of m
    newRoot->setParent(oldRoot->getParent());

    // If the old root is the root of the entire AVL tree (corner)
    if(!oldRoot->getParent()) {

        this->root_ = newRoot;
    }

    // If the old root was the left child, set the new root to be the left child
    else if (oldRoot==oldRoot->getParent()->getLeft()){

        oldRoot->getParent()->setLeft(newRoot);
    }

    // If the old root was the right child, set the new root to be the right child
    else {
         oldRoot->getParent()->setRight(newRoot);
    }
    
    // The new root now becomes the parent of the old root, on the left since its bigger
    newRoot->setLeft(oldRoot);

    // Connect the old root node to the new parent
    oldRoot->setParent(newRoot);


    // Need to update the balance of the new Nodes and trees
    oldRoot->setBalance(calculateBalance(oldRoot));
    newRoot->setBalance(calculateBalance(newRoot));
}

template<class Key, class Value>
void AVLTree<Key, Value>::rotateRight(AVLNode<Key, Value>* oldRoot) {

    // Since it's right rotation, grab the left child node
    AVLNode<Key, Value>* newRoot = oldRoot->getLeft();

    // The left subtree of m becomes the new right subtree of x
    oldRoot->setLeft(newRoot->getRight());

    // If the new Root has a left subtree, the old root becomes the parent of it
    if (newRoot->getRight()) {

        newRoot->getRight()->setParent(oldRoot);
    }

    // Make newRoot and oldRoot have the same parent so we do not lose track of m
    newRoot->setParent(oldRoot->getParent());

    // If the old root is the root of the entire AVL tree (corner)
    if(!oldRoot->getParent()) {

        this->root_ = newRoot;
    }

    // If the old root was the left child, set the new root to be the left child
    else if (oldRoot==oldRoot->getParent()->getLeft()){

        oldRoot->getParent()->setLeft(newRoot);
    }

    // If the old root was the right child, set the new root to be the right child
    else {
         oldRoot->getParent()->setRight(newRoot);
    }
    
    // The new root now becomes the parent of the old root, on the right since its smaller
    newRoot->setRight(oldRoot);

    // Connect the old root node to the new parent
    oldRoot->setParent(newRoot);

    // Need to update the balance of the new Nodes and trees
    oldRoot->setBalance(calculateBalance(oldRoot));
    newRoot->setBalance(calculateBalance(newRoot));
}

// Code for rebalancing a tree after removing/inserting
template<class Key, class Value>
void AVLTree<Key, Value>::rebalance(AVLNode<Key, Value>* current) {

    // First order of business, corner case, if current node doesnt exist
    if (!current){
        
        return;
    }

    // Make an int for balance which is the current balance
    int balance = calculateBalance(current);

    // If the balance is bigger than one then it's a left tree case
    if (balance > 1) {

        // If the balance of the left child is smaller than 0, then it is a left right case
        if (calculateBalance(current->getLeft()) < 0){
            // In left right cases, rotate left then right, first with the left subtree
            rotateLeft(current->getLeft());

        }
        
        // Otherwise just rotate right from the pivot
        rotateRight(current); 

    }

    // Right cases
    else if ( balance < -1) {

        // If the case is a right left case, also rotate right first
        if (calculateBalance(current->getRight()) > 0) {

            rotateRight(current->getRight());

        }
        // Otherwise just rotate left
        rotateLeft(current);

    }
}

// Helper function that helps map out the actions of insert through recurison
template<class Key, class Value>
AVLNode<Key, Value>* AVLTree<Key, Value>::insertHelper(AVLNode<Key, Value>* node, AVLNode<Key, Value>* parent, const std::pair<const Key, Value>& keyValuePair) {

    // Base case
    // if the node is empty, then it is the right place to insert the new key and value
    if (node == nullptr) {
        // Inserts the new key and value
        AVLNode<Key, Value>* newNode = new AVLNode<Key, Value>(keyValuePair.first, keyValuePair.second, parent);
        
        // Attatch the new node to the tree
        // If the ke is smaller than the parents key then put on left, otherwise put on the right of the parent
        if (keyValuePair.first < parent->getKey()) {
            parent->setLeft(newNode);
        }
        else{
            parent->setRight(newNode);
        }

        // Make sure to rebalance 
        rebalance(parent);

        // Returns the node, this should end the recursion, leading to actual insert code
        return newNode;
    }

    // Recursive section 

    // If the given key is equal to the current key, then replace the value
    if (keyValuePair.first == node->getKey()) {

        node->setValue(keyValuePair.second);
    }

    // If the give key is smaller, then you go to the next node on the left
    else if (keyValuePair.first < node->getKey()) {
        insertHelper(node->getLeft(), node, keyValuePair);
    }

    // If the given key is larger, then go to the node on the right
    else {
        insertHelper(node->getRight(), node, keyValuePair);
    }


    // Code for when it is working backwards:

    rebalance(node);
    return node;


}

#endif
