/* Team 1-F:
    deradaam
    derrowap
    finngw
*/

/* Milestone 2 required functions */
void printString(char *chars);
void readString(char *buffer);
void readSector(char *buffer, int sector);
void handleInterrupt21(int ax, int bx, int cx, int dx);

/* Milestone 3 required functions */
int readFile(char *fileName, char *buffer);
void executeProgram(char *name, int wait_pid);
void terminate();

/* Milestone 4 required functions */
void writeSector(char *buffer, int sector);
void deleteFile(char *fileName);
void writeFile(char* name, char* buffer, int numberOfSectors);

/* Milestone 5 required functions */
void handleTimerInterrupt(int segment, int sp);
void killProcess(int pid);

/* Milestone helper functions */
int mod(int a, int b);
int div(int a, int b);
void backspace();
void loadErrorMessage(char *buffer);
void loadFileNotFound(char *buffer);
void printProcessTable();
void stopWaiting(int pid);
void waitOnProcess(int waiting_process, int process_to_wait_on);

typedef struct entry {
  int active, stack_pointer, waiting;
} Entry;

Entry ProcessTable[8];
int currentProcess;

int main() {
  char shell[6];
  shell[0] = 's';
  shell[1] = 'h';
  shell[2] = 'e';
  shell[3] = 'l';
  shell[4] = 'l';
  shell[5] = 0x0;

  for (currentProcess = 0; currentProcess < 8; currentProcess++) {
    ProcessTable[currentProcess].active = 0;
    ProcessTable[currentProcess].stack_pointer = 0xff00;
    ProcessTable[currentProcess].waiting = -1;
  }
  currentProcess = 0;

  makeInterrupt21();
  interrupt(0x21, 4, shell, -1, 0);
  makeTimerInterrupt();

  while (1) {}
  return 0;
}

/* Displays to the console the given string at the current cursor's position */
void printString(char *chars) {
  for (; *chars != '\0'; chars++) {
    if (*chars == 0xa) {
      interrupt(0x10, 0xe * 256 + 0xd, 0, 0, 0);
    }
    interrupt(0x10, 0xe * 256 + *chars, 0, 0, 0);
  }
}

/* Reads and stores into the buffer input from the keyboard.

  When the ENTER key is pressed, it will stop reading input and fix the buffer
  to end in a new line and null character.

  When the backspace is pressed, it removes that input from the buffer and
  overrides the previously displayed character on the console. If the
  backsapce is pressed even when there are no more characters to remove,
  it will remain at the cursor's current position as it should.
*/
void readString(char *buffer) {
  char key;
  int i;
  key = interrupt(0x16, 0, 0, 0, 0);
  i = 0;
  while (key != 0xd && i < 78) {
    if (key == 0x8) {
      if (i > 0) {
        backspace();
        i--;
      }
      key = interrupt(0x16, 0, 0, 0, 0);
      continue;
    }
    interrupt(0x10, 0xe * 256 + key, 0, 0, 0);
    *(buffer + i) = key;
    i++;
    key = interrupt(0x16, 0, 0, 0, 0);
  }
  interrupt(0x10, 0xe * 256 + 0xd, 0, 0, 0);
  interrupt(0x10, 0xe * 256 + 0xa, 0, 0, 0);
  *(buffer + i) = 0xa;
  *(buffer + i + 1) = 0x0;
}

/* Reads and stores to the buffer the data at the given sector.

int interrupt (int number, int AX, int BX, int CX, int DX)

where:
  AX = AH * 256 + AL
  BX = BH * 256 + BL
  CX = CH * 256 + CL
  DX = DH * 256 + DL
*/
void readSector(char *buffer, int sector) {
  interrupt(0x13, 2 * 256 + 1,
    buffer,
    div(sector, 36) * 256 + mod(sector, 18) + 1,
    mod(div(sector, 18), 2) * 256 + 0);
}

/* This function is executed whenever a 0x21 interrupt occurs.

  The makeInterrupt21() function must be called before this will actually
  execute every time a 0x21 interrupt occurs.

  If AX == 0:
    print the string passed in the BX register.
  If AX == 1:
    execute readString by passing buffer passed in the BX register.
  If AX == 2:
    execute readSector by passing the buffer passed in BX register at
    the sector number passed in by the CX register.
  If AX == 3:
    BX = address of character array containing the file name
    CX = address of a buffer to hold the file
  If AX == 4:
    BX = address of character array holding the name of the program
    CX = segment in memory to put the program
  If AX == 5:
    terminate
  Else:
    print an error message.
*/
void handleInterrupt21(int ax, int bx, int cx, int dx) {
  int i;
  char error[19];
  if (ax == 0) {
    printString((char *) bx);
  } else if (ax == 1) {
    readString((char *) bx);
  } else if (ax == 2) {
    readSector((char *) bx, cx);
  } else if (ax == 3) {
    readFile((char *) bx, (char *) cx);
  } else if (ax == 4) {
    executeProgram((char *) bx, cx);
  } else if (ax == 5) {
    terminate();
  } else if (ax == 6) {
    writeSector((char *) bx, cx);
  } else if (ax == 7) {
    deleteFile((char *) bx);
  } else if (ax == 8) {
    writeFile((char *) bx, (char *) cx, dx);
  } else if (ax == 9) {
    killProcess(bx);
  } else if (ax == 11) {
    printProcessTable();
  } else if (ax == 12) {
    waitOnProcess(bx, cx);
  } else {
    loadErrorMessage(error);
    printString(error);
  }
}

int readFile(char *fileName, char *buffer) {
  char directory[512];
  int i, j, sector;

  readSector(directory, 2);

  for (i = 0; i < 512; i += 32) {
    for (j = 0; j < 6; j++) {
      if (fileName[j] != directory[i + j]) {
        break;
      }
    }
    if (j == 6) {
      for (j = 0; j < 26; j++) {
        sector = (int) directory[i + 6 + j];
        if (sector == 0) {
          break;
        }
        readSector(buffer + 512 * j, sector);
      }
      return 1;
    }
  }
  loadFileNotFound(buffer);
  return -1;
}

/*
  void putInMemory (int segment, int address, char character)
*/
void executeProgram(char *name, int wait_pid) {
  char buffer[13312];
  int i, address, segment, active;

  if (readFile(name, buffer) == -1) {
    return;
  }

  for (i = 0; i < 8; i++) {
    setKernelDataSegment();
    active = ProcessTable[i].active;
    restoreDataSegment();
    if (active == 0) {
      segment = (i + 2) * 0x1000;
      setKernelDataSegment();
      ProcessTable[i].stack_pointer = 0xff00;
      ProcessTable[i].waiting = -1;
      restoreDataSegment();
      break;
    }
  }

  for (address = 0; address < 13312; address++) {
    putInMemory(segment, address, buffer[address]);
  }

  initializeProgram(segment);

  if (wait_pid >= 0 && wait_pid < 8) {
    waitOnProcess(wait_pid, i);
  }

  setKernelDataSegment();
  ProcessTable[i].active = 1;
  restoreDataSegment();
}

void terminate() {
  int i;
  setKernelDataSegment();
  i = currentProcess;
  ProcessTable[currentProcess].active = 0;
  restoreDataSegment();
  stopWaiting(i);
  while (1) {}
}

/* Milestone 4 */
void writeSector(char *buffer, int sector) {
  interrupt(0x13, 3 * 256 + 1,
    buffer,
    div(sector, 36) * 256 + mod(sector, 18) + 1,
    mod(div(sector, 18), 2) * 256 + 0);
}

void deleteFile(char *fileName) {
  char directory[512], map[512];
  int i, j, sector;
  readSector(map, 1);
  readSector(directory, 2);

  for (i = 0; i < 512; i += 32) {
    for (j = 0; j < 6; j++) {
      if (fileName[j] != directory[i + j]) {
        break;
      }
    }
    if (j == 6) {
      for (j = 0; j < 26; j++) {
        sector = (int) directory[i + 6 + j];
        if (sector == 0) {
          break;
        }
        map[sector] = 0x00;
      }
      directory[i] = 0x00;
      break;
    }
  }

  writeSector(map, 1);
  writeSector(directory, 2);
}

void writeFile(char* name, char* buffer, int numberOfSectors) {
  char directory[512], map[512];
  int i, j, sector, sectorsWritten, directoryIndex;
  readSector(map, 1);
  readSector(directory, 2);

  for (i = 0; i < 512; i += 32) {
    if (directory[i] == 0x00) {
      directoryIndex = i;
      for (j = 0; j < 6; j++) {
        directory[i + j] = name[j];
      }
      break;
    }
  }

  sectorsWritten = 0;
  for (i = 0; i < 512 && sectorsWritten < numberOfSectors; i += 1) {
    if (map[i] == 0x00) {
      map[i] = 0xFF;
      directory[directoryIndex + 6 + sectorsWritten] = i;
      writeSector(buffer + sectorsWritten * 512, i);
      sectorsWritten++;
    }
  }
  for (i = sectorsWritten + 6; i < 32; i++) {
    directory[directoryIndex + i] = 0x00;
  }

  writeSector(map, 1);
  writeSector(directory, 2);
}

/* Milestone 5 */
void handleTimerInterrupt(int segment, int sp) {
  int i, count;
  /*char message[5];
  message[0] = 'T';
  message[1] = 'i';
  message[2] = 'c';
  message[3] = '\n';
  message[4] = '\0';
  printString(message);*/

  i = (segment / 0x1000) - 2;
  if (i >= 0 && i < 8 && ProcessTable[i].active) {
    ProcessTable[i].stack_pointer = sp;
  }

  i = currentProcess + 1;
  for (count = 0; count <= 8; count++ && i++) {
    if (i > 7) {
      i = 0;
    }
    if (ProcessTable[i].active && ProcessTable[i].waiting == -1) {
      currentProcess = i;
      sp = ProcessTable[i].stack_pointer;
      segment = (i + 2) * 0x1000;
      break;
    }
  }

  returnFromTimer(segment, sp);
}

void killProcess(int pid) {
  setKernelDataSegment();
  ProcessTable[pid].active = 0;
  restoreDataSegment();
  stopWaiting(pid);
}

/* Helper functions */

/* Computes a mod b */
int mod(int a, int b) {
  for (; a >= b; a -= b) {}
  return a;
}

/* Computes a / b, such that it is integer division. */
int div(int a, int b) {
  int quotient;
  for (quotient = 0; (quotient + 1) * b <= a; quotient++) {}
  return quotient;
}

/* Prints a backspace by overwriting what was previously there. */
void backspace() {
  interrupt(0x10, 0xe * 256 + 0x8, 0, 0, 0);
  interrupt(0x10, 0xe * 256 + ' ', 0, 0, 0);
  interrupt(0x10, 0xe * 256 + 0x8, 0, 0, 0);
}

void loadErrorMessage(char *buffer) {
  buffer[0] = 'I';
  buffer[1] = 'n';
  buffer[2] = 'v';
  buffer[3] = 'a';
  buffer[4] = 'l';
  buffer[5] = 'i';
  buffer[6] = 'd';
  buffer[7] = ' ';
  buffer[8] = 'i';
  buffer[9] = 'n';
  buffer[10] = 't';
  buffer[11] = 'e';
  buffer[12] = 'r';
  buffer[13] = 'r';
  buffer[14] = 'u';
  buffer[15] = 'p';
  buffer[16] = 't';
  buffer[17] = '\n';
  buffer[18] = '\0';
}

void loadFileNotFound(char *buffer) {
  buffer[0] = 'F';
  buffer[1] = 'i';
  buffer[2] = 'l';
  buffer[3] = 'e';
  buffer[4] = ' ';
  buffer[5] = 'n';
  buffer[6] = 'o';
  buffer[7] = 't';
  buffer[8] = ' ';
  buffer[9] = 'f';
  buffer[10] = 'o';
  buffer[11] = 'u';
  buffer[12] = 'n';
  buffer[13] = 'd';
  buffer[14] = '\n';
  buffer[15] = '\0';
}

void printProcessTable() {
  int i, j, wait_pid;
  char message[24];
  message[0] = 'p';
  message[1] = 'i';
  message[2] = 'd';
  message[3] = ' ';
  message[4] = '|';
  message[5] = ' ';
  message[6] = 'a';
  message[7] = 'c';
  message[8] = 't';
  message[9] = 'i';
  message[10] = 'v';
  message[11] = 'e';
  message[12] = ' ';
  message[13] = '|';
  message[14] = ' ';
  message[15] = 'w';
  message[16] = 'a';
  message[17] = 'i';
  message[18] = 't';
  message[19] = 'i';
  message[20] = 'n';
  message[21] = 'g';
  message[22] = '\n';
  message[23] = '\0';
  printString(message);

  for (i = 0; i < 8; i++) {
    message[0] = ' ';
    message[1] = '0' + i;
    for (j = 2; j < 9; j++) {
      message[j] = ' ';
    }
    setKernelDataSegment();
    message[9] = '0' + ProcessTable[i].active;
    restoreDataSegment();

    for (j = j + 1; j < 18; j++) {
      message[j] = ' ';
    }
    setKernelDataSegment();
    wait_pid = ProcessTable[i].waiting;
    restoreDataSegment();
    if (wait_pid == -1) {
      message[j] = 'X';
    } else {
      message[j] = '0' + wait_pid;
    }

    message[19] = '\n';
    message[20] = '\0';
    printString(message);
  }
}

void stopWaiting(int pid) {
  int i, wait_id;
  for (i = 0; i < 8; i++) {
    setKernelDataSegment();
    wait_id = ProcessTable[i].waiting;
    restoreDataSegment();
    if (wait_id == pid) {
      setKernelDataSegment();
      ProcessTable[i].waiting = -1;
      restoreDataSegment();
    }
  }
}

void waitOnProcess(int waiting_process, int process_to_wait_on) {
  setKernelDataSegment();
  ProcessTable[waiting_process].waiting = process_to_wait_on;
  restoreDataSegment();
}
