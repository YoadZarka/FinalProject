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

/* The files and blocks are represent by there SN in the input file, first the blocks
 * starts from 0 (as in the input file) and the files are continuous after it. the real
 * file SN stored in "vector<int> files" so we can retrieve from the solver result.
 * Dont forget that the Solver result describe the files we want to keep
 * and blocks we want to delete.
 */
class Reduction1 {
public:
	Reduction1(char* path, int delFiles, int delBlocks);
	Reduction1(char* inputPath, char* outputPath, int delFiles, int delBlocks, int op,char* elpParseTime, char* elpSolverTime);
	virtual ~Reduction1();
	void parser();
	void convert2cnf();
	void print();
	void dnf2cnf (std::vector< std::vector <bool> >& dnf);
	//void encodeCNFDiff ();
	void encodeDNFDiff ();
	int summtion (int delFiles, int delBlocks);
	void writeCNF(int delFiles, int delBlocks);
	void liteParser();
	int fromBin(long n);
	std::string decodedOutput (int delFiles, int delBlocks);
	void writeOutputSTDout (char* elpParseTime, char* elpSolverTime,int delFiles, int delBlocks);
	std::vector<bool> id2dnf (int id);
	std::vector< std::vector <bool> > DNFFile;      //store one dnf files clause
	std::vector< std::vector <bool> > DNFBlocks;	//store one dnf blocks clause
	std::vector< std::vector <bool> > DNFEdges;		//store one dnf edges clause
	std::vector< std::vector <bool> > CNFFile;      //store one cnf files clause
	std::vector< std::vector <bool> > CNFBlocks;	//store one cnf blocks clause
	std::vector< std::vector <bool> > DNFDiff;		//store one dnf difference check clause
	std::vector< std::vector <bool> > CNFDiff;		//store one cnf difference check clause
	std::vector< std::vector <bool> > tempCnf;		//using in the cnf convert function
	std::vector <int> deletedBlocks;  	 //contain all the blocks id for deletion
	std::vector <int> remainFiles;		 //contain all the files id for remaining
	std::vector<int> blocksSize;
	std::string numOfVar;
	std::string numOfClause;
	int TotalblocksSize=0;							//save the total blocks size (bytes) in the output parsing
	int numOfLiterals=0;
	int totalLiterals=0;
	int firstFile=0;
	int firstBlock=0;
	int firstZvar=0;
	std::vector<int> files;
	File* inputfile;
	File* outputfile;
};

#endif /* REDUCTION1_H_ */
