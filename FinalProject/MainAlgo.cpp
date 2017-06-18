/*
 * MainAlgo.cpp
 *
 *  Created on: Jun 4, 2017
 *      Author: yoad
 */

#include "MainAlgo.h"
#include <string>
#include <iostream>

using namespace std;

MainAlgo::MainAlgo() {
	// TODO Auto-generated constructor stub

}

MainAlgo::~MainAlgo() {
	// TODO Auto-generated destructor stub
}

/** Argument Guide
* Arg number    [       1       ,            2        ,            3          ,         4       ,    5    (Continues in two rows)
 * FinalProject [input file path, k files for deletion, k' blocks for deletion, Reduction number, Op code (see below)
 * , output file path, elapsed time parser, elapsed time solver, number of clause, number of variables]
 * ,       6         ,         7          ,         8          ,        9        ,         10         ]
 *
 * There is 3 option for runs:
 * 1. Op=0 for parse input - there is no need for {output file path, elapsed time parser, elapsed time solver, number of clause, number of variables} arguments
 * 2. Op=1 for decode output for our GUI - needed all the arguments
 * 3. Op=2 for decode output for sarai - - needed all the arguments
 *  **/

int main(int argc, char *argv[]){
	if (argc < 6){
		cout<<"Wrong number of arguments passed"<<endl;
		return -1;
	}
	string str = argv[1];     //arg 1 is the txt file system input path
	char *input = &str[0u];
	string str2 = argv[2];		//arg 2 is the required deleted files
	string::size_type sz;
	int delfile = stoi(str2,&sz);
	str2 = argv[3];						//arg 3 is the required deleted blocks
	int delblock = stoi(str2,&sz);
	str2 = argv[4];						// arg 4 is the required reduction number for using
	int red = stoi(str2,&sz);
	str2 = argv[5];						// arg 5 is the op code:
	int op = stoi(str2,&sz);			// op=0 for parse input, op=1 for decode output for our GUI, op=2 for decode output for sarai
	if (op==0)  //parse input file only
	{
		if (red==1)
			Reduction1 r (input,delfile,delblock);
		else
		if (red==2)
			Reduction2 r (input,delfile,delblock);
		else
			cout << "The 'Reduction Number' argument is not correct!"<<endl;
		return 1;
	}
	else
	if (op==1 || op==2)  //decode output file
	{
		if (argc < 9){
			cout<<"Wrong number of arguments passed"<<endl;
			return 0;
		}
		string str1 = argv[6];				// arg 6 is the solver result output file path
		char *output = &str1[0u];
		string str3 = argv[7];				// arg 7 is elapsed runtime of parse the input txt file into cnf file
		char *ETparser = &str3[0u];
		string str4 = argv[8];				// arg 8 is elapsed runtime of solver
		char* ETsolver = &str4[0u];
		str2 = argv[9];						// arg 9 is number of clauses in the solver cnf file
		int32_t numOfClauses = stoi(str2,&sz);
		str2 = argv[10];						// arg 10 is number of variables in the solver cnf file
		int32_t numOfVars = stoi(str2,&sz);
		if (red==1)
			Reduction1 r (input,output,delfile,delblock,op,ETparser,ETsolver,numOfClauses,numOfVars);
		else
		if (red==2)
			red=2;
			//Reduction2 r (input,output,delfile,delblock,op);
		else
			cout << "The 'Reduction Number' argument is not correct!"<<endl;
		return 1;
	}
	else
		cout << "'Op code' argument is not exists"<<endl;
}
