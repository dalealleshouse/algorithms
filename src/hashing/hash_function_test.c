#include "../utils/error_reporter.h"
#include "../utils/test_helpers.h"
#include "./hash_functions.h"
#include "CUnit/Basic.h"
#include "CUnit/CUnit.h"

static void test_hash_function(hasher func) {
  char* key = "meaning of life";
  char* key2 = "create a random seed";
  hash value = func(key, strlen(key));
  CU_ASSERT_NOT_EQUAL(0, value);

  value = func(key2, strlen(key));
  CU_ASSERT_NOT_EQUAL(0, value);
}

static void hash_functions_generate_non_zero() {
  hasher funcs[] = {rotating_hash, djb_hash,   mod_djb_hash, sax_hash,
                    fnv_hash,      oat_hash,   elf_hash,     farm_hash,
                    farm_hash2,    farm_hash3, spooky_hash,  spooky_hash2,
                    spooky_hash3,  NULL};

  int tracker = 0;
  hasher func = funcs[tracker];
  while (func != NULL) {
    test_hash_function(func);
    tracker++;
    func = funcs[tracker];
  }
}

static void test_compressor(hash_compressor func) {
  const char* key = "42";
  const size_t n = 15;
  size_t sizes[] = {4,    8,    16,   32,   64,    128,   256,  512,
                    1024, 2048, 4096, 8192, 16384, 32767, 65536};

  hash test = farm_hash((void*)key, strlen(key));
  for (size_t i = 0; i < n; i++) {
    size_t val = func(test, sizes[i]);
    /* printf("result=%zu\n", val); */
    CU_ASSERT(val <= sizes[i]);
  }
}

static void div_compressor_happy_path() { test_compressor(div_compressor); }

static void mul_compressor_happy_path() {
  test_compressor(mul_compressor64);

  // example from Introduction to Algorithms p. 264
  size_t result = mul_compressor64(123456, 16384);
  CU_ASSERT_EQUAL(67, result);

  result = mul_compressor64(15, 8);
  CU_ASSERT_EQUAL(2, result);

  result = mul_compressor64(23, 8);
  CU_ASSERT_EQUAL(1, result);

  result = mul_compressor64(100, 8);
  CU_ASSERT_EQUAL(6, result);
}

int RegisterHashFunctionTests() {
  CU_TestInfo hash_function_tests[] = {
      CU_TEST_INFO(hash_functions_generate_non_zero),
      CU_TEST_INFO(div_compressor_happy_path),
      CU_TEST_INFO(mul_compressor_happy_path), CU_TEST_INFO_NULL};

  CU_SuiteInfo suites[] = {{.pName = "Hash Functions",
                            .pInitFunc = noop,
                            .pCleanupFunc = noop,
                            .pTests = hash_function_tests},
                           CU_SUITE_INFO_NULL};

  return CU_register_suites(suites);
}
