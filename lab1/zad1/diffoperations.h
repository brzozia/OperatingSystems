
struct block_struct{
    char **block;
    int size;
};

struct array_struct{
    struct block_struct *array;
    int size;
};

struct pair_struct{
    char *fileA;
    char *fileB;
};

struct array_struct *create_array(int size);             // creates array with a given number of cells

struct pair_struct *def_sequence(int size, char **input);

void compare_pairs(int size,struct pair_struct *sequence);

int create_blocks(int fname, struct array_struct *main_array);               // creates block of operations from a given file and returns index of main array, which refers to created block

int operations_counter(struct array_struct *main_array,int index);       // returns number of operations in the given block index

void remove_block(struct array_struct *main_array,int index);             // removes from the block array, block of a given indeks

void remove_operation(struct array_struct *main_array,int block_index, int operation_index);     // removes from the block array cell with a given index, operation with asecond given index
