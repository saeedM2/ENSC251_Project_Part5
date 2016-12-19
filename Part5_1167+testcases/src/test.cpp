/*
 * test.cpp
 *
 *  Created on: Fall 2016
 *      Author: Mohammad Akbari
 *
 *  Copyright (c) 2016 School of Engineering Science, Simon Fraser University
 */

#include <iostream>
#include <sstream>
#include "tokenClassifier.hpp"
using namespace std;

#define DONT_RUN_MAIN

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MAIN
#define BOOST_TEST_MODULE Course Project Part 5
#include <boost/test/unit_test.hpp>
#include <boost/algorithm/string.hpp>
using namespace boost;

void check(std::string input, std::vector<std::string> ids, std::vector<std::string> values)
{
	istringstream inputStream(input);

	ensc251::tokenObjectPs = tokenClassifier(inputStream);
	ensc251::Token *ast_root = ensc251::recursive_parser();
	if(ast_root)
	{
			ast_root->print_ast();
			ast_root->eval();
			bool noError=true;
			for(unsigned i=0; i < ids.size(); i++)
			{
				ensc251::varInfo varinfo = ensc251::evaluation_map.find(ids[i])->second;
				if (varinfo.sptc)
				{
					std::string userValue = varinfo.sptc->getStringValue();
					if(userValue != values[i])
					{
					    BOOST_ERROR("Error in the id ("+ids[i]+"): the user value ("+userValue+") != expected value ("+values[i]+").");
					    noError=false;
					}
				}
				else
				{
					BOOST_ERROR("Error in the id ("+ids[i]+"): Segmentation Fault!");
					noError=false;
				}
			}
			if(noError)
				ensc251::print_evaluation_map();

	}
	else
	{
		OUT << "Syntax error in input. Please try again..." << endl;
	}

	ensc251::evaluation_map.clear(); // Clear the map for the next test case!
	ensc251::tracker = 0; // Reset the tracker for the next case!
}

BOOST_AUTO_TEST_CASE(test1)
{
	std::string input("int a; a=2;");
	std::vector<std::string> ids  = {"a"};
	std::vector<std::string> values  = {"2"};
	check(input, ids, values);
}
BOOST_AUTO_TEST_CASE(test1B)
{
	std::string input("int b; int a; a = 10; b =  a + 5; a = ~a;");
	std::vector<std::string> ids  = {"a", "b"};
	std::vector<std::string> values  = {"-11","15"};
	check(input, ids, values);
}
BOOST_AUTO_TEST_CASE(test1C)
{
	std::string input("int b; int a; a = 10; b =  a - 5; a = ~a;");
	std::vector<std::string> ids  = {"a", "b"};
	std::vector<std::string> values  = {"-11","5"};
	check(input, ids, values);
}
BOOST_AUTO_TEST_CASE(test2)
{
	std::string input("int b; int a; int c; a = 10+10; b = a / 2; c = a % b;");
	std::vector<std::string> ids  = {"b","a","c"};
	std::vector<std::string> values  = {"10","20","0"};
	check(input, ids, values);
}

BOOST_AUTO_TEST_CASE(test3)
{
	std::string input("float a; float b; char c; a = 0.25; b = a++; c=10; c--;");
	std::vector<std::string> ids  = {"a","b","c"};
	std::vector<std::string> values  = {"1.250000","0.250000","9"};
	check(input, ids, values);
}


BOOST_AUTO_TEST_CASE(test4)
{
	std::string input("float n; int b; b=0; n = b ? 30 : 40.0;");
	std::vector<std::string> ids  = {"n","b"};
	std::vector<std::string> values  = {"40.000000","0"};
	check(input, ids, values);
}

BOOST_AUTO_TEST_CASE(test5)
{
	std::string input("float n; int b; int c; c=7; n=5.5; b=6+5+n+7; n = b ? c ? 20 : 30 : 40; n = ~b; n--; n =  b++ + 4;");
	std::vector<std::string> ids  = {"n","b","c"};
	std::vector<std::string> values  = {"27.000000","24","7"};
	check(input, ids, values);
}


BOOST_AUTO_TEST_CASE(test6)
{
	std::string input("int a; char c; a = '0'; c = ~ ~'z' + 'z' + 'z';");
	std::vector<std::string> ids  = {"a","c"};
	std::vector<std::string> values  = {"48","110"};
	check(input, ids, values);
}

BOOST_AUTO_TEST_CASE(test7)
{
	std::string input("int val; int shifted; val=10<<3; shifted=val>>1;");
	std::vector<std::string> ids  = {"val","shifted"};
	std::vector<std::string> values  = {"80","40"};
	check(input, ids, values);
}

BOOST_AUTO_TEST_CASE(test8)
{
	std::string input("int b; int a; char c; c = '0'; a = ~'0'; b= a - 2;");
	std::vector<std::string> ids  = {"a","b","c"};
	std::vector<std::string> values  = {"-49","-51","48"};
	check(input, ids, values);
}

BOOST_AUTO_TEST_CASE(test9)
{
	std::string input("int a; a=4 ; a /= 2;");
	std::vector<std::string> ids  = {"a"};
	std::vector<std::string> values  = {"2"};
	check(input, ids, values);
}

BOOST_AUTO_TEST_CASE(test10)
{
	std::string input("int a; a= 4 ; a += 2;");
	std::vector<std::string> ids  = {"a"};
	std::vector<std::string> values  = {"6"};
	check(input, ids, values);
}

BOOST_AUTO_TEST_CASE(test11)
{
	std::string input("int a; a= 4; a -= 2;");
	std::vector<std::string> ids  = {"a"};
	std::vector<std::string> values  = {"2"};
	check(input, ids, values);
}

BOOST_AUTO_TEST_CASE(test12)
{
	std::string input("int a; a= 2 ; a<<= 2;");
	std::vector<std::string> ids  = {"a"};
	std::vector<std::string> values  = {"8"};
	check(input, ids, values);
}

BOOST_AUTO_TEST_CASE(test13)
{
	std::string input("int a; a= 4 ; a>>= 2;");
	std::vector<std::string> ids  = {"a"};
	std::vector<std::string> values  = {"1"};
	check(input, ids, values);
}

// one bonus example (assignment operator for string literals)
/*
BOOST_AUTO_TEST_CASE(test9)
{
	std::string input("char * s; s = \"Hello\";");
	std::vector<std::string> ids  = {"s"};
	std::vector<std::string> values  = {"Hello"};
	check(input, ids, values);
}
*/
