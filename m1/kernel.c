/* Team 1-F:
    deradaam
    derrowap
    finngw
*/

int main() {
  putInMemory(0xB000, 0x8000, 'H');
  putInMemory(0xB000, 0x8001, 0x7);

  putInMemory(0xB000, 0x8002, 'e');
  putInMemory(0xB000, 0x8003, 0x7);

  putInMemory(0xB000, 0x8004, 'l');
  putInMemory(0xB000, 0x8005, 0x7);

  putInMemory(0xB000, 0x8006, 'l');
  putInMemory(0xB000, 0x8007, 0x7);

  putInMemory(0xB000, 0x8008, 'o');
  putInMemory(0xB000, 0x8009, 0x7);

  putInMemory(0xB000, 0x800A, ' ');
  putInMemory(0xB000, 0x800B, 0x7);

  putInMemory(0xB000, 0x800C, 'W');
  putInMemory(0xB000, 0x800D, 0x7);

  putInMemory(0xB000, 0x800E, 'o');
  putInMemory(0xB000, 0x800F, 0x7);

  putInMemory(0xB000, 0x8010, 'r');
  putInMemory(0xB000, 0x8011, 0x7);

  putInMemory(0xB000, 0x8012, 'l');
  putInMemory(0xB000, 0x8013, 0x7);

  putInMemory(0xB000, 0x8014, 'd');
  putInMemory(0xB000, 0x8015, 0x7);
  while (1) {}
}
