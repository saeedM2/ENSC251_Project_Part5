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
//% * Enter the above information in Test.cpp if you are submitting that file too.
//% * Edit the "Helpers" line and "Resources" line.
//% * Your group name should be "<userid1>_<userid2>" (eg. stu1_stu2)
//% * Form groups as described at:  https://courses.cs.sfu.ca/docs/students
//% * Submit one or two files to courses.cs.sfu.ca
//
// Name        : tokenClassifier.cpp
// Description : Course Project Part 2 (Token Classifier)
// Original portions Copyright (c) 2016 School of Engineering Science, Simon Fraser University
//============================================================================

#include <vector>
#include <sstream>
#include <algorithm>
#include "tokenClasses.hpp"

#define WHITE "\n\r\t\v\f "

using namespace std;
using namespace ensc251;

std::string delim33[] = {">>=", "<<="};
std::string delim22[] = {"+=","-=", "/=", "%=", "<<", ">>", "++", "--"};
std::string delim1andWhite1 = WHITE "/()*%:;=&+-~?";
std::string white1 = WHITE;

bool processDigits(std::string line, size_t &index)
{
 bool DigiFound=false;
 while (index < line.length() && line.at(index) <= '9' && line.at(index) >= '0')
 	 {
 	 	 index++;
 	 	 DigiFound=true;
 	 }
 return DigiFound;
}

// Construct token objects categorized into appropriate types
// Input: a stream
// Output: a vector containing the list of Token objects properly categorized
std::vector<ensc251::Token*> tokenClassifier(istream& code)
{
 	 std::vector<ensc251::Token*> tokenObjectPs;

 	 std::string line;

 	 while (getline (code, line)) {
 	 	 if (line.at(0) == '#')
 	 	 	 continue;

 	 	 size_t index2 = 0;

        auto length = line.length();

 	 	 while(index2 < length ) {
 	 	 	 auto index1 = line.find_first_not_of(white1, index2);
 	 	 	 if (index1 == std::string::npos)
 	 	 	 	 break;

 	 	 	 /// Dealing with int and float consts
 	 	 	 if ((line.at(index1) <= '9' && line.at(index1) >= '0') || line.at(index1) == '.')
 	 	 	 {
 	 	 	 	 index2 = index1;
 	 	 	 	 bool int_const = true;

 	 	 	 	 // integer consts
 	 	 	 	 bool preDigitFound = processDigits(line, index2);

 	 	 	 	 // floats
 	 	 	 	 if (index2 < length && line.at(index2) == '.')
 	 	 	 	 {
 	 	 	 	 	 index2++;
 	 	 	 	 	 bool postDigitFound = processDigits(line, index2);

 	 	 	 	 	 if(!preDigitFound && !postDigitFound) // detecting the single "." as an invalid token!
 	 	 	 	 	 {
 	 	 	 	 	 	 cout << "Error -- invalid token in input: " << line.substr(index1, index2 - index1) << endl;
 	 	 	 	 	 	 tokenObjectPs.push_back(new incorrect(line.substr(index1, index2 - index1)));
 	 	 	 	 	 	 continue;
 	 	 	 	 	 }
 	 	 	 	 	 int_const = false;
 	 	 	 	 }

 	 	 	 	 // exponentials
 	 	 	 	 if (index2 < length && (line.at(index2) == 'e' || line.at(index2) == 'E'))
 	 	 	 	 {
 	 	 	 	 	 if(line.at(index2+1) == '+' || line.at(index2+1) == '-')
 	 	 	 	 	 	 index2+=2;
 	 	 	 	 	 else
 	 	 	 	 	 	 index2++;

 	 	 	 	 	 if(!processDigits(line, index2)) // invalid float -> roll back ...
 	 	 	 	 	 {
 	 	 	 	 	 	 if(line.at(index2-1) == 'e' || line.at(index2-1) == 'E')
 	 	 	 	 	 	 	 index2--;
 	 	 	 	 	 	 else
 	 	 	 	 	 	 	 index2-=2;
 	 	 	 	 	 }
 	 	 	 	 	 int_const = false;
 	 	 	 	 }

 	 	 	 	 // suffix f and F
 	 	 	 	 if (index2 < length && (line.at(index2) == 'f' || line.at(index2) == 'F')){
 	 	 	 	 	 index2++;
 	 	 	 	 	 int_const = false;
 	 	 	 	 }

 	 	 	 	 auto subs = line.substr(index1, index2 - index1);
 	 	 	 	 if(int_const){ // integer constant
 	 	 	 	 	 tokenObjectPs.push_back(new ensc251::numeric_const<int>(atoi(subs.c_str())));
 	 	 	 	 }else{	 // float constant
 	 	 	 		 // isn't there a better method?  strtof ??
 	 	 	 	 	 istringstream os(subs);
 	 	 	 	 	 float f;
 	 	 	 	 	 os >> noskipws >> f;
 	 	 	 	 	 tokenObjectPs.push_back(new ensc251::numeric_const<float>(f));
 	 	 	 	 }
 	 	 	 	 continue;
 	 	 	 }

 	 	 	 // the below could go into a function.
 	 	 	 for (auto delim : delim33) {
 	 	 	 	 if (line.substr(index1, 3) == delim) {
 	 	 	 	 	 tokenObjectPs.push_back(new assignment_operator(delim));
 	 	 	 	 	 index2 = index1 + 3;
 	 	 	 	 	 break;
 	 	 	 	 }
 	 	 	 }
 	 	 	 if (index2 > index1) continue;

 	 	 	 for (auto delim : delim22) {
 	 	 	 	 if (line.substr(index1, 2) == delim) {
 	 	 	 	 	 if(delim == "+=" || delim == "-=" || delim == "/=" || delim == "%="){
 	 	 	 	 	 	 tokenObjectPs.push_back(new assignment_operator(delim));
 	 	 	 	 	 }else if(delim == "<<" || delim == ">>"){
 	 	 	 	 	 	 tokenObjectPs.push_back(new shift_operator(delim));
 	 	 	 	 	 }else if(delim == "++" || delim == "--"){
 	 	 	 	 	 	 tokenObjectPs.push_back(new postfix_operator(delim));
 	 	 	 	 	 }
 	 	 	 	 	 index2 = index1 + 2;
 	 	 	 	 	 break;
 	 	 	 	 }
 	 	 	 }
 	 	 	 if (index2 > index1) continue;

 	 	 	 if (line[index1] == '"') {
 	 	 	 	 index2 = index1 + 1;
 	 	 	 	 while (index2 < length) {
 	 	 	 	 	 if (line[index2] == '\\') {
 	 	 	 	 	 	 index2++; // skip over escape character
 	 	 	 	 	 }
 	 	 	 	 	 else
 	 	 	 	 	 	 if (line.at(index2) == '"') {
 	 	 	 	 	 	 	 tokenObjectPs.push_back(new ensc251::string(line.substr(index1, index2 + 1 - index1)));
 	 	 	 	 	 	 	 break;
 	 	 	 	 	 	 }
 	 	 	 	 	 index2++;
 	 	 	 	 }
 	 	 	 	 if (index2 >= length) { // String is not terminated!
 	 	 	 	 	 cout << "Error: string is not terminated!" << endl;
 	 	 	 	 	 tokenObjectPs.push_back(new incorrect(line.substr(index1, index2 - index1)));
 	 	 	 	 	 break;
 	 	 	 	 }
 	 	 	 	 index2++; // non-empty string was pushed on the result vector
 	 	 	 	 continue;
 	 	 	 }

 	 	 	 if (line[index1] == '\'') {
 	 	 	 	 index2 = index1 + 1;
 	 	 	 	 if (index2 < length && line[index2] == '\\') {
 	 	 	 	 	 index2++; // skip over escape character
 	 	 	 	 }
 	 	 	 	 index2++; // skip over character
 	 	 	 	 if (index2 >= length || line.at(index2) != '\'') // Character constant is not terminated!
 	 	 	 	 {
 	 	 	 	 	 cout << "Error: character constant is not terminated!" << endl;
 	 	 	 	 	 tokenObjectPs.push_back(new incorrect(line.substr(index1, index2 - index1)));
 	 	 	 	 }
 	 	 	 	 else {
 	 	 	 	 	 index2++;
 	 	 	 	 	 char tempChar = line.substr(index1, index2 - index1)[1];
 	 	 	 	 	 tokenObjectPs.push_back(new ensc251::numeric_const<char>(tempChar));
 	 	 	 	 }
 	 	 	 	 continue;
 	 	 	 }

 	 	 	 if(line[index1]=='_' ||
 	 	 	 	 	 (line[index1]>='a' && line[index1]<='z') ||
 	 	 	 	 	 (line[index1]>='A' && line[index1]<='Z')) {
 	 	 	 	 index2 = index1 + 1;
 	 	 	 	 while (index2 < length &&
 	 	 	 	 	 	 (line[index2]=='_' ||
 	 	 	 	 	 	 (line[index2]>='a' && line[index2]<='z') ||
 	 	 	 	 	 	 (line[index2]>='A' && line[index2]<='Z') ||
 	 	 	 	 	 	 (line[index2]>='0' && line[index2]<='9')
 	 	 	 	 	 	 )
 	 	 	 	 )
 	 	 	 	 	 index2 ++;
 	 	 	 	 std::string tempToken = line.substr(index1, index2 - index1);
 	 	 	 	 if(tempToken == "int" || tempToken == "float" || tempToken == "char" ){
 	 	 	 	 	 tokenObjectPs.push_back(new type_spec(tempToken));
 	 	 	 	 }else{
 	 	 	 	 	 tokenObjectPs.push_back(new id(tempToken));
 	 	 	 	 }
 	 	 	 	 continue;
 	 	 	 }

 	 	 	 index2 = line.find_first_of(delim1andWhite1, index1);
 	 	 	 if (index2 == index1) {
 	 	 	 	 index2++; // we found a 1-character token
 	 	 	 	 std::string tempToken = line.substr(index1, 1);
 	 	 	 	 char tempChar = line[index1];
 	 	 	 	 switch (tempChar) {
	 	 	        case '=': tokenObjectPs.push_back(new assignment_operator(tempToken)); break;
 	 	 	        case '?': tokenObjectPs.push_back(new conditional_operator(tempToken)); break;

 	 	 	        case '+':
 	 	 	        case '-': tokenObjectPs.push_back(new additive_operator(tempToken)); break;

 	 	 	        case '/':
 	 	 	        case '%': tokenObjectPs.push_back(new div_operator(tempToken)); break;

 	 	 	        case '&':
 	 	 	        case '~': tokenObjectPs.push_back(new unary_operator(tempToken)); break;

 	 	 	        case ';':
 	 	 	        case ':':
 	 	 	        case '*':
 	 	 	        case '(':
 	 	 	        case ')': tokenObjectPs.push_back(new punctuator(tempToken)); break;
 	 	 	 	 }

 	 	 	 	 continue;
 	 	 	 }
 	 	 	 cout << "Error -- invalid token in input: " <<
 	 	 	 	 	 line.substr(index1, index2 - index1) << endl;
 	 	 	 tokenObjectPs.push_back(new incorrect(line.substr(index1, index2 - index1)));
 	 	 }
 	 }

 	 return tokenObjectPs;
}
