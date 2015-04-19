int mod(int a, int b);
int div(int a, int b);
void printString(char* string);
void readString(char* line);
void readSector(char* buffer, int sector);
void handleInterrupt21(int ax, int bx, int cx, int dx);
void readFile(char* file_name);
void executeProgram(char* name, int segment);
void terminate();
void shell();

main() {
	
	//char buffer[13312]; /*this is the maximum size of a file*/
	//makeInterrupt21();
	//interrupt(0x21, 3, "messag\0", buffer, 0); /*read the file into buffer*/
	//interrupt(0x21, 0, buffer, 0, 0); /*print out the file*/
	//while(1); /*hang up*/
	
	//makeInterrupt21();
	//interrupt(0x21, 4, "tstpr2\0", 0x2000, 0);
	//while(1);
	
	
	
	makeInterrupt21();
	shell();
	while (1);
}

void shell() {
	char prog[6];
	prog[0] = 's';
	prog[1] = 'h';
	prog[2] = 'e';
	prog[3] = 'l';
	prog[4] = 'l';
	prog[5] = 0x0;
	interrupt(0x21, 4, prog, 0x2000, 0);
	
}

void printString(char* string) {
	while (*string != '\0') {
		interrupt(0x10, 0xe * 256 + *string, 0, 0, 0);
		string++;
	}
}

void readString(char* line) {
	char enter = 0xd;
	char back = 0x8;
	int i = 0;
	while (1) {
		char ch = interrupt(0x16, 0, 0, 0, 0);
		if (ch == enter) {
			line[i] = '\0';
			return;
		} else if (ch == back) {
			if (i > 0) {
				line[i] = 0x0;
				--i;
				interrupt(0x10, 0xe * 256 + ch, 0, 0, 0);
				interrupt(0x10, 0xe * 256 + 0x0, 0, 0, 0);
				interrupt(0x10, 0xe * 256 + ch, 0, 0, 0);
			}
		} else {
			line[i] = ch;
			interrupt(0x10, 0xe * 256 + ch, 0, 0, 0);
			++i;
		}
	}
}

void executeProgram(char* name, int segment) {
	
	char file_buffer[13312];
	int i = 0;
	
	readFile(name, file_buffer);
	while (i < 13312) {
		putInMemory(segment, i, file_buffer[i]);
		++i;
	}
	launchProgram(segment);
	
	
}

void terminate() {
	shell();
}

void readFile(char* file_name, char* buffer) {
	int i = 0;
	char dir_buffer[512];
	readSector(dir_buffer, 2);
	while (i < 16) {
		if (dir_buffer[(i * 32)] != 0x0) {
			int j = 0;
			while (j < 6) {
				if (dir_buffer[(i * 32) + j] == file_name[j]) ++j;
				else break;
			}
			if (j == 6) {
				int buffer_addr = 0;
				while (j < 32) {
					int sector = dir_buffer[(i * 32) + j];
					if (sector == 0x0) break;
					
					readSector(buffer + buffer_addr, sector);
					
					buffer_addr += 512;
					++j;
				}
			}
		}
		++i;
	}
}

void readSector(char* buffer, int sectorNumber) {
	int ah = 2; // tells BIOS to read
	int al = 1; // number of sectors to read
	int ax = ah * 256 + al;
	int bx = buffer; // address where the data should be stored to
	int ch = div(sectorNumber, 36); //0 // track number
	int cl = mod(sectorNumber, 18) + 1; //13; // relative sector number
	int cx = ch * 256 + cl;
	int dh = mod(div(sectorNumber, 18), 2); //1; // head number
	int dl = 0; // device number; 0=floppy
	int dx = dh * 256 + dl;
	
	interrupt(0x13, ax, bx, cx, dx);
}

int mod(int a, int b) {
	while (a >= b)
		a -= b;
	return a;
}

int div(int a, int b) {
	int q = 0;
	while (a >= b) {
		q++;
		a -= b;
	}
	return q;
}

void handleInterrupt21(int ax, int bx, int cx, int dx) {
	if (ax == 0) {
		printString(bx);
	} else if (ax == 1) {
		readString(bx);
	} else if (ax == 2) {
		readSector(bx, cx);
	} else if (ax == 3) {
		readFile(bx, cx);
	} else if (ax == 4) {
		executeProgram(bx, cx);
	} else if (ax == 5) {
		terminate();
	} else {
		printString("Error ax must be <= 5 \0");
	}
}
