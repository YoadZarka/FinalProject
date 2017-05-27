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
	virtual ~Reduction2();
	void parser();
	std::vector< std::vector< int32_t > > pb2cnf (std::vector< int32_t > literals, int k, int32_t firstAuxVar, std::string op);
	void writeVec (std::vector< std::vector< int32_t > > vec);
	void writeCNF(int32_t delFiles, int32_t delBlocks);
	std::vector< std::vector <int32_t> > MatCondCNF;
	std::vector< std::vector< int32_t > > BAtLeastCNF;
	std::vector< std::vector< int32_t > > FAtMostCNF;
	std::vector<int32_t> files;
	File* inputfile;
	File* outputfile;
	int32_t firstFile;
	int32_t firstBlock;
	int32_t firstFresh;
};

#endif /* REDUCTION2_H_ */
