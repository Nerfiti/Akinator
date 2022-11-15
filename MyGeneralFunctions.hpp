#ifndef MYGENERALFUNCTIONS_H
#define MYGENERALFUNCTIONS_H

#include "stdio.h"

///If the pointer value matches this value, then the memory allocated to this pointer had been freed
extern const void *const JUST_FREE_PTR;

typedef int comp_t(const void *item1, const void *item2);

//-----------------------------------------------------------
//! Sort array
//!
//! \param [in, out] arr array of the lines
//! \param [in] arr_size size of the array in bytes
//! \param [in] item_size size of the array item in bytes
//! \param [in] comp function-comparator
//-----------------------------------------------------------
void MG_qsort(void *arr, size_t arr_size, size_t item_size, comp_t comp);

//-----------------------------------------------------------
//! Swap 2 array elements
//!
//! \param [in, out] item1 first item
//! \param [in, out] item second item
//! \param [in] item_size size of the array item in bytes
//-----------------------------------------------------------
void MG_swap(void *item1, void *item2, size_t item_size);

#endif //MYGENERALFUNCTIONS_H