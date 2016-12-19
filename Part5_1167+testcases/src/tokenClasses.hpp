//============================================================================
//
//% Student Name 1: Saeed Molaie
//% Student 1 #:301189698
//% Student 1 userid (email): smolaie@sfu.ca
//
//% Student Name 2: Gabe Teeger
//% Student 2 #: 301261001
//% Student 2 userid (email): gteeger@sfu.ca
//
// Helpers: Mohammad Akbari (TA) and Vivekkumar Patel (TA)
// Name        : Part4.cpp and Part4.hpp
// Description : Course Project Part 4 (tokenClasses AST)
//
// Also, list any resources beyond the course textbook and the course pages on Piazza
// that you used in making your submission.
//
// Resources:  ______N/A_____
//
//%% Instructions:
//% * Put your name(s), student number(s), userid(s) in the above section.
//% * Enter the above information in tokenClasses.cpp  too.
//% * Edit the "Helpers" line and "Resources" line.
//% * Your group name should be "P5_<userid1>_<userid2>" (eg. P5_stu1_stu2)
//% * Form groups as described at:  https://courses.cs.sfu.ca/docs/students
//% * Submit files to courses.cs.sfu.ca
//
// Name        : tokenClasses.hpp
// Description : Course Project Part 5 (Interpreting/Evaluation)
// Original portions Copyright (c) 2016 School of Engineering Science, Simon Fraser University
//============================================================================

#ifndef TOKENCLASSES_HPP_
#define TOKENCLASSES_HPP_

#include <string>
#include <vector>
#include <iostream>
#include <typeinfo>
#include <map>
#include <memory>
#include <exception>
#include <stdexcept>

#include "TreeNode.hpp"

namespace ensc251 {

char *demangle(const char *typeName);

class Token;
class constant;

typedef std::shared_ptr<constant> shared_ptr_to_constant;

extern std::vector<Token*> tokenObjectPs; // objects for abstract syntax tree
extern unsigned int tracker; // tracker index

extern std::string empty;

void print_evaluation_map();

class Token: public TreeNode {
	// The Base Token Class
public:

	virtual bool has_string_value(const std::string& aString) {
		return false;
	}
	;
	virtual const std::string& get_string_value() {
		return empty;
	}
	;

	void print_ast() {
		print_traverse(nullptr);
		OUT<< ";" << std::endl;}

	friend std::ostream& operator <<(std::ostream& outs, const Token& token) {
		token.print(outs);
		return outs;
	}

	void print(std::ostream& str) const {str << demangle(typeid(*this).name()) << ": ";}

	//Here we declare the virtual member functions that will be overidden depending on the object they operate on
	virtual Token* process_punctuator(const std::string &puncValue) {return nullptr;}
	virtual Token* process_declaration() {return nullptr;}
	virtual Token* process_id() {return nullptr;}
	virtual Token* process_unary() {return nullptr;}
	virtual Token* process_primary_exp() {return nullptr;}
	virtual Token* process_postfix_operator() {return nullptr;}
	virtual Token* advance_past_conditional_operator() {return nullptr;}
	virtual Token* advance_past_shift_operator() {return nullptr;}
	virtual Token* advance_past_additive_operator() {return nullptr;}
	virtual Token* advance_past_div_operator() {return nullptr;}
	virtual Token* advance_past_assignment_operator() {return nullptr;}

	virtual shared_ptr_to_constant eval() {return nullptr;}; // should this be pure virtual?

	Token* getChildToken(unsigned childNum) const {return static_cast<Token*>(this->childPVector[childNum]);}
	shared_ptr_to_constant evalChildToken(unsigned childNum) /* const */{return getChildToken(childNum)->eval();}

	Token* process_token()
	{
		tracker++;
		if (tracker < tokenObjectPs.size()) {
			return this;
		}
		else
		// our grammar must end in ';'. If *this is the end
		//  then it is a premature end which we will report now.
		return nullptr;
	}

	virtual TreeNode* print_action() {return nullptr;};

	virtual void print_traverse(const TreeNode*)
	{
		if(childPVector.size() > 2)
		{
			traverse_children_post_order(
					static_cast<ensc251::traverse_func>(print_traverse),
					static_cast<ensc251::action_func>(print_action));
		}
		else if(childPVector.size() == 2)
		{
			traverse_children_in_order(
					static_cast<ensc251::traverse_func>(print_traverse),
					static_cast<ensc251::action_func>(print_action));
		}
		else
		{
			traverse_children_pre_order(
					static_cast<ensc251::traverse_func>(print_traverse),
					static_cast<ensc251::action_func>(print_action));
		}
	}

	virtual ~Token() {}

};
//Below we define preprocessor macros to automate the process of overloading the basic operators
//such as add, sub, and div.This method is much more concise.
#define MULTI_OP_IFACE(RNAME, OP, TYPE) \
		virtual shared_ptr_to_constant RNAME(TYPE v) = 0;

#define NUMERIC_OP_IFACE(NAME, OP) \
		MULTI_OP_IFACE(r##NAME, OP, float) \
		MULTI_OP_IFACE(r##NAME, OP, int) \
		MULTI_OP_IFACE(r##NAME, OP, char) \
		\
		virtual shared_ptr_to_constant NAME(constant& obj) = 0;\
		virtual shared_ptr_to_constant operator OP (constant& obj) = 0;

template<typename U>
class numeric_const;

class constant: public Token {
	//We use the constant class to define the pure virtual member functions that are implemented in the numeric_constant class.
	//Note numeric class inherets from the class constant. we also implement div, add, sub macros in order to make calculations easier
	// subclasses: numeric_const<T>
public:

	/* ***** this class needs work including for multiple operations supported on constants such as postfix operations, addition, subtraction, shift. */

	virtual shared_ptr_to_constant putValueIntoInt() = 0;
	virtual shared_ptr_to_constant putValueIntoChar() = 0;
	virtual shared_ptr_to_constant putValueIntoFloat() = 0;

	virtual std::string getStringValue() = 0;
	virtual bool isNotZero() = 0;

	//preprocessor macros used to automate these three operation. We cant use
	//template functions since template functions cannot be virtual
	NUMERIC_OP_IFACE(div, /)
	NUMERIC_OP_IFACE(add, +)
	NUMERIC_OP_IFACE(sub, -)

	//Here we have defined the pure virtual functions for our operator implementation
	virtual shared_ptr_to_constant complement() = 0;
	virtual shared_ptr_to_constant postfix_increment() = 0;
	virtual shared_ptr_to_constant postfix_decrement() = 0;
	virtual shared_ptr_to_constant mod(int temp) = 0;
	virtual shared_ptr_to_constant RightShift(int temp)=0;
	virtual shared_ptr_to_constant LeftShift(int temp)=0;

	// template function.  See beginning of chapter 17.
	template<typename U>
	shared_ptr_to_constant create(U v) {
		return shared_ptr_to_constant(new numeric_const<U>(v));
	}

	Token* process_primary_exp() {
		return process_token();
	}
};

// the below functions are defined in tokenClasses.cpp
Token* recursive_parser();
Token* assignment_exp();
Token* unary_exp();

class StringBasedToken: public Token {
	// subclasses: id, type_spec, assignment_operator, shift_operator, additive_operator, div_operator, unary_operator, string
protected:
	std::string stringValue;

public:
	StringBasedToken(const std::string &aStringValue) :
			stringValue(aStringValue) {
	}
	;
	void print(std::ostream& str) const {
		Token::print(str);
		str << ": " << stringValue;
	}
	bool has_string_value(const std::string& aString) {
		return stringValue == aString;
	}
	const std::string& get_string_value() {
		return stringValue;
	}
	TreeNode* print_action() {
		OUT<< stringValue << " "; return nullptr;};
};

class punctuator: public StringBasedToken {
	// e.g., ';', '*', '(', ')', ':', etc.
public:
	punctuator(const std::string &puncValue) :
			StringBasedToken(puncValue) {
	}
	;

	void print_traverse(const TreeNode*) {
		if (stringValue == "(") {
			traverse_children_pre_order(
					static_cast<ensc251::traverse_func>(print_traverse),
					static_cast<ensc251::action_func>(print_action));

			OUT<< ") ";
		}
		else
		traverse_children_in_order(
				static_cast<ensc251::traverse_func>(print_traverse),
				static_cast<ensc251::action_func>(print_action));
	}

	TreeNode* print_action()
	{
		StringBasedToken::print_action();
		if (stringValue==";")
		OUT << std::endl;
		return nullptr;
	};

	Token* process_primary_exp()
	{
		if(stringValue == "(" && process_token())
		{
			Token* tObjP = assignment_exp();
			if(tObjP)
			{
				if(tokenObjectPs[tracker]->process_punctuator(")"))
				{
					add_childP(tObjP);
					return this;
				}
			}

		}
		return nullptr;
	}

	Token* process_punctuator(const std::string &puncValue)
	{
		if (puncValue == stringValue)
		return process_token();
		return nullptr;
	}

	shared_ptr_to_constant eval()
	{
		// if sringValue is ";"
		try {
			this->evalChildToken(0);
		}
		catch (const std::exception& e)
		{
			OUT << e.what() << std::endl;
		};
		try {
			this->evalChildToken(1);
		}
		catch (const std::exception& e)
		{
			OUT << e.what() << std::endl;
		};
		return shared_ptr_to_constant();
	}
};

class type_spec: public StringBasedToken {
	// 'char' | 'int' | 'float'
public:
	type_spec() :
			StringBasedToken("int") {
	}
	;
	type_spec(const std::string &typeValue) :
			StringBasedToken(typeValue) {
	}
	;
	Token* process_declaration() {
		Token* subTreeP = process_token();
		Token* tObjP;
		if ((tObjP = tokenObjectPs[tracker]->process_punctuator("*"))) {
			tObjP->add_childP(subTreeP);
			subTreeP = tObjP;
		}

		if ((tObjP = tokenObjectPs[tracker]->process_id())) {
			subTreeP->add_childP(tObjP);
			return subTreeP;
		}
		return nullptr;
	}

	shared_ptr_to_constant ofTypeCreateConstant(
			//This function is pointed by shared pointer (in order to prevent memory leak). The function is tasked with determining the
			//type spec. So if the first character value of *this is 'i' then we create a constant of that type. The same process for char
			//and float types
			shared_ptr_to_constant constToCreateFromSP) {
		switch (this->stringValue[0]) {
		/* ***** this needs work for other supported data types i.e. char and float */
		case 'i': // int
			return constToCreateFromSP->putValueIntoInt();

		case 'c': //char
			return constToCreateFromSP->putValueIntoChar();

		case 'f': //char
			return constToCreateFromSP->putValueIntoFloat();
		}
		return shared_ptr_to_constant();  // should never get here
	}

	shared_ptr_to_constant eval(); // please see definition of this function in tokenClasses.hpp
};

struct varInfo {
	shared_ptr_to_constant sptc;
	type_spec ts;
};

extern std::map<std::string, varInfo> evaluation_map; // This map holds the evaluation results for AST.

class id: public StringBasedToken {
	// identifiers -- example:  sum
public:
	id(const std::string &idValue) :
			StringBasedToken(idValue) {
	}
	;

	Token* process_id() {
		return process_token();
	}
	Token* process_primary_exp() {
		return process_id(); // return process_token();
	}
	shared_ptr_to_constant eval() {
		return evaluation_map[get_string_value()].sptc;
	}
};

class assignment_operator: public StringBasedToken {
	// '=' | '*=' | '/=' | '%=' | '+=' | '-=' | '<<=' | '>>=' | '&=' | '^=' | '|='
public:
	assignment_operator(const std::string &opValue) :
			StringBasedToken(opValue) {
	}
	;
	Token* advance_past_assignment_operator() {
		return process_token();
	}

	/* ***** BONUS MARKS: Evaluation of string assignments would be considered for the bonus points.
	 * for example 's = "Hello world";'  */

	/*
	 * ***** BONUS MARKS: So far we have only considered '=' in evaluation.
	 * You may try implementing  '/=' | '+=' | '-=' | '<<=' | '>>='  to earn BONUS MARKS.
	 *
	 */
	shared_ptr_to_constant eval()
	//Here we implement the assignment operator, which is identicle to the regular assignment operator implemented for base cases.
	//Here we have combined the regular base case assignment operator with the add, sub, div, shift left and right operators. The results
	//are saved inside the map using the map key and map values. The sptc and ts are info about the variables inside a strcuture.
	{
			if (stringValue == "/=") {
				auto temp = evalChildToken(0)->div(*evalChildToken(1));
				return evaluation_map[getChildToken(0)->get_string_value()].sptc =
						evaluation_map[getChildToken(0)->get_string_value()].ts.ofTypeCreateConstant(
								temp);
			}
			if (stringValue == "+=") {
				auto temp = evalChildToken(0)->add(*evalChildToken(1));
				return evaluation_map[getChildToken(0)->get_string_value()].sptc =
						evaluation_map[getChildToken(0)->get_string_value()].ts.ofTypeCreateConstant(
								temp);
			}
			if (stringValue == "-=") {
				auto temp = evalChildToken(0)->sub(*evalChildToken(1));
				return evaluation_map[getChildToken(0)->get_string_value()].sptc =
						evaluation_map[getChildToken(0)->get_string_value()].ts.ofTypeCreateConstant(
								temp);

			}
			if (stringValue == "<<=") {

				auto temp = stoi(evalChildToken(1)->getStringValue());
				auto temp2 = evalChildToken(0)->LeftShift(temp);
				return evaluation_map[getChildToken(0)->get_string_value()].sptc =
						evaluation_map[getChildToken(0)->get_string_value()].ts.ofTypeCreateConstant(
								temp2);

			}
			if (stringValue == ">>=") {

				auto temp = stoi(evalChildToken(1)->getStringValue());
				auto temp2 = evalChildToken(0)->RightShift(temp);
				return evaluation_map[getChildToken(0)->get_string_value()].sptc =
						evaluation_map[getChildToken(0)->get_string_value()].ts.ofTypeCreateConstant(
								temp2);

			}
			return evaluation_map[getChildToken(0)->get_string_value()].sptc =
					evaluation_map[getChildToken(0)->get_string_value()].ts.ofTypeCreateConstant(
							evalChildToken(1));

		}
		;
};

class conditional_operator: public StringBasedToken {
public:
	conditional_operator(const std::string &opValue) :
			StringBasedToken(opValue) {
	}
	;
	Token* advance_past_conditional_operator() {
		return process_token();
	}
	void print_traverse(const TreeNode*) {
		static_cast<ensc251::Token*>(this->childPVector[0])->print_traverse(
				this);
		this->print_action();
		static_cast<ensc251::Token*>(this->childPVector[1])->print_traverse(
				this);
		OUT<< ": ";
		static_cast<ensc251::Token*>(this->childPVector[2])->print_traverse(
				this);
	}

	shared_ptr_to_constant eval()
	//We evaluate the conditional operator without use of pure virtual function since we cannot overload this operator. The operator for
	//?: cannot be overloaded since c++ does not support that. We simply evaluated the conditional operator using ifelse statements, which
	//simulates the same behavior as a conditional operator.
	{
		auto second_eval = evalChildToken(1);
		try {
			//it is not necessary to have stingValue=="?" since we already know we are working with the conditional operator inside the
			//the ternary class (conditional_operator class).
			if (stringValue == "?") {
				if (stoi(evalChildToken(0)->getStringValue())) {
					return second_eval;
				} else {
					auto third_eval = evalChildToken(2);
					return third_eval;
				}
			}
			return nullptr;
		} catch (const std::overflow_error& e) {
			OUT<<e.what()<<std::endl;
			return nullptr;

		}
	}

	/* ***** this needs work */
};

class shift_operator: public StringBasedToken {
	// '<<' | '>>'
public:
	shift_operator(const std::string &opValue) :
			StringBasedToken(opValue) {
	}
	;
	Token* advance_past_shift_operator() {
		return process_token();
	}
	shared_ptr_to_constant eval()
	//The implementation of this operator is same as the mod, shift, increment, etc. passed the values to the numeric template constant
	//class in order to evaluate and create the constant values on the heap. The constants are created using template class numeric
	{
		auto temp = atoi(evalChildToken(1)->getStringValue().c_str());
		if (this->stringValue == ">>")
			return evalChildToken(0)->RightShift(temp);
		else {
			return evalChildToken(0)->LeftShift(temp);
		}
	}
	/* ***** this needs work */
};

class additive_operator: public StringBasedToken {
	// '+' | '-'
public:
	additive_operator(const std::string &opValue) :
			StringBasedToken(opValue) {
	}
	;
	Token* advance_past_additive_operator() {
		return process_token();
	}

	shared_ptr_to_constant eval()
	//The evaluation for additive operator is simple. we use the preprocessor macros (add macro) to overload the + operator.
	//in the if statement we check for "+" then if that fails, then it must be the subtraction operator
	{
		if (this->stringValue == "+")
			return evalChildToken(0)->add(*evalChildToken(1));
		else
			return evalChildToken(0)->sub(*evalChildToken(1));
	}

};

class div_operator: public StringBasedToken {
	// '/' | '%'
public:
	div_operator(const std::string &opValue) :
			StringBasedToken(opValue) {
	}
	;
	Token* advance_past_div_operator() {
		return process_token();
	}

	shared_ptr_to_constant eval() {
		//The dive operator uses the same the evaluation function but this time we must also check of the denomenator is zero or not. We check for correct denomenator using
		//the try and catch exception handling. Like the addition operator, we use the div preprocessor macros to overload the + operator. The mod operator is implemented
		//a differently in that it must be converted to integer first then we create the implementation in the numeric constant class, using shared pointers.
		auto right_eval = evalChildToken(1);
		auto temp = atoi(right_eval->getStringValue().c_str());
		if (!right_eval->isNotZero())
			throw(std::overflow_error("Divide or Mod by zero"));
		else if (this->stringValue == "/")
			/* ***** this needs work */
			return evalChildToken(0)->div(*right_eval);
		else {
			return evalChildToken(0)->mod(temp);
		}

	}
};

class unary_operator: public StringBasedToken {
	//  '~'
public:
	unary_operator(const std::string &opValue) :
			StringBasedToken(opValue) {
	}
	;

	Token* process_unary() {
		Token* subTreeP;
		if (process_token() && (subTreeP = unary_exp())) {
			add_childP(subTreeP);
			return this;
		}
		return nullptr;
	}

	shared_ptr_to_constant eval() {
		return this->evalChildToken(0)->complement();
	}
	;
};

class postfix_operator: public StringBasedToken {
public:
	postfix_operator(const std::string &opValue) :
			StringBasedToken(opValue) {
	}
	;
	void print_traverse(const TreeNode*) {
		traverse_children_post_order(
				static_cast<ensc251::traverse_func>(print_traverse),
				static_cast<ensc251::action_func>(print_action));
	}

	Token* process_postfix_operator() {
		return process_token();
	}

	shared_ptr_to_constant eval()
	//The implementation of this operator is same as the mod, shift, increment, etc. passed the values to the numeric template constant
	//class in order to evaluate and create the constant values on the heap. The constants are created using template class numeric
	{
		if (this->stringValue == "++")
			return this->evalChildToken(0)->postfix_increment();
		else
			return this->evalChildToken(0)->postfix_decrement();
	}
	/* ***** this needs work */
};

class string: public StringBasedToken {
	// e.g., "sample string"
public:
	string(const std::string &aStringValue) :
			StringBasedToken(aStringValue) {
	}
	;

	Token* process_primary_exp() {
		return process_token();
	}

	/* ***** BONUS MARKS: Evaluation of string assignments would be considered for the bonus points.
	 * for example 's = "Hello world";'  */
};

class incorrect: public StringBasedToken {
public:
	incorrect(const std::string &aStringValue) :
			StringBasedToken(aStringValue) {
	}
	;
};

#define MULTI_OP(RNAME, OP, TYPE) \
		shared_ptr_to_constant RNAME(TYPE v) { return create(v OP value); }

#define NUMERIC_OP(NAME, OP) \
		MULTI_OP(r##NAME, OP, float) \
		MULTI_OP(r##NAME, OP, int) \
		MULTI_OP(r##NAME, OP, char) \
		\
		shared_ptr_to_constant NAME(constant& obj) {return obj.r##NAME(value); } \
		shared_ptr_to_constant operator OP (constant& obj) {return obj.r##NAME(value); }

template<typename T>
class numeric_const: public constant
//we have completed this class for in order to pass values from the evaluation functions into this class for creating
//constant values of required type on the heap. Individual operators are implemented in this class for shift, add, div,
//sub, post increment/decrement,etc.
{
private:
	T value;
public:
	/* ***** this class needs work including for multiple operations supported on constants such as postfix operations, addition, subtraction, shift. */

	numeric_const(const T& constValue) :
			value(constValue) {
	}
	//Fill in for additional constructors if desired

	//The member functions defined below are used to create constants of specific type. The string value is converted to the
	//mentioned numeric constant
	shared_ptr_to_constant putValueIntoInt() {
		return shared_ptr_to_constant(new numeric_const<int>(value));
	}
	shared_ptr_to_constant putValueIntoChar() {
		return shared_ptr_to_constant(new numeric_const<char>(value));
	}
	shared_ptr_to_constant putValueIntoFloat() {
		return shared_ptr_to_constant(new numeric_const<float>(value));
	}

	std::string getStringValue() {
		return std::to_string(value);
	}
	T& getValue() {
		return value;
	}
	bool isNotZero() {
		return value;
	}

	void print(std::ostream& str) const {
		Token::print(str);
		str << ": " << value;
	}
	TreeNode* print_action() {
		OUT<<value << " "; return nullptr;};

	NUMERIC_OP(div, /)
	NUMERIC_OP(add, +)
	NUMERIC_OP(sub, -)

	shared_ptr_to_constant complement()
	//Unary operator is complemented and is made sure not to operate on values other than integers.
	//an exception class is used to handle the potential errors
	{
		if(!std::is_same<T,float>::value)
		return create(~(int)value);
		else {
			OUT << "Error Occurred! Can not perform bit-complement on float" << std::endl;
			return shared_ptr_to_constant();
		}
	};

	shared_ptr_to_constant postfix_increment()
	//Unary operator increments constants created using the create
	//template class.
	{
		T temp=value;
		value++;
		return create(temp);
	};

	shared_ptr_to_constant postfix_decrement()
	//Unary operator decrements constants created using the create
	//template class.
	{
		T temp=value;
		value--;
		return create(temp);
	};
	shared_ptr_to_constant mod(int temp)
	//modulus of a values is taken only for integer values. constants created using the create
	//template class.
	{
		if(!std::is_same<T, float>::value)
		return create((int)value%temp);
		else
		{
			OUT<<"Error! : Modulus on Float is not a Valid Operation"<<std::endl;
			return shared_ptr_to_constant();
		}
	};
	shared_ptr_to_constant RightShift(int temp)
	//Right shift of a values is taken only for integer values. constants created using the create
	//template class.
	{
		if(!std::is_same<T, float>::value)
		return create((int)value>>temp);
		else
		{
			OUT<<"Error! : RightShift on Float is not a Valid Operation"<<std::endl;
			return shared_ptr_to_constant();
		}
	};
	shared_ptr_to_constant LeftShift(int temp)
	//Left shift of a values is taken only for integer values. constants created using the create
	//template class.
	{
		if(!std::is_same<T, float>::value)
		return create((int)value<<temp);
		else
		{
			OUT<<"Error! : LeftShift on Float is not a Valid Operation"<<std::endl;
			return shared_ptr_to_constant();
		}
	};

	shared_ptr_to_constant eval()
	{
		return create(value);
	};
};

}
 // namespace ensc251

#endif /* TOKENCLASSES_HPP_ */
