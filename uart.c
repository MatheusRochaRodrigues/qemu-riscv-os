#include "types.h"
#include "memlayout.h"

#define THR 0 // Registrador para transmissão de dados
#define RBR 0 // Registrador para recepção de dados
#define IER 1 // Configura interrupções
#define FCR 2 // Configura FIFO (buffer)
#define LCR 3 // Configura modo de operação: (transmissão de 8 bits)
#define LSR 5 // pronto para transmissão/recepção de dados?

char 
read_reg(uint8 reg) {
    char c;
    volatile uint8 *base = (uint8 *) UART;
    c = *(base + reg);
    return c;
}

void write_reg(uint8 reg, uint8 val) {
    volatile uint8 *base = (uint8 *) UART;
    *(base + reg) = val;
}

void 
uartinit() {
    write_reg(IER, 1); // Habilita interrupção
    write_reg(FCR, 1); // Habilita FIFO
    write_reg(LCR, 3); // Habilita transmissão de 8 bits
}

// Imprime caracteres no terminal via UART
// uartputc('a'); //imprime a letra a
void
uartputc(int c) {
   // verificar se o registrador de transmissão está vazio
   while( (read_reg(LSR) & (1<<5)) == 0 );
   write_reg(THR, c);
}

// Lê caracteres do terminal via UART
int
uartgetc() {
    int c;
    // verifica se há novo caracter para ler
    if ((read_reg(LSR) & 1) == 0) return -1;
    c = read_reg(RBR); 
    return c; 
}
