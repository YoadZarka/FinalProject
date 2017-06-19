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

class Reduction2 {
public:
	Reduction2(char* path, int32_t delFiles, int32_t delBlocks);
	Reduction2(char* inputPath, char* outputPath, int delFiles, int delBlocks, int op,char* elpParseTime, char* elpSolverTime,int numClas,int numVars);
	virtual ~Reduction2();
	void parser();
	void liteParser();
	std::vector< std::vector< int32_t > > pb2cnf (std::vector< int32_t > literals, int k, int32_t firstAuxVar, std::string op);
	void writeVec (std::vector< std::vector< int32_t > > vec);
	void writeCNF(int32_t delFiles, int32_t delBlocks);
	std::string decodedOutput (int delFiles, int delBlocks);
	void findBlocksInAir ();
	void writeOutputSTDout(char* elpParseTime, char* elpSolverTime,int delFiles, int delBlocks,int numClas,int numVars);
	std::vector< std::vector <int32_t> > MatCondCNF;
	std::vector< std::vector< int32_t > > BAtLeastCNF;
	std::vector< std::vector< int32_t > > FAtMostCNF;
	std::vector < std::vector <int32_t> > BTF;   //blocks to files edges, the index of the outer vector is the block ID
	std::vector<int32_t> files;
	std::vector<int32_t> remainFiles;
	std::vector<int32_t> deletedBlocks;
	std::vector<int32_t> deletedFiles;
	std::vector<int32_t> blocksInAir;
	std::vector<int> blocksSize;
	std::string HTarget;
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
};

#endif /* REDUCTION2_H_ */
