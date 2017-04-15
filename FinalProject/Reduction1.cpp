/*
 * Reduction1.cpp
 *
 *  Created on: Apr 14, 2017
 *      Author: yoad
 */

#include "Reduction1.h"
#include <math.h>
#include <string>
#include <bitset>

using namespace std;

Reduction1::Reduction1(char* path) {
	this->inputfile = new File (path,1);
	this->numOfLiterals = (int)(log2((double)(inputfile->numOfBlocks+inputfile->numOfFiles))+1);
	parser();

}

Reduction1::~Reduction1() {
	// TODO Auto-generated destructor stub
}

void Reduction1::parser(){
	string line;
	line=this->inputfile->getLine();
	while (line != "***end***"){
		if (line.at(0) == 'F'){
			for (int i=0; i<3 ; i++){
				size_t pos=line.find(",")+1;
				string temp_str=line.substr(pos);
				line = temp_str;

			}
			cout<<line;
		}

		if (line.at(0) == 'B'){

		}

		line=this->inputfile->getLine();
	}
}

int main(){
	std::string binary = std::bitset<8>(15).to_string(); //to binary
	std::cout<<binary<<"\n";
	string str = "filesystems_0145.txt";
	char *cstr = &str[0u];
	Reduction1 r (cstr);
return 0;
}
