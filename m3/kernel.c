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
void readFile(char *fileName, char *buffer);
void executeProgram(char *name, int segment);
void terminate();

/* Milestone helper functions */
int mod(int a, int b);
int div(int a, int b);
void backspace();
void loadErrorMessage(char *buffer);

int main() {
  char shell[6];
  shell[0] = 's';
  shell[1] = 'h';
  shell[2] = 'e';
  shell[3] = 'l';
  shell[4] = 'l';
  shell[5] = '\0';

  makeInterrupt21();
  interrupt(0x21, 4, shell, 0x2000, 0);

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
  char error[19];
  loadErrorMessage(error);
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
  } else {
    printString(error);
  }
}

void readFile(char *fileName, char *buffer) {
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
      break;
    }
  }
  if (i == 512) {
    return;
  }
}

/*
  void putInMemory (int segment, int address, char character)
*/
void executeProgram(char *name, int segment) {
  char buffer[13312];
  int address;
  readFile(name, buffer);

  for (address = 0; address < 13312; address++) {
    putInMemory(segment, address, buffer[address]);
  }

  launchProgram(segment);
}

void terminate() {
  char shell[6];
  shell[0] = 's';
  shell[1] = 'h';
  shell[2] = 'e';
  shell[3] = 'l';
  shell[4] = 'l';
  shell[5] = '\0';
  interrupt(0x21, 4, shell, 0x2000, 0);
}

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