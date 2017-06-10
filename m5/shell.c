/* Team 1-F:
    deradaam
    derrowap
    finngw
*/

/* helper functions */
int getCommandId(char *command);
int isTypeCommand(char *command);
int isExecuteCommand(char *command);
int isDeleteCommand(char *command);
int isCopyCommand(char *command);
int isDirCommand(char *command);
int isCreateCommand(char *command);
int isKillCommand(char *command);
int isExecforegoundCommand(char *command);
int isPSCommand(char *command);
int isHelpCommand(char *command);
int isWaitCommand(char *command);

void typeCommand(char *line);
void executeCommand(char *line);
void deleteCommand(char *line);
void copyCommand(char *line);
void dirCommand();
void createCommand(char *line);
void killCommand(char *line);
void execforegroundCommand(char *line);
void psCommand();
void waitCommand(char *line);

void fillPromptName(char *buffer);
void fillErrorMessage(char *buffer);
int mod(int a, int b);
int div(int a, int b);

int main() {
  char line[80], prompt[6], error[14];
  int command, segment, i;
  fillPromptName(prompt);
  fillErrorMessage(error);
  enableInterrupts();

  while (1) {
    interrupt(0x21, 0, prompt, 0, 0);
    interrupt(0x21, 1, line, 0, 0);
    command = getCommandId(line);
    if (command == 1) {
      typeCommand(line + 5);
    } else if (command == 2) {
      executeCommand(line);
    } else if (command == 3) {
      deleteCommand(line);
    } else if (command == 4) {
      copyCommand(line);
    } else if (command == 5) {
      dirCommand();
    } else if (command == 6) {
      createCommand(line + 7);
    } else if (command == 7) {
      killCommand(line + 5);
    } else if (command == 8) {
      execforegroundCommand(line + 15);
    } else if (command == 9) {
      psCommand();
    } else if (command == 10) {
      line[4] = line[5];
      line[5] = '.';
      line[6] = 0xa;
      typeCommand(line);
    } else if (command == 11) {
      waitCommand(line + 5);
    } else if (line[0] != 0xa || line[1] != 0x0) {
      /* unsupported command was called */
      interrupt(0x21, 0, error, 0, 0);
    }
  }
  return 0;
}

void typeCommand(char *line) {
  char fileName[6], buffer[13312];
  int i;
  for (i = 0; i < 6 && line[i] != 0xa; i++) {
    fileName[i] = line[i];
  }
  for (; i < 6; i++) {
    fileName[i] = 0x0;
  }
  interrupt(0x21, 3, fileName, buffer, 0);
  interrupt(0x21, 0, buffer, 0, 0);
}

void executeCommand(char *line) {
  char fileName[6];
  int i;
  for (i = 0; i < 6 && line[5 + i] != 0xa; i++) {
    fileName[i] = line[8 + i];
  }
  for (; i < 6; i++) {
    fileName[i] = 0x0;
  }
  interrupt(0x21, 4, fileName, -1, 0);
}

void deleteCommand(char *line) {
  char fileName[6];
  int i;
  for (i = 0; i < 6 && line[7 + i] != 0xa; i++) {
    fileName[i] = line[7 + i];
  }
  for (; i < 6; i++) {
    fileName[i] = 0x0;
  }
  interrupt(0x21, 7, fileName, 0, 0);
}

void copyCommand(char *line) {
  char file1[6], file2[6], buffer[13312], directory[512];
  int i, j, temp, numSectors;
  for (i = 5; line[i] != ' ' && i < 5 + 6; i++) {
    file1[i - 5] = line[i];
  }
  for (j = i; j < 5 + 6; j++) {
    file1[j - 5] = 0x0;
  }

  for (; line[i] != ' '; i++) {}
  i++;
  temp = i;
  for (; line[i] != 0xa && i < temp + 6; i++) {
    file2[i - temp] = line[i];
  }
  for (; i < temp + 6; i++) {
    file2[i - temp] = 0x0;
  }

  interrupt(0x21, 3, file1, buffer, 0);
  interrupt(0x21, 2, directory, 2);

  /* find number of sectors to write */
  numSectors = 0;
  for (i = 0; i < 512; i += 32) {
    for (j = 0; j < 6; j++) {
      if (file1[j] != directory[i + j]) {
        break;
      }
    }
    if (j == 6) {
      for (j = 0; j < 26; j++) {
        if ((int) directory[i + 6 + j] == 0) {
          break;
        }
        numSectors++;
      }
      break;
    }
  }

  interrupt(0x21, 8, file2, buffer, numSectors);
}

void dirCommand() {
  char directory[512], fileName[11];
  int i, j, k, numSectors;

  interrupt(0x21, 2, directory, 2, 0);

  for (i = 0; i < 512; i += 32) {
    if (directory[i] != 0x0) {
      numSectors = 0;
      for (j = i; j < i + 6 && directory[j] != 0x0; j++) {
        fileName[j - i] = directory[j];
      }
      fileName[j - i] = ' ';
      for (k = i + 6; k < i + 32 && directory[k] != 0x0; k++) {
        numSectors++;
      }
      if (numSectors > 9) {
        fileName[j - i + 1] = '0' + div(numSectors, 10);
        fileName[j - i + 2] = '0' + mod(numSectors, 10);
        fileName[j - i + 3] = '\n';
        fileName[j - i + 4] = '\0';
      } else {
        fileName[j - i + 1] = '0' + numSectors;
        fileName[j - i + 2] = '\n';
        fileName[j - i + 3] = '\0';
      }
      
      interrupt(0x21, 0, fileName, 0, 0);
    }
  }
}

void createCommand(char *line) {
  char fileName[6], buffer[13312];
  int i, fileSize;

  for (i = 0; i < 6 && line[i] != 0xa; i++) {
    fileName[i] = line[i];
  }
  for (; i < 6; i++) {
    fileName[i] = 0x0;
  }

  fileSize = 0;
  while (fileSize < 13312) {
    interrupt(0x21, 1, buffer + fileSize, 0, 0);
    if (buffer[fileSize] == 0xa) {
      buffer[fileSize] = '\0';
      if (mod(fileSize, 512) != 0) {
        interrupt(0x21, 8, fileName, buffer, div(fileSize, 512) + 1);
      } else {
        interrupt(0x21, 8, fileName, buffer, div(fileSize, 512));
      }
      return;
    }
    for (i = 0; buffer[fileSize + i] != 0xa; i++) {}
    fileSize += i + 1;
    buffer[fileSize - 1] = '\n';
  }
}

void killCommand(char *line) {
  interrupt(0x21, 9, ((int) line[0]) - 48, 0, 0);
}

void execforegroundCommand(char *line) {
  char fileName[6];
  int i;
  for (i = 0; i < 6 && line[i] != 0xa; i++) {
    fileName[i] = line[i];
  }
  for (; i < 6; i++) {
    fileName[i] = 0x0;
  }
  interrupt(0x21, 4, fileName, 0, 0);
}

void psCommand() {
  interrupt(0x21, 11, 0, 0, 0);
}

void waitCommand(char *line) {
  int p1, p2;
  p1 = ((int) line[0]) - 48;
  p2 = ((int) line[2]) - 48;
  interrupt(0x21, 12, p1, p2, 0);
}

int getCommandId(char *command) {
  if (isTypeCommand(command)) {
    return 1;
  } else if (isExecuteCommand(command)) {
    return 2;
  } else if (isDeleteCommand(command)) {
    return 3;
  } else if (isCopyCommand(command)) {
    return 4;
  } else if (isDirCommand(command)) {
    return 5;
  } else if (isCreateCommand(command)) {
    return 6;
  } else if (isKillCommand(command)) {
    return 7;
  } else if (isExecforegoundCommand(command)) {
    return 8;
  } else if (isPSCommand(command)) {
    return 9;
  } else if (isHelpCommand(command)) {
    return 10;
  } else if (isWaitCommand(command)) {
    return 11;
  }
  return 0;
}

int isTypeCommand(char *command) {
  if (command[0] == 't' && command[1] == 'y' && command[2] == 'p'
    && command[3] == 'e' && command[4] == ' ') {
    return 1;
  }
  return 0;
}

int isExecuteCommand(char *command) {
  if (command[0] == 'e' && command[1] == 'x' && command[2] == 'e'
    && command[3] == 'c' && command[4] == 'u' && command[5] == 't'
    && command[6] == 'e' && command[7] == ' ') {
    return 1;
  }
  return 0;
}

int isDeleteCommand(char *command) {
  if (command[0] == 'd' && command[1] == 'e' && command[2] == 'l'
    && command[3] == 'e' && command[4] == 't' && command[5] == 'e'
    && command[6] == ' ') {
    return 1;
  }
  return 0;
}

int isCopyCommand(char *command) {
  if (command[0] == 'c' && command[1] == 'o' && command[2] == 'p'
    && command[3] == 'y' && command[4] == ' ') {
    return 1;
  }
  return 0;
}

int isDirCommand(char *command) {
  if (command[0] == 'd' && command[1] == 'i' && command[2] == 'r') {
    return 1;
  }
  return 0;
}

int isCreateCommand(char *command) {
  if (command[0] == 'c' && command[1] == 'r' && command[2] == 'e'
    && command[3] == 'a' && command[4] == 't' && command[5] == 'e'
    && command[6] == ' ') {
    return 1;
  }
  return 0;
}

int isKillCommand(char *command) {
  if (command[0] == 'k' && command[1] == 'i' && command[2] == 'l'
    && command[3] == 'l' && command[4] == ' ') {
    return 1;
  }
  return 0;
}

int isPSCommand(char *command) {
  if (command[0] == 'p' && command[1] == 's') {
    return 1;
  }
  return 0;
}

int isExecforegoundCommand(char *command) {
  if (command[0] == 'e' && command[1] == 'x' && command[2] == 'e'
    && command[3] == 'c' && command[4] == 'f' && command[5] == 'o'
    && command[6] == 'r' && command[7] == 'e' && command[8] == 'g'
    && command[9] == 'r' && command[10] == 'o' && command[11] == 'u'
    && command[12] == 'n' && command[13] == 'd' && command[14] == ' ') {
    return 1;
  }
  return 0;
}

int isHelpCommand(char *command) {
  if (command[0] == 'h' && command[1] == 'e' && command[2] == 'l'
    && command[3] == 'p') {
    return 1;
  }
  return 0;
}

int isWaitCommand(char *command) {
  if (command[0] == 'w' && command[1] == 'a' && command[2] == 'i'
    && command[3] == 't' && command[4] == ' ') {
    return 1;
  }
  return 0;
}

void fillPromptName(char *buffer) {
  buffer[0] = '1';
  buffer[1] = 'F';
  buffer[2] = ':';
  buffer[3] = '>';
  buffer[4] = ' ';
  buffer[5] = '\0';
}

void fillErrorMessage(char *buffer) {
  buffer[0] = 'B';
  buffer[1] = 'a';
  buffer[2] = 'd';
  buffer[3] = ' ';
  buffer[4] = 'c';
  buffer[5] = 'o';
  buffer[6] = 'm';
  buffer[7] = 'm';
  buffer[8] = 'a';
  buffer[9] = 'n';
  buffer[10] = 'd';
  buffer[11] = '!';
  buffer[12] = '\n';
  buffer[13] = '\0';
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
