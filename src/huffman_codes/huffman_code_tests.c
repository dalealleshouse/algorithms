#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>

#include "../data_structures/hash_table.h"
#include "../utils/malloc_test_wrapper.h"
#include "../utils/test_helpers.h"
#include "./huffman_code.h"
#include "CUnit/Basic.h"
#include "CUnit/CUnit.h"

static void _printFreqs(SymbolFreq* freq) {
  printf("\nn=%zu\n", freq->n);

  for (size_t i = 0; i < freq->n; i++) {
    printf("symbol=%s, freq=%zu\n", freq->symbols[i].symbol,
           freq->symbols[i].frequency);
  }
}

static size_t _max = 0;
static size_t _min = SIZE_MAX;
static void _printCode(HuffmanCode* code, char* path) {
  Symbol* sym = code->symbol;
  if (sym != NULL) {
    size_t length = strlen(path) - 1;
    _max = (length > _max) ? length : _max;
    _min = (length < _min) ? length : _min;

    printf("%s = %s, , freq=%zu, len=%zu\n", path, sym->symbol, code->frequency,
           strlen(path) - 1);
    return;
  }

  size_t len = strlen(path);
  char new_path[len + 2];
  strncpy(new_path, path, len + 2);

  new_path[len] = '0';
  new_path[len + 1] = '\0';
  _printCode(code->zero, new_path);

  new_path[len] = '1';
  _printCode(code->one, new_path);
}

static SymbolFreq* _readFromFile(const char* path) {
  const static int BUFFER_SIZE = 1024;

  if (access(path, R_OK) != 0) {
    printf("File does not exist or access denied\n");
    return NULL;
  }

  FILE* file = fopen(path, "r");

  // Size is the first line of the file
  char line[BUFFER_SIZE];
  fgets(line, BUFFER_SIZE, file);

  size_t n = strtoul(line, NULL, 10);
  if (n == 0) {
    printf("Failed size_t conversion\n");
    return NULL;
  }

  SymbolFreq* freqs = NULL;
  SymbolFreq_Init(n, &freqs);

  size_t freq = 0;
  size_t curr_sym = 0;
  while (fgets(line, BUFFER_SIZE, file)) {
    freq = strtol(line, NULL, 10);
    freqs->symbols[curr_sym].frequency = freq;

    sprintf(freqs->symbols[curr_sym].symbol, "%zu", curr_sym);
    curr_sym++;
  }

  fclose(file);
  return freqs;
}

static size_t _maxLengthCode(HuffmanCode* code, size_t len) {
  if (code->symbol != NULL) return len;

  size_t zero = _maxLengthCode(code->zero, len + 1);
  size_t one = _maxLengthCode(code->one, len + 1);

  return (zero > one) ? zero : one;
}

static size_t _minLengthCode(HuffmanCode* code, size_t len) {
  if (code->symbol != NULL) return len;

  size_t zero = _minLengthCode(code->zero, len + 1);
  size_t one = _minLengthCode(code->one, len + 1);

  return (zero < one) ? zero : one;
}

static void _itemHandler(const KeyValuePair* item, const size_t index,
                         void* context) {
  SymbolFreq* freq = (SymbolFreq*)context;
  freq->symbols[index].frequency = *(int*)item->value;

  // We know all the key are single characters in this case
  char* symbol = (char*)item->key;
  freq->symbols[index].symbol[0] = symbol[0];
}

static SymbolFreq* _createSymbolFreq(char* input) {
  // Because this is a test function, there is no error checking
  size_t n = strlen(input);  // Don't do this in production code, a non null
                             // terminated string can wreak havoc

  HashTable* ht = HashTable_Create(n);
  for (size_t i = 0; i < n; i++) {
    size_t* freq = HashTable_Find(ht, &input[i], 1);
    if (freq == NULL) {
      freq = malloc(sizeof(size_t));
      *freq = 1;
      HashTable_Insert(ht, &input[i], 1, freq);
    } else
      ++*freq;
  }

  SymbolFreq* freqs = NULL;
  size_t freq_n = HashTable_GetN(ht);
  SymbolFreq_Init(freq_n, &freqs);

  // Move items from hash table to SymbolFreq array
  HashTable_Enumerate(ht, _itemHandler, freqs);

  HashTable_Destroy(ht, free);
  return freqs;
}

static void HuffmanCode_Calculate_NullParamerter() {
  Result result = HuffmanCode_Calculate(NULL, NULL);
  CU_ASSERT_EQUAL(result, NullParameter);

  HuffmanCode* code;
  result = HuffmanCode_Calculate(NULL, &code);
  CU_ASSERT_EQUAL(result, NullParameter);

  SymbolFreq* freqs = _createSymbolFreq("ABCADBB CCaa");
  result = HuffmanCode_Calculate(freqs, NULL);
  CU_ASSERT_EQUAL(result, NullParameter);

  SymbolFreq_Destroy(freqs);
}

static void HuffmanCode_Calculate_FailedMalloc() {
  SymbolFreq* freqs = _createSymbolFreq("ABCD");
  HuffmanCode* code;
  Result result;

  FAILED_MALLOC_TEST({ result = HuffmanCode_Calculate(freqs, &code); });

  CU_ASSERT_EQUAL(result, FailedMemoryAllocation);

  SymbolFreq_Destroy(freqs);
}

static void HuffmanCode_Calculate_SmallCode() {
  (void)_printCode;
  (void)_printFreqs;

  /*
   * Expected Code
   *        _ $ _
   *       /     \
   *      $       $
   *     / \     / \
   *    D   $   F   C
   *       / \
   *      $   A
   *     / \
   *    B   E
   *
   * D 00   = 8
   * C 11   = 6
   * F 10   = 6
   * A 011  = 3
   * B 0100 = 2
   * E 0101 = 2
   */
  SymbolFreq* freqs = _createSymbolFreq("AAABBCCCCCCDDDDDDDDEEFFFFFF");
  HuffmanCode* code = NULL;

  Result result = HuffmanCode_Calculate(freqs, &code);
  CU_ASSERT_EQUAL(result, Success);

  CU_ASSERT_STRING_EQUAL(code->zero->zero->symbol->symbol, "D")
  CU_ASSERT_STRING_EQUAL(code->one->one->symbol->symbol, "C")
  CU_ASSERT_STRING_EQUAL(code->one->zero->symbol->symbol, "F")
  CU_ASSERT_STRING_EQUAL(code->zero->one->one->symbol->symbol, "A")

  // Because B & E are equal, the side they are on is non-deterministic
  char* zero_symbol = code->zero->one->zero->zero->symbol->symbol;
  char* one_symbol = code->zero->one->zero->one->symbol->symbol;
  CU_ASSERT_TRUE(strcmp(zero_symbol, "E") == 0 ||
                 strcmp(zero_symbol, "B") == 0);
  CU_ASSERT_TRUE(strcmp(one_symbol, "E") == 0 ||
                 strcmp(one_symbol, "B") == 0);

  HuffmanCode_Destory(code);
  SymbolFreq_Destroy(freqs);
}

static void _validateFile(const char* path, size_t max, size_t min) {
  SymbolFreq* freqs = _readFromFile(path);

  HuffmanCode* code = NULL;

  Result result = HuffmanCode_Calculate(freqs, &code);
  CU_ASSERT_EQUAL(result, Success);

  CU_ASSERT_EQUAL(_maxLengthCode(code, 0), max);
  CU_ASSERT_EQUAL(_minLengthCode(code, 0), min);

  HuffmanCode_Destory(code);
  SymbolFreq_Destroy(freqs);
}

static void HuffmanCode_Calculate_Files() {
  _validateFile("src/huffman_codes/test_data/input_random_9_40.txt", 8, 4);
  _validateFile("src/huffman_codes/test_data/input_random_18_160.txt", 12, 6);
  _validateFile("src/huffman_codes/test_data/input_random_10_40.txt", 9, 4);
  _validateFile("src/huffman_codes/test_data/input_random_41_8000.txt", 23, 12);
  _validateFile("src/huffman_codes/test_data/input_random_48_10000.txt", 26,
                12);
  _validateFile("src/huffman_codes/test_data/input_random_45_10000.txt", 24,
                12);
  _validateFile("src/huffman_codes/test_data/huffman.txt", 19, 9);
}

int RegisterHuffmanCodeTests() {
  CU_TestInfo CalculateTests[] = {
      CU_TEST_INFO(HuffmanCode_Calculate_NullParamerter),
      CU_TEST_INFO(HuffmanCode_Calculate_FailedMalloc),
      CU_TEST_INFO(HuffmanCode_Calculate_SmallCode),
      CU_TEST_INFO(HuffmanCode_Calculate_Files), CU_TEST_INFO_NULL};

  CU_SuiteInfo suites[] = {{.pName = "Generate Frequencies",
                            .pInitFunc = noop,
                            .pCleanupFunc = noop,
                            .pTests = CalculateTests},
                           CU_SUITE_INFO_NULL};

  return CU_register_suites(suites);
}
