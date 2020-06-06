#include "./huffman_code.h"

#include <stdlib.h>

#include "../data_structures/hash_table.h"
#include "../data_structures/heap.h"
#include "../utils/overflow_checker.h"

static Result _huffmanCode_Init(Symbol* symbol, const size_t frequency,
                                HuffmanCode** result) {
  *result = calloc(sizeof(HuffmanCode), 1);
  if (*result == NULL) return FailedMemoryAllocation;

  (*result)->symbol = symbol;
  (*result)->frequency = frequency;
  return Success;
}

static int _codeNodeComparer(const void* x, const void* y) {
  const HuffmanCode* _x = x;
  const HuffmanCode* _y = y;

  if (_y->frequency == _x->frequency) return 0;
  if (_y->frequency > _x->frequency)
    return 1;
  else
    return -1;
}

static Result _combineNodes(HuffmanCode* x, HuffmanCode* y, HuffmanCode** new) {
  if (is_add_overflow_size_t(x->frequency, y->frequency))
    return ArithmeticOverflow;

  Result result = _huffmanCode_Init(NULL, x->frequency + y->frequency, new);
  if (result != Success) return result;

  (*new)->one = x;
  (*new)->zero = y;

  return Success;
}

// Add a code node to the heap for each symbol
static Result _initHeap(const SymbolFreq* freqs, Heap** heap) {
  (*heap) = Heap_Create(freqs->n, _codeNodeComparer);
  if ((*heap) == NULL) return FailedMemoryAllocation;

  for (size_t i = 0; i < freqs->n; i++) {
    HuffmanCode* cn;
    Result result =
        _huffmanCode_Init(&freqs->symbols[i], freqs->symbols[i].frequency, &cn);
    if (result != Success) {
      Heap_Destroy((*heap), NULL);
      return result;
    }

    HeapResult hr = Heap_Insert((*heap), cn);
    if (hr != HeapSuccess) {
      Heap_Destroy((*heap), HuffmanCode_Destory);
      return DependancyError;
    }
  }

  return Success;
}

Result SymbolFreq_Init(const size_t n, SymbolFreq** result) {
  *result = calloc(sizeof(SymbolFreq), 1);
  if (*result == NULL) return FailedMemoryAllocation;

  (*result)->symbols = calloc(sizeof(Symbol), n);
  if ((*result)->symbols == NULL) {
    SymbolFreq_Destroy(*result);
    return FailedMemoryAllocation;
  }

  (*result)->n = n;

  return Success;
}

void SymbolFreq_Destroy(SymbolFreq* freqs) {
  if (freqs == NULL) return;

  free(freqs->symbols);
  free(freqs);
}

void HuffmanCode_Destory(void* codenode) {
  if (codenode == NULL) return;

  HuffmanCode* _codenode = codenode;
  HuffmanCode_Destory(_codenode->zero);
  HuffmanCode_Destory(_codenode->one);

  free(codenode);
}

Result HuffmanCode_Calculate(const SymbolFreq* freqs, HuffmanCode** code) {
  Result result;

  if (freqs == NULL || code == NULL) return NullParameter;

  Heap* h = NULL;
  result = _initHeap(freqs, &h);
  if (result != Success) {
    Heap_Destroy(h, NULL);
    return result;
  }

  while (!Heap_IsEmpty(h)) {
    HuffmanCode* x = Heap_Extract(h);
    HuffmanCode* y = Heap_Extract(h);

    // If y is NULL, there was only one item left on the heap and it is the
    // final code
    if (y == NULL) {
      *code = x;
      Heap_Destroy(h, HuffmanCode_Destory);
      return Success;
    }

    HuffmanCode* new = NULL;
    result = _combineNodes(x, y, &new);
    if (result != Success) {
      Heap_Destroy(h, HuffmanCode_Destory);
      return result;
    }

    HeapResult hr = Heap_Insert(h, new);
    if (hr != HeapSuccess) {
      Heap_Destroy(h, HuffmanCode_Destory);
      return DependancyError;
    }
  }

  return Success;
}
