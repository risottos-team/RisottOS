// init.c - Independent Userspace Shell running entirely in Ring 3

static inline int syscall(int num, unsigned int arg) {
  int ret;
  __asm__ volatile("int $0x80" : "=a"(ret) : "a"(num), "b"(arg) : "memory");
  return ret;
}

static int str_eq(const char *a, const char *b) {
  while (*a && *a == *b) { a++; b++; }
  return *a == *b;
}

static int starts_with(const char *str, const char *prefix) {
  while (*prefix && *str == *prefix) { str++; prefix++; }
  return *prefix == '\0';
}

static void print_uint(unsigned int n) {
  char tmp[11]; int len = 0;
  if (n == 0) { char c[2] = {'0', 0}; syscall(1, (unsigned int)c); return; }
  while (n > 0) { tmp[len++] = '0' + (n % 10); n /= 10; }
  while (len > 0) { char c[2] = {tmp[--len], 0}; syscall(1, (unsigned int)c); }
}

void _start(void) __attribute__((section(".text.start"), noreturn));

void _start(void) {
  const char *welcome = "\n============================================\n"
  "RisottOS v1.0 Real Userspace Shell Activated!\n"
  "Running safely inside Ring 3 boundary.\n"
  "============================================\n\n";
  syscall(1, (unsigned int)welcome);

  const char *prompt = "RisottOS:/ # ";
  char buffer[256];
  int index = 0;

  syscall(1, (unsigned int)prompt);

  while (1) {
    int key = syscall(2, 0);

    if (key != 0) {
      // Remove the previous character.
      if (key == '\b') {
        if (index > 0) {
          index--;
          buffer[index] = '\0';
          syscall(5, '\b');
        }
        continue;
      }

      // Execute the buffered command.
      if (key == '\n' || key == '\r') {
        buffer[index] = '\0';
        syscall(5, '\n');

        if (index > 0) {
          if (str_eq(buffer, "help")) {
            const char *help = "Userspace Commands:\n"
            "  help   - show this menu\n"
            "  clear  - clear screen\n"
            "  ls     - list RFS directory contents\n"
            "  df     - show free disk sectors\n"
            "  mkdir <path> - create directory on RFS\n"
            "  cat <path>   - show file content from RFS\n";
            syscall(1, (unsigned int)help);
          }
          else if (str_eq(buffer, "clear")) {
            for(int l=0; l<25; l++) { char cl[] = {'\n', '\0'}; syscall(1, (unsigned int)cl); }
          }
          else if (str_eq(buffer, "ls")) {
            syscall(6, (unsigned int)"/");
          }
          else if (str_eq(buffer, "df")) {
            unsigned int free_sectors = syscall(9, 0);
            syscall(1, (unsigned int)"Free Space: ");
            print_uint(free_sectors);
            syscall(1, (unsigned int)" sectors.\n");
          }
          else if (starts_with(buffer, "mkdir ")) {
            syscall(7, (unsigned int)(buffer + 6));
          }
          else if (starts_with(buffer, "cat ")) {
            syscall(8, (unsigned int)(buffer + 4));
            char cl[] = {'\n', '\0'}; syscall(1, (unsigned int)cl);
          }
          else {
            syscall(1, (unsigned int)"Userspace Error: Command not found.\n");
          }
        }

        index = 0;
        syscall(1, (unsigned int)prompt);
        continue;
      }

      if (index < 255) {
        buffer[index++] = (char)key;
        syscall(5, (unsigned int)(unsigned char)key);
      }
    }
  }
}
