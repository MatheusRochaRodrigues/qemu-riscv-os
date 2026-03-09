// Declarações de funções do kernel que são usadas em 
// vários lugares são definidas aqui.

void uartputc(int);
int uartgetc();
void uartinit();

// funções de impressão de alto nível
void printlng(long, int);
void printptr(void *);
void puts(char *s);
void printf(char *fmt, ...);
void panic(char *s);
//função variadic
int perimetro(int lados, ...);

//Gerenciamento de memória (kalloc, kfree, ...)
void memory_init();
void* kalloc(int);

//string
void memset(void *dst, int c, int len);