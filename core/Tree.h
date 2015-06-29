/**
 * file: Tree.h
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

#include <cstdlib>
#include <map>
#include <list>

#include <iostream>

namespace mc 
{
    class Node {
    public:
        Node() : mValue(-1), mDepth(-1), mParent(0) { }
        Node(long int value, Node * parent=0);
        Node(const Node & orig);
        ~Node(); 

        long int & value() { return this->mValue; }
        long int & depth() { return this->mDepth; }
        Node * parent() { return this->mParent; }
        Node * first();
        Node * last();
        Node * prev() { return this->mPrev; }
        Node * next() { return this->mNext; }
        std::list<Node *> & children() { return this->mChildren; }

        void setParent( Node * node );
        bool setPrev( Node * node );
        bool setNext( Node * node );

        bool leaf() { return this->mChildren.empty(); }
        bool isroot() { return 0 == this->parent(); }
        size_t size() { return this->mChildren.size(); }
        
        Node * append(long int value);
        void append(Node * node);
        bool remove(Node * node);
        void clear();

        void print();

    private:
        long int mValue;
        long int mDepth;
        Node * mParent;
        Node * mPrev;
        Node * mNext;
        std::list<Node *> mChildren;
    };

    typedef std::map<long int, Node *>::iterator tree_iterator;

    class Tree {
    public:
        Tree() : mRoot(0), mMaxDepth(-1) { }
        Tree(long int value);
        ~Tree();

        Node * root() { return this->mRoot; }
        void root(long int value);

        size_t size() { return mTreeMap.size(); }
        bool empty() { return 0 == mRoot; }

        bool has(long int value) { return mTreeMap.count(value) == 1; }
        Node * find(long int value);
        Node * first(long int value); // first child
        Node * last(long int value); // last child
        Node * next(long int value); // next sibling
        Node * prev(long int value); // previous sibling

        long int depth(long int value);
        long int maxDepth();

        bool append(long int parent, long int child);
        void clear();
        long int computeMaxDepth();
        bool remove(long int value);
        bool remove(Node * node);
        bool removeChildren(long int value);
        bool removeChildren(Node * node);

        void print();

        tree_iterator begin() { return mTreeMap.begin(); }
        tree_iterator end() { return mTreeMap.end(); }
        
    private:
        void cleanup();
        void cleanup(Node * node);
        void cleanupChildren(Node * node);
        std::list<long int> removed;
        // this map is just used to accelerate some functions
        // and have an iterator across the tree (note that the
        // iterator does not follow branches or anything, it just
        // follow the map (i.e. ordered according to the "key")
        std::map<long int,Node *> mTreeMap;
        // this is the actual tree
        Node * mRoot;
        long int mMaxDepth;
    };

}
