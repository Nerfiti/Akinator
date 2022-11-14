#include "stack.h"

typedef long long CANARY_t;
typedef unsigned long long Hash;

const int      factor          =  2;
const size_t   STD_CAPACITY    =  10;
const CANARY_t CANARY          =  0xFEEEB1ED;
const elem_t   STD_POISON      = {};
const int      max_err_msg_len = 24;

struct stack
{
    CANARY_t LeftCanary   = CANARY;
    elem_t *data          = nullptr;
    size_t size           = 0;
    size_t capacity       = 0;
    size_t start_capacity = 0;
    elem_t POISON         = STD_POISON;
    Hash STACK_HASH       = 0;
    Hash DATA_HASH        = 0;
    CANARY_t RightCanary  = CANARY;
};

static unsigned long long GenHash(void *memptr, size_t size_of_memblock);
static void StackResize_UP(stack_id stk_id);
static void StackResize_DOWN(stack_id stk_id);
static elem_t *GetNewDataPtr(stack *stk, size_t new_size);
static void FillPoison(stack *stk);
static void ReHash(stack *stk);

#define REPORT(stk, err)                                        \
    if (err)                                                    \
    {                                                           \
        const char* msg = nullptr;                              \
        getErrMsg(err, &msg);                                   \
        printf("\nStack Error: %d (%s). In function %s.\n",     \
                                err, msg, __PRETTY_FUNCTION__); \
        StackDump(stk);                                         \
        ABORT_ON(abort());                                      \
        return err;                                             \
    }

int SetBirds(stack_id stk_id)
{
    stack *stk = (stack *)stk_id;
    int    err = 0;
    if ((err = StackOk(stk_id)) < WRONG_STACK_HASH && err != 0)
    {
        return err;
    }
    *((CANARY_t *) stk->data - 1            ) = CANARY;
    *( CANARY_t *)(stk->data + stk->capacity) = CANARY;

    return StackOk(stk);
}

int StackCtor(stack_id *stk_id_ptr, size_t capacity, elem_t POISON)
{
    assert(stk_id_ptr && !IsBadReadPtr(stk_id_ptr, sizeof(stack_id)));

    if (capacity < 1 || capacity >= __SIZE_MAX__ / sizeof(elem_t) - 2*sizeof(CANARY_t))
    {
        printf("Stack Error (StackCtor): Wrong capacity: %llu", capacity);
        ABORT_ON(abort());
    }
    
    stack *stk          = (stack *)calloc(sizeof(stack), 1);
    
    stk->LeftCanary     = CANARY;
    stk->data           = (elem_t *)((CANARY_t *)calloc(capacity*sizeof(elem_t) + 2*sizeof(CANARY_t), 1) + 1);
    stk->size           = 0;
    stk->capacity       = capacity;
    stk->start_capacity = capacity;
    stk->POISON         = POISON;
    stk->RightCanary    = CANARY;
    
    FillPoison(stk);
    SetBirds(stk);
    ReHash(stk);
    
    *stk_id_ptr = (stack_id)stk;
    
    int err = StackOk(stk);
    REPORT(stk, err);
    return err;
}

int StackPush(stack_id stk_id, elem_t item)
{
    stack *stk = (stack *)stk_id;
    int err = StackOk(stk);
    REPORT(stk, err);
    
    StackResize_UP(stk);

    stk->data[stk->size] = item;
    stk->size++;

    ReHash(stk);

    err = StackOk(stk);
    REPORT(stk, err);
    return err;
}

int StackPop(stack_id stk_id, elem_t *target)
{
    stack *stk = (stack *)stk_id;
    int err = StackOk(stk);

    if (stk->size == 0)
    {
        err |= NEGATIVE_SIZE;
    }
    REPORT(stk, err);
    
    stk->size--;
    *target              = stk->data[stk->size];
    stk->data[stk->size] = stk->POISON;

    StackResize_DOWN(stk);

    ReHash(stk);
    
    err = StackOk(stk);
    REPORT(stk, err);
    return err;
}

int StackDtor(stack_id *stk_id_ptr)
{
    bool bad = IsBadReadPtr(stk_id_ptr, sizeof(stack_id));
    assert(stk_id_ptr && !bad);

    stack *stk = (stack *)*stk_id_ptr;
    int err = StackOk(stk);

    REPORT(stk, err);
    free((CANARY_t *)stk->data - 1);
    stk->data = (elem_t *)JUST_FREE_PTR;

    stk->size       = -1;
    stk->capacity   = -1;
    stk->DATA_HASH  = -1;
    stk->STACK_HASH = -1;

    free(stk);
    *stk_id_ptr = (stack_id)JUST_FREE_PTR;

    return 0;
}

static void StackResize_UP(stack_id stk_id)
{
    stack *stk = (stack *)stk_id;

    if (stk->size >= stk->capacity)
    {
        size_t new_data_size = (stk->capacity*sizeof(elem_t)*factor) + 2*sizeof(CANARY_t);
        stk->data = GetNewDataPtr(stk, new_data_size);
        assert(stk->data);

        stk->capacity *= factor;
        FillPoison(stk);
        SetBirds(stk);
    }
}

static void StackResize_DOWN(stack_id stk_id)
{
    stack *stk = (stack *)stk_id;
    
    if (factor*factor*(stk->size) <= stk->capacity && stk->size != 0 &&
                             stk->capacity >= factor*factor*stk->start_capacity)
    {
        size_t new_data_size = (stk->capacity*sizeof(elem_t)/factor) + 2*sizeof(CANARY_t);

        stk->data = GetNewDataPtr(stk, new_data_size);       
        assert(stk->data); 

        stk->capacity /= factor;
        SetBirds(stk);
    }
}

int StackOk(stack_id stk_id)
{
    stack *stk = (stack *)stk_id;
    int err = 0;
    if (stk == nullptr || stk == JUST_FREE_PTR || IsBadReadPtr(stk, sizeof(stack_id)))
    {
        err |= NON_EXISTENT_STACK;
        return err;
    }
    if (stk->LeftCanary != CANARY || stk->RightCanary != CANARY)
    {
        err |= STACK_CANARY_SIGNAL;
    }
    if (stk->size < 0)
    {
        err |= NEGATIVE_SIZE;
    }
    if (stk->capacity < stk->size)
    {
        err |= CAPACITY_LESS_THAN_SIZE;
    }
    if (stk->data == nullptr || stk->data == JUST_FREE_PTR || 
        IsBadReadPtr((CANARY_t *)stk->data - 1, stk->capacity*sizeof(elem_t) + 2*sizeof(CANARY_t)))
    {
        err |= WRONG_DATA_PTR;
        return err;
    }
    if (stk->STACK_HASH != GenHash((CANARY_t *)stk + 1, sizeof(stack) - 2*sizeof(CANARY_t) - 2*sizeof(Hash)))
    {
        err |= WRONG_STACK_HASH;
    }
    if (*((CANARY_t *)stk->data - 1) != CANARY && 
        *((CANARY_t *)((char *)stk->data + stk->capacity)) != CANARY)
    {
        err |= DATA_CANARY_SIGNAL;
    }
    if (_msize((CANARY_t *)stk->data - 1) - 2*sizeof(CANARY_t) != stk->capacity*sizeof(elem_t))
    {
        err |= WRONG_CAPACITY;
    }
    if (stk->DATA_HASH != GenHash(stk->data, stk->capacity*sizeof(elem_t)))
    {
        err |= WRONG_DATA_HASH;
    }
    return err;
}

void StackDump(stack_id stk_id)
{
    stack *stk = (stack *)stk_id;
    int err = StackOk(stk);
    if (err & NON_EXISTENT_STACK)
    {
        printf("Stack Error: Wrong stack pointer: 0x%p\n\n", stk);
        return;
    }

    printf("\nStack info:\n");
    printf("Left Stack Canary: %llu\n", stk->RightCanary);
    printf("Right Stack Canary: %llu\n", stk->LeftCanary);
    printf("Stack Hash: %llu\n", stk->STACK_HASH);
    printf("Data Pointer: 0x%p\n", stk->data);
    printf("Size: %lu\n", stk->size);
    printf("Capacity: %lu\n", stk->capacity);

    if (err & WRONG_DATA_PTR)
    {
        printf("Wrong stack data pointer: 0x%p\n\n", stk->data);
        return;
    }
    printf("Data Hash: %llu\n", stk->DATA_HASH);
    printf("Data:\n");

    for (int i = 0; i < stk->capacity; ++i)
    {
        if (stk->data[i].def == stk->POISON.def)
        {
            printf("stack[%d] = POISON\n", i);
        } 
        else
        {
            printf("stack[%d] = %s %s\n", i, stk->data[i].negative ? "not" : "", stk->data[i].def);
        }
    }
    printf("\n\n");
}

static unsigned long long GenHash(void *memptr, size_t size_of_memblock)
{
    if (memptr == nullptr || IsBadReadPtr(memptr, size_of_memblock))
    {
        return 0;
    }

    unsigned long long hash = 5381;
    char *ptr = (char *)memptr;
    int i = 0;
    while (i < size_of_memblock)
    {
        hash = (~hash) * (ptr[i] + hash + 3) + ~(hash) ^ (CANARY);
        ++i;
    }
    return hash;
}

static elem_t *GetNewDataPtr(stack *stk, size_t new_size)
{
    CANARY_t *buff_pointer = (CANARY_t *)(realloc((CANARY_t *)stk->data - 1, new_size));
    if (buff_pointer == nullptr)
    {
        return nullptr;
    }
    return (elem_t *)(buff_pointer + 1);
}

static void FillPoison(stack *stk)
{
    for (int i = stk->size; i < stk->capacity; ++i)
    {
        stk->data[i] = stk->POISON;
    }
}

static void ReHash(stack *stk)
{
    stk->STACK_HASH = GenHash((CANARY_t *)stk + 1, sizeof(stack) - 2*sizeof(CANARY_t) - 2*sizeof(Hash));
    stk->DATA_HASH = GenHash(stk->data, stk->capacity*sizeof(elem_t));
}

void getErrMsg(int err, const char **msg)
{
    switch (err)
    {
    case OK:
        *msg = "Not Error";
        break;
    case NON_EXISTENT_STACK:
        *msg = "NON-EXISTENT STACK";
        break;
    case STACK_CANARY_SIGNAL:
        *msg = "STACK CANARY SIGNAL";
        break;
    case NEGATIVE_SIZE:
        *msg = "NEGATIVE SIZE";
        break;
    case CAPACITY_LESS_THAN_SIZE:
        *msg = "CAPACITY LESS THEN SIZE";
        break;
    case WRONG_DATA_PTR:
        *msg = "WRONG DATA POINTER";
        break;
    case WRONG_STACK_HASH:
        *msg = "WRONG STACK HASH";
        break;
    case DATA_CANARY_SIGNAL:
        *msg = "DATA CANARY SIGNAL";
        break;
    case WRONG_CAPACITY:
        *msg = "WRONG CAPACITY";
        break;
    case WRONG_DATA_HASH:
        *msg = "WRONG DATA HASH";
        break;    
    default:
        *msg = "UNDEFINED ERROR";
        break;
    }
}
