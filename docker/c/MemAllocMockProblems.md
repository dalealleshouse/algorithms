I tried a couple different options for creating a standard way to fake malloc,
calloc and the like.

- -Wl,wrap=malloc - when compiling with -flto, it is unable to located the wrap
    functions. Removing flto breaks the clang undefined behavior sanitizers
- LD_PRELOAD - memory and address sanitizers override malloc and calloc and they
    take precedence
- #define malloc - this requires that a custom header reference in each file,
    which seems like a hack supreme
