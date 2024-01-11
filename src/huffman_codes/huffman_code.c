#include "huffman_code.h"

#include <stdlib.h>

#include "heap.h"

static Result _huffmanCode_Init(Symbol* symbol, const size_t frequency,
                                HuffmanCode** result) {
  *result = calloc(sizeof(HuffmanCode), 1);
  if (*result == NULL) return kFailedMemoryAllocation;

  (*result)->symbol = symbol;
  (*result)->frequency = frequency;
  return kSuccess;
}

static int _codeNodeComparer(const void* x, const void* y) {
  const HuffmanCode* _x = x;
  const HuffmanCode* _y = y;

  if (_y->frequency == _x->frequency) return 0;
  if (_y->frequency > _x->frequency) {
    return 1;
  } else {
    return -1;
  }
}

static Result _combineNodes(HuffmanCode* x, HuffmanCode* y, HuffmanCode** new) {
  size_t new_size;
  if (__builtin_add_overflow(x->frequency, y->frequency, &new_size)) {
    return kArithmeticOverflow;
  }

  Result result = _huffmanCode_Init(NULL, new_size, new);
  if (result != kSuccess) return result;

  (*new)->one = x;
  (*new)->zero = y;

  return kSuccess;
}

// Add a code node to the heap for each symbol
static Result _initHeap(const SymbolFreq* freqs, Heap** heap) {
  ResultCode result_code = Heap_Create(freqs->n, _codeNodeComparer, heap);
  if (result_code != kSuccess) return result_code;

  for (size_t i = 0; i < freqs->n; i++) {
    HuffmanCode* cn;
    Result result =
        _huffmanCode_Init(&freqs->symbols[i], freqs->symbols[i].frequency, &cn);
    if (result != kSuccess) {
      Heap_Destroy((*heap), NULL);
      return result;
    }

    ResultCode hr = Heap_Insert((*heap), cn);
    if (hr != kSuccess) {
      Heap_Destroy((*heap), HuffmanCode_Destory);
      return kDependancyError;
    }
  }

  return kSuccess;
}

Result SymbolFreq_Init(const size_t n, SymbolFreq** result) {
  *result = calloc(sizeof(SymbolFreq), 1);
  if (*result == NULL) return kFailedMemoryAllocation;

  (*result)->symbols = calloc(sizeof(Symbol), n);
  if ((*result)->symbols == NULL) {
    SymbolFreq_Destroy(*result);
    return kFailedMemoryAllocation;
  }

  (*result)->n = n;

  return kSuccess;
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
  ResultCode result_code;

  if (freqs == NULL || code == NULL) return kNullParameter;

  Heap* h = NULL;
  result_code = _initHeap(freqs, &h);
  if (result_code != kSuccess) {
    Heap_Destroy(h, NULL);
    return result_code;
  }

  while (!Heap_IsEmpty(h)) {
    HuffmanCode* x = NULL;
    ResultCode result_code = Heap_Extract(h, (void**)&x);
    if (result_code != kSuccess) {
      Heap_Destroy(h, NULL);
      return result_code;
    }

    HuffmanCode* y = NULL;
    result_code = Heap_Extract(h, (void**)&y);
    // Underflow is OK here because it's possible that the previous extract
    // exhausted the queue. In this case, y = NULL is correct
    if (result_code != kSuccess && result_code != kUnderflow) {
      Heap_Destroy(h, NULL);
      return result_code;
    }

    // If y is NULL, there was only one item left on the heap and it is the
    // final code
    if (y == NULL) {
      *code = x;
      Heap_Destroy(h, HuffmanCode_Destory);
      return kSuccess;
    }

    HuffmanCode* new = NULL;
    result_code = _combineNodes(x, y, &new);
    if (result_code != kSuccess) {
      Heap_Destroy(h, HuffmanCode_Destory);
      return result_code;
    }

    result_code = Heap_Insert(h, new);
    if (result_code != kSuccess) {
      Heap_Destroy(h, HuffmanCode_Destory);
      return result_code;
    }
  }

  return kSuccess;
}
