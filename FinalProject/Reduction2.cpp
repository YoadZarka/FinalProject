/*
 * Reduction2.cpp
 *
 *  Created on: May 21, 2017
 *      Author: yoad
 */

#include "Reduction2.h"
#include <sstream>
#include <iostream>
#include <math.h>
#include <string>
#include "pblib/pb2cnf.h"

//using namespace PBLib;
using namespace std;

Reduction2::Reduction2(char* path, int32_t delFiles, int32_t delBlocks) {
	this->inputfile = new File (path,1);
	parser();
	writeCNF (delFiles,delBlocks);
	cout << "all done!!"<<endl;
}

Reduction2::~Reduction2() {
	// TODO Auto-generated destructor stub
}

void Reduction2::parser(){
	string line;
	line=this->inputfile->getLine();
	while (line != "***end***"){
		if (line.at(0) == 'F'){
			size_t pos=line.find(",")+1;
			string temp_str=line.substr(pos);
			pos=temp_str.find(",");
			string num = temp_str.substr(0,pos);
			int f;
			stringstream(num) >> f;  //get the file id number to f
			//cout <<"File" << f <<endl;
			this->files.push_back(f);  //add the file id to a vector of all files
			line=temp_str.substr(pos+1);
			for (int i=0; i<2 ; i++){// TODO Auto-generated constructor stub
				size_t pos=line.find(",")+1;
				string temp_str=line.substr(pos);
				line = temp_str;
			}
			pos=line.find(",");
			num = line.substr(0,pos);
			int bn;
			stringstream(num) >> bn;  //get the number of connected blocks to the file in same line
			pos=line.find(",")+1;
			temp_str=line.substr(pos);
			line=temp_str.substr(0);
			for (int i=0 ; i<bn ;i++){
				pos=line.find(",");
				num=line.substr(0,pos);
				int b;
				stringstream(num) >> b;  //get the block id number to b
				//call to kidod to edegs dnf f and b
				vector<int32_t> tmp;
				tmp.push_back(-(b+1));  //write the block id+1 because zero is not allowed
				tmp.push_back(this->files.size()+this->inputfile->numOfBlocks);
				this->MatCondCNF.push_back(tmp);
				tmp.clear();
				line=line.substr(pos+1);
				pos=line.find(",");
				line=line.substr(pos+1);
			}
			line=this->inputfile->getLine();
		}
		else
			line=this->inputfile->getLine();

		if (line.at(0) == 'B'){
			break;
		}
	}
}

vector< vector< int32_t > > Reduction2::pb2cnf (vector< int32_t > literals, int k, int32_t firstAuxVar, string op){
	   PB2CNF pb2cnf;
	   vector< vector< int32_t > > formula;
	   if (op=="AtLeast")
		   this->firstFresh = pb2cnf.encodeAtLeastK(literals, k, formula, firstAuxVar) + 1;
	   if (op=="AtMost")
		   this->firstFresh = pb2cnf.encodeAtMostK(literals, k, formula, firstAuxVar) + 1;
	   return formula;
}

void Reduction2::writeVec (vector< vector< int32_t > > vec){
	stringstream ss2;
	string str2;
	for (int32_t i=0 ; i<vec.size() ; i++){
			for (int j=0 ; j<vec[i].size() ; j++){
				ss2 << vec[i][j] <<" ";
			}
			ss2 << "0";
			str2=ss2.str();
			this->outputfile->writeLine(str2);
			ss2.str("");
			str2="";
	}
}

void Reduction2::writeCNF(int32_t delFiles, int32_t delBlocks){
	this->firstBlock = 1;
	this->firstFile = this->firstBlock+this->inputfile->numOfBlocks;
	this->firstFresh = this->firstFile+this->inputfile->numOfFiles;
	vector< int32_t > literals;
	for (int32_t i=1 ; i<this->inputfile->numOfBlocks+1 ; i++){
		literals.push_back(i);
	}
	this->BAtLeastCNF=pb2cnf(literals,delBlocks,this->firstFresh,"AtLeast");
	literals.clear();
	for (int32_t i=1 ; i<this->inputfile->numOfFiles+1 ; i++){
			literals.push_back(i+this->inputfile->numOfBlocks);
	}
	this->FAtMostCNF=pb2cnf(literals,delFiles,this->firstFresh,"AtMost");
	int32_t numOfClauses = this->BAtLeastCNF.size()+this->FAtMostCNF.size()+this->MatCondCNF.size();
	string outputName = "SAT_R2_input.cnf";
	char *cstr = &outputName[0u];
	this->outputfile = new File (cstr);
	this->outputfile->writeLine("c  SAT_R2_input.cnf");
	this->outputfile->writeLine("c");
	stringstream ss1;
	string str1;
	ss1 << "p cnf "<< this->firstFresh <<" "<<numOfClauses;
	str1=ss1.str();
	this->outputfile->writeLine(str1);
	ss1.str("");
	str1="";
	writeVec(this->MatCondCNF);
	writeVec(this->BAtLeastCNF);
	writeVec(this->FAtMostCNF);
	this->outputfile->~File();
}
