/*
 * BaseFunctions.h
 *
 * Author: Florian Lungu
 *
 * Version History:
 * 2020-Jun-15 Initial creation
 */

#include <string>
#include <vector>
using namespace std;

string ltrim(const string& s);

string rtrim(const string& s);

string trim(const string& s);

string ReplaceAll(string str, const string& from, const string& to);

vector<string> CsvlinePopulate(string line, string delimiter);
