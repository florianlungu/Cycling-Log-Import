/*
 * BaseFunctions.cpp
 *
 * Author: Florian Lungu
 *
 * Version History:
 * 2020-Jun-15 Initial creation
 */

#include <string>
#include <vector>
using namespace std;

const string WHITESPACE = " \n\r\t\f\v";


string ltrim(const string& s) {
	size_t start = s.find_first_not_of(WHITESPACE);
	return (start == string::npos) ? "" : s.substr(start);
}

string rtrim(const string& s) {
	size_t end = s.find_last_not_of(WHITESPACE);
	return (end == string::npos) ? "" : s.substr(0, end + 1);
}

string trim(const string& s) {
	return rtrim(ltrim(s));
}

string ReplaceAll(string str, const string& from, const string& to) {
    size_t start_pos = 0;
    while((start_pos = str.find(from, start_pos)) != string::npos) {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length(); // Handles case where 'to' is a substring of 'from'
    }
    return str;
}

vector<string> CsvlinePopulate(string line, string delimiter)
{
	vector<string> record;
	int linepos=0;
	bool inquotes=false;
	string c;
	int linemax=line.length();
	string curstring;
	record.clear();

	while(linepos < linemax){
		c = line.substr(linepos,1);
		if (!inquotes && curstring.length()==0 && c=="\"") {
		    //beginquotechar
		    inquotes=true;
		} else if (inquotes && c=="\"") {
		    //quotechar
		    if ( (linepos+1 <linemax) && (line.substr(linepos+1,1)=="\"") ) {
		        //encountered 2 double quotes in a row (resolves to 1 double quote)
		        curstring += c;
		        linepos++;
		    } else {
		        //endquotechar
		        inquotes=false;
		    }
		} else if (!inquotes && c==delimiter) {
		    //end of field
		    record.push_back( curstring );
		    curstring="";
		} else if (!inquotes && (c=="\r" || c=="\n") ) {
		    record.push_back( curstring );
		    return record;
		} else {
		    curstring += c;
		}
		linepos++;
	}
	record.push_back( curstring );
	return record;
}
