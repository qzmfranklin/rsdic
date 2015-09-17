#include "BitVectorIfc.h"

void BitVectorIfc::dump_debug() const
{
    printf("[%5llu] ", this->size());
    for (size_t i = 0; i < this->size(); i++) {
        if (i % 64 == 0  &&  i)
            printf("        ");
        printf("%c", this->get_bit(i) ? '1' : '0');
        if (i % 8 == 7)
            printf(" ");
        if (i % 64 == 63)
            printf("\n");
    }
    if (this->size() % 64 != 0  ||  this->size() == 0)
        printf("\n");
}
