#include <iostream>

#include "include/assembler.hpp"

int main(int argc, char** argv)
{
	// Validate input arguments
	if (argc <= 1 || argc > (REGISTER_SIZE+1)) {
		std::cout << "Error: Invalid argument list.\n"
			<< "\n\nFormat: ./app program.asm (R0) (R1) (R3) (R4) ... (R7)\n"
			<< "\n\nArgument list explanationd:\n"
			<< "\n(REQUIRED) program.asm : The executable assembly file\n"
			<< "\n(OPTIONAL) (R0) : Default value for the register R0\n"
			<< "\n(OPTIONAL) (R1) : Default value for the register R1\n"
			<< "\n(OPTIONAL) (R2) : Default value for the register R2\n"
			<< "\n(OPTIONAL) (R3) : Default value for the register R3\n"
			<< "\n(OPTIONAL) (...) : Default value for the register ...\n"
			<< "\n(OPTIONAL) (R7) : Default value for the register R7\n";
		return 0;
	}

	// Create register state and load default arguments
	int registers[REGISTER_SIZE] = { 0 };

	for (int i = 2; i < argc; i++) {
		registers[i-2] = std::stoi(argv[i]);
	}

	// Create the command list
	std::vector<std::vector<std::string>> commandList;

	/*
		A command in command list can be accessed by
		std::vector<std::string>& command = commandList[index];

		and each element in the command can be accessed by
		command[0]; // "MOV" for example
	*/

	if (!BuildCommandList(commandList, argv[1])) {
		std::cout << "Error: Could not open file '" << argv[1] << "'" << std::endl;
		return 0;
	}

	while (PeekNextCommandIndex(registers) < commandList.size()){
		std::vector<std::string> command = commandList[GetAndStepCommandIndex(registers)];
		if (command[0] == "MOV"){
			MOV(command, registers);
		}
		else if (command[0] == "ADD"){
			ADD(command, registers);
		}
			
		else if (command[0] == "SUB"){
			SUB(command, registers);
		}
		else if (command[0] == "CMP"){
			CMP(command, registers);
		}
			
		else if (command[0] == "JEQ"){
			JEQ(command, registers);
		}
			
		else if (command[0] == "JGT"){
			JGT(command, registers);
		}
		else if (command[0] == "JLT"){
			JLT(command, registers);
		}
	}

	// End application by printing the first four registers
	for (int i = 0; i < 4; i++) {
		std::cout << registers[i] << " ";
	}
	std::cout << std::endl;

	return 0;
}