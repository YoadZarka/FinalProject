/*
 * Reduction1.cpp
 *
 *  Created on: Apr 14, 2017
 *      Author: yoad
 */

#include "Reduction1.h"
#include <sstream>
#include <math.h>
#include <tgmath.h>
#include <string>
#include <bitset>
#include <algorithm>

using namespace std;

/***** Important: All the errors in the system are not handle in the cpp code but in the python side
 * Therefore when an error occurs, a matching message will be print to STDout and the program breaks ******/

/* Constructor for the CNF encoding and writing CNF file phase
 * get input file txt format, k k' parameters. Encode the CNF clauses and write the cnf file according to Reduction 1*/
Reduction1::Reduction1(char* path, int delFiles, int delBlocks,char *CnfName) {
	this->inputfile = new File (path,1);
	if ((this->inputfile->numOfBlocks==0)||(this->inputfile->numOfFiles==0)){
		cout << "The input file doesn't match";
		exit(1);}
	this->numOfLiterals = (int)(log2((double)(inputfile->numOfBlocks+inputfile->numOfFiles))+1);
	if (this->numOfLiterals>63){
		cout << "Too many litterals, can't calculate";
		exit(1);}
	if (this->inputfile->numOfFiles<delFiles){
		cout << "The k files input is bigger then the original files";
		exit(1);}
	if (this->inputfile->numOfBlocks<delBlocks){
		cout << "The k' blocks input is bigger then the original blocks";
		exit(1);}
	parser();
	this->inputfile->~File();
	encodeDNFDiff ();
	convert2cnf();
	//encodeCNFDiff (); 				exp version do not use
	writeCNF(delFiles, delBlocks,CnfName);
	cout << "all done!";
}

/* Constructor for decoding the solver result according to Reduction 1
 * get a solver result txt file, input txt file, k k' parameters, details from solver output (as strings).
 * Decode the output to set of files and block for deletion, write the summary result to txt file and STDout*/
Reduction1::Reduction1(char* inputPath, char* outputPath, int delFiles, int delBlocks, int op, char* elpParseTime
		, char* elpSolverTime,int numClas,int numVars,char* CNFSize, char* maxRAMSolver)
{
	this->inputfile = new File (inputPath,1);
	if ((this->inputfile->numOfBlocks==0)||(this->inputfile->numOfFiles==0)){
		cout << "The input file doesn't match";
		exit(1);}
	this->numOfLiterals = (int)(log2((double)(inputfile->numOfBlocks+inputfile->numOfFiles))+1);
	if (this->numOfLiterals>63){
		cout << "Too many litterals, can't calculate";
		exit(1);}
	if (this->inputfile->numOfFiles<delFiles){
		cout << "The k files input is bigger then the original files";
		exit(1);}
	if (this->inputfile->numOfBlocks<delBlocks){
		cout << "The k' blocks input is bigger then the original blocks";
		exit(1);}
	liteParser(op);
	this->outputfile = new File (outputPath,2);
	this->solverOUTPUT=decodedOutput(delFiles,delBlocks);
	if (op==1)
		writeOutputSTDout(elpParseTime, elpSolverTime,delFiles,delBlocks,numClas,numVars, CNFSize, maxRAMSolver);
	if (op==2)
		writeOutputSaraiGala(elpParseTime, elpSolverTime,delFiles,delBlocks,numClas,numVars, CNFSize, maxRAMSolver);
	if (op==3){
		writeOutputSTDout(elpParseTime, elpSolverTime,delFiles,delBlocks,numClas,numVars, CNFSize, maxRAMSolver);
		validetionTset();
	}
	this->inputfile->~File();
	this->outputfile->~File();
	cout << "all done!";
}

Reduction1::~Reduction1() {}

/* This function convert one DNF clause into CNF form by adding extra variable as the subclauses number
 * according to the algorithm explained in section 2.4.1 in the project paper */
void Reduction1::dnf2cnf (vector< vector <bool> >& dnf){
	vector <bool> subcnf;
	int numOfExtVar=dnf.size();
	for (int i=0; i<numOfExtVar ; i++){
		subcnf.push_back(true);				//push z=1 as number of clauses
	}
	this->tempCnf.push_back(subcnf);
	subcnf.clear();
	for (int i=0; i<numOfExtVar ; i++){
		int literalsInClause = dnf[i].size();
		for (int j=0; j<literalsInClause ; j++){
			subcnf.push_back(false);		// push ~z
			subcnf.push_back(dnf[i][j]);    // push literal j of each clause
			this->tempCnf.push_back(subcnf);
			subcnf.clear();
		}
	}
}

/*take a file system txt file input and encode the files, blocks & edges clauses in DNF form stored in 'DNFBlocks' 'DNFFile' 'DNFEdges' respectively
 * During the process the original files id's saved in 'files' vector and the files gets a new id continuously after blocks */
void Reduction1::parser(){
	vector<bool> blocks (this->inputfile->numOfBlocks,false);
	string line;
	line=this->inputfile->getLine();
	while (line != "***end***"){
		if (line.at(0) == 'F'){
			size_t pos=line.find(",")+1;
			string temp_str=line.substr(pos);
			pos=temp_str.find(",");
			string num = temp_str.substr(0,pos);
			int f;
			stringstream(num) >> f;  	//get the file id number to f
			//cout <<"File" << f <<endl; 	 //for debugging
			this->files.push_back(f);  //add the file id to a vector of all files
			this->DNFFile.push_back(id2dnf(this->files.size()+this->inputfile->numOfBlocks));  //call the id2dnf to file dnf with the file index of f + number of blocks
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
				if (blocks[b]==false){
					this->DNFBlocks.push_back(id2dnf(b));  //call to id2dnf to blocks dnf
					blocks[b]=true;
				}

				vector<bool> tmpF = this->DNFFile[this->DNFFile.size()-1];
				vector<bool> tmpB = id2dnf(b);
				tmpF.insert(tmpF.end(),tmpB.begin(),tmpB.end());
				this->DNFEdges.push_back(tmpF);
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

/* Auxiliary function for converting an int id number to its DNF binary representation - gets int id and return boolean dnf vector*/
vector<bool> Reduction1::id2dnf (int id){
	vector<bool> clause;
	std::string binary = std::bitset<64>(id).to_string();
	for (int i=0 ; i<this->numOfLiterals ; i++){
		if (binary[64-this->numOfLiterals+i]=='1')
			clause.push_back(true);
		else
			clause.push_back(false);
	}
	return clause;
}


void Reduction1::encodeDNFDiff (){
	vector<bool> xordnf1;
	xordnf1.push_back(true);
	xordnf1.push_back(false);
	vector<bool> xordnf2;
	xordnf2.push_back(false);
	xordnf2.push_back(true);
	for (int i=0 ; i<this->numOfLiterals ; i++){
		this->DNFDiff.push_back(xordnf1);
		this->DNFDiff.push_back(xordnf2);
	}
}

/**/
void Reduction1::convert2cnf(){
	dnf2cnf(this->DNFFile);
	this->CNFFile=this->tempCnf;
	/*clean the temp vector*/
	for (uint i=0 ; i<tempCnf.size() ; i++){
		this->tempCnf[i].clear();
	}
	this->tempCnf.clear();
	/*						*/
	dnf2cnf(this->DNFBlocks);
	this->CNFBlocks=this->tempCnf;
	/*clean the temp vector*/
	for (uint i=0 ; i<tempCnf.size() ; i++){
		this->tempCnf[i].clear();
	}
	this->tempCnf.clear();
	/*						*/
	dnf2cnf(this->DNFDiff);
	this->CNFDiff=this->tempCnf;

	for (uint i=0 ; i<this->DNFEdges.size() ; i++){     //Making a negation of the edges clause by de Morgan Law
		this->DNFEdges[i].flip();
	}
}

/*Auxiliary function for summation a Sigma like described in Redution 1 formula in section 2.4.1 in the project paper*/
int Reduction1::summation (int delFiles, int delBlocks){
	int sum=0;
	for (int i=0 ; i<(this->inputfile->numOfFiles-delFiles) ; i++){    		 // i=0 to m-k
		for (int j=i+1 ; j<(this->inputfile->numOfFiles-delFiles) ; j++){
			sum++;
		}
	}
	for (int i=0 ; i<(delBlocks) ; i++){    		 // i=0 to k'
		for (int j=i+1 ; j<(delBlocks) ; j++){
			sum++;
		}
	}
	return sum;
}

/*Main function for writing the CNF file in DIMASC format according to k k' parameters and the reduction conditions
 * get the parameters k, k', file name and write the file*/
void Reduction1::writeCNF(int delFiles, int delBlocks,char *CnfName){
	int zBlnum=CNFBlocks[0].size();      //number of z for the blocks clause
	int numOfDiff = summation(delFiles,delBlocks);
	this->totalLiterals=(this->numOfLiterals*((this->inputfile->numOfFiles-delFiles)+delBlocks))+(this->inputfile->numOfFiles*(this->inputfile->numOfFiles-delFiles))+(zBlnum*delBlocks)+((this->numOfLiterals*2)*numOfDiff);
	this->firstFile =1;
	this->firstBlock =(numOfLiterals*(this->inputfile->numOfFiles-delFiles))+1;
	this->firstZvar =(this->numOfLiterals*((this->inputfile->numOfFiles-delFiles)+delBlocks))+1;
	uint numOfClauses = (this->CNFFile.size()*(this->inputfile->numOfFiles-delFiles)) + (this->CNFBlocks.size()*delBlocks) + (this->DNFEdges.size()*((this->inputfile->numOfFiles-delFiles)*delBlocks) + ((1+4*this->numOfLiterals)*numOfDiff));
	this->outputfile = new File (CnfName);
	this->outputfile->writeLine("c  SAT_R1_input.cnf");
	this->outputfile->writeLine("c");
	stringstream ss1;
	string str1;
	ss1 << "p cnf "<< this->totalLiterals <<" "<<numOfClauses;
	str1=ss1.str();
	this->outputfile->writeLine(str1);
	ss1.str("");
	str1="";
	/* writing the cnf files clause to the cnf solver input file*/
	for (int i=0 ; i<(this->inputfile->numOfFiles-delFiles) ; i++){
		stringstream ss;
		string str;
		for (uint j=0 ; j<this->CNFFile[0].size() ; j++){
			ss << (this->firstZvar+(i*this->inputfile->numOfFiles)+j) << " ";
		}
		ss << "0";
		str=ss.str();
		this->outputfile->writeLine(str);
		ss.str("");
		str="";

		int n = this->CNFFile.size();
		for (int j=1 ; j<n ; j++){
			if (this->CNFFile[j][0]==true){
				ss << this->firstZvar+(i*this->inputfile->numOfFiles)+((j-1)/this->numOfLiterals)<<" ";
			}
			else{
				ss << -(this->firstZvar+(i*this->inputfile->numOfFiles)+((j-1)/this->numOfLiterals))<<" ";
			}
			if (this->CNFFile[j][1]==true){
				ss << firstFile+(i*this->numOfLiterals)+((j-1)%this->numOfLiterals)<<" ";
			}
			else{
				ss << -(firstFile+(i*this->numOfLiterals)+((j-1)%this->numOfLiterals))<<" ";
			}
			ss << "0";
			str=ss.str();
			this->outputfile->writeLine(str);
			ss.str("");
			str="";
		}
	}

	/* writing the cnf blocks clause to the cnf solver input file*/
	this->firstZvar =this->firstZvar +((this->inputfile->numOfFiles-delFiles)*this->CNFFile[0].size());
	for (int i=0 ; i<(delBlocks) ; i++){
		stringstream ss;
		string str;
		for (uint j=0 ; j<this->CNFBlocks[0].size() ; j++){
			ss << (this->firstZvar+(i*zBlnum)+j) << " ";
		}
		ss << "0";
		str=ss.str();
		this->outputfile->writeLine(str);
		ss.str("");
		str="";

		int n = this->CNFBlocks.size();
		for (int j=1 ; j<n ; j++){
			if (this->CNFBlocks[j][0]==true){
				ss << this->firstZvar+(i*zBlnum)+((j-1)/this->numOfLiterals)<<" ";
			}
			else{
				ss << -(this->firstZvar+(i*zBlnum)+((j-1)/this->numOfLiterals))<<" ";
			}
			if (this->CNFBlocks[j][1]==true){
				ss << this->firstBlock+(i*this->numOfLiterals)+((j-1)%this->numOfLiterals)<<" ";
			}
			else{
				ss << -(this->firstBlock+(i*this->numOfLiterals)+((j-1)%this->numOfLiterals))<<" ";
			}
			ss << "0";
			str=ss.str();
			this->outputfile->writeLine(str);
			ss.str("");
			str="";
		}
	}

	/* writing the cnf edges clause to the cnf solver input file*/
	for (int i=0 ; i<(this->inputfile->numOfFiles-delFiles) ; i++){    		 // i=0 to m-k
		stringstream ss;
		string str;
		for (int j=0 ; j<delBlocks ; j++){									// j=0 to k'
			for (int s=0 ; s<(int)this->DNFEdges.size() ; s++){				// s=0 to num of edges
				for (int u=1 ; u<(int)this->DNFEdges[s].size()+1 ; u++){	// u=1 to numOfLiterals*2
					if ((u / (this->numOfLiterals+1))==0){   				//this is file encode
						if (this->DNFEdges[s][u-1]==1)  ss << this->firstFile+(i*this->numOfLiterals)+((u-1)%this->numOfLiterals)<<" ";
						else ss << -(this->firstFile+(i*this->numOfLiterals)+((u-1)%this->numOfLiterals))<<" ";
					}
					else{
						if (this->DNFEdges[s][u-1]==1)  ss << this->firstBlock+(j*this->numOfLiterals)+((u-1)%this->numOfLiterals)<<" ";
						else ss << -(this->firstBlock+(j*this->numOfLiterals)+((u-1)%this->numOfLiterals))<<" ";
					}
				}
				ss << "0";
				str=ss.str();
				this->outputfile->writeLine(str);
				ss.str("");
				str="";
			}
		}
	}

	/* writing the cnf files difference clause to the cnf solver input file*/
	this->firstZvar =this->firstZvar +(delBlocks*this->CNFBlocks[0].size());
	int numOfZ=0;
	for (int i=0 ; i<(this->inputfile->numOfFiles-delFiles) ; i++){    		 // i=0 to m-k
		stringstream ss;
		string str;
		for (int j=i+1 ; j<(this->inputfile->numOfFiles-delFiles) ; j++){	// j=i+1 to m-k
			//write the z clause
			for (uint z=0 ; z<this->CNFDiff[0].size() ; z++){
				ss << (this->firstZvar + numOfZ) << " ";
				numOfZ++;
			}
			ss << "0";
			str=ss.str();
			this->outputfile->writeLine(str);
			ss.str("");
			str="";
			for (int s=1 ; s<(this->numOfLiterals*2)+1 ; s++){
				int temp=numOfZ -(this->numOfLiterals*2)+s-1;
				if (this->CNFDiff[(s-1)*2+1][1]==true){
					ss << -(this->firstZvar + temp) << " ";
					ss << this->firstFile + (i*this->numOfLiterals)+((s-1)/2)<<" ";  //literal a
				}
				else{
					ss << -(this->firstZvar + temp) << " ";
					ss << -(this->firstFile + (i*this->numOfLiterals)+((s-1)/2))<<" ";  //literal ~a
				}
				ss << "0";
				str=ss.str();
				this->outputfile->writeLine(str);
				ss.str("");
				str="";
				if (this->CNFDiff[(s-1)*2+2][1]==true){
					ss << -(this->firstZvar + temp) << " ";
					ss << this->firstFile + (j*this->numOfLiterals)+((s-1)/2)<<" ";  //literal b
				}
				else{
					ss << -(this->firstZvar + temp) << " ";
					ss << -(this->firstFile + (j*this->numOfLiterals)+((s-1)/2))<<" ";  //literal ~b
				}
				ss << "0";
				str=ss.str();
				this->outputfile->writeLine(str);
				ss.str("");
				str="";
			}
		}
	}

	/* writing the cnf blocks difference clause to the cnf solver input file*/
	this->firstZvar =this->firstZvar + numOfZ;
	numOfZ=0;
	for (int i=0 ; i<(delBlocks) ; i++){    		 // i=0 to k'
		stringstream ss;
		string str;
		for (int j=i+1 ; j<(delBlocks) ; j++){	// j=i+1 to k'
			//write the z clause
			for (uint z=0 ; z<this->CNFDiff[0].size() ; z++){
				ss << (this->firstZvar + numOfZ) << " ";
				numOfZ++;
			}
			ss << "0";
			str=ss.str();
			this->outputfile->writeLine(str);
			ss.str("");
			str="";
			for (int s=1 ; s<(this->numOfLiterals*2)+1 ; s++){
				int temp=numOfZ -(this->numOfLiterals*2)+s-1;
				if (this->CNFDiff[(s-1)*2+1][1]==true){
					ss << -(this->firstZvar + temp) << " ";
					ss << this->firstBlock + (i*this->numOfLiterals)+((s-1)/2)<<" ";  //literal a
				}
				else{
					ss << -(this->firstZvar + temp) << " ";
					ss << -(this->firstBlock + (i*this->numOfLiterals)+((s-1)/2))<<" ";  //literal ~a
				}
				ss << "0";
				str=ss.str();
				this->outputfile->writeLine(str);
				ss.str("");
				str="";
				if (this->CNFDiff[(s-1)*2+2][1]==true){
					ss << -(this->firstZvar + temp) << " ";
					ss << this->firstBlock + (j*this->numOfLiterals)+((s-1)/2)<<" ";  //literal b
				}
				else{
					ss << -(this->firstZvar + temp) << " ";
					ss << -(this->firstBlock + (j*this->numOfLiterals)+((s-1)/2))<<" ";  //literal ~b
				}
				ss << "0";
				str=ss.str();
				this->outputfile->writeLine(str);
				ss.str("");
				str="";
			}
		}
	}

	this->outputfile->~File();
}

/*Take a file system txt file input and parse from it the desire data for the solver output decode
 * The needed Data is: blocks sizes - stored in blocksSize vector
 * block to files edges - stored in BTF vector, files to block edges - stored in FTB vector
 * and meta data about the input file system*/
void Reduction1::liteParser(int op){
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
			vector <int> temp;
			temp.push_back(f);
			for (int i=0 ; i<bn ;i++){
				pos=line.find(",");
				num=line.substr(0,pos);
				int b;
				stringstream(num) >> b;  //get the block id number to b
				if (op==3){
					temp.push_back(b);
				}
				line=line.substr(pos+1);
				pos=line.find(",");
				num=line.substr(0,pos);
				int size;
				stringstream(num) >> size;  //get the block size of b
				line=line.substr(pos+1);
				if (this->blocksSize[b]==0)
					this->blocksSize[b]=size;  //save the block size at b cell in the vector
			}
			if (op==3){
				this->FTB.push_back(temp);
			}
			temp.clear();
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

			if ((line.at(0) == 'B')&&(op==2)){
				for (int i=0; i<3 ; i++){
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

/* convert a binary number to its decimal representation */
int Reduction1::fromBin(long n){
	long factor = 1;
	long total = 0;

	while (n != 0)
	{
		total += (n%10) * factor;
		n /= 10;
		factor *= 2;
	}

	return (int)total;
}

/* The function finds if there are blocks in air, which means there is no files attached to these blocks
 * and store them in blocksInAir vector*/
void Reduction1::findBlocksInAir (){
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

/* The function decode a solver output file and return SAT, UNSAT or INDET string
 * if SAT then store the results in deletedBlocks and deletedFiles vectors*/
string Reduction1::decodedOutput (int delFiles, int delBlocks){
	string line;
	line=this->outputfile->getLine();
	if (line=="UNSAT")
		return "UNSATISFIABLE";
	if (line=="SAT"){
		line=this->outputfile->getLine();
		for (int i=0 ; i<(this->inputfile->numOfFiles-delFiles) ; i++){
			stringstream ss;
			for (int j=0 ; j<this->numOfLiterals ; j++){
				size_t pos=line.find(" ");
				string temp_str=line.substr(0,pos);
				pos++;
				int bit;
				stringstream(temp_str) >> bit;  //get the file id number to f
				if (bit>0)
					ss << 1;
				else
					ss << 0;
				line = line.substr(pos);
			}
			int file_bit;
			ss >> file_bit;
			int file_id = fromBin(file_bit);
			this->remainFiles.push_back(this->files[file_id-this->inputfile->numOfBlocks-1]);
		}
		for (int i=0 ; i<delBlocks ; i++){
			stringstream ss;
			for (int j=0 ; j<this->numOfLiterals ; j++){
				size_t pos=line.find(" ");
				string temp_str=line.substr(0,pos);
				pos++;
				int bit;
				stringstream(temp_str) >> bit;  //get the file id number to f
				if (bit>0)
					ss << 1;
				else
					ss << 0;
				line = line.substr(pos);
			}
			int block_bit;
			ss >> block_bit;
			int block_id = fromBin(block_bit);
			this->deletedBlocks.push_back(block_id);
		}
		findBlocksInAir ();
		for (int i=0 ; i<this->deletedBlocks.size() ; i++){
			this->TotalDelBlockSize = this->TotalDelBlockSize + this->blocksSize[this->deletedBlocks[i]];
		}
		for (int i=0 ; i<this->blocksInAir.size() ; i++){
			this->TotalDelBlockSize = this->TotalDelBlockSize + this->blocksSize[this->blocksInAir[i]];
		}
		for (int i=0 ; i<this->files.size() ; i++){
			if (!(find(this->remainFiles.begin(), this->remainFiles.end(),this->files[i]) != this->remainFiles.end())){
				this->deletedFiles.push_back(this->files[i]);
			}
		}
		return "SATISFIABLE";
	}
	if (line == "***end***" || line == "INDET")
		return " INDETERMINATE ";
	cout << "Damaged Solver output file";
	exit (1);
}

/*The function get a file system input and finds (according the decoded deletedBlocks vector) for each deleted block
 * the original blocks that merge to him and count the original number of blocks to deletedBlocks vector and DelBlocksInAir vector
 * respectively */
void Reduction1::findOrigDelBlocks (){
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

/* The function gets all the meta data from the solver results and the python script. The solver output decoded data
 * stored in global variables and write the results formated into STDout and to txt file*/
void Reduction1::writeOutputSTDout(char* elpParseTime, char* elpSolverTime,int delFiles, int delBlocks,int numClas
		,int numVars, char* CNFSize, char* maxRAMSolver){
	string strSize = "            ";
	string ePT(elpParseTime);
	string eST(elpSolverTime);
	ePT.resize(6);
	eST.resize(6);
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

	/* resize all the string into same size*/
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
	/* 		*		*		*		*		 */

	cout << "=========================[ Problem Statistics ]========================="<<endl;
	cout <<endl;
	cout << "								[ Reduction 1 selected ]"<<endl;
	cout <<endl;
	if (this->solverOUTPUT=="SATISFIABLE")
		cout << "								 	SATISFIABLE"<<endl;
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
	if (this->solverOUTPUT=="SATISFIABLE"){
		cout << "==========================[ Eliminated files ]=========================="<<endl;
		cout << "	Original # Files	|	Original Size	|	Deleted # Files	|	Deleted Size	|"<<endl;
		cout << "	"<< origNumFiles <<"   	|	"<< origSize <<" GB	|	"<<  totalDelFiles <<"   	|	"<< DelSize <<" GB	|"<<endl;
		cout << "===================================================================="<<endl;
	}
	else
		cout << "===================================================================="<<endl;
	string solOut = "Solution_Reduction1_K_"+to_string(delFiles)+"_K'_"+to_string(delBlocks)+"targetBlocks_"+this->HTarget.substr(1)+"_filesystems_"+to_string(this->firstFS)+"_to_"+to_string(this->lastFS)+".txt";
	char *cstr = &solOut[0u];
	File* solutionFile = new File (cstr);
	solutionFile->writeLine("============================[ Problem Statistics ]=============================");
	solutionFile->writeLine("|                                                                             |");
	solutionFile->writeLine("|                          [ Reduction 1 selected ]                           |");
	solutionFile->writeLine("|                                                                             |");
	stringstream ss1;
	if (this->solverOUTPUT=="SATISFIABLE")
		solutionFile->writeLine("|                                 SATISFIABLE                                 |");
	else{

		ss1 << 				   "|                            ***"<<this->solverOUTPUT<<"***                              |";
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
	if (this->solverOUTPUT=="SATISFIABLE"){
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
	cout << solOut;
}

/* The function gets all the meta data from the solver results and the python script. The solver output decoded data
 * stored in global variables and writes results line to STDout in the next format:
 * Input type (file/block), Number of input file systems, Number of first file system, Number of last file system
 * ,Heuristic target, Number of input files, Number of input blocks, K, K'
 * ,Processing time (time required to read input file and generate input for the solver)
 * ,Size of input (size in bytes of input file for solver), Execution time (of solver)
 * ,Maximum RAM used by solver, Number of files marked for deletion by solver, Number of blocks marked for deletion by solver
 * , Number of deleted blocks (including those marked by solver, and those that were not marked by solver but all the files
 *  pointing to them are deleted) */
void Reduction1::writeOutputSaraiGala(char* elpParseTime, char* elpSolverTime,int delFiles, int delBlocks,int numClas,int numVars, char* CNFSize, char* maxRAMSolver){
	string ePT(elpParseTime);
	string eST(elpSolverTime);
	string maxRAMS(maxRAMSolver);
	string cnfSize(CNFSize);
	findOrigDelBlocks ();
	string MarkedBlocksBySover = to_string(this->DelBlocksBySolver);
	string FilesDelBySolver = to_string(this->deletedFiles.size());
	string totalDelBlocks = to_string(this->DelBlocksBySolver+this->DelBlocksInAir);
	string::size_type sZ;
	ePT.resize(6);
	eST.resize(6);
	if (this->solverOUTPUT=="SATISFIABLE"){
		cout << "block,"+to_string(this->numOfFSystems)+","+to_string(this->firstFS)+","+to_string(this->lastFS)+","+this->HTarget+","
				+to_string(this->inputfile->numOfFiles)+","+to_string(this->inputfile->numOfBlocks)+","+to_string(delFiles)+","
				+to_string(delBlocks)+","+ePT+","+cnfSize+","+eST+","+maxRAMS+","+FilesDelBySolver+","+MarkedBlocksBySover+","
				+totalDelBlocks+","<<endl;
	}
	else{
		cout << "block,"+to_string(this->numOfFSystems)+","+to_string(this->firstFS)+","+to_string(this->lastFS)+","+this->HTarget+","
				+to_string(this->inputfile->numOfFiles)+","+to_string(this->inputfile->numOfBlocks)+","+to_string(delFiles)+","
				+to_string(delBlocks)+","+ePT+","+cnfSize+","+eST+","+maxRAMS+","+"0,0,0,"<<endl;
	}
}

/*Test function for checking the the correctness of the result - if there is marked deleted block that connected to remaining
 * file the error message will print and the program breaks
 * else print successful message and return normally*/
void Reduction1::validetionTset(){
	for(int i=0; i<this->FTB.size(); i++){
		if (!(find(this->deletedFiles.begin(), this->deletedFiles.end(),this->FTB[i][0]) != this->deletedFiles.end()))  //if the file id is not in the deleted files
			for(int j=1; j<this->FTB[i].size(); j++){
				if (find(this->deletedBlocks.begin(), this->deletedBlocks.end(),this->FTB[i][j]) != this->deletedBlocks.end()){
					cout << "Validation Test failed! - The deleted Block "<<this->FTB[i][j]<<" connected to remaining File "<<this->FTB[i][0];
					exit(1);
				}
			}
	}
	cout << "Validation Test succeeded!";
}

//	Difference CNF encoding exponential version - for study DO NOT USE!!!
//	/* writing the cnf files difference clause to the cnf solver input file*/
//	for (int i=0 ; i<(this->inputfile->numOfFiles-delFiles) ; i++){    		 // i=0 to m-k
//		stringstream ss;
//		string str;
//		for (int j=i+1 ; j<(this->inputfile->numOfFiles-delFiles) ; j++){	// j=i+1 to m-k
//			for (uint w=0 ; w<this->CNFDiff.size() ; w++){
//				for (int s=0 ; s<this->numOfLiterals ; s++){					// writing xor of every 2 literals in the numOfLiterals
//					if (this->CNFDiff[w][s]==true){
//						ss << this->firstFile+(i*this->numOfLiterals)+s<<" ";
//						ss << this->firstFile+(j*this->numOfLiterals)+s<<" ";
//					}
//					else{
//						ss << -(this->firstFile+(i*this->numOfLiterals)+s)<<" ";
//						ss << -(this->firstFile+(j*this->numOfLiterals)+s)<<" ";
//					}
//				}
//				ss << "0";
//				str=ss.str();
//				this->outputfile->writeLine(str);
//				ss.str("");
//				str="";
//			}
//		}
//	}
//
//	/* writing the cnf files difference clause to the cnf solver input file*/
//		for (int i=0 ; i<(delBlocks) ; i++){    		 // i=0 to k'
//			stringstream ss;
//			string str;
//			for (int j=i+1 ; j<(delBlocks) ; j++){	// j=i+1 to k'
//				for (uint w=0 ; w<this->CNFDiff.size() ; w++){
//					for (int s=0 ; s<this->numOfLiterals ; s++){					// writing xor of every 2 literals in the numOfLiterals
//						if (this->CNFDiff[w][s]==true){
//							ss << this->firstBlock+(i*this->numOfLiterals)+s<<" ";
//							ss << this->firstBlock+(j*this->numOfLiterals)+s<<" ";
//						}
//						else{
//							ss << -(this->firstBlock+(i*this->numOfLiterals)+s)<<" ";
//							ss << -(this->firstBlock+(j*this->numOfLiterals)+s)<<" ";
//						}
//					}
//					ss << "0";
//					str=ss.str();
//					this->outputfile->writeLine(str);
//					ss.str("");
//					str="";
//				}
//			}
//		}


// Print method of all the DNF vectors for debugging
//void Reduction1::print(){
//	cout << "DNF files:"<< endl;
//	for (uint i=0 ; i<this->DNFFile.size() ; i++){
//		for (uint j=0 ; j<this->DNFFile[i].size() ; j++){
//			if (this->DNFFile[i][j]==true)
//				cout << 1;
//			else
//				cout << 0;
//		}
//		cout<<endl;
//	}
//	cout << "CNF files:"<< endl;
//	for (uint i=0 ; i<this->CNFFile.size() ; i++){
//		for (uint j=0 ; j<this->CNFFile[i].size() ; j++){
//			if (this->CNFFile[i][j]==true)
//				cout << 1;
//			else
//				cout << 0;
//		}
//		cout<<endl;
//	}
//	cout << "DNF Blocks:"<< endl;
//	for (uint i=0 ; i<this->DNFBlocks.size() ; i++){
//		for (uint j=0 ; j<this->DNFBlocks[i].size() ; j++){
//			if (this->DNFBlocks[i][j]==true)
//				cout << 1;
//			else
//				cout << 0;
//		}
//		cout<<endl;
//	}
//	cout << "CNF Blocks:"<< endl;
//	for (uint i=0 ; i<this->CNFBlocks.size() ; i++){
//		for (uint j=0 ; j<this->CNFBlocks[i].size() ; j++){
//			if (this->CNFBlocks[i][j]==true)
//				cout << 1;
//			else
//				cout << 0;
//		}
//		cout<<endl;
//	}
//	cout << "CNF Edges:"<< endl;
//	for (uint i=0 ; i<this->DNFEdges.size() ; i++){
//		for (uint j=0 ; j<this->DNFEdges[i].size() ; j++){
//			if (this->DNFEdges[i][j]==true)
//				cout << 1;
//			else
//				cout << 0;
//		}
//		cout<<endl;
//	}
//	cout << "CNF Diff:"<< endl;
//	for (uint i=0 ; i<this->CNFDiff.size() ; i++){
//		for (uint j=0 ; j<this->CNFDiff[i].size() ; j++){
//			if (this->CNFDiff[i][j]==true)
//				cout << 1;
//			else
//				cout << 0;
//		}
//		cout<<endl;
//	}
//}
