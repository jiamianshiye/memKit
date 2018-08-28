#include <stdio.h>

#include "list.h"
#include "memkit.h"


int main(int argc, char *argv[])
{
    printf("hello world!\n");
    struct MemKitHandle hdl;
    int block_len;
    char *ptr = NULL;

    mk_handle_init(&hdl, 1000, 1024);

    mk_print_handle_info(&hdl);

    struct MemPacket *packet = NULL;

    packet = mk_malloc(&hdl, 3890, "Chengdapeng");

    mk_print_pkt_info(packet);

    struct MemItorVec itor;

    mk_set_itor(packet, &itor);
    int i = 0;
    while(1)
    {
        ptr = mk_next_entry(&itor);
        if(ptr)
        {
            sprintf(ptr, "test str %d\n", i);
            i++;
        }
        else
        {
            break;
        }
    }

    mk_free(packet);



    return 0;
}