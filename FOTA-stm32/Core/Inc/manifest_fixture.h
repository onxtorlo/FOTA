#ifndef MANIFEST_FIXTURE_H
#define MANIFEST_FIXTURE_H

#include <stddef.h>
#include <stdint.h>

/*
 * Read-only CBOR/SUIT input consumed by main.c.
 * Replace manifest_fixture.c with a generated signed .suit byte array when
 * beginning real parser and secure-boot tests.
 */

// main.c에 fixture 배열과 길이 공개

extern const uint8_t manifest_fixture[];
extern const size_t manifest_fixture_len;

#endif /* MANIFEST_FIXTURE_H */
