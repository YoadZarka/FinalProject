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
	if (argc != 5){
		cout<<"Wrong arguments passed"<<endl;
		return -1;
	}
	string str = argv[1];
	char *cstr = &str[0u];
	string str2 = argv[2];
	string::size_type sz;
	int delfile = stoi(str2,&sz);
	str2 = argv[3];
	int delblock = stoi(str2,&sz);
	str2 = argv[4];
	int red = stoi(str2,&sz);
	if (red==1)
		Reduction1 r (cstr,delfile,delblock);
	if (red==2)
		Reduction2 r (cstr,delfile,delblock);
	return 1;
}
