/*
 * File.cpp
 *
 *  Created on: Apr 3, 2017
 *      Author: yoad
 */
#include "File.h"

using namespace std;

/***** Important: All the errors in the system are not handle in the cpp code but in the python side
 * Therefore when an error occurs, a matching message will be print to STDout and the program breaks ******/

/*Constructor for opening a input file and count the files and blocks number*/
File::File(char* path, int reduce) {
	this->myfile.open(path,ios::in);
	if (!this->myfile.is_open()){
		cout << "Unable to open file";
		exit(1);
	}
	readNumFilesBlocks();
}

/*Constructor for opening a input file without count the files and blocks number
 * ******When open a file, first the file in the path is deleted and then open again as new file****** */
File::File(char* path){
	this->myfile.open(path, std::ofstream::out | std::ofstream::trunc);
	this->myfile.close();
	this->myfile.open(path,ios::out);
		if (!this->myfile.is_open()){
			cout << "Unable to open file";
			exit(1);
		}
		this->numOfBlocks=0;
		this->numOfFiles=0;
}

/*Costructor for empty object - not in use*/
File::File(){this->numOfBlocks=0;
this->numOfFiles=0;}

/*Destructor  - close the opened file*/
File::~File() {
	this->myfile.close();
}

/*The function read the file pointed by 'myfile' and count the number of blocks and number of files
 * the numbers stored in 'numOfFiles' and 'numOfBlocks' respectively*/
void File::readNumFilesBlocks (){
	string line=getLine();
	int F_count=0, B_count=0;
	while (line != "***end***"){
		if (line.at(0) == 'F')
			F_count++;
		if (line.at(0) == 'B')
			B_count++;
		line=getLine();
	}
	this->numOfFiles = F_count;
	this->numOfBlocks = B_count;
}

/* The function read one line from the file pointed by 'myfile' and return it
 * If the file is empty the line will be - '***end***' */
string File::getLine(){
	string line;
	if (getline(this->myfile,line))
		return line;
	else{
		line="***end***";
		this->myfile.clear();
		this->myfile.seekg(0, ios::beg);
		return line;
	}
}

/* The function write one line to the file pointed by 'myfile'*/
void File::writeLine (string line) {
	if (this->myfile.is_open())
	  {
	    this->myfile << line+"\n";
	  }
	  else
	  { cout << "Unable to open file";
	  	  	exit(1);
	  }
}

