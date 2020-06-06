#pragma once

#include <stdio.h>

#include "../utils/result_code.h"

enum { MAX_SYMBOL_LENGTH = 10 };

typedef struct {
  char symbol[MAX_SYMBOL_LENGTH];
  size_t frequency;
} Symbol;

typedef struct {
  Symbol* symbols;
  size_t n;
} SymbolFreq;

typedef struct HuffmanCode {
  size_t frequency;
  Symbol* symbol;
  struct HuffmanCode* zero;
  struct HuffmanCode* one;
} HuffmanCode;

// Initialize/Destroy struct functions
Result SymbolFreq_Init(const size_t n, SymbolFreq** result);
void SymbolFreq_Destroy(SymbolFreq* symbol);
void HuffmanCode_Destory(void* code);

/*
 * Calculates a Huffman Code for a given set of symbol frequencies
 *
 * parameters:
 *  <freqs>:    Symbols to create the code from
 *  <code>:     Pointer will be populated with results
 *
 * returns:
 *  Result code indicates success of an error code
 */
Result HuffmanCode_Calculate(const SymbolFreq* freqs, HuffmanCode** code);
