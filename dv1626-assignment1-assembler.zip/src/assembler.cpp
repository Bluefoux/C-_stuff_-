#include "../include/assembler.hpp"
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
//#include <bits/stdc++.h>

int GetRegIndex(std::string reg){
	return (std::stoi(reg.substr(1, 1)));
}
int GetConstant(std::string constant){
	return (std::stoi(constant.substr(1, constant.size())));
}
bool IsReg(std::string term) 
{
	return (term.substr(0, 1) =="R");
}
int ExtractValue(std::string source, int* registers){
	if (IsReg(source)){
		return (registers[GetRegIndex(source)]);
	}
	return GetConstant(source);
}
bool BuildCommandList(std::vector<std::vector<std::string>>& commandList, std::string filepath)
{
	std::string str;
	std::vector<std::string> labelNames = {};
	std::vector<std::string> labelRows = {};
	commandList.clear();
	std::ifstream myfile;
	myfile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	try{
		myfile.open(filepath);
	}
	catch(std::ifstream::failure e){
		return false;
	}
	while (!myfile.eof()){
		std::getline(myfile, str);
		//myfile.get();
		if (!str.empty()){
			int count = 0;
			std::string word;
			std::vector<std::string> lineParts = {};
			for (char x : str){
				if (x == ' ')
				{
					lineParts.push_back(word);
					word = "";
				}
				else{
					word = word + x;
				}
				count++;
			}
			lineParts.push_back(word);
			if (lineParts[0].substr(lineParts[0].size()-1, lineParts[0].size()) == ":"){
				lineParts[0] = lineParts[0].substr(0, lineParts[0].size()-1);
				labelNames.push_back(lineParts[0]);
				labelRows.push_back(std::to_string(commandList.size()));
			}
			else{
				commandList.push_back(lineParts);
			}
		}
	}
	myfile.close();
	for(int i=0; i < commandList.size(); i++){
		std::string kalle = commandList[i][0];
		if ((commandList[i][0] == "JEQ") || (commandList[i][0] == "JGT") || (commandList[i][0] == "JLT")){
			int index = -1;
			int k = 0;
			for (k=0; k < labelNames.size(); k++){
				if (labelNames[k] == commandList[i][1]){
					index = k;
					commandList[i][1] = labelRows[index];
				}
			}
		}
	}
	return true;
}
int PeekNextCommandIndex(int* registers){
	return registers[REGISTER_SIZE-1];
}
int GetAndStepCommandIndex(int* registers){
	int index = registers[REGISTER_SIZE-1];
	registers[REGISTER_SIZE-1] +=1;
	return index;
}
void MOV(std::vector<std::string>& command, int* registers){
	registers[GetRegIndex(command[1])] = ExtractValue(command[2], registers);
}
void ADD(std::vector<std::string>& command, int* registers){
	registers[GetRegIndex(command[1])] = ExtractValue(command[2], registers) + ExtractValue(command[3], registers);
}
void SUB(std::vector<std::string>& command, int* registers){
	registers[GetRegIndex(command[1])] = ExtractValue(command[2], registers) - ExtractValue(command[3], registers);
}
void CMP(std::vector<std::string>& command, int* registers){
	int value1 = ExtractValue(command[1], registers);
	int value2 = ExtractValue(command[2], registers);
	if (value1 == value2){
		registers[REGISTER_SIZE-2] = 1;
	}
	else if (value1 > value2){
		registers[REGISTER_SIZE-2] = 2;
	}
	else{
		registers[REGISTER_SIZE-2] = 3;
	}
}
void JEQ(std::vector<std::string>& command, int* registers){
	if (registers[REGISTER_SIZE-2] == 1){
		registers[REGISTER_SIZE-1] = std::stoi(command[1]);
	}
}
void JGT(std::vector<std::string>& command, int* registers){
	if (registers[REGISTER_SIZE-2] == 2){
		registers[REGISTER_SIZE-1] = std::stoi(command[1]);
	}
}
void JLT(std::vector<std::string>& command, int* registers){
	if (registers[REGISTER_SIZE-2] == 3){
		registers[REGISTER_SIZE-1] = std::stoi(command[1]);
	}
}
