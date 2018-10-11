#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <stdlib.h>

#include <pthread.h>

#include "list.h"
#include "memkit.h"

#define     MEM_BLOCK_LEN       (10*1024)
#define     MEM_BLOCKS_NUM      (1000)


#define     RAW_DATA_LEN        (MEM_BLOCK_LEN * 90)

pthread_mutex_t     mtx_data;
pthread_cond_t      cond_data;
static int          count = 0;
struct list_head    data_list;


void *thread_consumer(void *arg)
{
    struct MemPacket *packet = NULL;
    struct list_head *pos = NULL, *next = NULL;

    while(1)
    {
        pthread_mutex_lock(&mtx_data);
        while(count <= 0)
        {
            pthread_cond_wait(&cond_data, &mtx_data);
        }
        count--;
        list_for_each_safe(pos, next, &data_list)
        {
            packet = list_entry(pos, struct MemPacket, list);
            if(packet)
            {
                list_del(&packet->list);
                break;
            }
        }
        pthread_mutex_unlock(&mtx_data);

        //do process

        mk_free(packet);
        usleep( (rand() % 10) * 1000 );
    }
}

int main(int argc, char *argv[])
{
    struct MemKitHandle hdl;
    unsigned int block_len;
    char *ptr = NULL;
    char *raw = NULL;
    unsigned int i = 0;
    int ret = 0;
    struct MemItorVec itor;
    pthread_t pid;
    struct MemPacket *packet = NULL;

    mk_handle_init(&hdl, MEM_BLOCKS_NUM, MEM_BLOCK_LEN);
    //mk_print_handle_info(&hdl);

    srand(time(NULL));
    raw = (char *)malloc(RAW_DATA_LEN);
    for(i = 0; i < RAW_DATA_LEN; i++)
    {
        raw[i] = 'a' + rand() % 25;
    }

    pthread_mutex_init(&mtx_data, NULL);
    pthread_cond_init(&cond_data, NULL);

    INIT_LIST_HEAD(&data_list);

    ret = pthread_create(&pid, NULL, thread_consumer, NULL);

    unsigned int datalen = 0;
    char *entry = NULL;
    unsigned int thislen;
    while(1)
    {
        datalen = rand() % (MEM_BLOCK_LEN * 2);
        packet = mk_malloc(&hdl, datalen, NULL);
        if(packet == NULL)
        {
            ERROR("Cannot malloc, do next circle!\n");
            sleep(1);
            continue;
        }
        mk_print_pkt_info(packet);

        mk_set_itor(packet, &itor);
        ptr = raw;
        while(datalen > 0 && 0 == mk_next_entry(&itor, &block_len))
        {
            thislen = datalen > block_len ? block_len : datalen;
            memcpy(itor.entry, ptr, thislen);
            ptr += thislen;
            datalen -= thislen;
        }

        pthread_mutex_lock(&mtx_data);
        list_add_tail(&packet->list, &data_list);
        if(count < 0)
        {
            count = 0;
        }
        count++;
        pthread_mutex_unlock(&mtx_data);
        pthread_cond_signal(&cond_data);

        datalen = 0;
        ptr = NULL;

        usleep( (rand() % 10) * 1000);
    }


///////////////////////////////////////////////////
    packet = mk_malloc(&hdl, 3890, NULL);

    mk_print_pkt_info(packet);


    //get mem block, start to write data to block
    mk_set_itor(packet, &itor);
    while(1)
    {
        if(!mk_next_entry(&itor, &block_len))
        {
            sprintf((char *)itor.entry, "test str %d", (int)time(NULL));
            *itor.poffset += strlen((char *)itor.entry); //must set offset of this block
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
    INFO("//////////////////////////////////////////////////\n");
    mk_realloc(packet, 100);
    mk_print_pkt_info(packet);
    mk_realloc(packet, 289013);
    mk_print_pkt_info(packet);
    mk_realloc(packet, 1025);
    mk_print_pkt_info(packet);

    mk_free(packet);



    return 0;
}
