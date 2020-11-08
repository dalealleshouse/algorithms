/*******************************************************************************
 * Copyright (C) 2020 Dale Alleshouse (AKA Hideous Humpback Freak)
 *  dale@alleshouse.net https://hideoushumpbackfreak.com/
 *
 * This file is subject to the terms and conditions defined in the 'LICENSE'
 * file, which is part of this source code package.
 ******************************************************************************/
#include <stdlib.h>
#include <unistd.h>

#include "CUnit/Basic.h"
#include "CUnit/CUnit.h"
#include "disjoint_set.h"
#include "hash_table.h"
#include "test_helpers.h"

typedef enum RelationshipType {
  kUnknown = 0,
  kKnows = 1,
  kKin = 2
} RelationshipType;

typedef char CharacterName[20];
typedef struct Relationship Relationship;
typedef struct Character Character;

typedef struct Character {
  CharacterName name;
  size_t len;
  Relationship* relationships;
} Character;

struct Relationship {
  RelationshipType type;
  Character* character;
  Relationship* next;
};

typedef struct CollectContext {
  HashTable* collector;
  DisjointSet* sets;
} CollectContext;

static Character kNullCharacter = {"NULL", 5, NULL};

static RelationshipType ParseTypeString(char* type) {
  size_t len = strlen(type);
  if (strncmp(type, "kin", len) == 0) return kKin;
  if (strncmp(type, "knows", len) == 0) return kKnows;

  fprintf(stderr, "Error parsing relationship string, %s", type);

  return kUnknown;
}

static void AddRelationship(Character* character1, Character* character2,
                            RelationshipType type) {
  Relationship* relationship = malloc(sizeof(Relationship));
  if (relationship == NULL) {
    PRINT_ERROR("AddRelationship", kFailedMemoryAllocation);
    return;
  }

  relationship->type = type;
  relationship->next = character1->relationships;
  relationship->character = character2;
  character1->relationships = relationship;
}

static Character* AddCharacter(HashTable* deduper, CharacterName name) {
  Character* character = NULL;
  size_t len = strlen(name) + 1;

  ResultCode result_code =
      HashTable_Get(deduper, name, len, (void**)&character);
  if (result_code != kSuccess && result_code != kNotFound) {
    PRINT_ERROR("AddCharacter", result_code);
    return NULL;
  }

  // Character already exists in so just return it
  if (result_code == kSuccess) return character;

  // Create a new character and add it to the hash table
  character = malloc(sizeof(Character));
  if (character == NULL) {
    PRINT_ERROR("AddCharacter", kFailedMemoryAllocation);
    return NULL;
  }

  strncpy(character->name, name, len);
  character->len = len;
  character->relationships = NULL;
  HashTable_Put(deduper, name, len, character);

  return character;
}

static void Character_Destroy(void* character) {
  if (character == NULL) return;

  Character* character_t = (Character*)character;

  Relationship* relationship = character_t->relationships;
  while (relationship != NULL) {
    Relationship* temp = relationship->next;
    free(relationship);
    relationship = temp;
  }

  free(character);
}

static void Characters_Destroy(Character** characters) {
  if (characters == NULL) return;

  Character** ch = characters;
  while (*ch != &kNullCharacter) {
    Character_Destroy(*ch);
    ch++;
  }

  free(characters);
}

static void AddCharactersToArray(const KeyValuePair* kvp, const size_t index,
                                 void* context) {
  Character** chars = (Character**)context;
  chars[index] = kvp->value;
}

static Character** GenerateCharacters(char* path) {
  static const int kBufferSize = 512;

  if (access(path, R_OK) != 0) {
    fprintf(stderr, "File does not exist or access denied\n");
    return NULL;
  }

  // Use a hash table as an easy de-duping tool
  HashTable* deduper = NULL;
  ResultCode result_code = HashTable_Create(20, &deduper);
  if (result_code != kSuccess) {
    PRINT_ERROR("GenerateCharacters", result_code);
    return NULL;
  }

  FILE* file = fopen(path, "r");
  if (file == NULL) {
    fprintf(stderr, "Error opening file\n");
    HashTable_Destroy(deduper, NULL);
    return NULL;
  }

  // Parse one line at a time
  char line[kBufferSize];
  while (fgets(line, kBufferSize, file)) {
    CharacterName character1;
    CharacterName character2;
    char relationship[20];

    int result = sscanf(line, "%[^\t]\t%s\t%[^\t\n]", character1, relationship,
                        character2);

    if (result != 3) {
      fprintf(stderr, "%d, Error parsing line %s\n", result, line);
      HashTable_Destroy(deduper, Character_Destroy);
      return NULL;
    }

    Character* char1 = AddCharacter(deduper, character1);
    Character* char2 = AddCharacter(deduper, character2);
    RelationshipType rtype = ParseTypeString(relationship);
    if (char1 == NULL || char2 == NULL || rtype == kUnknown) {
      HashTable_Destroy(deduper, Character_Destroy);
      return NULL;
    }

    AddRelationship(char1, char2, rtype);
  }

  fclose(file);

  // Move hash table items to array
  size_t character_count = HashTable_GetN(deduper);
  Character** characters = malloc(sizeof(Character*) * (character_count + 1));
  if (characters == NULL) {
    HashTable_Destroy(deduper, Character_Destroy);
    PRINT_ERROR("AddCharacter", kFailedMemoryAllocation);
    return NULL;
  }

  HashTable_Enumerate(deduper, AddCharactersToArray, characters);
  characters[character_count] = &kNullCharacter;

  HashTable_Destroy(deduper, NULL);
  return characters;
}

static ResultCode AddCharactersToDisjointSet(DisjointSet* ds,
                                             Character** characters) {
  // Create a set for each distinct character
  Character** ch = characters;
  while (*ch != &kNullCharacter) {
    SetItem* item = NULL;

    ResultCode result_code = DisjointSet_MakeSet(ds, *ch, &item);
    if (result_code != kSuccess) return result_code;

    ch++;
  }

  return kSuccess;
}

static ResultCode MergeKinRelationships(DisjointSet* ds,
                                        Character** characters) {
  Character** ch = characters;
  while (*ch != &kNullCharacter) {
    Relationship* rs = (*ch)->relationships;
    while (rs != NULL) {
      if (rs->type == kKin) {
        ResultCode result_code = DisjointSet_Union(ds, *ch, rs->character);
        if (result_code != kSuccess) return result_code;
      }

      rs = rs->next;
    }

    ch++;
  }

  return kSuccess;
}

static void CollectSets(const KeyValuePair* kvp, const size_t index,
                        void* context) {
  // This will be run once for each individual character added to the disjoint
  // set

  const size_t kMemberSize = 512;
  (void)index;
  CollectContext* cc = (CollectContext*)context;

  // Get the current character
  SetItem* si = kvp->value;
  Character* ch = (Character*)si->payload;

  // Get the set representative for the character
  SetItem* rep_set = NULL;
  ResultCode result_code = DisjointSet_FindSet(cc->sets, ch, &rep_set);
  if (result_code != kSuccess) {
    PRINT_ERROR("CollectSets", result_code);
    return;
  }

  // Get a typed Character object that is the set representative
  Character* rep_ch = (Character*)rep_set->payload;

  // If the rep already exists in the collector, concatenate the current name
  if (HashTable_KeyExists(cc->collector, rep_ch->name, rep_ch->len)) {
    char* members = NULL;
    result_code = HashTable_Get(cc->collector, rep_ch->name, rep_ch->len,
                                (void*)&members);
    if (result_code != kSuccess) {
      PRINT_ERROR("CollectSets", kFailedMemoryAllocation);
      return;
    }

    strncat(members, ", ", 3);
    strncat(members, ch->name, ch->len);

  } else {
    // If the rep does not already exists in the collector, create it
    char* members = malloc(kMemberSize);
    if (members == NULL) {
      PRINT_ERROR("CollectSets", kFailedMemoryAllocation);
      return;
    }

    strncpy(members, ch->name, ch->len);
    HashTable_Put(cc->collector, rep_ch->name, rep_ch->len, members);
  }
}

static void PrintSets(const KeyValuePair* kvp, const size_t index,
                      void* context) {
  (void)index;
  (void)context;
  char* rep = kvp->key;
  char* members = kvp->value;

  printf("Set Representative = %s, Members = %s\n", rep, members);
}

static void AnalyzeRomeoAndJulietGraph() {
  // parse the file into character and relationship data
  Character** characters = GenerateCharacters(
      "./src/data_structures/test_data/romeo_juliet_graph.txt");

  // if there was an error, GenerateCharacters will print it
  if (characters == NULL) return;

  // Create the disjoint set
  DisjointSet ds;
  ResultCode result_code = DisjointSet_Init(&ds, 20);
  if (result_code != kSuccess) {
    PRINT_ERROR("AnalyzeRomeoAndJulietGraph", result_code);
    Characters_Destroy(characters);
    return;
  }

  // Create one set per character
  result_code = AddCharactersToDisjointSet(&ds, characters);
  if (result_code != kSuccess) {
    PRINT_ERROR("AnalyzeRomeoAndJulietGraph", result_code);
    Characters_Destroy(characters);
    DisjointSet_Destory(&ds);
    return;
  }

  // Merge all characters that have a kin relationship
  result_code = MergeKinRelationships(&ds, characters);
  if (result_code != kSuccess) {
    PRINT_ERROR("AnalyzeRomeoAndJulietGraph", result_code);
    Characters_Destroy(characters);
    DisjointSet_Destory(&ds);
    return;
  }

  // Create a hash table to group results
  HashTable* collector = NULL;
  result_code = HashTable_Create(20, &collector);
  if (result_code != kSuccess) {
    PRINT_ERROR("AnalyzeRomeoAndJulietGraph", result_code);
    Characters_Destroy(characters);
    DisjointSet_Destory(&ds);
    return;
  }

  CollectContext* context = malloc(sizeof(CollectContext));
  context->collector = collector;
  context->sets = &ds;

  // Group results
  HashTable_Enumerate(ds.items, CollectSets, context);

  // Print results
  printf("total number of sets = %lu\n", ds.num_sets);
  HashTable_Enumerate(collector, PrintSets, NULL);

  // Clean up
  free(context);
  DisjointSet_Destory(&ds);
  HashTable_Destroy(collector, free);
  Characters_Destroy(characters);
}

int RegisterDisjointSetTestCase() {
  CU_TestInfo DisjointSetTests[] = {CU_TEST_INFO(AnalyzeRomeoAndJulietGraph),
                                    CU_TEST_INFO_NULL};

  CU_SuiteInfo suites[] = {{.pName = "Disjoint Set Test Case",
                            .pInitFunc = noop,
                            .pCleanupFunc = noop,
                            .pTests = DisjointSetTests},
                           CU_SUITE_INFO_NULL};

  return CU_register_suites(suites);
}
