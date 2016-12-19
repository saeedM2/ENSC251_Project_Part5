//============================================================================
//
// ***** work on this file is optional, and might only be necessary for bonus work *****
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
//% * Enter the above information in tokenClasses.hpp  too.
//% * Edit the "Helpers" line and "Resources" line.
//% * Your group name should be "P5_<userid1>_<userid2>" (eg. P5_stu1_stu2)
//% * Form groups as described at:  https://courses.cs.sfu.ca/docs/students
//% * Submit files to courses.cs.sfu.ca
//
// Name        : tokenClasses.cpp
// Description : Course Project Part 5 (Interpreting/Evaluation)
// Original portions Copyright (c) 2016 School of Engineering Science, Simon Fraser University
//============================================================================

#include "tokenClasses.hpp"
#include <memory>

// I'm not sure if next line works with other than GNU compiler
#include <cxxabi.h>

using namespace std;

namespace ensc251 {

unsigned int tracker = 0; // tracker index
std::vector<Token*> tokenObjectPs; // objects for abstract syntax tree

std::map<std::string, varInfo > evaluation_map;
std::string empty = "";

char *demangle(const char *typeName) {
	int status;
	// I'm not sure if the below line works with compilers other than GNU
	return abi::__cxa_demangle(typeName, 0, 0, &status);
}

void print_evaluation_map()
{
	OUT<<std::endl<<"-----Evaluation Map-----"<< std::endl;
	for(auto it = evaluation_map.cbegin(); it != evaluation_map.cend(); ++it)
	{
		 OUT << it->first << "     =     " << it->second.sptc->getStringValue()  << std::endl;
	}
	OUT << std::endl;
}

Token* postfix_exp()
{
	auto subTreeP = tokenObjectPs[tracker]->process_primary_exp();
	if (subTreeP) {
		Token* tObjP;
		while((tObjP = tokenObjectPs[tracker]->process_postfix_operator()))
		{
				tObjP->add_childP(subTreeP);
				subTreeP = tObjP;
		}
	}
	return subTreeP;
}

Token* unary_exp()
{
	int old_tracker = tracker;

	Token* tObjP;

	if(!(tObjP = tokenObjectPs[tracker]->process_unary()))
	{
		tracker = old_tracker;
		tObjP = postfix_exp();
	}
	return tObjP;
}

Token* div_exp()
{
	Token* ueTreeP = nullptr;
	Token* lowerNodeP = nullptr;
	Token* upperNodeP = nullptr;
	if ((lowerNodeP = unary_exp())) {
		while ((upperNodeP = tokenObjectPs[tracker]->advance_past_div_operator()))
		{
			if(!(ueTreeP = unary_exp()))
				return nullptr;
			upperNodeP->add_childP(lowerNodeP);
			upperNodeP->add_childP(ueTreeP);
			lowerNodeP = upperNodeP;
		}
	}
	return lowerNodeP;
}

Token* additive_exp()
{
	Token* deTreeP = nullptr;
	Token* lowerNodeP = nullptr;
	Token* upperNodeP = nullptr;
	if ((lowerNodeP = div_exp())) {
		while ((upperNodeP = tokenObjectPs[tracker]->advance_past_additive_operator()))
		{
			if(!(deTreeP = div_exp()))
				return nullptr;
			upperNodeP->add_childP(lowerNodeP);
			upperNodeP->add_childP(deTreeP);
			lowerNodeP = upperNodeP;
		}
	}
	return lowerNodeP;
}

Token* shift_exp()
{
	Token* subTreeP = nullptr;
	Token* lowerNodeP = nullptr;
	Token* upperNodeP = nullptr;
	if ((lowerNodeP = additive_exp())) {
		while ((upperNodeP = tokenObjectPs[tracker]->advance_past_shift_operator()))
		{
			if(!(subTreeP = additive_exp()))
				return nullptr;
			upperNodeP->add_childP(lowerNodeP);
			upperNodeP->add_childP(subTreeP);
			lowerNodeP = upperNodeP;
		}
	}
	return lowerNodeP;
}

Token* ternary_exp()
{
	int old_tracker = tracker;
	Token* subTreeP;

	if((subTreeP = tokenObjectPs[tracker]->process_id()))
	{
		Token* tObjP;
		if((tObjP = tokenObjectPs[tracker]->advance_past_conditional_operator()))
		{
			tObjP->add_childP(subTreeP);
			subTreeP = assignment_exp();
			if(subTreeP)
			{
				tObjP->add_childP(subTreeP);
				if(tokenObjectPs[tracker]->process_punctuator(":"))
				{
					subTreeP = ternary_exp();
					if(subTreeP)
					{
						tObjP->add_childP(subTreeP);
						return tObjP;
					}
				}
			}
			return nullptr;
		}
	}
	tracker = old_tracker;
	subTreeP = shift_exp();
	if(subTreeP)
		return subTreeP;
	else
		return nullptr;
}

Token* assignment_exp()
{
	int old_tracker = tracker;

	// production: id assignment_operator assignment_exp
	Token* subTreeP = tokenObjectPs[tracker]->process_id();
	if(subTreeP)
	{
		if(Token* tObjP = tokenObjectPs[tracker]->advance_past_assignment_operator())
		{
			tObjP->add_childP(subTreeP);
			subTreeP = assignment_exp();
			if(subTreeP)
			{
				tObjP->add_childP(subTreeP);
				return tObjP;
			}
			else
				// we have assignment operator, but not valid assignment expression,
				//	so there must be an error and it should be reported.
				return nullptr;
		}
		// no assignment operator, so this is not the first production

		// start production:  ternary_exp
		if(tokenObjectPs[tracker]->has_string_value(";"))
			// we have already built a unary expression subtree, which satisfies the ternary_exp production.
			return subTreeP;
	}

	// production:  ternary_exp
	tracker = old_tracker;
	subTreeP = ternary_exp();
	if(subTreeP)
		return subTreeP;
	else
		return nullptr;
}

Token* stat()
{
	int old_tracker = tracker;
	if(auto subTreeP = tokenObjectPs[tracker]->process_declaration())
		return subTreeP;
	else
	{
		tracker = old_tracker;
		return assignment_exp();
	}
}

// because the below function deals with ";", it is a bit different than the pattern seen elsewhere.
Token* stat_list()
{
	Token* statTreeP = nullptr;
	Token* lowerNodeP = nullptr;
	Token* upperNodeP = nullptr;
	if ((lowerNodeP = stat())) {

		while (tokenObjectPs[tracker]->has_string_value(";")) {
			tracker++;
			if (tracker == tokenObjectPs.size())
				return lowerNodeP;
			upperNodeP = tokenObjectPs[tracker - 1];

			if(!(statTreeP = stat()))
				return nullptr;

			upperNodeP->add_childP(lowerNodeP);
			upperNodeP->add_childP(statTreeP);
			lowerNodeP = upperNodeP;
		}
		return nullptr;

	}
	return nullptr;
}

Token* recursive_parser()
{
	if (tokenObjectPs.size())
		return stat_list();
	else
		return nullptr;
}

shared_ptr_to_constant
type_spec::
eval()
{
	evaluation_map[this->getChildToken(0)->get_string_value()].sptc = nullptr; // storing id value as nullptr in map
	evaluation_map[this->getChildToken(0)->get_string_value()].ts = *this;
	return shared_ptr_to_constant();
};

}

