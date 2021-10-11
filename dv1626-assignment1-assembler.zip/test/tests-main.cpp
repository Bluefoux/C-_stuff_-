#define CATCH_CONFIG_MAIN


#include <vector>
#include <string>

#include "catch.hpp"
#include "assembler.hpp"

const int DEFAULT = -5;
const int COMMAND_REG = REGISTER_SIZE-1;
const int COMPARE_REG = REGISTER_SIZE-2;

SCENARIO("A command list should be able to be built") {
	GIVEN("An .asm file") {

		WHEN("The .asm file is loaded") {

			std::vector<std::vector<std::string>> commandsCorrect = {
				{ "MOV", "R1", "#1" },
				{ "MOV", "R2", "R0" },
				{ "ADD", "R1", "R1", "R2" },
				{ "ADD", "R3", "R1", "#4" },
				{ "ADD", "R3", "#-5", "#4" },
				{ "ADD", "R0", "#131", "R2" },
				{ "SUB", "R1", "R1", "R2" },
				{ "SUB", "R3", "R1", "#4" },
				{ "SUB", "R3", "#-5", "#4" },
				{ "SUB", "R0", "#131", "R2" },
				{ "CMP", "R0", "R1" },
				{ "CMP", "R3", "#6" },
				{ "JEQ", "10" },
				{ "JGT", "2" },
				{ "JLT", "10" }
			};

			std::vector<std::vector<std::string>> commands;
			bool returnValue = BuildCommandList(commands, "test/test.asm");

			THEN("The BuildCommandList should return true if build is possible") {
				REQUIRE(returnValue);
			}

			THEN("The command should contain the correct amount of commands") {
				REQUIRE(commands.size() == commandsCorrect.size());
			}

			THEN("Each sub-command should contain the correct amount of elements") {
				int size = commandsCorrect.size();
				for (int i = 0; i < size; i++) {
					REQUIRE(commands[i].size() == commandsCorrect[i].size());
				}
			}

			THEN("Each sub-command should be correctly formatted") {
				int size = commandsCorrect.size();
				for (int i = 0; i < size; i++) {
					int subSize = commandsCorrect[i].size();
					for (int j = 0; j < subSize; j++) {
						REQUIRE(commands[i][j] == commandsCorrect[i][j]);
					}
				}
			}
		}
	}
}

SCENARIO("'BuildCommandList' should handle incorrect file names") {
	GIVEN("The file notExisting.asm do not exist") {
		WHEN("The file notExisting.asm is loaded") {
			std::vector<std::vector<std::string>> commands;
			bool returnValue = true;
			bool exceptionWasThrowed = false;

			try {
				returnValue = BuildCommandList(commands, "notExisting.asm");
			} catch (...) {
				exceptionWasThrowed = true;
			}

			THEN("BuildCommandList should not throw any exception") {
				INFO("This function can return without an exception.")
				REQUIRE(exceptionWasThrowed == false);
			}

			THEN("BuildCommandList should return false") {
				REQUIRE(returnValue == false);
			}
		}
	}
}

SCENARIO("The last register should contain the next command index") {
	GIVEN("The command register is given the value 5") {

		WHEN("PeekNextCommandIndex is used to retrieve the value") {

			int registers[REGISTER_SIZE] = { 0 };
			registers[COMMAND_REG] = 5;

			int retrieved_value = PeekNextCommandIndex(registers);

			THEN("Returned values should be 5") {
				REQUIRE(retrieved_value == 5);
			}

			THEN("The value in the command register should still be 5") {
				REQUIRE(registers[COMMAND_REG] == 5);
			}
		}

		WHEN("GetAndStepCommandIndex is used to retrieve the value") {

			int registers[REGISTER_SIZE] = { 0 };
			registers[COMMAND_REG] = 5;

			int retrieved_value = GetAndStepCommandIndex(registers);

			THEN("Returned value should be 5") {
				REQUIRE(retrieved_value == 5);
			}

			THEN("The value in the command register should be incremented to 6") {
				REQUIRE(registers[COMMAND_REG] == 6);
			}
		}
	}
}

SCENARIO("MOV can move a constant into a specified register") {
	GIVEN("All registers are set to any value") {

		WHEN("A positive constant is moved into R0") {
			
			int registers[REGISTER_SIZE];
			for (int i = 0; i < REGISTER_SIZE; i++)
				registers[i] = DEFAULT;

			std::vector<std::string> command = {"MOV", "R0", "#5"};
			MOV(command, registers);


			THEN("R0 should contain the positive constant") {
				REQUIRE(registers[0] == 5);
			}

			THEN("No other register should be changed") {
				for (int i = 1; i < REGISTER_SIZE; i++)
					REQUIRE(registers[i] == DEFAULT);
			}
		}

		WHEN("A negative constant is moved into R1") {

			int registers[REGISTER_SIZE];
			for (int i = 0; i < REGISTER_SIZE; i++)
				registers[i] = DEFAULT;

			std::vector<std::string> command = {"MOV", "R1", "#-1"};
			MOV(command, registers);

			THEN("R1 should contain the negative constant") {
				REQUIRE(registers[1] == -1);
			}

			THEN("No other register should be changed") {
				for (int i = 0; i < REGISTER_SIZE; i++) {
					if (i == 1)
						continue;
					REQUIRE(registers[i] == DEFAULT);
				}
			}
		}

		WHEN("A multiple digit constant is moved into R2") {

			int registers[REGISTER_SIZE];
			for (int i = 0; i < REGISTER_SIZE; i++)
				registers[i] = DEFAULT;

			std::vector<std::string> command = {"MOV", "R2", "#3141"};
			MOV(command, registers);

			THEN("R2 should contain the multiple digit constant") {
				REQUIRE(registers[2] == 3141);
			}

			THEN("No other register should be changed") {
				for (int i = 0; i < REGISTER_SIZE; i++) {
					if (i == 2)
						continue;
					REQUIRE(registers[i] == DEFAULT);
				}
			}
		}

		WHEN("A negative multiple digit constant is moved into R3") {

			int registers[REGISTER_SIZE];
			for (int i = 0; i < REGISTER_SIZE; i++)
				registers[i] = DEFAULT;
				
			std::vector<std::string> command = {"MOV", "R3", "#-3141"};
			MOV(command, registers);

			THEN("R3 should contain the negative multiple digit constant") {
				REQUIRE(registers[3] == -3141);
			}

			THEN("No other register should be changed") {
				for (int i = 0; i < REGISTER_SIZE; i++) {
					if (i == 3)
						continue;
					REQUIRE(registers[i] == DEFAULT);
				}
			}
		}
	}
}

SCENARIO("MOV can move a register value into another specified register") {
	GIVEN("The register R0 is initialized with the value 13") {

		WHEN("The value of R0 is moved to R1") {

			const int VALUE = 13;

			int registers[REGISTER_SIZE];
			for (int i = 0; i < REGISTER_SIZE; i++)
				registers[i] = DEFAULT;

			registers[0] = VALUE;
			std::vector<std::string> command = {"MOV", "R1", "R0"};
			MOV(command, registers);

			THEN("R1 should contain the value that initialized R0") {
				REQUIRE(registers[1] == VALUE);
			}

			THEN("R0 should contain its initialized value") {
				REQUIRE(registers[0] == VALUE);
			}

			THEN("No other register should be changed") {
				for (int i = 2; i < REGISTER_SIZE; i++) {
					REQUIRE(registers[i] == DEFAULT);
				}
			}
		}

		WHEN("The value of R3 is moved to R2") {

			const int VALUE = 13;

			int registers[REGISTER_SIZE];
			for (int i = 0; i < REGISTER_SIZE; i++)
				registers[i] = DEFAULT;

			registers[3] = VALUE;
			std::vector<std::string> command = {"MOV", "R2", "R3"};
			MOV(command, registers);

			THEN("R2 should contain the value that initialized R3") {
				REQUIRE(registers[2] == VALUE);
			}

			THEN("R3 should contain its initialized value") {
				REQUIRE(registers[3] == VALUE);
			}

			THEN("No other register should be changed") {
				for (int i = 0; i < REGISTER_SIZE; i++) {
					if (i == 2 || i == 3)
						continue;
					REQUIRE(registers[i] == DEFAULT);
				}
			}
		}
	}
}

SCENARIO("ADD can add two registers and store the sum in a specified register") {
	GIVEN("All registers contain different values") {

		WHEN("The values of R1 and R2 is added, sum is stored in R0") {

			int registers[REGISTER_SIZE];
			for (int i = 0; i < REGISTER_SIZE; i++)
				registers[i] = i;

			int sum_r1_r2 = registers[1] + registers[2];

			std::vector<std::string> command = {"ADD", "R0", "R1", "R2"};
			ADD(command, registers);

			THEN("R0 should contain the sum of the values in R1 and R2") {
				REQUIRE(registers[0] == sum_r1_r2);
			}

			THEN("No other register should be changed") {
				for (int i = 1; i < REGISTER_SIZE; i++) {
					REQUIRE(registers[i] == i);
				}
			}
		}

		WHEN("The values of R1 and R2 is added, sum is stored in R1") {

			int registers[REGISTER_SIZE];
			for (int i = 0; i < REGISTER_SIZE; i++)
				registers[i] = i;

			int sum_r1_r2 = registers[1] + registers[2];

			std::vector<std::string> command = {"ADD", "R1", "R1", "R2"};
			ADD(command, registers);

			THEN("R1 should contain the sum of the values in R1 and R2") {
				REQUIRE(registers[1] == sum_r1_r2);
			}

			THEN("No other register should be changed") {
				for (int i = 0; i < REGISTER_SIZE; i++) {
					if (i == 1)
						continue;
					REQUIRE(registers[i] == i);
				}
			}
		}
	}
}

SCENARIO("SUB can subtract two registers and store the difference in a specified register") {
	GIVEN("All registers contain different values") {

		WHEN("The values of R1 and R2 is subtracted, difference is stored in R0") {

			int registers[REGISTER_SIZE];
			for (int i = 0; i < REGISTER_SIZE; i++)
				registers[i] = i;

			int diff_r1_r2 = registers[1] - registers[2];

			std::vector<std::string> command = {"SUB", "R0", "R1", "R2"};
			SUB(command, registers);

			THEN("R0 should contain the difference of the values in R1 and R2") {
				REQUIRE(registers[0] == diff_r1_r2);
			}

			THEN("No other register should be changed") {
				for (int i = 1; i < REGISTER_SIZE; i++) {
					REQUIRE(registers[i] == i);
				}
			}
		}

		WHEN("The values of R1 and R2 is subtracted, difference is stored in R1") {

			int registers[REGISTER_SIZE];
			for (int i = 0; i < REGISTER_SIZE; i++)
				registers[i] = i;

			int diff_r1_r2 = registers[1] - registers[2];

			std::vector<std::string> command = {"SUB", "R1", "R1", "R2"};
			SUB(command, registers);

			THEN("R1 should contain the sum of the values in R1 and R2") {
				REQUIRE(registers[1] == diff_r1_r2);
			}

			THEN("No other register should be changed") {
				for (int i = 0; i < REGISTER_SIZE; i++) {
					if (i == 1)
						continue;
					REQUIRE(registers[i] == i);
				}
			}
		}
	}
}

SCENARIO("CMP can compare two registers and write the result into the compare register R6") {
	GIVEN("R0 contain a value equal to the value in R1") {
		WHEN("R0==R1 is compared") {
			THEN("The compare register R6 should contain 1 (the value for 'equal')") {
				int registers[REGISTER_SIZE];
				for (int i = 0; i < REGISTER_SIZE; i++)
					registers[i] = DEFAULT;
				
				registers[0] = registers[1] = 3;

				std::vector<std::string> command = {"CMP", "R0", "R1"};
				CMP(command, registers);

				REQUIRE(registers[COMPARE_REG] == 1);
			}
		}
	}

	GIVEN("R3 contain a value greater than the value in R5") {
		WHEN("R3>R5 is compared") {
			THEN("The compare register R6 should contain 2 (the value for 'greater than')") {
				int registers[REGISTER_SIZE];
				for (int i = 0; i < REGISTER_SIZE; i++)
					registers[i] = DEFAULT;
				
				registers[3] = 10;
				registers[5] = 2;

				std::vector<std::string> command = {"CMP", "R3", "R5"};
				CMP(command, registers);

				REQUIRE(registers[COMPARE_REG] == 2);
			}
		}
	}

	GIVEN("R4 contain a value less than the value in R1") {
		WHEN("R4<R1 is compared") {
			THEN("The compare register R6 should contain 3 (the value for 'less than')") {
				int registers[REGISTER_SIZE];
				for (int i = 0; i < REGISTER_SIZE; i++)
					registers[i] = DEFAULT;
				
				registers[4] = 1;
				registers[1] = 32;

				std::vector<std::string> command = {"CMP", "R4", "R1"};
				CMP(command, registers);

				REQUIRE(registers[COMPARE_REG] == 3);
			}
		}
	}
}

SCENARIO("JEQ can change the command register R7 if the compare flag register R6 has a value of 1 (the value for 'equal to')") {
	GIVEN("The cammand register R7 contain the value 5") {

		WHEN("The compare flag register R6 contain the value 1, the command \"JEQ 2\" is executed (the label has been converted)") {

			int registers[REGISTER_SIZE];
			for (int i = 0; i < REGISTER_SIZE; i++)
				registers[i] = DEFAULT;
			
			registers[COMPARE_REG] = 1;
			registers[COMMAND_REG] = 5;

			std::vector<std::string> command = {"JEQ", "2"};
			JEQ(command, registers);

			THEN("The command register R7 should be modified by jump instruction (== 2)") {
				REQUIRE(registers[COMMAND_REG] == 2);
			}
		}

		WHEN("The compare flag register R6 contain the value 2, the command \"JEQ 2\" is executed (the label has been converted)") {

			int registers[REGISTER_SIZE];
			for (int i = 0; i < REGISTER_SIZE; i++)
				registers[i] = DEFAULT;
			
			registers[COMPARE_REG] = 2;
			registers[COMMAND_REG] = 5;

			std::vector<std::string> command = {"JEQ", "2"};
			JEQ(command, registers);

			THEN("The command register R7 should not be modified by jump instruction, since 2 is the value for 'greater than'") {
				REQUIRE(registers[COMMAND_REG] == 5);
			}
		}
	}
}

SCENARIO("JGT can change the command register R7 if the compare flag register R6 has a value of 2 (the value for 'greater than')") {
	GIVEN("The cammand register R7 contain the value 5") {

		WHEN("The compare flag register R6 contain the value 2, the command \"JGT 2\" is executed (the label has been converted)") {

			int registers[REGISTER_SIZE];
			for (int i = 0; i < REGISTER_SIZE; i++)
				registers[i] = DEFAULT;
			
			registers[COMPARE_REG] = 2;
			registers[COMMAND_REG] = 5;

			std::vector<std::string> command = {"JGT", "2"};
			JGT(command, registers);

			THEN("The command register R7 should be modified by jump instruction (== 2)") {
				REQUIRE(registers[COMMAND_REG] == 2);
			}
		}

		WHEN("The compare flag register R6 contain the value 1, the command \"JGT 2\" is executed (the label has been converted)") {

			int registers[REGISTER_SIZE];
			for (int i = 0; i < REGISTER_SIZE; i++)
				registers[i] = DEFAULT;
			
			registers[COMPARE_REG] = 1;
			registers[COMMAND_REG] = 5;

			std::vector<std::string> command = {"JGT", "2"};
			JGT(command, registers);

			THEN("The command register R7 should not be modified by jump instruction, since 1 is the value for 'equal to'") {
				REQUIRE(registers[COMMAND_REG] == 5);
			}
		}
	}
}

SCENARIO("JLT can change the command register R7 if the compare flag register R6 has a value of 3 (the value for 'less than')") {
	GIVEN("The cammand register R7 contain the value 5") {

		WHEN("The compare flag register R6 contain the value 3, the command \"JLT 2\" is executed (the label has been converted)") {

			int registers[REGISTER_SIZE];
			for (int i = 0; i < REGISTER_SIZE; i++)
				registers[i] = DEFAULT;
			
			registers[COMPARE_REG] = 3;
			registers[COMMAND_REG] = 5;

			std::vector<std::string> command = {"JLT", "2"};
			JLT(command, registers);

			THEN("The command register R7 should be modified by jump instruction (== 2)") {
				REQUIRE(registers[COMMAND_REG] == 2);
			}
		}

		WHEN("The compare flag register R6 contain the value 1, the command \"JLT 2\" is executed (the label has been converted)") {

			int registers[REGISTER_SIZE];
			for (int i = 0; i < REGISTER_SIZE; i++)
				registers[i] = DEFAULT;
			
			registers[COMPARE_REG] = 1;
			registers[COMMAND_REG] = 5;

			std::vector<std::string> command = {"JLT", "2"};
			JLT(command, registers);

			THEN("The command register R7 should not be modified by jump instruction, since 1 is the value for 'equal to'") {
				REQUIRE(registers[COMMAND_REG] == 5);
			}
		}
	}
}