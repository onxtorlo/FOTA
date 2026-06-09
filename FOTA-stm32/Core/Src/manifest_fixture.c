#include "manifest_fixture.h"

/*
 * Minimal envelope containing an empty bstr-wrapped authentication list:
 *
 *   a1       map(1)
 *     02     SUIT_ENVELOPE_AUTH
 *     41 80  bstr([ ])
 *
 * It verifies that the parser is linked and that an unsigned/empty
 * authentication object is rejected. It is not a valid update manifest.
 * Replace this array with a signed .suit fixture before vulnerability testing.
 */

// 원본 CLI의 open(), fstat(), mmap() 기반 입력을 STM32 Flash 배열로 대체

const uint8_t manifest_fixture[] = {
    0xa1, 0x02, 0x41, 0x80
};

const size_t manifest_fixture_len = sizeof(manifest_fixture);
