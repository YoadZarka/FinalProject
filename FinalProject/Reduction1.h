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

/* The files and blocks are represented by there SN in the input file, first the blocks
 * starts from 0 (as in the input file) and the files are continuous after it. the real
 * file SN stored in "vector<int> files" so we can retrieve from the solver result.
 * Dont forget that the Solver result describe the files we want to keep and blocks we want to delete.
 */
class Reduction1 {
public:
	Reduction1(char* path, int delFiles, int delBlocks,char *CnfName);
	Reduction1(char* inputPath, char* outputPath, int delFiles, int delBlocks, int op,char* elpParseTime, char* elpSolverTime
			,int numClas,int numVars,char* CNFSize, char* maxRAMSolver);
	virtual ~Reduction1();
	void parser();
	void convert2cnf();
	void print();
	void dnf2cnf (std::vector< std::vector <bool> >& dnf);
	//void encodeCNFDiff ();
	void encodeDNFDiff ();
	int summation(int delFiles, int delBlocks);
	void writeCNF(int delFiles, int delBlocks,char *CnfName);
	void liteParser(int op);
	int fromBin(long n);
	std::string decodedOutput (int delFiles, int delBlocks);
	void findOrigDelBlocks ();
	void writeOutputSTDout(char* elpParseTime, char* elpSolverTime,int delFiles, int delBlocks,int numClas,int numVars, char* CNFSize, char* maxRAMSolver);
	std::vector<bool> id2dnf (int id);
	void findBlocksInAir ();
	void writeOutputSaraiGala(char* elpParseTime, char* elpSolverTime,int delFiles, int delBlocks,int numClas,int numVars, char* CNFSize, char* maxRAMSolver);
	void validetionTset();
	std::vector< std::vector <bool> > DNFFile;      //store one dnf files clause
	std::vector< std::vector <bool> > DNFBlocks;	//store one dnf blocks clause
	std::vector< std::vector <bool> > DNFEdges;		//store one dnf edges clause
	std::vector< std::vector <bool> > CNFFile;      //store one cnf files clause
	std::vector< std::vector <bool> > CNFBlocks;	//store one cnf blocks clause
	std::vector< std::vector <bool> > DNFDiff;		//store one dnf difference check clause
	std::vector< std::vector <bool> > CNFDiff;		//store one cnf difference check clause
	std::vector< std::vector <bool> > tempCnf;		//using in the cnf convert function
	std::vector<int> files;							//contain all the original files id's, the index represents the new file id used for encode and decode
	std::vector <int> deletedBlocks;  	 			//contain all the original blocks id for deletion
	std::vector <int> remainFiles;		  			//contain all the original files id for remaining
	std::vector <int> deletedFiles;		  			//contain all the original files id for deletion
	std::vector <int> blocksInAir;		  			//contain all the original blocks id that deleted and not mark by the solver
	std::vector <int> blocksSize;					//contain all the original blocks sizes, the index represent the block id
	std::vector < std::vector <int> > BTF;   		//blocks to files edges, the index of the outer vector is the block ID
	std::vector < std::vector <int> > FTB;   		//files to blocks edges, the first object of every inner vector is the File id
	std::string numOfVar;
	std::string numOfClause;
	std::string HTarget;
	std::string solverOUTPUT;
	long TotalblocksSize=0;							//save the total blocks size (bytes) in the output parsing
	long TotalDelBlockSize=0;
	int numOfLiterals=0;
	int totalLiterals=0;
	int firstFile=0;
	int firstBlock=0;
	int firstZvar=0;
	int numOfFSystems=0;
	int firstFS =0;
	int lastFS =0;
	int32_t DelBlocksBySolver=0;
	int32_t DelBlocksInAir=0;
	File* inputfile;
	File* outputfile;
};

#endif /* REDUCTION1_H_ */
