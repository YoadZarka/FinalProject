/*
 * Reduction1.cpp
 *
 *  Created on: Apr 14, 2017
 *      Author: yoad
 */

#include "Reduction1.h"
#include <sstream>
#include <math.h>
#include <string>
#include <bitset>

using namespace std;

Reduction1::Reduction1(char* path, int delFiles, int delBlocks) {
	this->inputfile = new File (path,1);
	this->numOfLiterals = (int)(log2((double)(inputfile->numOfBlocks+inputfile->numOfFiles))+1);
	if (this->numOfLiterals>63){
		cout << "There are too many litterals";
		exit(1);}
	parser();
	this->inputfile->~File();
	convert2cnf();
	encodeCNFDiff ();
	writeCNF(delFiles, delBlocks);
	print();              //need to delete in the end
}

Reduction1::~Reduction1() {
	// TODO Auto-generated destructor stub
}

/* This function convert one DNF clause into CNF form by adding extra variable as
 *  the subclause number */
void Reduction1::dnf2cnf (vector< vector <bool> >& dnf){
	vector <bool> subcnf;
	int numOfExtVar=dnf.size();
	for (int i=0; i<numOfExtVar ; i++){
		subcnf.push_back(true);				//push z=1 as number of clauses
	}
	this->tempCnf.push_back(subcnf);
	subcnf.clear();
	for (int i=0; i<numOfExtVar ; i++){
		for (int j=0; j<this->numOfLiterals ; j++){
			subcnf.push_back(false);		// push ~z
			subcnf.push_back(dnf[i][j]);    // push literal j of each clause
			this->tempCnf.push_back(subcnf);
			subcnf.clear();
		}
	}
}

/*take a filesystem txt file input and encode the files, blocks & edges clauses in DNF
 * form*/
void Reduction1::parser(){
	int blocks [this->inputfile->numOfBlocks]={};
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
			cout <<"File" << f <<endl;
			this->files.push_back(f);  //add the file id to a vector of all files
			this->DNFFile.push_back(id2dnf(this->files.size()+this->inputfile->numOfBlocks));  //call the kidod to file dnf with the file index of f + number of blocks
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
			line = temp_str;
			for (int i=0 ; i<bn ;i++){
				pos=line.find(",");
				num=line.substr(0,pos);
				int b;
				stringstream(num) >> b;  //get the block id number to b
				if (blocks[b-1]==0){
					this->DNFBlocks.push_back(id2dnf(b));  //call to kidod to blocks dnf
					blocks[b-1]=1;
				}
				//call to kidod to edegs dnf f and b
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

/* Auxiliary function for converting an int id number to its DNF binary representation*/
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


void Reduction1::encodeCNFDiff (){
	vector<bool> cnfxor;				//one xor of 2 vars a,b is (a or b) and (~a or ~b)
	cnfxor.push_back(true);
	cnfxor.push_back(true);
	for (int i=0 ; i<this->numOfLiterals ; i++){
		this->CNFDiff.push_back(cnfxor);
		cnfxor.flip();
		this->CNFDiff.push_back(cnfxor);
		cnfxor.flip();
	}
}

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

	for (uint i=0 ; i<this->DNFEdges.size() ; i++){     //Making a negation of the edges clause by de Morgan Law
		this->DNFEdges[i].flip();
	}
}

void Reduction1::writeCNF(int delFiles, int delBlocks){
	int zBlnum=CNFBlocks[0].size();      //number of z for the blocks clause
	this->totalLiterals=(this->numOfLiterals*((this->inputfile->numOfFiles-delFiles)+delBlocks))+(this->inputfile->numOfFiles*(this->inputfile->numOfFiles-delFiles))+(zBlnum*delBlocks);
	this->firstFile =1;
	this->firstBlock =(numOfLiterals*(this->inputfile->numOfFiles-delFiles))+1;
	this->firstZvar =(this->numOfLiterals*((this->inputfile->numOfFiles-delFiles)+delBlocks))+1;
	// need to fix
	int numOfClauses = (this->CNFFile.size()*this->inputfile->numOfFiles-delFiles) + (this->CNFBlocks.size()*delBlocks) + (this->DNFEdges.size()*((this->inputfile->numOfFiles-delFiles)+delBlocks) + (2*this->numOfLiterals*(delBlocks)));
	//
	string str2 = "SATinput.cnf";
	char *cstr = &str2[0u];
	this->outputfile = new File (cstr);
	this->outputfile->writeLine("c  SATinput.cnf");
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
				ss << this->firstZvar+(i*this->inputfile->numOfFiles)+((j-1)/this->inputfile->numOfFiles)<<" ";
			}
			else{
				ss << -(this->firstZvar+(i*this->inputfile->numOfFiles)+((j-1)/this->inputfile->numOfFiles))<<" ";
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
					ss << this->firstZvar+(i*zBlnum)+((j-1)/zBlnum)<<" ";
				}
				else{
					ss << -(this->firstZvar+(i*zBlnum)+((j-1)/zBlnum))<<" ";
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
	for (int i=0 ; i<(this->inputfile->numOfFiles-delFiles) ; i++){    		 // i=0 to m-k
		stringstream ss;
		string str;
		for (int j=i+1 ; j<(this->inputfile->numOfFiles-delFiles) ; j++){	// j=i+1 to m-k
			for (int s=0 ; s<this->numOfLiterals ; s++){					// writing xor of every 2 literals in the numOfLiterals
				ss << this->firstFile+(i*this->numOfLiterals)+s<<" ";
				ss << this->firstFile+(j*this->numOfLiterals)+s<<" ";
				ss << "0";
				str=ss.str();
				this->outputfile->writeLine(str);
				ss.str("");
				str="";
				ss << -(this->firstFile+(i*this->numOfLiterals)+s)<<" ";
				ss << -(this->firstFile+(j*this->numOfLiterals)+s)<<" ";
				ss << "0";
				str=ss.str();
				this->outputfile->writeLine(str);
				ss.str("");
				str="";
			}
		}
	}

	/* writing the cnf files difference clause to the cnf solver input file*/
		for (int i=0 ; i<(delBlocks) ; i++){    		 // i=0 to k'
			stringstream ss;
			string str;
			for (int j=i+1 ; j<(delBlocks) ; j++){	// j=i+1 to k'
				for (int s=0 ; s<this->numOfLiterals ; s++){					// writing xor of every 2 literals in the numOfLiterals
					ss << this->firstBlock+(i*this->numOfLiterals)+s<<" ";
					ss << this->firstBlock+(j*this->numOfLiterals)+s<<" ";
					ss << "0";
					str=ss.str();
					this->outputfile->writeLine(str);
					ss.str("");
					str="";
					ss << -(this->firstBlock+(i*this->numOfLiterals)+s)<<" ";
					ss << -(this->firstBlock+(j*this->numOfLiterals)+s)<<" ";
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

void Reduction1::print(){
	cout << "DNF files:"<< endl;
	for (uint i=0 ; i<this->DNFFile.size() ; i++){
		for (uint j=0 ; j<this->DNFFile[i].size() ; j++){
			if (this->DNFFile[i][j]==true)
				cout << 1;
			else
				cout << 0;
		}
		cout<<endl;
	}
	cout << "CNF files:"<< endl;
		for (uint i=0 ; i<this->CNFFile.size() ; i++){
			for (uint j=0 ; j<this->CNFFile[i].size() ; j++){
				if (this->CNFFile[i][j]==true)
					cout << 1;
				else
					cout << 0;
			}
			cout<<endl;
		}
	cout << "DNF Blocks:"<< endl;
	for (uint i=0 ; i<this->DNFBlocks.size() ; i++){
			for (uint j=0 ; j<this->DNFBlocks[i].size() ; j++){
				if (this->DNFBlocks[i][j]==true)
					cout << 1;
				else
					cout << 0;
			}
			cout<<endl;
	}
	cout << "CNF Blocks:"<< endl;
		for (uint i=0 ; i<this->CNFBlocks.size() ; i++){
				for (uint j=0 ; j<this->CNFBlocks[i].size() ; j++){
					if (this->CNFBlocks[i][j]==true)
						cout << 1;
					else
						cout << 0;
				}
				cout<<endl;
		}
	cout << "CNF Edges:"<< endl;
	for (uint i=0 ; i<this->DNFEdges.size() ; i++){
				for (uint j=0 ; j<this->DNFEdges[i].size() ; j++){
					if (this->DNFEdges[i][j]==true)
						cout << 1;
					else
						cout << 0;
				}
				cout<<endl;
		}
	cout << "CNF Diff:"<< endl;
		for (uint i=0 ; i<this->CNFDiff.size() ; i++){
					for (uint j=0 ; j<this->CNFDiff[i].size() ; j++){
						if (this->CNFDiff[i][j]==true)
							cout << 1;
						else
							cout << 0;
					}
					cout<<endl;
			}
}

int main(){
	string str = "filesystems_0145.txt";
	char *cstr = &str[0u];
	Reduction1 r (cstr,2,3);
return 0;
}
