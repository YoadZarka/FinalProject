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

int main(int argc, char *argv[]){
	if (argc > 7){
		cout<<"Wrong number of arguments passed"<<endl;
		return -1;
	}
	string str = argv[1];
	char *input = &str[0u];
	string str2 = argv[2];
	string::size_type sz;
	int delfile = stoi(str2,&sz);
	str2 = argv[3];
	int delblock = stoi(str2,&sz);
	str2 = argv[4];
	int red = stoi(str2,&sz);
	str2 = argv[5];
	int op = stoi(str2,&sz);			// op=0 for parse input, op=1 for decode output for our GUI, op=2 for decode output for sarai
	if (op==0)  //parse input file only
	{
		if (red==1)
			Reduction1 r (input,delfile,delblock);
		if (red==2)
			Reduction2 r (input,delfile,delblock);
		return 1;
	}
	if (op==1 || op==2)  //decode output file
	{
		string str1 = argv[6];
		char *output = &str1[0u];
		if (red==1)
			Reduction1 r (input,output,delfile,delblock,op,"0.754568","5.8767",356,7657);
		if (red==2)
			//Reduction2 r (input,output,delfile,delblock,op);
		return 1;
	}
}
