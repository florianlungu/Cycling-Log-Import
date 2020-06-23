/*
 * BaseFunctions.h
 *
 *  Created on: Jun 15, 2020
 *      Author: Florian Lungu
 */

#include <string>
#include <vector>
using namespace std;

string ltrim(const string& s);

string rtrim(const string& s);

string trim(const string& s);

string ReplaceAll(string str, const string& from, const string& to);

vector<string> CsvlinePopulate(string line, string delimiter);
