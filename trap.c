#include "proc.h"
#include "defs.h"

void mtrap(uint64 tval, uint64 cause, 
            trap_frame_t *tf) {

    if((long)cause > 0) {
        // Exceções síncronas    
        switch(cause) {
            case 2: // instrução ilegal
                printf("<TRAP> Instrução ilegal [#CPU:%d], epc:%p\n",
                    tf->hartid, (uint64 *)tf->pc);
                // o processo volta a executar uma instrução após a instrução ilegal    
                tf->pc += 4; 
                break;
            case 9: // Chamada de sistema do supervisor (modo-S)
                printf("<TRAP> Chamada de sistema (invocada pelo próprio SO:modo-S) [#CPU:%d], epc:%p\n",
                    tf->hartid, (uint64 *)tf->pc);
                if(tf->a7 == 1) {
                    //syscall void ola();
                    printf("Olá, mundo! <minha primeira syscall...>\n");                    
                }
                if (tf->a7 == 2) {
                    //syscall void write(char *s, int len);
                }
                // o processo volta a executar uma instrução após a instrução ilegal    
                tf->pc += 4; 
                break;
            default:
                printf("<TRAP> Exceção não tratada [#CPU:%d], epc:%p\n",
                    tf->hartid, (uint64 *)tf->pc);
                //Entrar em pânico
                panic("TRAP: exceção não tratada");
                break;
        }
    }
    else {
        // Interrupções (assíncronas)
    }

}