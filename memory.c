#include "types.h"
#include "defs.h"
#include "memlayout.h"
#include "proc.h"
#include "riscv.h"

extern uint64 stack_start[];
extern uint64 stack_end[];
extern uint64 text_end[];

#define HEAP_START stack_end
#define HEAP_END (uint8 *)(KERNEL_START + MEMORY_LENGTH)
#define HEAP_SIZE  ( (uint64)HEAP_END - (uint64)HEAP_START )
#define FREE_PAGE 0x01
#define LAST_PAGE 0x02

long total_pages; // Numero total de páginas do heap
long alloc_start; // início da região alocável do heap

trap_frame_t trap_frame[4]; // uma para cada CPU

//Verifica se a página referenciada por desc
// está livre
int free_page(uint8 desc) {
    if(desc & FREE_PAGE) return 1;
    return 0;
}
// Verifica se é a última página 
// de um bloco
int last_page(uint8 desc) {
    if(desc & LAST_PAGE) return 1;
    return 0;
}

/* Marcar página como livre/ocupada
     Se bit 0 de desc = 1, página livre
     Se bit 0 de desc = 0, página ocupada
*/
void set_free_page_flag(uint8 *desc, uint8 freedom) {
    if(freedom == 1)
        *desc = *desc | FREE_PAGE;
    else
        *desc = *desc & (0xFF << 1);
}

void set_last_page_flag(uint8 *desc, uint8 last) {
    if(last) *desc = *desc | LAST_PAGE;
    else { 
        // *desc = *desc & ~LAST_PAGE;
        *desc = *desc & ( (0xFF << 2) | 1);
    }
}

/* 
Arredonda o end. para o múltiplo de PAGE_SIZE mais próximo
 page_round_up(4095) -> 4096
 page_round_up(4096) -> 4096
 page_round_up(5001) -> 8192 
*/
uint64
page_round_up(uint64 addr) {
    if(addr % PAGE_SIZE == 0) return addr; // end. já é múltiplo de PAGE_SIZE
    return addr - (addr % PAGE_SIZE) + PAGE_SIZE; // arredonda para cima
}

/* Arredonda o end. para o múltiplo de PAGE_SIZE mais próximo (para baixo)
 page_round_down(4095) -> 0
 page_round_down(4096) -> 4096
 page_round_down(5001) -> 4096 
*/
uint64
page_round_down(uint64 addr) {
    return addr - (addr % PAGE_SIZE);
}

//Cria e inicializa descritores de página
void pages_init() {
    uint8 *desc = (uint8 *)HEAP_START;
    // páginas não alocáveis (p/ os descritores)
    int reserved_pages; 
    total_pages = HEAP_SIZE / PAGE_SIZE;
    reserved_pages = total_pages / PAGE_SIZE;
    if(total_pages % PAGE_SIZE) reserved_pages++;
    total_pages -= reserved_pages; // total de páginas alocáveis
    for(int i = 0; i < total_pages; i++) {
        desc[i] = FREE_PAGE;
    }
    printf("Páginas reservadas aos descritores: %d\n", reserved_pages);
    printf("Páginas livres: %d\n", total_pages);
    alloc_start = page_round_up((uint64)HEAP_START + reserved_pages * PAGE_SIZE);
}

void memory_init() {
    printf("Código:\n");
    printf("\t Início:\t %p\n", KERNEL_START);
    printf("\t Fim:\t\t %p\n", text_end);
    printf("\t Tamanho:\t %d (bytes) \n", (uint64)text_end - KERNEL_START);
    // Seção de dados 
    printf("Dados:\n");
    //!!! Início, fim e tamanho
    //Seção de pilha
    printf("Pilha:\n");
    printf("\t Início:\t %p\n", stack_start);
    printf("\t Fim:\t\t %p\n", stack_end);
    //Área do heap
    printf("Heap:\n");
    printf("\t Início:\t %p\n", HEAP_START);
    printf("\t Fim:\t\t %p\n", HEAP_END);
    printf("Tamanho: %d (bytes) ~%d (MiB)\n", 
        HEAP_SIZE, HEAP_SIZE/1024/1024);
    
    //Inicializa descritores de página    
    pages_init();

    // Configuração do trap frame
    trap_frame[0].hartid = 0;
    trap_frame[0].trap_stack = (uint8 *)kalloc(1);
    memset(trap_frame[0].trap_stack, 0, PAGE_SIZE);
    //A pilha cresce de cima para baixo
    trap_frame[0].trap_stack += PAGE_SIZE;
    w_mscratch((uint64) &trap_frame[0]);
    printf("Início da região alocável do heap %p\n", alloc_start);
    printf("Arredondamento para cima de %d: %d\n", 
        4077, page_round_up(4077));
    printf("Arredondamento para cima de %d: %d\n", 
        5000, page_round_up(5000));
    printf("Arredondamento para baixo de %d: %d\n", 
        5000, page_round_down(5000));
    
    int *ponteiro =  (int *) kalloc(1);
    *ponteiro = 007;
    printf("Pagina 1: %p\n", ponteiro);
    int *p2 = (int *) kalloc(2);
    char *p3 = (char *) kalloc(1);
    printf("Pagina 2: %p\n", p2);
    printf("Espaço entre pagina 1 e pagina 2: %d\n", 
        (uint64)p2 - (uint64)ponteiro);
    printf("Espaço entre pagina 3 e pagina 2: %d\n", 
        (uint64)p3 - (uint64)p2);

    *p3 = 'U';
    *(p3 + 1) = 'F';
    *(p3 + 2) = 'M';
   *(p3 + 3) = 'T'; 
   *(p3 + 4) = '\0'; // *(p3 + 4) = 0;
   
   printf("p3: %p, String: %s\n", p3, p3);
  // Tarefa
  // Fazer uma função que copia uma string para um endereço de memória
  // copia_string(char *str, void * end)
  // Ex.: copia_string("UFMT", p3);
}

void*
kalloc(int pages) {
    uint8 *ptr;
    uint8 *fp_desc = 0; // descritor da página
    int count = 0;
    if(pages == 0) return 0;
    for(int i = 0; i < total_pages; i++) {
        ptr = (uint8*) HEAP_START + i;
        if(free_page(*ptr)) {
            if(count == 0) fp_desc = ptr;
            count++;
        }
        else {
            count = 0;
            fp_desc = 0;
        }
        if (count == pages) break;
    }
    // Não encontramos páginas livres
    if(count < pages) fp_desc = 0;
    if (fp_desc != 0) {
        // encontramos as páginas livres solicitadas
        for(int i = 0; i < pages; i++) {
            // set_free_page_flag(fp_desc + i, 0);
            set_free_page_flag(fp_desc + i, !FREE_PAGE);
            set_last_page_flag(fp_desc + i, !LAST_PAGE);
        }
        // última página do bloco
        set_last_page_flag(fp_desc + pages - 1, LAST_PAGE);
        //Calcula endereço de retorno
        int pos_desc = (uint64) fp_desc - (uint64) HEAP_START;
        //End. da primeira página do bloco alocado
        return (void*) (alloc_start + PAGE_SIZE * pos_desc);
    }
    return 0;
}