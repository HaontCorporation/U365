
#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

typedef enum irq_handler_state irq_state;
enum irq_handler_state
{
    IRQ_HANDLED,
    IRQ_UNHANDLED
};

typedef struct irq irq;
struct irq
{
    int present;
    void *handler[8];
    void *data[8];
};

int irq_register(int num, void *handler, void *data);
void irq_unregister(int num, int id);
void irq_install();
void irq_handler(int irq_num);