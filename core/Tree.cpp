/**
 * file: Tree.cpp
 * author: Alexis Moinet
 *
 * @copyright (c) 2015 – UMONS - Numediart
 * 
 * MediaCycle of University of Mons – Numediart institute is 
 * licensed under the GNU AFFERO GENERAL PUBLIC LICENSE Version 3 
 * licence (the “License”); you may not use this file except in compliance 
 * with the License.
 * 
 * This program is free software: you can redistribute it and/or 
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 * 
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 * 
 * Each use of this software must be attributed to University of Mons – 
 * Numediart Institute
 * 
 * Any other additional authorizations may be asked to avre@umons.ac.be 
 * <mailto:avre@umons.ac.be>
 */

#include "Tree.h"

using namespace mc;

/*
 * Node
 */

Node::Node( long int value, Node * parent )
{
    this->mValue = value;
    this->setParent(parent);
    this->mPrev = 0;
    this->mNext = 0;
}

Node::Node( const Node & orig )
{
    this->mValue = orig.mValue;
    this->mChildren = orig.mChildren;
    this->mParent = orig.mParent;
    this->mDepth = orig.mDepth;
    this->mPrev = orig.mPrev;
    this->mNext = orig.mNext;
}

Node::~Node()
{
    if (!this->leaf()) {
        std::list<Node *>::iterator child = this->children().begin();
        while (child != this->children().end()) {
            delete (*child);
            ++child;
        }
    }
}

void Node::setParent( Node * node )
{
    this->mParent = node;
    
    if (this->mParent)
        this->mDepth = this->mParent->depth() + 1;
    else
        this->mDepth = 0; // root of the tree
}

bool Node::setPrev( Node * node )
{
    if (node && (node->parent() != this->parent() || node->depth() != this->depth())) {
        std::cerr << "Node::setPrev: error: parent or depth is not the same" << std::endl;
        return false;
    } else {
        this->mPrev = node;
        return true;
    }
}

bool Node::setNext( Node * node )
{
    if (node && (node->parent() != this->parent() || node->depth() != this->depth())) {
        std::cerr << "Node::setNext: error: parent or depth is not the same" << std::endl;
        return false;
    } else {
        this->mNext = node;
        return true;
    }
}

Node * Node::first()
{
    if (this->leaf())
        return 0;
    else
        return this->children().front();
}

Node * Node::last()
{
    if (this->leaf())
        return 0;
    else
        return this->children().back();
}

Node * Node::append( long int value )
{
    Node * node = new Node(value,this);

    // this should be here instead of Node() because we may want a prepend some day
    if (!this->leaf()) {
        node->setPrev(this->children().back());
        this->children().back()->setNext(node);
    }

    this->mChildren.push_back(node);
    return node;
}

void Node::append( Node * node )
{
    if (!node)
        return;

    node->setParent(this);
    
    // this should be here instead of Node() because we may want a prepend some day
    if (!this->leaf()) { // if this is not a leaf, then node is not the first child
        this->last()->setNext(node);
        node->setPrev(this->last());
        node->setNext(0);
    } else { // first child of this node
        node->setPrev(0);
        node->setNext(0);
    }

    this->mChildren.push_back(node);
}

bool Node::remove( Node * node )
{
    std::list<Node *>::iterator it = this->mChildren.begin();

    while (it != this->mChildren.end()) {
        if (*it == node) {
            this->mChildren.erase(it);

            if ( node->prev() )
                node->prev()->setNext( node->next() );

            if ( node->next() )
                node->next()->setPrev( node->prev() );

            delete node;

            return true;
        }
        ++it;
    }

    return false;
}

void Node::clear()
{

    std::list<Node *>::iterator it = this->mChildren.begin();

    while (it != this->mChildren.end()) {
        delete (*it);
        ++it;
    }

    this->mChildren.clear();
}

void Node::print()
{
    int rootdepth = this->depth();

    std::cout << "m=" << this->value() << std::endl;

    mc::Node * current;
    // will return 0 if no child.
    current = this->first();

    while(current) {
        for(int i=0; i < current->depth() - rootdepth; ++i)
            std::cout << "  ";

        std::cout << "m=" << current->value() << std::endl;

        // next
        if (!current->leaf()) {
            current = current->children().front();
        } else {
            if (current->next()) {
                current = current->next();
            } else {
                while (current != this && !current->next()) {
                    current = current->parent();
                }

                if (current) {
                    if (current == this)
                        break;

                    current = current->next();
                }
            }
        }
    }
}

/*
 * Tree
 */

Tree::Tree( long int value )
{
    this->mRoot = 0;
    this->root(value);
}

Tree::~Tree()
{
    this->clear();
}

void Tree::root( long int value )
{
    this->clear();

    this->mRoot = new Node(value);
    this->mMaxDepth = 0;

    this->mTreeMap.insert( std::make_pair(value,this->mRoot) );
}

bool Tree::append( long int parent, long int child )
{
    if ( this->has(child) ) {
        //already in the Tree
        return false;
    }

    Node * p = find(parent);
   
    if (!p) {
        //parent doesn't exist
        return false;
    }

    Node * c = p->append(child);
    this->mTreeMap[child] = c;

    if (c->depth() > this->mMaxDepth)
        this->mMaxDepth = c->depth();
    
    return true;
}

void Tree::clear()
{
    this->remove(this->mRoot);
    this->mRoot = 0;
    this->mMaxDepth = -1;
}

// TODO reimplement this to travel across the actual tree and check
//      whether a value exists or not. If it does not exist (which
//      should *never* happen anyway), remove it from mTreeMap.
void Tree::cleanup()
{
    tree_iterator node;
    std::list<long int>::iterator it = this->removed.begin();

    while (it != this->removed.end()) {
        node = this->mTreeMap.find(*it);
        if (node != this->mTreeMap.end())
            this->mTreeMap.erase(node);

        ++it;
    }

    this->removed.clear();

    this->computeMaxDepth();
}

long int Tree::computeMaxDepth()
{
    //TODO find a better way to do this
    this->mMaxDepth = -1;

    tree_iterator node = this->mTreeMap.begin();
    while (node != this->mTreeMap.end()) {
        if (node->second->depth() > this->maxDepth())
            this->mMaxDepth = node->second->depth();
        ++node;
    }

    return this->mMaxDepth;
}

void Tree::cleanup(Node * node)
{
    this->cleanupChildren(node);
    this->mTreeMap.erase(node->value());
}

void Tree::cleanupChildren(Node * node)
{
    if (!node->leaf()) {
        Node * child = node->first();
        Node * next;
        do {
            next = child->next();
            this->cleanup(child);
            child = next;
        } while (child);
    }
}

Node * Tree::find( long int value )
{
    tree_iterator it;
    it = this->mTreeMap.find(value);
    if (it != this->mTreeMap.end()) {
        return it->second;
    } else {
        return 0;
    }
}

Node * Tree::first( long int value )
{
    Node * node = find(value);
    if (node) {
        return node->first();
    } else {
        return 0;
    }
}

Node * Tree::last( long int value )
{
    Node * node = find(value);
    if (node) {
        return node->last();
    } else {
        return 0;
    }
}

Node * Tree::next( long int value )
{
    Node * node = find(value);

    if (node)
        return node->next();
    else
        return 0;
}

Node * Tree::prev( long int value )
{
    Node * node = find(value);

    if (node)
        return node->prev();
    else
        return 0;
}

long int Tree::depth( long int value )
{
    Node * node = find(value);

    if (node) {
        return node->depth();
    } else {
        return -1;
    }
}

long int Tree::maxDepth()
{
    return this->mMaxDepth;
}

bool Tree::remove( long int value )
{
    Node * node = this->find(value);

    return this->remove(node);
}

bool Tree::remove( Node * node )
{
    if (!node)
        return false;

    // remove from mTreeMap 
    // nb: recursive
    this->cleanup(node);

    Node * parent = node->parent();

    // it the Node has a parent, we need to tell it that the Node is going away
    // might as well make the memory deallocation since the parent allocated it
    // in the first place ... (aka "The Parent giveth and the Parent taketh away")
    // nb: recursive as well
    if (parent) {
        parent->remove(node);
    } else { // root
        delete node;
    }
    
    // maybe we removed the lowest element ?
    // TODO optimize this and recompute only as needed in the cleanup step
    this->computeMaxDepth();

    return true;
}

bool Tree::removeChildren( long int value )
{
    Node * node = this->find(value);

    return this->removeChildren(node);
}

bool Tree::removeChildren( Node * node )
{
    if (!node)
        return false;

    // this is just to cleanup mTreeMap
    this->cleanupChildren(node);

    // delete all children of this Node
    node->clear();

    this->computeMaxDepth();

    return true;
}

void Tree::print()
{
    this->mRoot->print();
}

