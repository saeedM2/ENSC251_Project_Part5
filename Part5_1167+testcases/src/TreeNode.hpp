//============================================================================
//
//% Student Name 1: student1
//% Student 1 #: 123456781
//% Student 1 userid (email): stu1 (stu1@sfu.ca)
//
//% Student Name 2: student2
//% Student 2 #: 123456782
//% Student 2 userid (email): stu2 (stu2@sfu.ca)
//
//% Below, edit to list any people who helped you with the code in this file,
//%      or put ‘none’ if nobody helped (the two of) you.
//
// Helpers: _everybody helped us/me with the assignment (list names or put ‘none’)__
//
// Also, list any resources beyond the course textbook and the course pages on Piazza
// that you used in making your submission.
//
// Resources:  ___________
//
//%% Instructions:
//% * Put your name(s), student number(s), userid(s) in the above section.
//% * Enter the above information in Directory.hpp too.
//% * Edit the "Helpers" line and "Resources" line.
//% * Your group name should be "P3_<userid1>_<userid2>" (eg. P3_stu1_stu2)
//% * Form groups as described at:  https://courses.cs.sfu.ca/docs/students
//% * Submit one or two files to courses.cs.sfu.ca
//
// Name        : TreeNode.hpp
// Description : Course Project Part 3 (Directory Tree)
//============================================================================

#ifndef SRC_TREENODE_HPP_
#define SRC_TREENODE_HPP_

#include <vector>

//#define USE_OSS

#ifndef USE_OSS
#include <iostream>
#define OUT std::cout

#else
#include <sstream>
extern std::ostringstream oss;
#define OUT oss
#endif

namespace ensc251 {
	// using namespace std;

	static unsigned count = 0;

	class TreeNode;

	typedef TreeNode* (TreeNode::*action_func)(void);
	typedef void (TreeNode::*traverse_func)(const TreeNode*);

	class TreeNode
	{
		typedef std::vector<TreeNode*> TreeNodePVect;

		protected:
			TreeNodePVect childPVector;

		public:
			TreeNode() {};
			TreeNode(const TreeNode& nodeToCopy) { 
				for (auto childP: nodeToCopy.childPVector) {
					add_childP(childP->clone());
				}
			}

			virtual TreeNode* clone() const { return new TreeNode(*this); };

			virtual ~TreeNode() {
				OUT << "Destroying TreeNode with " << childPVector.size() << " children."<< std::endl;
				for (auto child : childPVector)
					delete child;
			}

		    void swap(TreeNode & other) // the swap member function (should never fail!)
		    {
		        std::swap(childPVector, other.childPVector);
		    }

		    TreeNode & operator = (TreeNode other) // note: argument passed by value, copy made!
		    {
		        swap(other); // swap *this with other
		        return *this; // by convention, always return *this
		    }// swapped other (original *this) is destroyed, releasing the memory

			void add_childP(TreeNode* child) { childPVector.push_back(child); }
			void add_children(const TreeNodePVect& childPV) { childPVector.insert(childPVector.end(), childPV.begin(), childPV.end()); }
			const TreeNodePVect& get_children() const { return childPVector; }

			TreeNode* traverse_children_post_order(traverse_func tf, action_func af)
			{
			    for(auto childP : childPVector) {
			        (childP->*tf)(this); // traverse child's children using tf
			    }
			    return (this->*af)();
			}

			TreeNode* traverse_children_pre_order(traverse_func tf, action_func af)
			{
				(this->*af)();
				for(auto childP : childPVector)
				{
					(childP->*tf)(this);
				}
				return nullptr;
			}

			TreeNode* traverse_children_in_order(traverse_func tf, action_func af)
			{
				if(childPVector.size() != 2)
				{
					OUT << "Error -- can only do inorder traversal on a node with 2 children" << std::endl;
					return nullptr;
				}
				(childPVector[0]->*tf)(this);
				(this->*af)();
				(childPVector[1]->*tf)(this);
				return nullptr;
			}

			TreeNode* count_action()
			{
				count++;
				return nullptr;
			}

			void count_traverse(const TreeNode*)
			{
				traverse_children_post_order(&TreeNode::count_traverse, &TreeNode::count_action);
			}
			void reset_count() { count = 0; }
			unsigned get_count() const { return count; }

	};
}

#endif /* SRC_TREENODE_HPP_ */
