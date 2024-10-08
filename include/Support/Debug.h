#ifndef LANG_DEBUG_H
#define LANG_DEBUG_H

#define PANIC(...)                                                             \
    do {                                                                       \
        fprintf(stderr, "[%s:%d] ", __FILE__, __LINE__);                       \
        fprintf(stderr, __VA_ARGS__);                                          \
        fprintf(stderr, "\n");                                                 \
        exit(1);                                                               \
    } while (0)

#ifndef NDEBUG
#define DEBUG(...)                                                             \
    do {                                                                       \
        fprintf(stdout, "[%s:%d] ", __FILE__, __LINE__);                       \
        fprintf(stdout, __VA_ARGS__);                                          \
        fprintf(stdout, "\n");                                                 \
    } while (0)
#else
#define DEBUG(...)
#endif

#define UNREACHABLE() PANIC("Unreachable code reached")

#define UNIMPLEMENTED() PANIC("Unimplemented code reached")

#endif // LANG_DEBUG_H
