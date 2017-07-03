/*
 * Reduction2.h
 *
 *  Created on: May 21, 2017
 *      Author: yoad
 */

#ifndef REDUCTION2_H_
#define REDUCTION2_H_
#include "File.h"
#include <vector>
#include <cstdint>

/***** Important: All the errors in the system are not handle in the cpp code but in the python side
 * Therefore when an error occurs, a matching message will be print to STDout and the program breaks ******/

/* The files and blocks are represented by there SN in the input file, first the blocks
 * starts from 0 (as in the input file) and the files are continuous after it. the real
 * file SN stored in "vector<int> files" so we can retrieve from the solver result.
 * Dont forget that the Solver result describe the files we want to keep and blocks we want to delete.
 */
class Reduction2 {
public:
	Reduction2(char* path, int32_t delFiles, int32_t delBlocks,char *CnfName);
	Reduction2(char* inputPath, char* outputPath, int delFiles, int delBlocks, int op,char* elpParseTime, char* elpSolverTime
			,int numClas,int numVars,char* CNFSize, char* maxRAMSolver);
	virtual ~Reduction2();
	void parser();
	void liteParser(int op);
	std::vector< std::vector< int32_t > > pb2cnf (std::vector< int32_t > literals, int k, int32_t firstAuxVar, std::string op);
	void writeVec (std::vector< std::vector< int32_t > > vec);
	void writeCNF(int32_t delFiles, int32_t delBlocks,char *CnfName);
	std::string decodedOutput (int delFiles, int delBlocks);
	void findBlocksInAir ();
	void findOrigDelBlocks ();
	void writeOutputSTDout(char* elpParseTime, char* elpSolverTime,int delFiles, int delBlocks,int numClas,int numVars, char* CNFSize, char* maxRAMSolver);
	void writeOutputSaraiGala(char* elpParseTime, char* elpSolverTime,int delFiles, int delBlocks,int numClas,int numVars, char* CNFSize, char* maxRAMSolver);
	void validetionTset();
	std::vector< std::vector <int32_t> > MatCondCNF;	//contain the CNF Material conditional encodes
	std::vector< std::vector< int32_t > > BAtLeastCNF;	//contain the blocks at least condition CNF encodes
	std::vector< std::vector< int32_t > > FAtMostCNF;	//contain the files at most condition CNF encodes
	std::vector < std::vector <int32_t> > BTF;   		//blocks to files edges, the index of the outer vector is the block ID
	std::vector < std::vector <int> > FTB;  			//files to blocks edges, the first object of every inner vector is the File id
	std::vector<int32_t> files;							//contain all the original files id's, the index represents the new file id used for encode and decode
	std::vector<int32_t> remainFiles;					//contain all the original files id for remaining
	std::vector<int32_t> deletedBlocks;					//contain all the original blocks id for deletion
	std::vector<int32_t> deletedFiles;					//contain all the original files id for deletion
	std::vector<int32_t> blocksInAir;					//contain all the original blocks id that deleted and not mark by the solver
	std::vector<int> blocksSize;						//contain all the original blocks sizes, the index represent the block id
	std::string HTarget;
	std::string OrigBlocks;
	std::string solverOUTPUT;
	File* inputfile;
	File* outputfile;
	int32_t firstFile=0;
	int32_t firstBlock=0;
	int32_t firstFresh=0;
	int numOfFSystems=0;
	int firstFS=0;
	int lastFS;
	int32_t TotalblocksSize=0;
	int32_t TotalDelBlockSize=0;
	int32_t DelBlocksBySolver=0;
	int32_t DelBlocksInAir=0;
};

#endif /* REDUCTION2_H_ */
