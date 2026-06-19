#include "suit_platform_stm32.h"

#include "suit_parser.h"
#include "suit_platform.h"

#include <stdio.h>

/*
 * This file replaces the platform-dependent part of:
 *   IoTSecureUpdate/SUIT-Parser/test/cli/stubs.c
 *
 * The repository implementation targets a host CLI and uses Mbed TLS, files,
 * mmap(), and print-only fetch/run stubs. This STM32 parser-lab adapter provides
 * the same symbols without pretending to be a production secure bootloader.
 */

// 원본 SUIT-Parser/test/cli/stubs.c의 플랫폼 의존 부분 대체
// 서명·digest·fetch·run을 실제 수행하지 않고 parser-lab 성공 처리
// 모든 우회 시 [PARSER-LAB BYPASS] 로그 출력

/* Device policy IDs used by the repository example manifests. */
const uint8_t vendor_id[16] = {
    0xfa, 0x6b, 0x4a, 0x53, 0xd5, 0xad, 0x5f, 0xdf,
    0xbe, 0x9d, 0xe6, 0x63, 0xe4, 0xd4, 0x1f, 0xfe
};

const uint8_t class_id[16] = {
    0x14, 0x92, 0xaf, 0x14, 0x25, 0x69, 0x5e, 0x48,
    0xbf, 0x42, 0x9b, 0x2d, 0x51, 0xf2, 0xab, 0x45
};

/* Valid dummy storage used only when a parser handler requests image memory. */
static const uint8_t parser_lab_image[SUIT_PARSER_LAB_IMAGE_SIZE] = {0};

static int parser_lab_bypass(const char *check_name)
{
#if SUIT_PARSER_LAB_BYPASS_SECURITY
    /*
     * CHECK: Intentional security bypass for manifest parser research.
     * The UART marker makes it impossible to mistake this for real validation.
     */
    printf("[PARSER-LAB BYPASS] %s -> success\r\n", check_name);
    return CBOR_ERR_NONE;
#else
    printf("[PARSER-LAB] %s has no production implementation\r\n", check_name);
    return CBOR_ERR_UNIMPLEMENTED;
#endif
}

// 서명 검증
int COSEAuthVerify(const uint8_t *msg, size_t msg_len,
				   const uint8_t *sig, size_t sig_len,
				   const uint8_t *kid, size_t kid_len,
				   int alg)
{
    (void)msg;
    (void)msg_len;
    (void)sig;
    (void)sig_len;
    (void)kid;
    (void)kid_len;
    (void)alg;

    /* CHECK: Bypasses the Mbed TLS ES256 implementation in host stubs.c. */
    return parser_lab_bypass("COSE ES256 signature verification");
}

int suit_platform_verify_digest(
    const uint8_t *data, size_t data_len,
    const uint8_t *exp, size_t exp_len,
    int alg)
{
    (void)data;
    (void)exp;
    (void)exp_len;
    (void)alg;

    printf("[trace] digest data_len(size_t)=%lu\r\n", (unsigned long)data_len);

    /* CHECK: Bypasses manifest/image SHA-256 verification only. */
    return parser_lab_bypass("manifest/image digest verification");
}

int suit_platform_get_image_ref(
    suit_reference_t *component_id,
    const uint8_t **image)
{
    (void)component_id;

    if (image == NULL) {
        return CBOR_ERR_OVERRUN;
    }

    /*
     * Replaces the host TODO that mentions mmap().
     * Returning initialized dummy memory avoids the uninitialized-pointer bug
     * in the repository CLI stub while keeping real Flash access disabled.
     */
    *image = parser_lab_image;
    printf("[PARSER-LAB MOCK] image reference -> dummy buffer\r\n");
    return CBOR_ERR_NONE;
}

int suit_platform_do_fetch(
    suit_reference_t *component_id,
    int digest_type,
    const uint8_t *digest_bytes,
    size_t digest_len,
    size_t image_size,
    const uint8_t *uri,
    size_t uri_len)
{
    if (image_size == 0 || image_size > (size_t)SUIT_MAX_IMAGE_SIZE) {
        return SUIT_ERR_IMAGE_SIZE;
    }

    (void)component_id;
    (void)digest_type;
    (void)digest_bytes;
    (void)digest_len;
    (void)uri;
    (void)uri_len;

    printf("[trace] fetch image_size(size_t)=%lu\r\n", (unsigned long)image_size);

    /*
     * CHECK: No network or Flash write occurs. Success lets parser research
     * continue to later manifest commands without starting real FOTA behavior.
     */
    return parser_lab_bypass("firmware fetch/write");
}

int suit_platform_do_run(const uint8_t *component_id)
{
    (void)component_id;

    /*
     * CHECK: No MSP/VTOR change or firmware jump occurs. Success only lets the
     * parser finish an invoke command during manifest parsing research.
     */
    return parser_lab_bypass("firmware run/jump");
}

int suit_platform_verify_image(
    suit_reference_t *component_id,
    int digest_type,
    const uint8_t *expected_digest,
    size_t image_size)
{
    (void)component_id;
    (void)digest_type;
    (void)expected_digest;
    (void)image_size;

    /* CHECK: Compatibility callback; current parser does not directly call it. */
    return parser_lab_bypass("complete image verification");
}
