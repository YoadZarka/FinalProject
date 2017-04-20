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
	std::vector<bool> id2dnf (int id);
	std::vector< std::vector <bool> > DNFFile;
	std::vector< std::vector <bool> > DNFBlocks;
	std::vector< std::vector <bool> > DNFEdges;
	std::vector< std::vector <bool> > DNFDiff;
	int numOfLiterals;
	std::vector<int> files;
	File* inputfile;
	File* outputfile;

};

#endif /* REDUCTION1_H_ */
