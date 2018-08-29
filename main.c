#include <stdio.h>
#include <string.h>
#include <time.h>

#include "list.h"
#include "memkit.h"


int main(int argc, char *argv[])
{
    struct MemKitHandle hdl;
    int block_len;
    char *ptr = NULL;

    mk_handle_init(&hdl, 1000, 1024);

    mk_print_handle_info(&hdl);

    struct MemPacket *packet = NULL;

    packet = mk_malloc(&hdl, 3890, "Chengdapeng");

    mk_print_pkt_info(packet);

    struct MemItorVec itor;

    //get mem block, start to write data to block
    mk_set_itor(packet, &itor);
    int i = 0;
    while(1)
    {
        if(!mk_next_entry(&itor, &block_len))
        {
            sprintf(itor.entry, "test str %d", (int)time(NULL));
            *itor.poffset += strlen(itor.entry); //must set offset of this block
            i++;
        }
        else
        {
            break;
        }
    }
    INFO("echo in over i:%d\n", i);

    mk_set_itor(packet, &itor);
    i = 0;
    while(1)
    {
        if(!mk_next_entry(&itor, &block_len))
        {
            INFO("entry:%s, %p, offset:%d\n", itor.entry, itor.entry, *itor.poffset);
            i++;
        }
        else
        {
            break;
        }        
    }
    INFO("cout out over i:%d\n", i);
    mk_free(packet);



    return 0;
}