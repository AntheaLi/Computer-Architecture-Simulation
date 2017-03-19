//============================================================================
// Name        : CS472_PS3_Yichen_Anthea_Li.cpp
// Author      : Anthea Yichen Li
// Version     :
// Copyright   : Your copyright notice
// Description : Pipelined Data path Simulation
//============================================================================

#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

using namespace std;

// main memory and register initiation

short Main_Mem[0x400]; // 1k main memory
int Regs[32]; // 32 registers
bool test;

//const int INSTRUCTION_NUMBER = 9;
//int InstructionCache[INSTRUCTION_NUMBER] = { 0x00a63820, 0x8d0f0004, 0xad09fffc,
//		0x00625022, 0x10c8fffb, 0x00000000, 0x00000000, 0x00000000, 0x00000000 };

const int INSTRUCTION_NUMBER = 12;
int InstructionCache[INSTRUCTION_NUMBER] = { 0xa1020000, 0x810AFFFC, 0x00831820,
		0x01263820, 0x01224820, 0x81180000, 0x81510010, 0x00624022, 0x00000000,
		0x00000000, 0x00000000, 0x00000000 };

int instrCount = 0;
uint32_t NOP = 0x00000000;

struct Control {
	short RegWrite;
	short RegDst;
	short MemToReg;
	short MemRead;
	short MemWrite;
	short ALUSrc;
	short Branch;
	short ALUOP;
};

struct IF_ID {
	uint32_t inst;
	Control control;

	int IncrPC; // PC counter
};

struct ID_EX {
	Control control;

	uint32_t inst;
	int IncrPC; // PC counter
	short ReadReg1Value;
	short ReadReg2Value;
	short SEOffset;
	short WriteReg_20_16;
	short WriteReg_15_11;
	short Function;
};

struct EX_MEM {
	Control control;

	uint32_t inst;

	short CalcBTA;
	bool Zero;
	short ALUResult;
	short SWValue;
	short WriteRegNum;
	short Function;

	int IncrPC; // PC counter

};

struct MEM_WB {
	Control control;

	uint32_t inst;
	short SWValue;
	short LWDataValue;
	short ALUResult;
	short WriteRegNum;

	int IncrPC; // PC counter

};

/*
 * initiate pipeline registers as public variables
 * @Author Anthea Yichen Li
 * @Since fall 2016
 */
IF_ID IF_ID_Write = IF_ID();
IF_ID IF_ID_Read = IF_ID();

ID_EX ID_EX_Write = ID_EX();
ID_EX ID_EX_Read = ID_EX();

EX_MEM EX_MEM_Write = EX_MEM();
EX_MEM EX_MEM_Read = EX_MEM();

MEM_WB MEM_WB_Write = MEM_WB();
MEM_WB MEM_WB_Read = MEM_WB();

/**
 * IF stage fetch instruction from the InstructionCache as specified above to put into the IF_ID_Write register
 * @author Anthea Yichen Li
 * @since fall 2016
 */
void IF_stage() {
	// fetch instruction from the instruction cache with the global pointer instrCount
	// then increment the instrCount
	uint32_t instruction = InstructionCache[instrCount];
	instrCount += 1;

	IF_ID_Write.inst = instruction;

	// increment PC counter
	IF_ID_Write.IncrPC += 0x04;
}

/**
 * ID stage decodes the instruction fetched by the IF stage
 * set control signals
 * @author anthea yichen li
 * @since fall 2016
 */
void ID_stage() {
	uint32_t instruction = IF_ID_Read.inst;

	ID_EX_Write.inst = IF_ID_Read.inst;

	// increment PC counter
	ID_EX_Write.IncrPC = IF_ID_Read.IncrPC;

	if (instruction == NOP) {
		return;
	} else {

		// different bit masks to help break down / extract the hex code to different components
		int bitMask5 = 0x0001f;
		int bitMask6 = 0x3f;
		int bitMask16 = 0xffff;
		// decode instruction to OpCode in order to set control signals
		int opcode = instruction >> 26 & bitMask6;

		switch (opcode) {
		case 0x0: // add or sub
		{
			/**
			 * Control Fields:
			 * 	short RegWrite;
			 * 	short RegDst;
			 * 	short MemToReg;
			 * 	short MemRead;
			 * 	short MemWrite;
			 * 	short ALUSrc;
			 * 	short Branch;
			 * 	short ALUOP;
			 */
			ID_EX_Write.control.RegWrite = 1;
			ID_EX_Write.control.RegDst = 1;
			ID_EX_Write.control.MemToReg = 0;
			ID_EX_Write.control.MemRead = 0;
			ID_EX_Write.control.MemWrite = 0;
			ID_EX_Write.control.ALUSrc = 0;
			ID_EX_Write.control.Branch = 0;
			ID_EX_Write.control.ALUOP = 2; // FUNC

			break;
		}
		case 0x20: //load
		case 0x23: {
			/**
			 * Control Fields:
			 * 	short RegWrite;
			 * 	short RegDst;
			 * 	short MemToReg;
			 * 	short MemRead;
			 * 	short MemWrite;
			 * 	short ALUSrc;
			 * 	short Branch;
			 * 	short ALUOP;
			 */
			ID_EX_Write.control.RegWrite = 1;
			ID_EX_Write.control.RegDst = 0;
			ID_EX_Write.control.MemToReg = 1;
			ID_EX_Write.control.MemRead = 1;
			ID_EX_Write.control.MemWrite = 0;
			ID_EX_Write.control.ALUSrc = 1;
			ID_EX_Write.control.Branch = 0;
			ID_EX_Write.control.ALUOP = 0; // add

			break;
		}
		case 0x28: //sb
		case 0x2b: {
			/**
			 * Control Fields:
			 * 	short RegWrite;
			 * 	short RegDst;
			 * 	short MemToReg;
			 * 	short MemRead;
			 * 	short MemWrite;
			 * 	short ALUSrc;
			 * 	short Branch;
			 * 	short ALUOP;
			 */
			ID_EX_Write.control.RegWrite = 0;
			ID_EX_Write.control.MemRead = 0;
			ID_EX_Write.control.MemWrite = 1;
			ID_EX_Write.control.ALUSrc = 1;
			ID_EX_Write.control.Branch = 0;
			ID_EX_Write.control.ALUOP = 0; // add

			break;
		}
		}

			// source regiser 1
			int rg1 = instruction >> 21 & bitMask5;
			// source regiser 2
			int rg2 = instruction >> 16 & bitMask5;
			int rg_20_16 = rg2;
			// the destination register
			int rg_11_15 = instruction >> 11 & bitMask5;
			// last 16 bits offset
			short offSet = instruction & bitMask16;

			// source register 1
			ID_EX_Write.ReadReg1Value = Regs[rg1];
			// source register 2
			ID_EX_Write.ReadReg2Value = Regs[rg2];

			ID_EX_Write.SEOffset = offSet;
			ID_EX_Write.WriteReg_20_16 = rg_20_16;
			ID_EX_Write.WriteReg_15_11 = rg_11_15;

			int func = instruction & bitMask6;
			ID_EX_Write.Function = func;
	}

}

/**
 * Execution Stage -- read from the read version of the register and
 * write the result to the write version of the register
 * some don't care values are not set, like CalcBTA for R-type, lb, and sb.
 * @Author Anthea Yichen Li
 * @since fall 2016
 */
void EX_stage() {
	// update instruction
	uint32_t instruction = ID_EX_Read.inst;

	EX_MEM_Write.inst = instruction;

	// read from the read version of the ID_EX pipeline register and
	// write to the write version of the EX_MEM pipeline register
	EX_MEM_Write.control = ID_EX_Read.control;
	EX_MEM_Write.IncrPC = ID_EX_Read.IncrPC;
	EX_MEM_Write.Function = ID_EX_Read.Function;

	// set write register number
	if (EX_MEM_Write.control.RegDst == 0) {
		EX_MEM_Write.WriteRegNum = ID_EX_Read.WriteReg_20_16;

	} else if (EX_MEM_Write.control.RegDst == 1) {
		EX_MEM_Write.WriteRegNum = ID_EX_Read.WriteReg_15_11;
	}

	//if instruction is nop, execute instruction
	if (instruction == NOP) {
		return;
	} else {
		switch (EX_MEM_Write.control.ALUOP) {
		case 0x0:	// load or store
		{
			EX_MEM_Write.ALUResult = ID_EX_Read.ReadReg1Value
					+ ID_EX_Read.SEOffset;

			EX_MEM_Write.SWValue = ID_EX_Read.ReadReg2Value;

			EX_MEM_Write.Zero = false;

			break;
		}
		case 0x2:	// add or subtract (R-type)
		{
			if (EX_MEM_Write.Function == 0x20) { // add
				EX_MEM_Write.ALUResult = ID_EX_Read.ReadReg1Value
						+ ID_EX_Read.ReadReg2Value;

				EX_MEM_Write.SWValue = ID_EX_Read.ReadReg2Value;

				EX_MEM_Write.Zero = false;
			} else if (EX_MEM_Write.Function == 0x22) { // subtract
				EX_MEM_Write.ALUResult = ID_EX_Read.ReadReg1Value
						- ID_EX_Read.ReadReg2Value;

				EX_MEM_Write.SWValue = ID_EX_Read.ReadReg2Value;

				EX_MEM_Write.Zero = false;
			}

			else {
				cout << " unrecognized function code at EX stage" << endl;

			}
			break;
		}
		case 0x1: // beq
		{
			EX_MEM_Write.ALUResult = ID_EX_Read.ReadReg1Value
					- ID_EX_Read.ReadReg2Value;

			EX_MEM_Write.SWValue = ID_EX_Read.ReadReg2Value;

			EX_MEM_Write.Zero = true;

			EX_MEM_Write.CalcBTA = 1;
			break;
		}
		default:
			cout << "not recognized ALUOP code at EX stage" << endl;
			break;
		}

	}
}

/**
 * MEM stage
 * Read from the read version of the EX mem pipeline register and
 * write to the write version of the EX MEM pipeline register
 * @author anthea yichen Li
 * @since fall 2016
 */
void MEM_stage() {
	// update incremetal PC counter
	MEM_WB_Write.IncrPC = EX_MEM_Read.IncrPC; // PC counter
	// updte instruction
	uint32_t instruction = EX_MEM_Read.inst;
	MEM_WB_Write.inst = instruction;

	MEM_WB_Write.control = EX_MEM_Read.control;

	MEM_WB_Write.ALUResult = EX_MEM_Read.ALUResult;
	MEM_WB_Write.SWValue = EX_MEM_Read.SWValue;
	MEM_WB_Write.WriteRegNum = EX_MEM_Read.WriteRegNum;

	// load instruction read from the mem
	if (MEM_WB_Write.control.MemRead == 1) {
		MEM_WB_Write.LWDataValue = Main_Mem[MEM_WB_Write.ALUResult];
	}
	// store instruction write to mem
	if (MEM_WB_Write.control.MemWrite == 1) {
		Main_Mem[MEM_WB_Write.ALUResult] = MEM_WB_Write.SWValue & 0xff;
	}

}

/**
 * WB Stage: memory write back to registers
 * @author Anthea Yichen LI
 * @since 2016 fall
 */
void WB_stage() {
	// update incremetal PC counter
	MEM_WB_Write.IncrPC = EX_MEM_Read.IncrPC; // PC counter
	// updte instruction
	uint32_t instruction = EX_MEM_Read.inst;
	MEM_WB_Write.inst = instruction;

	MEM_WB_Write.control = EX_MEM_Read.control;

	if (MEM_WB_Write.control.RegWrite == 1) {
		if (MEM_WB_Write.control.MemToReg == 1) {
			// load instruction write back
			Regs[MEM_WB_Write.WriteRegNum] = MEM_WB_Write.LWDataValue;
		} else if (MEM_WB_Write.control.MemToReg == 0) {
			// R-type write back
			Regs[MEM_WB_Write.WriteRegNum] = MEM_WB_Write.ALUResult;
		}
	}
}

/**
 * Print call Clock Cycles
 * @author Anthea Yichen Li
 * @since fall 2016
 */
void Print_out_everything() {
	// print out regs
	cout << " Clock Cycle " << std::dec << instrCount;
	cout << endl;
	cout << endl;
	cout << " Registers : " << endl;
	for (int i = 0; i <= 0xf; i++) {
		printf(" %3x ", Regs[i]);
	}
	cout << endl;
	for (int i = 0x10; i <= 0x1f; i++) {
		printf(" %3x ", Regs[i]);
	}
	cout << endl;
	/*
	 short RegWrite;
	 short RegDst;
	 short MemToReg;
	 short MemRead;
	 short MemWrite;
	 short ALUSrc;
	 short Branch;
	 short ALUOP;
	 */
	// print out the write version of the IF_ID register
	cout << endl;
	cout
			<< "**********************PIPELINE REGISTER INFO**********************************"
			<< endl;
	cout << " IF_ID Write register (written to by the IF stage)" << endl;
	if (IF_ID_Write.inst == NOP) {
		printf("instruction = 0x%x     ", IF_ID_Write.inst);
		cout << "NOP" << endl;
		cout << endl;
	} else {
		printf("instruction = 0x%x     ", IF_ID_Write.inst);
		printf("incr PC (Incremental programming counter) = 0x%x",
				IF_ID_Write.IncrPC);
		cout << endl;
		cout << endl;
	}

	// print out the read version of the IF_ID register
	cout << " IF_ID Read register (read by the ID stage)" << endl;
	if (IF_ID_Read.inst == NOP) {
		printf("instruction = 0x%x     ", IF_ID_Read.inst);
		cout << "NOP" << endl;
		cout << endl;
	} else {
		printf("instruction = 0x%x     ", IF_ID_Read.inst);
		printf("incr PC (Incremental programming counter) = 0x%x",
				IF_ID_Read.IncrPC);
		cout << endl;
		cout << endl;
	}

	// print out the write version of the ID_EX register
	cout << " ID_EX Write register (written to by the ID stage)" << endl;
	std::cout << "instruction = " << std::hex << ID_EX_Write.inst << endl;
	if (ID_EX_Write.inst == NOP) {
		cout << "NOP" << endl;
		cout << endl;
	} else {
		cout << "control: ";

		printf(
				"RegDst=%1x, ALUSrc=%1x, ALUOP=%1x, MemRead=%1x, \n MemWrite=%1x, Branch=%1x,  MemToReg=%1x,  RegWrite=%1x \n",
				ID_EX_Write.control.RegDst, ID_EX_Write.control.ALUSrc,
				ID_EX_Write.control.ALUOP, ID_EX_Write.control.MemRead,
				ID_EX_Write.control.MemWrite, ID_EX_Write.control.Branch,
				ID_EX_Write.control.MemToReg, ID_EX_Write.control.RegWrite);
		cout << endl;
		printf(
				"IncrPC (incremental program counter) = 0x%x     ReadReg1Value = 0x%x     ReadReg2Value = 0x%x  \n SEOffset = 0x%x    WriteReg_20_16 = %d          WriteReg_15_11=%d \n function = 0x%x \n",
				ID_EX_Write.IncrPC, ID_EX_Write.ReadReg1Value,
				ID_EX_Write.ReadReg2Value, ID_EX_Write.SEOffset,
				ID_EX_Write.WriteReg_20_16, ID_EX_Write.WriteReg_15_11,
				ID_EX_Write.Function);

		cout << endl;
		cout << endl;
	}

	// print out the read version of the ID_EX register
	cout << " ID_EX Read register (read by the EX stage)" << endl;

	std::cout << "instruction = " << std::hex << ID_EX_Read.inst << endl;
	if (ID_EX_Read.inst == NOP) {
		cout << "NOP" << endl;
		cout << endl;
	} else {
		cout << "control: ";
		printf(
				"RegDst=%1x, ALUSrc=%1x, ALUOP=%1x, MemRead=%1x, \n MemWrite=%1x, Branch=%1x,  MemToReg=%1x,  RegWrite=%1x  \n",
				ID_EX_Read.control.RegDst, ID_EX_Read.control.ALUSrc,
				ID_EX_Read.control.ALUOP, ID_EX_Read.control.MemRead,
				ID_EX_Read.control.MemWrite, ID_EX_Read.control.Branch,
				ID_EX_Read.control.MemToReg, ID_EX_Read.control.RegWrite);
		cout << endl;
		printf(
				"IncrPC (incremental program counter) = 0x%x     ReadReg1Value = 0x%x     ReadReg2Value = 0x%x  \n SEOffset = 0x%x    WriteReg_20_16 = %d       WriteReg_15_11=%d \n function = 0x%x \n",
				ID_EX_Read.IncrPC, ID_EX_Read.ReadReg1Value,
				ID_EX_Read.ReadReg2Value, ID_EX_Read.SEOffset,
				ID_EX_Read.WriteReg_20_16, ID_EX_Read.WriteReg_15_11,
				ID_EX_Read.Function);

		cout << endl;
		cout << endl;
	}

	// print out the write version of the EX_MEM register
	cout << " EX_MEM Write register (written to by the EX stage)" << endl;
	std::cout << "instruction = " << std::hex << EX_MEM_Write.inst << endl;
	if (EX_MEM_Write.inst == NOP) {
		cout << "NOP" << endl;
		cout << endl;
	} else {
		cout << "control: ";
		printf(
				"RegDst=%1x, ALUSrc=%1x, ALUOP=%1x, MemRead=%1x, \n MemWrite=%1x, Branch=%1x,  MemToReg=%1x,  RegWrite=%1x \n",
				EX_MEM_Write.control.RegDst, EX_MEM_Write.control.ALUSrc,
				EX_MEM_Write.control.ALUOP, EX_MEM_Write.control.MemRead,
				EX_MEM_Write.control.MemWrite, EX_MEM_Write.control.Branch,
				EX_MEM_Write.control.MemToReg, EX_MEM_Write.control.RegWrite);
		cout << endl;
		printf(
				"CalcBTA = %d      Zero = %d   IncrPC = 0x%x \nALUResult = 0x%x      SWValue = 0x%x     WriteRegNum = %d  \n",
				EX_MEM_Write.CalcBTA, EX_MEM_Write.Zero, EX_MEM_Write.IncrPC,
				EX_MEM_Write.ALUResult, EX_MEM_Write.SWValue,
				EX_MEM_Write.WriteRegNum);

		cout << endl;
		cout << endl;
	}

	// print out the read version of the EX_MEM register
	cout << " EX_MEM Read register (read by the MEM stage)" << endl;
	std::cout << "instruction = " << std::hex << EX_MEM_Read.inst << endl;
	if (EX_MEM_Read.inst == NOP) {
		cout << "NOP" << endl;
		cout << endl;
	} else {
		cout << "control: ";
		printf(
				"RegDst=%1x, ALUSrc=%1x, ALUOP=%1x, MemRead=%1x, \n MemWrite=%1x, Branch=%1x,  MemToReg=%1x,  RegWrite=%1x \n",
				EX_MEM_Read.control.RegDst, EX_MEM_Read.control.ALUSrc,
				EX_MEM_Read.control.ALUOP, EX_MEM_Read.control.MemRead,
				EX_MEM_Read.control.MemWrite, EX_MEM_Read.control.Branch,
				EX_MEM_Read.control.MemToReg, EX_MEM_Read.control.RegWrite);
		cout << endl;
		printf(
				"CalcBTA = %d      Zero = %d   IncrPC = 0x%x \nALUResult = 0x%x      SWValue = 0x%x       WriteRegNum = %d  \n",
				EX_MEM_Read.CalcBTA, EX_MEM_Read.Zero, EX_MEM_Read.IncrPC,
				EX_MEM_Read.ALUResult, EX_MEM_Read.SWValue,
				EX_MEM_Read.WriteRegNum);
		cout << endl;
		cout << endl;
	}

	// print out the write version of the MEM_WB register
	cout << " MEM_WB Write register (written to by the MEM stage)" << endl;
	std::cout << "instruction = " << std::hex << MEM_WB_Write.inst << endl;
	if (MEM_WB_Write.inst == NOP) {
		cout << "NOP" << endl;
		cout << endl;
	} else {
		cout << "control: ";
		printf(
				"RegDst=%1x, ALUSrc=%1x, ALUOP=%1x, MemRead=%1x, \n MemWrite=%1x, Branch=%1x,  MemToReg=%1x,  RegWrite=%1x \n",
				MEM_WB_Write.control.RegDst, MEM_WB_Write.control.ALUSrc,
				MEM_WB_Write.control.ALUOP, MEM_WB_Write.control.MemRead,
				MEM_WB_Write.control.MemWrite, MEM_WB_Write.control.Branch,
				MEM_WB_Write.control.MemToReg, MEM_WB_Write.control.RegWrite);
		cout << endl;
		printf(
				"SWValue = %x       LWDataValue =  %x         \nALUResult = %x          WriteRegNum =  %x \n",
				MEM_WB_Write.SWValue, MEM_WB_Write.LWDataValue,
				MEM_WB_Write.ALUResult, MEM_WB_Write.WriteRegNum);

		cout << endl;
		cout << endl;
	}

	// print out the read version of the MEM_WB register
	cout << " MEM_WB Read register (read by the WB stage)" << endl;
	std::cout << "instruction = " << std::hex << MEM_WB_Read.inst << endl;
	if (MEM_WB_Read.inst == NOP) {
		cout << "NOP" << endl;
		cout << endl;
	} else {
		cout << "control: ";
		printf(
				"RegDst=%1x, ALUSrc=%1x, ALUOP=%1x, MemRead=%1x, \n MemWrite=%1x, Branch=%1x,  MemToReg=%1x,  RegWrite=%1x \n",
				MEM_WB_Read.control.RegDst, MEM_WB_Read.control.ALUSrc,
				MEM_WB_Read.control.ALUOP, MEM_WB_Read.control.MemRead,
				MEM_WB_Read.control.MemWrite, MEM_WB_Read.control.Branch,
				MEM_WB_Read.control.MemToReg, MEM_WB_Read.control.RegWrite);
		cout << endl;
		printf(
				"SWValue = %x       LWDataValue =  %x         \nALUResult = %x          WriteRegNum =  %x \n",
				MEM_WB_Read.SWValue, MEM_WB_Read.LWDataValue,
				MEM_WB_Read.ALUResult, MEM_WB_Read.WriteRegNum);
	}

	cout
			<< "______________________________________________________________________________"
			<< endl;
	cout << endl;
	cout << endl;

	// TODO print out cache?
}

/**
 * copy all the write registers to read registers
 * @author Anthea Yichen Li
 * @since fall 2016
 */
void Copy_write_to_read() {
	IF_ID_Read = IF_ID_Write;
	ID_EX_Read = ID_EX_Write;
	EX_MEM_Read = EX_MEM_Write;
	MEM_WB_Read = MEM_WB_Write;
}

/**
 * Initialize all the register control instructions to NOP
 * @author Anthea Yichen Li
 * @since fall 2016
 */
void initialize_pipeline_registers() {

	IF_ID_Write.inst = NOP;
	IF_ID_Read.inst = NOP;

	ID_EX_Write.inst = NOP;
	ID_EX_Read.inst = NOP;

	EX_MEM_Write.inst = NOP;
	EX_MEM_Read.inst = NOP;

	MEM_WB_Write.inst = NOP;
	MEM_WB_Read.inst = NOP;

}

int main() {
// main memory initialization;
	for (int j = 0x000; j <= 0x300; j += 0x100) {

		for (int i = 0; i <= 0xFF; i++) {
			Main_Mem[j + i] = i;
		}
	}

// register initiation
	Regs[0] = 0;
	for (int i = 1; i < 32; i++) {
		Regs[i] = 0x100 + i;
	}

// initialize pipeline register
	initialize_pipeline_registers();

	while (instrCount < INSTRUCTION_NUMBER) {
		IF_stage();
		ID_stage();
		EX_stage();
		MEM_stage();
		WB_stage();
		Print_out_everything();
		Copy_write_to_read();
	}

	return 0;

}

