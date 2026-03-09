#include "types.h"
#include "defs.h" 
#include <stdarg.h>
static char *digits = "0123456789abcdef";

void puts(char *s) {
    // while(*s != 0) {  
    //     putc(*s);
    //     s++;    
    // }
    while(*s) uartputc(*s++);  
}
void printlng(long val, int base) {
    long l;
    int i = 0; // índice da string s
    char s[66]; // string a ser impressa
    if (val < 0 ) l = -val;
    else l = val;

    while(l != 0 ) {
        // Obter o código ASCII correspondente ao digito e armazenar em s[i]
        s[i++] = digits[l % base];
        l = l / base;
    }
    if(base == 16) {
        s[i++] = 'x';
        s[i++] = '0';
    }
    // Se o número for negativo, adicionar o sinal de menos
    if (val < 0) s[i++] = '-'; 
    // s contém o número invertido
    while(i > 0) {
        uartputc(s[--i]);
    }
}
void printptr(void *ptr) {
    uint64 addr = (uint64) ptr;
    char s[66];
    int i = 0;
    while(addr != 0) {
        s[i++] = digits[addr % 16];
        addr = addr / 16;
    }
    s[i++] = 'x';
    s[i++] = '0';
    while(i > 0)
        uartputc(s[--i]);
}

// void printf(char *fmt, ...) {
//     va_list ap;
//     int c;
//     // char *str;
//     va_start(ap, fmt);
//     for(; (c = *fmt) != 0; fmt++) {
//         if (c == '%') {
//         // Em busca do formatador (d, l, p, %, x)
//             c = *(fmt+1);
//             if (c != 0) fmt++; // verificar se há caracteres após o %
//             switch(c) {
//                 case '%':
//                     break;
//                 case 'd':
//                     break;
//                 case 'l':
//                     break;
//                 case 'x':
//                     break;
//                 case 'p':
//                     break;
//                 case 's':
//                     break;    
//             }
//         }
//         else {
//             uartputc(c);
//         }
//     }
// }

void 
printf(char * s, ...) {
    va_list ap; // ponteiro para a lista de argumentos de printf
    int c;  // caracter lido de s;
    char *stra; //string que é um argumento
    va_start(ap, s);
    // percorre todos os elementos da string s
    for(; (c = *s) != 0; s++) {
        if  (c == '%') {
            c = *(s+1);
            if( c != 0) { //string não está mal formatada
                s++; //salta o caracter que segue ao '%'
            }
            switch(c) {
                case '%':
                    uartputc(c);
                    break;
                case 'd':
                    printlng(va_arg(ap, int), 10);
                    break;
                case 'x':
                    printlng(va_arg(ap, int), 16);
                    break;
                case 'l':
                    printlng(va_arg(ap, long), 10);
                case 'p':
                    printptr(va_arg(ap, uint64 *));
                    break;
                case 's':
                    stra = va_arg(ap, char *);
                    while(*stra != 0) {
                        uartputc(*stra);
                        stra++;
                    }
                    break;
                // default:
                //     uartputc(c);
                //     break;

            } //switch
        }
        else { // um caracter a ser impresso
            uartputc(c);
        }

    }

}

int perimetro(int lados, ...) {
//ponteiro para a lista de argumentos variáveis
    va_list ap; 
    // ap aponta para início da lista de argumentos
    va_start(ap, lados);
    int per = 0;
    for(int i = 0; i < lados; i++)
        per += va_arg(ap, int);
    return per;
}

void panic(char *s) {
    printf("\U0001F480 PANIC: %s\n", s);
    for(;;)
    ;
}