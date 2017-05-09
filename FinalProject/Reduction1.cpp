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

Reduction1::Reduction1(char* path) {
	this->inputfile = new File (path,1);
	this->numOfLiterals = (int)(log2((double)(inputfile->numOfBlocks+inputfile->numOfFiles))+1);
	if (this->numOfLiterals>63){
		cout << "There are too many litterals";
		exit(1);}
	parser();
	convert2cnf();
	print();
}

Reduction1::~Reduction1() {
	// TODO Auto-generated destructor stub
}

void Reduction1::dnf2cnf (vector< vector <bool> >& dnf){
	vector <bool> subcnf;
	int numOfExtVar=dnf.size();
	for (int i; i<numOfExtVar ; i++){
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

void Reduction1::convert2cnf(){

	dnf2cnf(this->DNFFile);
	this->CNFFile=this->tempCnf;
	for (uint i=0 ; i<tempCnf.size() ; i++){
		this->tempCnf[i].clear();
	}
	this->tempCnf.clear();
	dnf2cnf(this->DNFBlocks);
	this->CNFBlocks=this->tempCnf;
	for (uint i=0 ; i<this->DNFEdges.size() ; i++){     //Making a negation of the edges clause by de Morgan Law
		this->DNFEdges[i].flip();
	}
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
	cout << "DNF Edges:"<< endl;
	for (uint i=0 ; i<this->DNFEdges.size() ; i++){
				for (uint j=0 ; j<this->DNFEdges[i].size() ; j++){
					if (this->DNFEdges[i][j]==true)
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
	Reduction1 r (cstr);
return 0;
}
