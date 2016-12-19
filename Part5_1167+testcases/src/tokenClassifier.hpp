/*
 * tokenClassifier.hpp
 *
 *  Created on: Oct 2, 2016
 *      Author: wcs
 */

#ifndef TOKENCLASSIFIER_HPP_
#define TOKENCLASSIFIER_HPP_

#include <vector>
#include <sstream>

#include "tokenClasses.hpp"

// Construct the token objects and categorize them into appropriate types
// Input: a string vector of tokens
// Output: a vector including the list of Token objects properly categorized
std::vector<ensc251::Token*> tokenClassifier(std::istream& code)
;

#endif /* TOKENCLASSIFIER_HPP_ */
