//Mahima Goyen 633002259
#include <iostream> // For Standard I/O operations
#include <ctype.h>  // For string operations
#include <string.h> // For strcmp()
#include <math.h> // for log function
#include <cstdlib>  // For rand()
#include <vector> // For file operations


#define TOTAL_ADDRESS_BITS 64
#define KB 1024

using namespace std;
typedef struct 
{
    bool ispresent;  
    unsigned long long int tag;
    int block_age;
}cache_entries;

unsigned long int miss = 0;
unsigned long int rmiss = 0;
unsigned long int wmiss = 0;
unsigned long int total_access = 0;
unsigned long int total_read = 0;
unsigned long int total_write = 0;
unsigned long int total_hit = 0;


void check_cache_entry (vector<vector<cache_entries> > &cache, int set_id, int assoc,
        unsigned long long int addr, char mode, char r_policy, unsigned long int tag)
{
    bool found = false;
    int j = 0;

    total_access++;
    if (mode == 'r')
        total_read++;
    else 
        total_write++;

    for (int i = 0; i < assoc; i++)
    {
        if ((cache[set_id][i].ispresent == true) && (cache[set_id][i].tag == tag))//check for tag in cache block
        {
            j=i; //this is found
            found = true;
            total_hit++;
            //cout << "hit count " << total_hit<<  endl;

            break;
        }
    }
    if(found ==true)
    {
        for (int i = 0; i < assoc; i++)
        { 
              cache[set_id][i].block_age +=1;
        }
        cache[set_id][j].block_age=0;
    }

    if (found==false)
    {
        //cout << "No t found " << endl;
        miss++;
        if (mode == 'r')
            rmiss++;
        else 
            wmiss++;

        if (r_policy == 'l')
        {   
            j=0;
            int max=-1;
            for (int i = 0; i < assoc; i++)
            {
               if (cache[set_id][i].ispresent == false)
                {
                    /* Update the tag in empty block */
                    j = i;
                    break;
                }
               else if(cache[set_id][i].block_age>max)
               {
                max=cache[set_id][i].block_age;
                j=i;
               }
            }
            cache[set_id][j].ispresent = true;
            cache[set_id][j].tag = tag;

            for (int i = 0; i < assoc; i++)
            {
                cache[set_id][i].block_age +=1;
            }
            cache[set_id][j].block_age=0;
        }
        else if (r_policy == 'r')
        {
            for (int i = 0; i < assoc; i++)
            {
                if (cache[set_id][i].ispresent == false)
                {
                    /* Update the tag in empty block */
                    j = i;
                    break;
                }
                else
                {
                    /* Replace the block with random block */
                    j = rand() % assoc;
                }
            }
            cache[set_id][j].ispresent = true;
            cache[set_id][j].tag = tag;    //cache block is updated with a new tag and markes as present

        }

    }
}

int main(int argc,char *argv[])
{

    int nk = 0, assoc = 0, blocksize = 0;
    char r_policy;
    unsigned long int nb = 0, ns = 0;
    int bo = 0, si = 0, tag = 0;
    int block_id = 0;
    int set_id = 0;
    int offset = 0;
    unsigned long int tag_num = 0;

    char mode;
    nk = atoi (argv[1]);
    assoc = atoi (argv[2]);
    blocksize = atoi (argv[3]);
    r_policy = argv[4][0];

    nb = (nk * KB) / blocksize; //number of blocks
    ns = nb / assoc; //number of sets
    bo = log2(blocksize);//block offset
    si = log2(ns);//set index
    tag =  TOTAL_ADDRESS_BITS - si - bo; //A-bo-si

    unsigned long long int addr;

    vector<vector<cache_entries> > cache(ns, vector<cache_entries>(assoc));
    for (int i = 0; i < ns; i++)
        for (int j = 0; j < assoc; j++)
            {cache[i][j].ispresent = false;
            cache[i][j].block_age =0;}

   
    while(1)
    {
        mode='\0';
        cin >> mode;
        cin >>  hex >> addr;
        if(mode=='\0')//check if end of file is reached
        {
            break;
        }
        block_id = addr / blocksize; //Block number
        set_id = block_id % ns; //Set index number
        offset = addr % blocksize; //Block offset number
        tag_num=(addr >> (bo+si));//Tag number
        check_cache_entry (cache, set_id, assoc, addr, mode, r_policy, tag_num);
       }

    cout.setf(ios_base::fixed, ios_base::floatfield);   
    cout<< miss << " " << 100*(double)miss/(double)total_access <<"% " << rmiss << " " << 100*(double)rmiss/(double)total_read <<"% "<< wmiss << " " << 100*(double)wmiss/(double)total_write <<"%"<< endl;
    return 0;

}
