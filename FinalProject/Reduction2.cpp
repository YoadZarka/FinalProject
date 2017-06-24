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
#include <algorithm>
#include "pblib/pb2cnf.h"

using namespace std;

Reduction2::Reduction2(char* path, int32_t delFiles, int32_t delBlocks) {
	this->inputfile = new File (path,1);
	if ((this->inputfile->numOfBlocks==0)||(this->inputfile->numOfFiles==0)){
		cout << "The input file doesn't match";
		exit(1);}
	if ((this->inputfile->numOfBlocks<delBlocks)||(this->inputfile->numOfFiles<delFiles)){
		cout << "The k or k' input is bigger then the original input";
		exit(1);}
	parser();
	writeCNF (delFiles,delBlocks);
	cout << "all done!";
}

Reduction2::Reduction2(char* inputPath, char* outputPath, int delFiles, int delBlocks, int op, char* elpParseTime, char* elpSolverTime,int numClas,int numVars,char* CNFSize, char* maxRAMSolver) {
	this->inputfile = new File (inputPath,1);
	if ((this->inputfile->numOfBlocks==0)||(this->inputfile->numOfFiles==0)){
		cout << "The input file doesn't match";
		exit(1);}
	if ((this->inputfile->numOfBlocks<delBlocks)||(this->inputfile->numOfFiles<delFiles)){
		cout << "The k or k' input is bigger then the original input";
		exit(1);}
	liteParser();
	this->outputfile = new File (outputPath,2);
	this->solverOUTPUT=decodedOutput(delFiles,delBlocks);
	if (op==1)
		writeOutputSTDout(elpParseTime, elpSolverTime,delFiles,delBlocks,numClas,numVars, CNFSize, maxRAMSolver);
	if (op==2)
		writeOutputSaraiGala(elpParseTime, elpSolverTime,delFiles,delBlocks,numClas,numVars, CNFSize, maxRAMSolver);
	this->inputfile->~File();
	this->outputfile->~File();
	cout << "all done!";
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
		this->outputfile->writeLine(str2);findOrigDelBlocks ();
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

void Reduction2::liteParser(){
	for (int h=0; h<this->inputfile->numOfBlocks ; h++)
		this->blocksSize.push_back(0);
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
			this->files.push_back(f);  //add the file id to a vector of all files
			line=temp_str.substr(pos+1);
			for (int i=0; i<2 ; i++){
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
				line=line.substr(pos+1);
				pos=line.find(",");
				num=line.substr(0,pos);
				int size;
				stringstream(num) >> size;  //get the block size of b
				line=line.substr(pos+1);
				if (this->blocksSize[b]==0)
					this->blocksSize[b]=size;  //save the block size at b cell in the vector
			}
			line=this->inputfile->getLine();
		}
		else{
			string tmpStr = line.substr(0,7);
			if ( tmpStr == "#Input "){
				size_t pos=line.find(":")+1;
				line = line.substr(pos);
				istringstream stream(line);
				vector <int> filesystems;
				while (line.find(',')!=-1)
				{
					string token;
					filesystems.push_back(stoi(token=line.substr(0,line.find(','))));
					line=line.substr(line.find(',')+1);
				}
				this->numOfFSystems=filesystems.size();
				this->firstFS=filesystems[0];
				this->lastFS=filesystems[filesystems.size()-1];
			}
			if (tmpStr ==("#Target")){
				size_t pos=line.find(":")+1;
				this->HTarget = line.substr(pos);
			}
			if (tmpStr == ("#Origin")){
				size_t pos=line.find(":")+1;
				this->OrigBlocks = line.substr(pos);
			}
			if (line.at(0) == 'B'){
				for (int i=0; i<4 ; i++){
					line=line.substr(line.find(",")+1);
				}
				size_t pos=line.find(",");
				string num = line.substr(0,pos);
				int fn;
				stringstream(num) >> fn;
				vector <int> CFiles;
				for (int i=0 ; i<fn ;i++){
					pos=line.find(",");
					line=line.substr(pos+1);
					pos=line.find(",");
					num=line.substr(0,pos);
					int f;
					stringstream(num) >> f;  //get the file id number to f
					CFiles.push_back(f);
				}
				this->BTF.push_back(CFiles);
				CFiles.clear();
			}
			line=this->inputfile->getLine();
		}

		if (line.at(0) == 'D')
			break;
	}
	for (int j=0 ; j<this->blocksSize.size() ; j++){
		this->TotalblocksSize = this->TotalblocksSize + this->blocksSize[j];
	}
}

void Reduction2::findBlocksInAir (){
	for(int i=0; i<this->BTF.size(); i++){
		int cnt = this->BTF[i].size();
		for(int j=0; j<this->BTF[i].size(); j++){
			if (find(this->deletedFiles.begin(), this->deletedFiles.end(),this->BTF[i][j]) != this->deletedFiles.end())
				cnt--;
		}
		if(cnt == 0 && !(find(this->deletedBlocks.begin(), this->deletedBlocks.end(),i) != this->deletedBlocks.end()))
			this->blocksInAir.push_back(i);
	}
}

string Reduction2::decodedOutput (int delFiles, int delBlocks){
	string line;
	line=this->outputfile->getLine();
	if (line=="UNSAT")
		return "UNSATISFIESABLE";
	if (line=="SAT"){
		line=this->outputfile->getLine();
		for (int i=0 ; i<(this->inputfile->numOfBlocks) ; i++){
			size_t pos=line.find(" ");
			string temp_str=line.substr(0,pos);
			pos++;
			int block_id;
			stringstream(temp_str) >> block_id;  //get the block id +1 number to f
			if (block_id>0)
				this->deletedBlocks.push_back(block_id-1);
			line = line.substr(pos);
		}
		for (int i=0 ; i<this->inputfile->numOfFiles ; i++){
			size_t pos=line.find(" ");
			string temp_str=line.substr(0,pos);
			pos++;
			int file_id;
			stringstream(temp_str) >> file_id;  //get the file id +1 number to f
			if (file_id>0)
				this->deletedFiles.push_back(this->files[file_id-1-this->inputfile->numOfBlocks]);
			if (file_id<0)
				this->remainFiles.push_back(this->files[abs(file_id)-1-this->inputfile->numOfBlocks]);
			line = line.substr(pos);
		}
		findBlocksInAir ();
		for (uint i=0 ; i<this->deletedBlocks.size() ; i++){
			this->TotalDelBlockSize = this->TotalDelBlockSize + this->blocksSize[this->deletedBlocks[i]];
		}
		for (uint i=0 ; i<this->blocksInAir.size() ; i++){
			this->TotalDelBlockSize = this->TotalDelBlockSize + this->blocksSize[this->blocksInAir[i]];
		}
		return "SATISFIESABLE";
	}
	if (line == "***end***")
		return " INDETERMINATE ";
	cout << "Damaged Solver output file";
	exit (1);
}

void Reduction2::findOrigDelBlocks (){
	int32_t BCnt=0;
	string line;
	line=this->inputfile->getLine();
	while (line != "***end***"){
		if (line.at(0) == 'M'){
			size_t pos=line.find(",");
			line = line.substr(pos+1);
			pos=line.find(",");
			string num = line.substr(0,pos);
			int b;
			stringstream(num) >> b;  //get the number of block ID
			if (find(this->deletedBlocks.begin(), this->deletedBlocks.end(),b) != this->deletedBlocks.end()){
				line = line.substr(pos+1);
				pos=line.find(",");
				num = line.substr(0,pos);
				int numOfOrigBlocks;
				stringstream(num) >> numOfOrigBlocks;
				this->DelBlocksBySolver = this->DelBlocksBySolver + numOfOrigBlocks;
			}
			if (find(this->blocksInAir.begin(), this->blocksInAir.end(),b) != this->blocksInAir.end()){
				line = line.substr(pos+1);
				pos=line.find(",");
				num = line.substr(0,pos);
				int numOfOrigBlocks;
				stringstream(num) >> numOfOrigBlocks;
				this->DelBlocksInAir = this->DelBlocksInAir + numOfOrigBlocks;
			}
			line=this->inputfile->getLine();
		}
		else
			line=this->inputfile->getLine();
	}
}

void Reduction2::writeOutputSTDout(char* elpParseTime, char* elpSolverTime,int delFiles, int delBlocks,int numClas
		,int numVars, char* CNFSize, char* maxRAMSolver){
	string strSize = "            ";
	string ePT(elpParseTime);
	string eST(elpSolverTime);
	ePT.resize(4);
	eST.resize(4);
	string Dfiles =to_string(delFiles);
	string Dblocks =to_string(delBlocks);
	string NClas = to_string(numClas);
	string NVars = to_string(numVars);
	string NBlocks = to_string(this->inputfile->numOfBlocks);
	string origSize = to_string(this->TotalblocksSize/(pow(2,30)));
	string totalDelFiles = to_string(this->files.size()-this->remainFiles.size());
	string origNumFiles = to_string(this->files.size());
	string DelSize = to_string(this->TotalDelBlockSize/(pow(2,30)));
	string SolverRam (maxRAMSolver);
	size_t pos = SolverRam.find('M');
	SolverRam.insert(pos," ");
	string cnfSize (CNFSize);
	string::size_type sZ;
	int CNFS = stoi(cnfSize,&sZ);
	CNFS=CNFS/1000;
	cnfSize = to_string(CNFS);

	unsigned sz = strSize.size();
	while (ePT.size()!= sz)  ePT.insert(0," ");
	while (eST.size()!=sz)  eST.insert(0," ");
	while (Dfiles.size()!=sz)  Dfiles.insert(0," ");
	while (Dblocks.size()!=sz)  Dblocks.insert(0," ");
	while (NClas.size()!=sz)  NClas.insert(0," ");
	while (NVars.size()!=sz)  NVars.insert(0," ");
	while (NBlocks.size()!=sz)  NBlocks.insert(0," ");
	while (origSize.size()!=sz)  origSize.insert(0," ");
	while (totalDelFiles.size()!=sz)  totalDelFiles.insert(0," ");
	while (origNumFiles.size()!=sz)  origNumFiles.insert(0," ");
	while (DelSize.size()!=sz)  DelSize.insert(0," ");
	while (SolverRam.size()!=sz)  SolverRam.insert(0," ");
	while (cnfSize.size()!=sz)  cnfSize.insert(0," ");


	cout << "=========================[ Problem Statistics ]========================="<<endl;
		cout <<endl;
		cout << "								[ Reduction 2 selected ]"<<endl;
		cout <<endl;
		if (this->solverOUTPUT=="SATISFIESABLE")
			cout << "									SATISFIESABLE"<<endl;
		else
			cout << "								***"<<this->solverOUTPUT<<"***"<<endl;
		cout <<endl;
		cout << "	Number of Files for Deletion:	"<<Dfiles<<endl;
		cout << "	Number of Blocks for Deletion: "<<Dblocks<<endl;
		cout << "	Original number of blocks:		"<<NBlocks<<endl;
		cout << "	Number of variables:			"<<NVars<<endl;
		cout << "	Number of clauses:				"<<NClas<<endl;
		cout << "	Parse time:						"<<ePT<<  " s"<<endl;
		cout << "	Solver time:						"<<eST<<" s"<<endl;
		cout << "	Solver used RAM:				"<<SolverRam<<endl;
		cout << "	Solver CNF input size:			"<<cnfSize<<" KB"<<endl;
		cout <<endl;
		if (this->solverOUTPUT=="SATISFIESABLE"){
			cout << "==========================[ Eliminated files ]=========================="<<endl;
			cout << "	Original # Files	|	Original Size	|	Deleted # Files	|	Deleted Size	|"<<endl;
			cout << "	"<< origNumFiles <<"   	|	"<< origSize <<" GB	|	"<<  totalDelFiles <<"   	|	"<< DelSize <<" GB	|"<<endl;
			cout << "===================================================================="<<endl;
		}
		else
			cout << "===================================================================="<<endl;
	string solOut = "Solution_Reduction2_heuristic_targetBlocks_"+this->HTarget.substr(1)+"_filesystems_"+to_string(this->firstFS)+"_to_"+to_string(this->lastFS)+"_output";
	char *cstr = &solOut[0u];
	File* solutionFile = new File (cstr);
	solutionFile->writeLine("============================[ Problem Statistics ]=============================");
	solutionFile->writeLine("|                                                                             |");
	solutionFile->writeLine("|                          [ Reduction 2 selected ]                           |");
	solutionFile->writeLine("|                                                                             |");
	stringstream ss1;
	if (this->solverOUTPUT=="SATISFIESABLE")
		solutionFile->writeLine("|                                SATISFIESABLE                                |");
	else{

		ss1 << 				   "|                           ***"<<this->solverOUTPUT<<"***                             |";
		solutionFile->writeLine(ss1.str());
		ss1.str("");
	}
	solutionFile->writeLine("|                                                                             |");
	ss1 << 				   "|  Number of Files for Deletion:  "<<Dfiles<< "                                |";
	solutionFile->writeLine(ss1.str());
	ss1.str("");
	ss1 << 				   "|  Number of Blocks for Deletion: "<<Dblocks<<"                                |";
	solutionFile->writeLine(ss1.str());
	ss1.str("");
	ss1 << 				   "|  Original number of blocks:     "<<NBlocks<<"                                |";
	solutionFile->writeLine(ss1.str());
	ss1.str("");
	ss1 << 				   "|  Number of variables:           "<<NVars<<"                                |";
	solutionFile->writeLine(ss1.str());
	ss1.str("");
	ss1 << 				   "|  Number of clauses:             "<<NClas<<"                                |";
	solutionFile->writeLine(ss1.str());
	ss1.str("");
	ss1 << 				   "|  Parse time:                    "<<ePT<<    " s                              |";
	solutionFile->writeLine(ss1.str());
	ss1.str("");
	ss1 << 				   "|  Solver time:	                  "<<eST<<    " s                              |";
	solutionFile->writeLine(ss1.str());
	ss1.str("");
	ss1 << 				   "|  Solver used RAM:                  "<<SolverRam<<    "                             |";
	solutionFile->writeLine(ss1.str());
	ss1.str("");
	ss1 << 				   "|  Solver CNF input size:         "<<cnfSize<<    " KB                             |";
	solutionFile->writeLine(ss1.str());
	ss1.str("");
	solutionFile->writeLine("|                                                                             |");
	if (this->solverOUTPUT=="SATISFIESABLE"){
		solutionFile->writeLine("=============================[ Eliminated files ]==============================");
		solutionFile->writeLine("| Original # Files |  Original Size   |   Deleted # Files   |   Deleted Size  |");
		ss1 <<   "|   "<< origNumFiles <<"   |  "<< origSize <<" GB |    "<<  totalDelFiles <<"     | "<< DelSize <<" GB |";
		solutionFile->writeLine(ss1.str());
		ss1.str("");
		solutionFile->writeLine("===============================================================================");
		solutionFile->writeLine("                                                                               ");
		solutionFile->writeLine("============================[ Deleted files ID's ]=============================");
		for (int i=0 ; i<this->deletedFiles.size() ; i++){
			ss1 <<   this->deletedFiles[i];
			solutionFile->writeLine(ss1.str());
			ss1.str("");
		}
	}
	else
		solutionFile->writeLine("===============================================================================");
	solutionFile->~File();

}

void Reduction2::writeOutputSaraiGala(char* elpParseTime, char* elpSolverTime,int delFiles, int delBlocks,int numClas,int numVars, char* CNFSize, char* maxRAMSolver){
	string ePT(elpParseTime);
	string eST(elpSolverTime);
	string maxRAMS(maxRAMSolver);
	string cnfSize(CNFSize);
	findOrigDelBlocks ();
	string MarkedBlocksBySover = to_string(this->DelBlocksBySolver);
	string FilesDelBySolver = to_string(this->deletedFiles.size());
	string totalDelBlocks = to_string(this->DelBlocksBySolver+this->DelBlocksInAir);
	string::size_type sZ;
	int CNFS = stoi(cnfSize,&sZ);
	CNFS=CNFS/1000;
	cnfSize = to_string(CNFS);
	ePT.resize(4);
	eST.resize(4);
	cout << "block,"+to_string(this->numOfFSystems)+","+to_string(this->firstFS)+","+to_string(this->lastFS)+","+this->HTarget+","
			+to_string(this->inputfile->numOfFiles)+","+to_string(this->inputfile->numOfBlocks)+","+to_string(delFiles)+","
			+to_string(delBlocks)+","+ePT+" s,"+cnfSize+" KB,"+eST+" s,"+maxRAMS+","+FilesDelBySolver+","+MarkedBlocksBySover+","
			+totalDelBlocks+","<<endl;
}
