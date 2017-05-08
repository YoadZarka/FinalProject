/*
 * File.h
 *
 *  Created on: Apr 3, 2017
 *      Author: yoad
 */
#include <iostream>
#include <fstream>
#include <string>
#include <stdlib.h>

#ifndef FILE_H_
#define FILE_H_

class File {
public:
	int numOfFiles;
	int numOfBlocks;
	std::fstream myfile;
	File(char* path,int reduce);
	File(char* path);
	File();
	virtual ~File();
	std::string getLine ();
	void writeLine (std::string line);
	File& operator=(const File &A) {return *this;}
	File& operator=(File &A) {return *this;}
private:
	void readNumFilesBlocks ();
};

#endif /* FILE_H_ */
