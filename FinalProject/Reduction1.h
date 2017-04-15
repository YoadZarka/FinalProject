/*
 * Reduction1.h
 *
 *  Created on: Apr 14, 2017
 *      Author: yoad
 */

#ifndef REDUCTION1_H_
#define REDUCTION1_H_
#include "File.h"
#include <vector>


class Reduction1 {
public:
	Reduction1(char* path);
	virtual ~Reduction1();
	void parser();
	std::vector< std::vector<char> > DNFFile;
	std::vector< std::vector<char> > DNFBlocks;
	std::vector< std::vector<char> > DNFEdges;
	std::vector< std::vector<char> > DNFDiff;
	int numOfLiterals;
	File* inputfile;
	File* outputfile;

};

#endif /* REDUCTION1_H_ */
