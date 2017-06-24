/*
 * File.cpp
 *
 *  Created on: Apr 3, 2017
 *      Author: yoad
 */
#include "File.h"

using namespace std;

File::File(char* path, int reduce) {
	this->myfile.open(path,ios::in);
	if (!this->myfile.is_open()){
		cout << "Unable to open file";
		exit(1);
	}
	readNumFilesBlocks();
}

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

File::File(){this->numOfBlocks=0;
this->numOfFiles=0;}

File::~File() {
	this->myfile.close();
}

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

