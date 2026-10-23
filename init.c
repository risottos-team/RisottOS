// init.c - the RisottOS shell, running entirely in Ring 3.
// Everything privileged goes through "int 0x80" (see syscall.c for the ABI).

// ---------------------------------------------------------------------------
// Syscall layer
// ---------------------------------------------------------------------------

static inline int syscall3(int num, unsigned int a, unsigned int b, unsigned int c) {
  int ret;
  __asm__ volatile("int $0x80"
                   : "=a"(ret)
                   : "a"(num), "b"(a), "c"(b), "d"(c)
                   : "memory");
  return ret;
}

#define PTR(p) ((unsigned int)(p))

static void print_string(const char *s)   { syscall3(1, PTR(s), 0, 0); }
static int  get_key(void)                 { return syscall3(2, 0, 0, 0); }
static void sleep_ticks(unsigned int t)   { syscall3(3, t, 0, 0); }
static void clear_screen(void)            { syscall3(4, 0, 0, 0); }
static void print_char(char c)            { syscall3(5, (unsigned char)c, 0, 0); }

static int rfs_list(const char *p)        { return syscall3(6, PTR(p), 0, 0); }
static int rfs_mkdir(const char *p)       { return syscall3(7, PTR(p), 0, 0); }
static unsigned int rfs_free_sectors(void){ return (unsigned int)syscall3(9, 0, 0, 0); }
static int rfs_deldir(const char *p)      { return syscall3(10, PTR(p), 0, 0); }
static int rfs_create(const char *p)      { return syscall3(11, PTR(p), 0, 0); }
static int rfs_delete(const char *p)      { return syscall3(12, PTR(p), 0, 0); }
static int rfs_write(const char *p, const void *d, unsigned int n) { return syscall3(13, PTR(p), PTR(d), n); }
static int rfs_size(const char *p)        { return syscall3(14, PTR(p), 0, 0); }
static int rfs_read(const char *p, void *b, unsigned int n)        { return syscall3(15, PTR(p), PTR(b), n); }
static int rfs_isdir(const char *p)       { return syscall3(16, PTR(p), 0, 0); }
static int rfs_info(void)                 { return syscall3(17, 0, 0, 0); }
static int rfs_init(void)                 { return syscall3(18, 0, 0, 0); }
static int rfs_format(void)               { return syscall3(19, 0, 0, 0); }

static void rtc_print(int which)          { syscall3(23, (unsigned int)which, 0, 0); }

static void *r_malloc(unsigned int n)     { return (void *)syscall3(25, n, 0, 0); }
static void r_free(void *p)               { syscall3(26, PTR(p), 0, 0); }
static int ata_read(unsigned int lba, void *buf) { return syscall3(20, lba, PTR(buf), 0); }
static int ata_write(unsigned int lba, const void *buf) {
  return syscall3(21, lba, PTR(buf), 0);
}
static unsigned int ata_sectors(void)     { return (unsigned int)syscall3(22, 0, 0, 0); }
static void ui_demo(void)                 { syscall3(30, 0, 0, 0); }
static void ui_colors(void)               { syscall3(31, 0, 0, 0); }
static void gfx_demo(void)                { syscall3(32, 0, 0, 0); }
static void gfx_ball(void)                { syscall3(33, 0, 0, 0); }

// Pseudo error codes for errors the shell detects itself (match syscall.c).
#define U_ERR_INVAL  1000
#define U_ERR_NOTDIR 1001
static void print_rfs_error(int code)     { syscall3(27, (unsigned int)code, 0, 0); }


// ---------------------------------------------------------------------------
// Small helpers (no libc)
// ---------------------------------------------------------------------------

typedef struct {
  const char *name;
  void (*handler)(const char *args);
} ShellCommand;

static int str_eq(const char *a, const char *b) {
  while (*a && *a == *b) { a++; b++; }
  return *a == *b;
}

static int starts_with(const char *str, const char *prefix) {
  while (*prefix && *str == *prefix) { str++; prefix++; }
  return *prefix == '\0';
}

static int is_digit(char c) { return c >= '0' && c <= '9'; }

static int simple_atoi(const char *str) {
  int res = 0;
  if (*str == '\0') return -1;
  while (*str) {
    if (!is_digit(*str)) return -1;
    res = res * 10 + (*str - '0');
    str++;
  }
  return res;
}

static void print_uint(unsigned int n) {
  char tmp[11];
  int len = 0;
  if (n == 0) { print_char('0'); return; }
  while (n > 0) { tmp[len++] = (char)('0' + (n % 10)); n /= 10; }
  while (len > 0) print_char(tmp[--len]);
}

// ---------------------------------------------------------------------------
// Shell state.  NOTE: init.bin is a flat binary, so .bss is NOT zeroed by
// anything.  Every non-initialized global is set explicitly in _start().
// ---------------------------------------------------------------------------

#define PATH_MAX_LEN 128

static char buffer[256];
static int index;
static char cwd[PATH_MAX_LEN] = "/";   // initialized data, stored in the binary

// ---------------------------------------------------------------------------
// Simple commands
// ---------------------------------------------------------------------------

static void sh_clear(const char *args) { (void)args; clear_screen(); }

static void sh_help(const char *args) {
  (void)args;
  print_string("Commands:\n"
    "  clear - clear screen\n"
    "  help  - show help page\n"
    "  echo <msg> - print text\n"
    "  rtc - show local time and other things\n"
    "  sleep - wait for the number of seconds you specified\n"
    "  mem test - verify the Ring 3 allocator\n"
    "  ata test|avail - test storage access or show capacity\n"
    "  rfs info - show filesystem info\n"
    "  rfs init - validate filesystem\n"
    "  rfs format - format filesystem (destructive)\n"
    "  cd [path] - change directory (cd alone goes to /)\n"
    "  pwd - show the current directory\n"
    "  ls [path] - list a directory (default: current)\n"
    "  mkdir <path> - create a directory\n"
    "  rmdir <path> - remove an empty directory\n"
    "  touch <path> - create an empty file\n"
    "  write <path> <text> - write text to a file\n"
    "  cat <path> - show a file\n"
    "  rm <path> - delete a file\n"
    "  df - show free disk space\n"
    "  ui demo|colors - show text-mode demos\n"
    "  gfx demo|ball - show pixel graphics demos\n");
}

static void sh_echo(const char *args) {
  print_string(args);
  print_char('\n');
}

static void sh_rtc(const char *args) {
  if (args[0] == '\0' || (args[0] == ' ' && args[1] == '\0') || str_eq(args, "help")) {
    print_string("Commands:\n  rtc get-time - show full time\n  rtc get-seconds/minutes/hours - show seconds/minutes/hours\n");
  } else if (str_eq(args, "get-time")) {
    rtc_print(0); print_char('\n');
  } else if (str_eq(args, "get-seconds")) {
    rtc_print(1); print_char('\n');
  } else if (str_eq(args, "get-minutes")) {
    rtc_print(2); print_char('\n');
  } else if (str_eq(args, "get-hours")) {
    rtc_print(3); print_char('\n');
  } else {
    print_string("rtc ");
    print_string(args);
    print_string(": Command not found.\n");
  }
}

static void sh_sleep(const char *args) {
  if (str_eq(args, "help")) {
    print_string("Command:\n  sleep time(1,3,6...) - wait for the number of seconds you specified\n");
  } else {
    int seconds = simple_atoi(args);
    if (seconds < 0) {
      print_string("Error: Invalid time format. Use numbers only.\n");
    } else {
      sleep_ticks((unsigned int)seconds * 100u);
      print_string("Done sleeping\n");
    }
  }
}

static void sh_mem(const char *args) {
  if (!str_eq(args, "test")) {
    print_string("Usage: mem test\n");
    return;
  }

  char *memory = (char *)r_malloc(100);
  if (memory == 0) {
    print_string("Malloc failed: out of memory.\n");
    return;
  }

  memory[0] = 'R'; memory[1] = 'i'; memory[2] = 's'; memory[3] = 'o';
  memory[4] = 't'; memory[5] = 't'; memory[6] = 'O'; memory[7] = 'S';
  memory[8] = '\0';
  print_string("Memory test: ");
  print_string(memory);
  print_string(" (passed)\n");
  r_free(memory);
}

static void sh_ata(const char *args) {
  if (str_eq(args, "avail")) {
    print_uint(ata_sectors());
    print_string(" sectors\n");
    return;
  }

  if (!str_eq(args, "test")) {
    print_string("Usage: ata test|avail\n");
    return;
  }

  unsigned short *out = (unsigned short *)r_malloc(512);
  unsigned short *in = (unsigned short *)r_malloc(512);
  if (out == 0 || in == 0) {
    print_string("ATA test failed: out of memory.\n");
    if (out) r_free(out);
    if (in) r_free(in);
    return;
  }

  for (unsigned int i = 0; i < 256; i++) {
    out[i] = (unsigned short)(0xA000 + i);
    in[i] = 0;
  }

  int ok = ata_write(2000, out) == 0 && ata_read(2000, in) == 0;
  if (ok) {
    for (unsigned int i = 0; i < 256; i++) {
      if (in[i] != out[i]) { ok = 0; break; }
    }
  }
  print_string(ok ? "ATA test passed.\n" : "ATA test failed.\n");
  r_free(out);
  r_free(in);
}

static void sh_rfs(const char *args) {
  while (*args == ' ') args++;

  if (str_eq(args, "info")) {
    if (rfs_info() != 0) print_string("RFS info failed. Is the filesystem formatted?\n");
  } else if (str_eq(args, "init")) {
    if (rfs_init() == 0) print_string("RFS initialized successfully.\n");
    else print_string("RFS init failed: invalid or unformatted filesystem.\n");
  } else if (str_eq(args, "format")) {
    print_string("WARNING: rfs format erases existing RFS metadata!\n");
    print_string("Run this only when you intentionally want to format the filesystem.\n");
    if (rfs_format() == 0) print_string("RFS formatted successfully.\n");
    else print_string("RFS format failed.\n");
  } else {
    print_string("RFS commands:\n"
                 "  rfs info   - show filesystem information\n"
                 "  rfs init   - validate the filesystem\n"
                 "  rfs format - create/reset filesystem metadata (destructive)\n");
  }
}

// ---------------------------------------------------------------------------
// File and directory commands
// ---------------------------------------------------------------------------

// Builds a clean absolute path in out from "in" (absolute or relative to cwd).
// Handles ".", ".." and repeated slashes. out holds PATH_MAX_LEN bytes.
static int resolve_path(const char *in, char *out) {
  unsigned int len = 0;

  if (in[0] != '/') {
    while (cwd[len] != '\0') { out[len] = cwd[len]; len++; }
    if (len == 1) len = 0;
  }

  while (*in != '\0') {
    unsigned int clen = 0;

    while (*in == '/') in++;
    if (*in == '\0') break;
    while (in[clen] != '\0' && in[clen] != '/') clen++;

    if (clen == 1 && in[0] == '.') {
      // stay
    } else if (clen == 2 && in[0] == '.' && in[1] == '.') {
      while (len > 0 && out[len - 1] != '/') len--;
      if (len > 0) len--;
    } else {
      if (len + 1 + clen > PATH_MAX_LEN - 1) return -1;
      out[len++] = '/';
      for (unsigned int i = 0; i < clen; i++) out[len++] = in[i];
    }
    in += clen;
  }

  if (len == 0) { out[0] = '/'; out[1] = '\0'; }
  else out[len] = '\0';
  return 0;
}

static void print_prompt(void) {
  print_string("RisottOS");
  if (cwd[1] != '\0') {
    print_char(':');
    print_string(cwd);
  }
  print_string("> ");
}

// First word of args -> clean absolute path. *rest = text after it.
static int take_path(const char *args, char *path, const char **rest) {
  char raw[PATH_MAX_LEN];
  unsigned int n = 0;

  while (*args == ' ') args++;
  if (*args == '\0') return -1;

  while (*args != '\0' && *args != ' ') {
    if (n >= PATH_MAX_LEN - 1) return -1;
    raw[n++] = *args++;
  }
  raw[n] = '\0';

  if (*args == ' ') args++;
  *rest = args;
  return resolve_path(raw, path);
}

static void path_cmd(const char *args, const char *usage, int (*op)(const char *)) {
  char path[PATH_MAX_LEN];
  const char *rest;

  if (take_path(args, path, &rest) != 0) { print_string(usage); return; }
  int r = op(path);
  if (r < 0) print_rfs_error(r);
}

static void sh_ls(const char *args) {
  char path[PATH_MAX_LEN];
  const char *rest;

  while (*args == ' ') args++;
  if (*args == '\0') {
    unsigned int i = 0;
    while ((path[i] = cwd[i]) != '\0') i++;
  } else if (take_path(args, path, &rest) != 0) {
    print_rfs_error(U_ERR_INVAL);
    return;
  }
  int r = rfs_list(path);
  if (r < 0) print_rfs_error(r);
  else if (r == 0) print_string("(empty)\n");
}

static void sh_mkdir(const char *args) { path_cmd(args, "Usage: mkdir <path>\n", rfs_mkdir); }
static void sh_touch(const char *args) { path_cmd(args, "Usage: touch <path>\n", rfs_create); }
static void sh_rm(const char *args)    { path_cmd(args, "Usage: rm <path>\n",    rfs_delete); }

static void sh_rmdir(const char *args) {
  char path[PATH_MAX_LEN];
  const char *rest;

  if (take_path(args, path, &rest) != 0) { print_string("Usage: rmdir <path>\n"); return; }
  if (str_eq(path, cwd)) {
    print_string("Error: cannot remove the current directory\n");
    return;
  }
  int r = rfs_deldir(path);
  if (r < 0) print_rfs_error(r);
}

static void sh_write(const char *args) {
  char path[PATH_MAX_LEN];
  const char *text;
  unsigned int len = 0;

  if (take_path(args, path, &text) != 0) { print_string("Usage: write <path> <text>\n"); return; }
  while (text[len] != '\0') len++;

  int r = rfs_write(path, text, len);
  if (r < 0) { print_rfs_error(r); return; }
  print_string("Wrote ");
  print_uint(len);
  print_string(" byte(s)\n");
}

static void sh_cat(const char *args) {
  char path[PATH_MAX_LEN];
  const char *rest;

  if (take_path(args, path, &rest) != 0) { print_string("Usage: cat <path>\n"); return; }

  int size = rfs_size(path);
  if (size < 0) { print_rfs_error(size); return; }
  if (size == 0) { print_string("(empty file)\n"); return; }

  unsigned char *data = (unsigned char *)r_malloc((unsigned int)size);
  if (data == 0) { print_string("Out of memory!\n"); return; }

  int got = rfs_read(path, data, (unsigned int)size);
  if (got < 0) {
    print_rfs_error(got);
    r_free(data);
    return;
  }

  for (int i = 0; i < got; i++) {
    unsigned char c = data[i];
    if (c == '\n' || (c >= 32 && c <= 126)) print_char((char)c);
    else print_char('.');
  }
  print_char('\n');
  r_free(data);
}

static void sh_df(const char *args) {
  (void)args;
  unsigned int sectors = rfs_free_sectors();

  print_string("Free: ");
  print_uint(sectors);
  print_string(" sectors (");
  print_uint(sectors / 2u);
  print_string(" KB)\n");
}

static void sh_cd(const char *args) {
  char path[PATH_MAX_LEN];
  const char *rest;

  while (*args == ' ') args++;
  if (*args == '\0') { cwd[0] = '/'; cwd[1] = '\0'; return; }
  if (take_path(args, path, &rest) != 0) { print_rfs_error(U_ERR_INVAL); return; }

  int r = rfs_isdir(path);
  if (r < 0) {
    print_rfs_error(r);
  } else if (r == 0) {
    print_rfs_error(U_ERR_NOTDIR);
  } else {
    unsigned int i = 0;
    while ((cwd[i] = path[i]) != '\0') i++;
  }
}

static void sh_pwd(const char *args) {
  (void)args;
  print_string(cwd);
  print_char('\n');
}

static void sh_ui(const char *args) {
  if (str_eq(args, "demo")) ui_demo();
  else if (str_eq(args, "colors")) ui_colors();
  else print_string("Usage: ui demo|colors\n");
}

static void sh_gfx(const char *args) {
  if (str_eq(args, "demo")) gfx_demo();
  else if (str_eq(args, "ball")) gfx_ball();
  else print_string("Usage: gfx demo|ball\n");
}



// ---------------------------------------------------------------------------
// Command table
// ---------------------------------------------------------------------------

static const ShellCommand cmd_table[] = {
  {"clear", sh_clear},
  {"help",  sh_help},
  {"echo",  sh_echo},
  {"rtc",   sh_rtc},
  {"sleep", sh_sleep},
  {"mem",   sh_mem},
  {"ata",   sh_ata},
  {"rfs",   sh_rfs},
  {"ls",    sh_ls},
  {"mkdir", sh_mkdir},
  {"rmdir", sh_rmdir},
  {"touch", sh_touch},
  {"write", sh_write},
  {"cat",   sh_cat},
  {"rm",    sh_rm},
  {"df",    sh_df},
  {"cd",    sh_cd},
  {"pwd",   sh_pwd},
  {"ui",    sh_ui},
  {"gfx",   sh_gfx}

};

#define CMD_COUNT (sizeof(cmd_table) / sizeof(ShellCommand))

static void run_command(void) {
  unsigned int i;

  for (i = 0; i < CMD_COUNT; i++) {
    unsigned int cmd_len = 0;
    while (cmd_table[i].name[cmd_len] != '\0') cmd_len++;

    if (str_eq(buffer, cmd_table[i].name)) {
      cmd_table[i].handler("");
      return;
    }
    if (starts_with(buffer, cmd_table[i].name) && buffer[cmd_len] == ' ') {
      cmd_table[i].handler(buffer + cmd_len + 1);
      return;
    }
  }

  print_string("Command not found: ");
  print_string(buffer);
  print_char('\n');
}

// ---------------------------------------------------------------------------
// Entry point (must be the very first thing in the flat binary)
// ---------------------------------------------------------------------------

void _start(void) __attribute__((section(".text.start"), noreturn));

void _start(void) {
  // .bss is not part of the flat binary, so initialize everything ourselves.
  index = 0;
  for (int i = 0; i < 256; i++) buffer[i] = '\0';

  print_string("\n============================================\n"
               "RisottOS v1.0 Real Userspace Shell Activated!\n"
               "Running safely inside Ring 3 boundary.\n"
               "============================================\n\n");

  print_prompt();

  while (1) {
    int key = get_key();       // blocking
    char c = (char)key;

    if (key == 0) continue;

    if (c == '\b') {
      if (index > 0) {
        index--;
        buffer[index] = '\0';
        print_char('\b');
      }
    } else if (c == '\n' || c == '\r') {
      buffer[index] = '\0';
      print_char('\n');

      if (buffer[0] != '\0') run_command();

      index = 0;
      print_prompt();
    } else if (index < 255) {
      buffer[index++] = c;
      print_char(c);
    }
  }
}