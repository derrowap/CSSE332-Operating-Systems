/* Team 1-F:
    deradaam
    derrowap
    finngw
*/

/* Milestone required functions */
void printString(char *chars);
void readString(char *buffer);
void readSector(char *buffer, int sector);
void handleInterrupt21(int ax, int bx, int cx, int dx);

/* Milestone helper functions */
int mod(int a, int b);
int div(int a, int b);
void backspace();


int main() {
  char buffer[512];
  char line[80];

  printString("Hello World\n");

  /* set the interrupt handler */
  makeInterrupt21();

  /* should print out an error message for invalid ax value */
  interrupt(0x21, 3, line, 0, 0);

  /* reads the message.txt from sector 30 and then prints it */
  interrupt(0x21, 2, buffer, 30, 0);
  interrupt(0x21, 0, buffer, 0, 0);

  /* reads input from keyboard until ENTER key is pressed, then prints it */
  interrupt(0x21, 1, line, 0, 0);
  interrupt(0x21, 0, line, 0, 0);

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
  Else:
    print an error message.
*/
void handleInterrupt21(int ax, int bx, int cx, int dx) {
  if (ax == 0) {
    printString((char *) bx);
  } else if (ax == 1) {
    readString((char *) bx);
  } else if (ax == 2) {
    readSector((char *) bx, cx);
  } else {
    printString("Error: invalid ax to handleInterrupt21!\n");
  }
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
