#include "defs.h"
#include "console.h"
#include "types.h"
#include "riscv.h"


extern void mvector(void); // declarando uma função que existe em outro lugar
extern void ola(void);

// Executa no Modo-S (supervisor)
void main() {
    //Limpa toda a tela e posiciona cursor.
    // puts(LIMPA_TELA CURSOR_CIMA_ESQUERDA);
    puts("\u26F0 ROS - Roncador Operating System \U0001F525");
    printlng(4, 2);
    puts("\n");
    int i = 10;
    int *p;
    p = &i;
    char *s = "Hello World!";
    printptr(s);
    puts("\n");
    printptr(p);
    puts("\n");
    // printf("%s\n%p\n", s, p);
    int per = perimetro(3, 3, 4, 5); // 5 parâmetros  
    puts("Perímetro do triângulo:");
    printlng(per, 10); 
    puts("\n");
    puts("Perímetro do retângulo:");
    per  = 0;
    per = perimetro(4, 8, 8, 8, 8); // 6 parâmetros
    printlng(per, 10);
    puts("\n");
    // uartputc(0x1B);
    // uartputc('['); //uartputc(0x5B); 
    // uartputc('2');
    // uartputc('B'); 
    int c; 

    // panic("Teste de pânico");
    ola(); // jal ra, ola
    // asm volatile("ecall"); // chamada de sistema
    for(;;) { // while(1)    
        c = uartgetc();
        if(c == -1) { //O usuário digitou algo?
            continue; // Volta ao início do laço
        }
        switch(c) {
            case 'I':
                /*
                    Estamos no modo-S e tentamos ler um registrador  (mstatus)
                    acessível  apenas no Modo-M
                */
                r_mstatus();
                break;
            case '\r': //0x0d (em hexa) //Enter
                uartputc('\r'); // \r  = 13 em ascii
                uartputc('\n');
                break;
            case 127: //tecla DEL em ASCII
                uartputc('\b'); //8 // backspace
                uartputc(' '); //32 //espaço
                uartputc('\b'); //8 // backspace
                break;
            default:
                uartputc(c);
                break;
        }
    }
}

// O boot.s salta para a função entry() que
// executa no Modo-M (máquina)
void entry() {
    // Depois que mudar para o modo-S (supervisor), 
    // a máquina deve saltar para a função main()
    w_mepc((uint64) main);
    // Para onde a máquina deve saltar quando uma exceção ocorrer
    w_mtvec((uint64) mvector);
    printf("Endereço do vetor de interrupções: %p\n", mvector);
    memory_init();
    // Habilitar Modo-S (supervisor)
    // mstatus.MPP = 1
    uint64 x = r_mstatus();
    x = x & ~MSTATUS_MPP_MASK; // limpa os bits de MPP
    x = x | MSTATUS_MPP_S; // seta MPP para 1
    w_mstatus(x);
    // salta para main() no modo-S
    asm volatile("mret"); 
}