/* Team 1-F:
    deradaam
    derrowap
    finngw
*/

/* helper functions */
int getCommandId(char *command);
int isTypeCommand(char *command);
int isExecuteCommand(char *command);
void typeCommand(char *line);
void executeCommand(char *line);
void fillPromptName(char *buffer);
void fillErrorMessage(char *buffer);

int main() {
  char line[80], prompt[6], error[14];
  int command, segment;
  fillPromptName(prompt);
  fillErrorMessage(error);

  while (1) {
    interrupt(0x21, 0, prompt, 0, 0);
    interrupt(0x21, 1, line, 0, 0);
    command = getCommandId(line);
    if (command == 1) {
      /* type command was called */
      typeCommand(line);
    } else if (command == 2) {
      /* execute command was called */
      executeCommand(line);
    } else if (line[0] != 0xa || line[1] != 0x0) {
      /* unsupported command was called */
      interrupt(0x21, 0, error, 0, 0);
    }
  }
  return 0;
}

void typeCommand(char *line) {
  char fileName[7], buffer[13312];
  int i;
  for (i = 0; i < 6; i++) {
    fileName[i] = line[5 + i];
  }
  fileName[6] = '\0';
  interrupt(0x21, 3, fileName, buffer, 0);
  interrupt(0x21, 0, buffer, 0, 0);
}

void executeCommand(char *line) {
  char fileName[7];
  int i;
  for (i = 0; i < 6; i++) {
    fileName[i] = line[8 + i];
  }
  fileName[6] = '\0';
  interrupt(0x21, 4, fileName, 0x2000, 0);
}

int getCommandId(char *command) {
  if (isTypeCommand(command) == 1) {
    return 1;
  } else if (isExecuteCommand(command) == 1) {
    return 2;
  }
  return 0;
}

int isTypeCommand(char *command) {
  if (command[0] == 't' && command[1] == 'y' && command[2] == 'p'
    && command[3] == 'e' && command[4] == ' ' && command[11] == 0xa
    && command[12] == 0x0) {
    return 1;
  }
  return 0;
}

int isExecuteCommand(char *command) {
  if (command[0] == 'e' && command[1] == 'x' && command[2] == 'e'
    && command[3] == 'c' && command[4] == 'u' && command[5] == 't'
    && command[6] == 'e' && command[7] == ' '
    && command[14] == 0xa
    && command[15] == 0x0) {
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