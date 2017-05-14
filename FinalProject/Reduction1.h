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
	Reduction1(char* path, int delFiles, int delBlocks);
	virtual ~Reduction1();
	void parser();
	void convert2cnf();
	void print();
	void dnf2cnf (std::vector< std::vector <bool> >& dnf);
	void encodeCNFDiff ();
	void writeCNF(int delFiles, int delBlocks);
	std::vector<bool> id2dnf (int id);
	std::vector< std::vector <bool> > DNFFile;      //store one dnf files clause
	std::vector< std::vector <bool> > DNFBlocks;	//store one dnf blocks clause
	std::vector< std::vector <bool> > DNFEdges;		//store one dnf edges clause
	std::vector< std::vector <bool> > CNFFile;      //store one cnf files clause
	std::vector< std::vector <bool> > CNFBlocks;	//store one cnf blocks clause
//	std::vector< std::vector <bool> > CNFEdges;		//store one cnf edges clause
	std::vector< std::vector <bool> > CNFDiff;		//store one cnf difference check clause
	std::vector< std::vector <bool> > tempCnf;		//using in the cnf convert function
	int numOfLiterals;
	int totalLiterals;
	int firstFile;
	int firstBlock;
	int firstZvar;
	std::vector<int> files;
	File* inputfile;
	File* outputfile;
};

#endif /* REDUCTION1_H_ */
