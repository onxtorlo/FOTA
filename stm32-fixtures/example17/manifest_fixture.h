#ifndef MANIFEST_FIXTURE_H
#define MANIFEST_FIXTURE_H

#include <stddef.h>
#include <stdint.h>

/* Raw .suit bytes embedded in read-only STM32 Flash. */
extern const uint8_t manifest_fixture[];
extern const size_t manifest_fixture_len;

#endif /* MANIFEST_FIXTURE_H */
