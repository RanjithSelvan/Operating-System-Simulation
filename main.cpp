#include <iostream>
const int TIMEOUTWAIT = 1;
const int EXITWAIT = 1;
const int MEM_SIZE = 1024;
const int FRAME_SIZE = 16;
const int NUM_FRAMES = MEM_SIZE / FRAME_SIZE;

struct PTR_LINE{
	int frame;
	int valid;
};

struct MEMORY{
	int arr[MEM_SIZE];
	MEMORY(){
		for (int i = 0; i < MEM_SIZE; i++){
			arr[i] = 0;
		}
	}

	int READWORD(int loc){
		if (loc >= 0 && loc < 1000)
			return arr[loc];
	}
	void WRITEWORD(int loc, int val){
		if (loc >= 0 && loc < 1000)
			arr[loc] = val;
	}
	int* READFRAME(int frameNum){
		int* frame = new int[FRAME_SIZE];
		int loc = frameNum * 16;
		for (int i = 0; i < FRAME_SIZE; i++){
			frame[i] = arr[loc];
			loc++;
		}
		return frame;
	}
	void WRITEFRAME(int frameNum, int* frame){
		int loc = frameNum * 16;
		if (loc >= 0 && loc < 1000 - 16){
			for (int i = 0; i < 16; i++){
				arr[loc] = frame[i];
				loc++;
			}
		}
	}
};

struct CPU{
	int PC;
	struct{
		int inst = 0;
		int A(){
			return (inst & 0x80000000) >> 31;
		}
		int M(){
			return (inst & 0x40000000) >> 30;
		}
		int OPCODE(){
			return (inst & 0x3ff00000) >> 20;
		}
		int OP1(){
			return (inst & 0x000FFC00) >> 10;
		}
		int OP2(){
			return inst & 0x000003FF;
		}
	}INST;
	PTR_LINE PTR[NUM_FRAMES];
	int REG[4];
	int OPR1;
	int OPR2;

	int InterruptFlag;
	int counter;
	MEMORY* MEM;

	CPU(MEMORY* mem) :PC(0), OPR1(0), OPR2(0), InterruptFlag(0), counter(0),MEM(mem){
		REG[0] = 0;
		REG[1] = 0;
		REG[2] = 0;
		REG[3] = 0;
	}
	int run(){
		while (true){

			//Fetch Instruction
			FETCH();
			int addMode = (INST.A() << 1) | INST.M();
			int OPCODE = INST.OPCODE();
			//Fetch Data
			if (addMode == 0){
				FETCH_DIR_DIR();
			}
			else if (addMode == 1){
				FETCH_DIR_REG();
			}
			else if (addMode == 2){
				FETCH_IND_REG();
			}
			else if (addMode == 3){
				FETCH_REG_REG();
			}
			//Execute
			printf("\tExecuting:\n");
			counter++;
			switch (OPCODE){
			case 0:
				//NOP
				break;
			case 1:
				ADD();
				break;
			case 2:
				SUB();
				break;
			case 3:
				MUL();
				break;
			case 4:
				DIV();
				break;
			case 5:
				MOD();
				break;
			case 6:
				AND();
				break;
			case 7:
				OR();
				break;
			case 8:
				JMP();
				break;
			case 9:
				ISZ();
				break;
			case 10:
				LDM();
				break;
			case 11:
				LDI();
				break;
			case 12:
				STO();
				break;
			case 13:
				EXIT();
				break;
			}
			if (counter == 4 && InterruptFlag == 0){
				InterruptFlag = 1;
			}
			//Interrupt
			if (InterruptFlag)
				return InterruptFlag;
		}
	}

	//CPU Cycles
	void FETCH(){
		printf("\tFetching INSTRUCTION:\n");
		int offset = PC % 16;
		int page = PC / 16;
		int frame = PTR[page].frame;
		int addr = frame * 16 + offset;
		INST.inst = MEM->READWORD(addr);
		printf("\t\tINST = MEM[%d] = %d => |A|M|OPCODE|OP1|OP2| = |%d|%d|%d|%d|%d|\n", addr, INST.inst, INST.A(), INST.M(), INST.OPCODE(), INST.OP1(), INST.OP2());
		PC++;
	}
	void FETCH_DIR_DIR(){
		printf("\tFetching DATA:\n");
		int offset = INST.OP1() % 16;
		int page = INST.OP1() / 16;
		int frame = PTR[page].frame;
		int addr = frame * 16 + offset;
		OPR1 = MEM->READWORD(addr);

		offset = INST.OP2() % 16;
		page = INST.OP2() / 16;
		frame = PTR[page].frame;
		addr = frame * 16 + offset;
		OPR2 = MEM->READWORD(addr);
		printf("\t\tOPR1 = MEM[%d] = %d\n\t\tOPR2 = MEM[%d] = %d\n", INST.OP1(), OPR1, INST.OP2(), OPR2);
	}
	void FETCH_DIR_REG(){
		int offset = INST.OP1() % 16;
		int page = INST.OP1() / 16;
		int frame = PTR[page].frame;
		int addr = frame * 16 + offset;
		OPR1 = MEM->READWORD(addr);

		OPR2 = REG[INST.OP2()];
	}
	void FETCH_IND_REG(){
		OPR1 = MEM->READWORD(MEM->READWORD(INST.OP1()));
		OPR2 = INST.OP2();
	}
	void FETCH_REG_REG(){
		OPR1 = REG[INST.OP1()];
		OPR2 = REG[INST.OP2()];
	}

	//CPU Instructions
	void ADD(){
		printf("\t\tADD\n");
		OPR2 = OPR2 + OPR1;

		int offset = INST.OP2() % 16;
		int page = INST.OP2() / 16;
		int frame = PTR[page].frame;
		int addr = frame * 16 + offset;
		MEM->WRITEWORD(addr, OPR2);
	}
	void SUB(){
		printf("\t\tSUB\n");
		OPR2 = OPR2 - OPR1;

		int offset = INST.OP2() % 16;
		int page = INST.OP2() / 16;
		int frame = PTR[page].frame;
		int addr = frame * 16 + offset;
		MEM->WRITEWORD(addr, OPR2);
	}
	void MUL(){
		printf("\t\tMUL\n");
		OPR2 = OPR2 * OPR1;

		int offset = INST.OP2() % 16;
		int page = INST.OP2() / 16;
		int frame = PTR[page].frame;
		int addr = frame * 16 + offset;
		MEM->WRITEWORD(addr, OPR2);
	}
	void DIV(){
		printf("\t\tDIV\n");
		OPR2 = OPR2 / OPR1;

		int offset = INST.OP2() % 16;
		int page = INST.OP2() / 16;
		int frame = PTR[page].frame;
		int addr = frame * 16 + offset;
		MEM->WRITEWORD(addr, OPR2);
	}
	void MOD(){
		printf("\t\tMOD\n");
		OPR2 = OPR2 % OPR1;

		int offset = INST.OP2() % 16;
		int page = INST.OP2() / 16;
		int frame = PTR[page].frame;
		int addr = frame * 16 + offset;
		MEM->WRITEWORD(addr, OPR2);
	}
	void AND(){
		printf("\t\tAND\n");
		OPR2 = OPR1 & OPR2;

		int offset = INST.OP2() % 16;
		int page = INST.OP2() / 16;
		int frame = PTR[page].frame;
		int addr = frame * 16 + offset;
		MEM->WRITEWORD(addr, OPR2);
	}
	void OR(){
		printf("\t\tOR\n");
		OPR2 = OPR1 | OPR2;

		int offset = INST.OP2() % 16;
		int page = INST.OP2() / 16;
		int frame = PTR[page].frame;
		int addr = frame * 16 + offset;
		MEM->WRITEWORD(addr, OPR2);
	}
	void JMP(){
		printf("\t\tJMP\n");
		PC = OPR1;
	}
	void ISZ(){
		printf("\t\tISZ\n");
		OPR1 = OPR1 + 1;

		int offset = INST.OP1() % 16;
		int page = INST.OP1() / 16;
		int frame = PTR[page].frame;
		int addr = frame * 16 + offset;
		MEM->WRITEWORD(addr, OPR1);
		if (OPR1 == 0)
			PC = PC + 1;
	}
	void LDM(){
		printf("\t\tLDM\n");
		REG[OPR2] = OPR1;
	}
	void LDI(){
		printf("\t\tLDI\n");
		REG[OPR2] = INST.OP1();
	}
	void STO(){
		printf("\t\tSTO\n");
		int offset = INST.OP2() % 16;
		int page = INST.OP2() / 16;
		int frame = PTR[page].frame;
		int addr = frame * 16 + offset;
		MEM->WRITEWORD(addr, OPR1);
	}
	void EXIT(){
		printf("\t\tEXIT\n");
		InterruptFlag = -1;
	}
};

struct PCB{
	int PC;
	int INST;
	PTR_LINE PTR[NUM_FRAMES];
	int REG[4];
	int OPR1;
	int OPR2;
};

struct NODE{
	int name;
	NODE* prev;
	NODE* next;
	PCB pcb;
};

struct OS{
	CPU* cpu;
	MEMORY* mem;
	NODE* READYQ;

	//Initialize Simulation
	OS(){
		mem = new MEMORY();
		cpu = new CPU(mem);

		//Setup circular Ready queue of PCB NODES
		int i;
		//Setup PCB for Prog 1
		NODE* start = READYQ = new NODE();

		READYQ->name = 1;
		READYQ->pcb.INST = 0;
		READYQ->pcb.PC = 0;
		READYQ->pcb.OPR1 = 0;
		READYQ->pcb.OPR2 = 0;
		for (int i = 0; i < 4; i++){
			READYQ->pcb.REG[i] = 0;
		}
		READYQ->pcb.PTR[0].frame = 0;
		READYQ->pcb.PTR[0].valid = 1;

		READYQ->pcb.PTR[1].frame = 2;
		READYQ->pcb.PTR[1].valid = 1;

		printf("Loading Program: 1 to Memory\n");
		//16 Adds
		i = 16 * 0;
		for (i; i < 16; i++){
			mem->arr[i] = 0x105015;
		}
		//Next Frame
		i = 16 * 2;
		mem->arr[i++] = 0x904c00;	//ISZ
		mem->arr[i++] = 0x805800;	//JMP
		mem->arr[i++] = 0xd00000;	//EXIT
		mem->arr[i++] = -0x5;		//Loop Count
		mem->arr[i++] = 0x1;		//Adder
		mem->arr[i++] = 0x0;		//Total

		//Setup PCB for Prog 2
		READYQ->next = new NODE();
		READYQ->next->prev = READYQ;
		READYQ = READYQ->next;
		READYQ->name = 2;
		
		READYQ->pcb.INST = 0;
		READYQ->pcb.PC = 0;
		READYQ->pcb.OPR1 = 0;
		READYQ->pcb.OPR2 = 0;
		for (int i = 0; i < 4; i++){
			READYQ->pcb.REG[i] = 0;
		}
		READYQ->pcb.PTR[0].frame = 1;
		READYQ->pcb.PTR[0].valid = 1;

		READYQ->pcb.PTR[1].frame = 3;
		READYQ->pcb.PTR[1].valid = 1;

		printf("Loading Program: 2 to Memory\n");
		//16 Subs
		i = 16 * 1;
		for (i; i < 32; i++){
			mem->arr[i] = 0x205015;
		}
		//Next Frame
		i = 16 * 3;
		mem->arr[i++] = 0x904c00;	//ISZ
		mem->arr[i++] = 0x805800;	//JMP
		mem->arr[i++] = 0xd00000;	//EXIT
		mem->arr[i++] = -0x9;		//Loop Count
		mem->arr[i++] = 0x1;		//Adder
		mem->arr[i++] = 0x0;		//Total

		//Setup PCB for Prog 3
		READYQ->next = new NODE();
		READYQ->next->prev = READYQ;
		READYQ = READYQ->next;
		READYQ->name = 3;

		READYQ->pcb.INST = 0;
		READYQ->pcb.PC = 0;
		READYQ->pcb.OPR1 = 0;
		READYQ->pcb.OPR2 = 0;
		for (int i = 0; i < 4; i++){
			READYQ->pcb.REG[i] = 0;
		}
		READYQ->pcb.PTR[0].frame = 4;
		READYQ->pcb.PTR[0].valid = 1;

		READYQ->pcb.PTR[1].frame = 6;
		READYQ->pcb.PTR[1].valid = 1;

		printf("Loading Program: 3 to Memory\n");
		//16 Mults
		i = 16 * 4;
		for (i; i < 80; i++){
			mem->arr[i] = 0x305015;
		}
		//Next Frame
		i = 16 * 6;
		mem->arr[i++] = 0x904c00;	//ISZ
		mem->arr[i++] = 0x805800;	//JMP
		mem->arr[i++] = 0xd00000;	//EXIT
		mem->arr[i++] = -0x3;		//Loop Count
		mem->arr[i++] = 0x2;		//Adder
		mem->arr[i++] = 0x2;		//Total

		//Close Circle
		start->prev = READYQ;
		READYQ->next = start;
		READYQ = start;

		printf("Loading program 1 to CPU\n");
		//Set Prog1 as intial Program in CPU
		for (int i = 0; i < FRAME_SIZE; i++){
			cpu->PTR[i] = start->pcb.PTR[i];
		}

	}
	void RUN(){
		do{
			int interrupt = cpu->run();
			switch (interrupt)
			{
			case -1:
				EXIT();
			case 1:
				TIMEOUT();
				break;
			default:
				break;
			}
		} while (true);
	};

	//OS Interrupts
	void EXIT(){
		printf("Program %d has completed and is Exiting.\n", READYQ->name);
		NODE* complete = READYQ;
		READYQ->prev->next = READYQ->next;
		READYQ->next->prev = READYQ->prev;
		READYQ = READYQ->next;
		
		if (EXITWAIT == 1)
			std::cin.get();
		if (complete == READYQ){
			printf("End of Simulation. Press Enter/Return to quit.\n");
			std::cin.get();
			exit(0);
		}
		else{
			delete complete;
			printf("Loading program %d to CPU\n", READYQ->name);
			cpu->PC = READYQ->pcb.PC;
			for (int i = 0; i < NUM_FRAMES; i++){
				cpu->PTR[i] = READYQ->pcb.PTR[i];
			}
		}
	};
	void TIMEOUT(){
		cpu->counter = 0;
		cpu->InterruptFlag = 0;
		
		//Store to PCB
		printf("Program %d, timed out. Saving to PCB\n", READYQ->name);
		READYQ->pcb.PC = cpu->PC;

		if (TIMEOUTWAIT == 1)
			std::cin.get();
		//Load NEXT PCB
		READYQ = READYQ->next;
		printf("Loading program %d to CPU\n", READYQ->name);
		cpu->PC = READYQ->pcb.PC;
		for (int i = 0; i < NUM_FRAMES; i++){
			cpu->PTR[i] = READYQ->pcb.PTR[i];
		}
	};
};

using namespace std;

int main(int argc, int**argv){
	OS RJ;


	RJ.RUN();
	cin.get();
}