#ifndef SUIT_PLATFORM_STM32_H
#define SUIT_PLATFORM_STM32_H

#include <stddef.h>
#include <stdint.h>

/*
 * PARSER-LAB ONLY:
 * Security-boot-related platform checks are bypassed so malformed manifests can
 * reach deeper parser handlers. This must be set to 0 before secure-boot work.
 */

// 새 STM32 parser-lab 설정 파일
// SUIT_PARSER_LAB_BYPASS_SECURITY=1로 보안 callback 우회를 명시

#ifndef SUIT_PARSER_LAB_BYPASS_SECURITY
#define SUIT_PARSER_LAB_BYPASS_SECURITY 1
#endif

/*
 * A valid address returned to parser code that requests an image reference.
 * Digest verification is bypassed in parser-lab mode, so this buffer is not
 * treated as a real firmware image.
 */
#define SUIT_PARSER_LAB_IMAGE_SIZE ((size_t)4096u)

#endif /* SUIT_PLATFORM_STM32_H */
