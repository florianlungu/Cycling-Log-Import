/* Author: Florian Lungu
 *
 * Description:
 * This C++ code reads data from the Golden Cheetah default export file
 * golden_cheetah_export.csv then writes the data I want to a new csv file so that
 * I can easily copy and paste into my annual cycling log Excel file
 *
 * Version History:
 * 2020-Jun-13 Initial creation
 * 2020-Aug-14 Added distance comparison for when there are two workouts in one day
 * 2020-Nov-12 Removed TrainingPeaks code
 *
 */

#include "BaseFunctions.h"
#include <fstream>
#include <iostream>
#include <vector>

#include <string>
#include <stdexcept> // runtime_error
#include <sstream>
#include <istream>
#include <cmath>
#include <windows.h>
#include <ctime>
#include <algorithm>

using namespace std;


namespace patch
{
    template < typename T > std::string to_string( const T& n )
    {
        std::ostringstream stm ;
        stm << n ;
        return stm.str() ;
    }
}

int main() {
	// settings from ini file
	string line, line_tp, word, param, val, temp;
	string golden_cheetah_export = "";
	string cycling_log_import = "";
	string just_this_year = "";
	ifstream settings_file;

	// csv files
	const char *bom = "\xef\xbb\xbf";
	ifstream gc_file, tp_file;
	ofstream cl_file;
	vector<string> gc_row, tp_row;
	int rownum = 1,
		tcount = 0;

	// columns and fields
	int column_distance,
		column_moving_time,
		column_elevation_gain,
		column_TSS,
		column_hrTSS,
		column_IF,
		column_hrIF,
		column_np,
		column_temp,
		column_avg_hr ,
		column_max_hr;

	string field_date,
		field_distance,
		field_moving_time,
		field_elevation_gain,
		field_TSS,
		field_IF,
		field_np,
		field_temp,
		field_avg_hr,
		field_max_hr,
		field_date_raw,
		field_mph,
		this_year_str,
		row_date_str;

    std::time_t t = std::time(0);
    std::tm* now = std::localtime(&t);
    this_year_str = patch::to_string(now->tm_year + 1900) + "/01/01";

	// get file locations from ini file
	settings_file.open("Cycling Log Import Settings.ini");
	if (settings_file) {
		cout << "Reading from the \'Cycling Log Import Settings.ini\' file" << endl;
		while (getline(settings_file,line)) {
			param = trim(line.substr(0, line.find("=")));
			val = trim(line.substr(line.find("=")+1, line.length()-line.find("=")-1));

			if (param=="golden_cheetah_export") {
				golden_cheetah_export = ReplaceAll(val, "\\", "\\\\");
			} else if (param=="cycling_log_import") {
				cycling_log_import = ReplaceAll(val, "\\", "\\\\");
			} else if (param=="just_this_year") {
				just_this_year = ReplaceAll(val, "\\", "\\\\");
				transform(just_this_year.begin(), just_this_year.end(), just_this_year.begin(), ::toupper);
			}
		}
		settings_file.close();
	} else {
		cout << "There was an error opening \'Cycling Log Import Settings.ini\' file" << endl;
		Sleep(3000); // wait 3 seconds to close the window
		return 0;
	}

	// open csv files
	gc_file.open(golden_cheetah_export);
	if (!gc_file) {
		cout << "There was an error opening golden_cheetah_export file" << endl;
		Sleep(3000); // wait 3 seconds to close the window
		return 0;
	}
	cl_file.open(cycling_log_import);
	if (!cl_file) {
		cout << "There was an error opening cycling_log_import file" << endl;
		Sleep(3000); // wait 3 seconds to close the window
		return 0;
	} else {
		cl_file << bom;
	}

	// parse golden cheetah export file
	if (gc_file && tp_file && cl_file) {
		cout << "Reading from the Golden Cheetah Export file..." << endl;

		while (getline(gc_file,line)) {
			// parse line
			string this_line = line;
			stringstream s(this_line);
			gc_row.clear();

			while (getline(s, word, ',')) {
				// figure out column numbers based on first row
				if (rownum==1) {
					if (word==" Distance") {
						column_distance = tcount;
					} else if (word==" Time Moving") {
						column_moving_time = tcount;
					} else if (word==" Elevation Gain") {
						column_elevation_gain = tcount;
					} else if (word==" TSS") {
						column_TSS = tcount;
					} else if (word==" TRIMP Zonal Points") {
						column_hrTSS = tcount;
					} else if (word==" IF") {
						column_IF = tcount;
					} else if (word==" Heart Rate Intensity") {
						column_hrIF = tcount;
					} else if (word==" NP") {
						column_np = tcount;
					} else if (word==" Average Temp") {
						column_temp = tcount;
					} else if (word==" Average Heart Rate") {
						column_avg_hr = tcount;
					} else if (word==" Max Heartrate") {
						column_max_hr = tcount;
					}
					tcount++;

				// parse each activity into a vector
				} else {
					gc_row.push_back(word);
				}
			}

		    if (rownum>1) {
				// golden cheetah fields
				field_date = "";
				field_distance = "";
				field_moving_time = "";
				field_elevation_gain = "";
				field_TSS = "";
				field_IF = "";
				field_np = "";
				field_temp = "";
				field_avg_hr = "";
				field_max_hr = "";

				field_date_raw = gc_row[0];
				row_date_str = "20"+field_date_raw.substr(6,2)+"/"+field_date_raw.substr(0,2)+"/"+field_date_raw.substr(3,2);

				if (this_year_str <= row_date_str || just_this_year != "YES") {
					field_date = "20"+field_date_raw.substr(6,2)+"-"+field_date_raw.substr(0,2)+"-"+field_date_raw.substr(3,2);

					if (stof(gc_row[column_distance]) > 0) {
						field_distance = patch::to_string(round(stof(gc_row[column_distance])*0.621371*10)/10);
						field_moving_time = patch::to_string(int(stof(gc_row[column_moving_time])/60));
						field_elevation_gain = patch::to_string(round(stof(gc_row[column_elevation_gain])*3.28084));
						if (gc_row[column_np] != "0") {
							field_np = patch::to_string(ceil(stof(gc_row[column_np])));
							field_IF = patch::to_string(round(stof(gc_row[column_IF])*100)/100);
							field_TSS = patch::to_string(round(stof(gc_row[column_TSS])));
						}
						if (field_IF == "") {
							field_IF = patch::to_string(round(stof(gc_row[column_hrIF])*100)/100);
							field_TSS = patch::to_string(round(stof(gc_row[column_hrTSS])));
						}
						if (stof(gc_row[column_temp]) > -20) {
							field_temp = patch::to_string(round((stof(gc_row[column_temp]) * 9/5 + 32)));
							}
						field_avg_hr = patch::to_string(round(stof(gc_row[column_avg_hr])));
						field_max_hr = gc_row[column_max_hr];
					}

					// output row to cycling_log_import file
					cl_file << "\"" << field_date << "\"" << ",";
					cl_file << "\"" << field_distance << "\"" << ",";
					cl_file << "\"" << field_moving_time << "\"" << ",";
					cl_file << "\"" << field_elevation_gain << "\"" << ",";
					cl_file << "\"" << field_TSS << "\"" << ",";
					cl_file << "\"" << field_IF << "\"" << ",";
					cl_file << "\"" << field_np << "\"" << ",";
					cl_file << "\"" << field_avg_hr << "\"" << ",";
					cl_file << "\"" << field_max_hr << "\"" << ",";
					cl_file << "\"" << field_temp << "\"" << endl;
				}
		    }
		    rownum++;
		}

		gc_file.close();
		tp_file.close();
		cl_file.close();

		cout << "Successfully processed " << rownum << " activities from Golden Cheetah" << endl;
	}

	cout << "Program completed successfully!!" << endl;
	Sleep(3000); // wait 3 seconds to close the window
	return 0;
}
