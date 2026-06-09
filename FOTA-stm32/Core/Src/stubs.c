/*
 * stubs.c
 *
 *  Created on: May 31, 2026
 *      Author: pje03
 */

#include "main.h"
#include "suit_parser.h"
#include "suit_platform.h"
#include <string.h>
#include <stdio.h>

#ifndef MBEDTLS_MD_SHA256
#define MBEDTLS_MD_SHA256 4  // mbedTLS 규격의 SHA256 식별자 값 강제 정의
#endif

/* -------------------------------------------------------------------------
   [상수 및 변수 선언부]
   기기 식별자 및 공개키. (현재는 변경 없이 그대로 사용)
   ------------------------------------------------------------------------- */

/* 기기 식별을 위한 ID */
const uint8_t vendor_id[16] = {
    0xfa, 0x6b, 0x4a, 0x53, 0xd5, 0xad, 0x5f, 0xdf,
    0xbe, 0x9d, 0xe6, 0x63, 0xe4, 0xd4, 0x1f, 0xfe
};

const uint8_t class_id[16] = {
    0x14, 0x92, 0xaf, 0x14, 0x25, 0x69, 0x5e, 0x48,
    0xbf, 0x42, 0x9b, 0x2d, 0x51, 0xf2, 0xab, 0x45
};

const uint8_t public_key[] = {
    0x04, 0x07, 0x30, 0xc9, 0xc4, 0xae, 0x4b, 0x76, 0x7a, 0xb6,
    0x9c, 0x4b, 0xab, 0xac, 0x00, 0x85, 0x8d, 0x07, 0x52, 0x90,
    0x2a, 0xcb, 0x52, 0x33, 0x75, 0x1b, 0x92, 0xfe, 0x38, 0xe9,
    0xdb, 0x32, 0xd9, 0xd4, 0x8b, 0xcd, 0x61, 0x7b, 0x6c, 0x45,
    0x9f, 0xc1, 0xa0, 0x89, 0xc7, 0x7f, 0xcd, 0x60, 0x6d, 0x6c,
    0x02, 0x8c, 0x0c, 0xce, 0x04, 0xc8, 0xef, 0x42, 0x5a, 0xe7,
    0x3f, 0x38, 0xa8, 0x89, 0x8d
};

size_t public_key_size = sizeof(public_key);

int dry_run = 0;

/* -------------------------------------------------------------------------
   [취약점 시뮬레이터 코어]
   실제 암호화 연산을 수행하는 대신 조작된 바이트 패턴에 따라 검증을 강제 통과시킴
   ------------------------------------------------------------------------- */

/* -------------------------------------------------------------------------
   [유틸리티 함수]
   ------------------------------------------------------------------------- */
void x_print(const uint8_t *p, size_t n) {
    for (size_t i = 0; i < n; i++) {
        printf("%02x", p[i]);
    }
}

/* -------------------------------------------------------------------------
   [1. 서명 검증 하드코딩 구간 - ES256_verify]
   * 하드코딩 이유: 파싱 취약점(오버플로우 등)을 트리거하려면 조작된 매니페스트를 주입해야 함
     만약 실제 ECDSA 검증을 수행하면 서명이 깨져서 패킷이 초반에 드랍됨
     파서의 깊숙한 로직까지 도달시키기 위해 서명 검증은 무조건 성공(0) 처리
   ------------------------------------------------------------------------- */

int ES256_verify(const uint8_t *msg, size_t msg_len,
                 const uint8_t *sig, size_t sig_len,
                 const uint8_t *pub, size_t pub_len)
{
	return 0; // 검증 성공 리턴

//    return mbedtls_ecdsa_helper(  // 원본 로직
//        msg, msg_len,
//        sig, sig_len,
//        pub, pub_len,
//        MBEDTLS_ECP_DP_SECP256R1,
//        MBEDTLS_MD_SHA256);
}

/* -------------------------------------------------------------------------
   [2. 해시 검증 하드코딩 구간 - mbedtls_md_verify_helper256]
   * 하드코딩 이유: Secure Boot 체인 취약점(TOCTOU) 테스트의 핵심 타깃이야.
     나중에 글리칭이나 메모리 조작으로 이 검증 로직을 우회하는 실험을 할 수 있도록
     뼈대는 남겨두되, 현재는 외부 암호 라이브러리 의존성을 없애기 위해
     기대값(exp)을 그대로 해시 결과(hash)로 복사해서 성공하게 만듦.
   ------------------------------------------------------------------------- */
int mbedtls_md_verify_helper256(const uint8_t *msg, size_t msg_len,
                                const uint8_t *exp, size_t exp_len,
								int mdtype)  // int type으로 단순화
//                                mbedtls_md_type_t mdtype)
{
    uint8_t hash[32];

    // 기대값 해시 길이가 SHA-256 규격(32바이트)인지 검사
    if (exp_len != 32) {
//        RETURN_ERROR(SUIT_ERROR_DIGEST_MISMATCH, NULL);  // 원본 로직
        return 1;  // SUIT_ERROR_DIGEST_MISMATCH 대체
    }

//    int ret;  // unused variable

    // 강제 통과 조작: 매니페스트에서 읽어온 기대값을 그대로 해시값인 것처럼 복사
    memcpy(hash, exp, 32);

    // TOCTOU 공격 연구 시, 아래 분기문이 평가되는 시점(Time of Check)을 노리게 됨
    if (0 == memcmp(hash, exp, sizeof(hash))) {
//        return CBOR_ERR_NONE;  // 원본
        return 0;  // CBOR_ERR_NONE 대체 (성공)
    }
    else {
//        RETURN_ERROR(SUIT_ERROR_DIGEST_MISMATCH, NULL);  // 원본
        return 1;  // 실패
    }
}

/* -------------------------------------------------------------------------
   [3. 원본 포팅 구역 (직접 구현 유지) - COSEAuthVerify & verify_digest]
   * 유지 이유: SUIT-Parser 본체 코드가 이 함수 시그니처(이름, 매개변수 구조)를
     그대로 호출해. 만약 이 구조를 바꾸면 파서의 실행 흐름 자체가 깨지므로,
     형태는 원본과 동일하게 유지하고 내부에서 하드코딩된 함수를 호출하도록 연결함.
   ------------------------------------------------------------------------- */

// 서명 검증
int COSEAuthVerify(const uint8_t *msg, size_t msg_len,
                   const uint8_t *sig, size_t sig_len,
                   const uint8_t *kid, size_t kid_len,
                   int alg)
{
    int rc;
    //TODO: Lookup public key by key-id
    switch (alg) {
        case COSE_ES256:  // SUIT 매니페스트 기본 서명 알고리즘
        	// 내부적으로 위에서 하드코딩한 무조건 성공 함수 호출
            rc = ES256_verify(msg, msg_len, sig, sig_len, public_key, public_key_size);
            break;
        default:
//            SET_ERROR(rc, CBOR_ERR_UNIMPLEMENTED, NULL);  // 원본
            rc = 1;  // 미지원 알고리즘 에러
            break;
    }
    return rc;
}

// 무결성 해시(Digest) 검증
int suit_platform_verify_digest(const uint8_t *data, size_t data_len,
								const uint8_t *exp, size_t exp_len,
								int alg)
{
	// ----- 하드코딩 -----
//	if (alg == SUIT_DIGEST_TYPE_SHA256) {
//		// 내부적으로 위에서 하드코딩한 해시 우회 함수 호출
//		return mbedtls_md_verify_helper256(data, data_len, exp, exp_len, 0);
//	}
//	return 1; // SUIT_ERROR_DIGEST_MISMATCH

    if (dry_run){  // 원본
        printf("Checking digest (alg=%d): ", alg);
    }
    switch (alg) {
        // TODO: expected digest length.
        case SUIT_DIGEST_TYPE_SHA256:
            if (dry_run) {
                printf("Matching SHA256: ");
                x_print(exp,exp_len);
                printf("\n");
                return CBOR_ERR_NONE;
            } else {
                printf("Matching SHA256: ");
                x_print(exp,exp_len);
                printf("\n");
                return mbedtls_md_verify_helper256(data, data_len, exp, exp_len, MBEDTLS_MD_SHA256);
            }
    }
    RETURN_ERROR(SUIT_ERROR_DIGEST_MISMATCH, NULL);
}

int suit_platform_get_image_ref(
    suit_reference_t *component_id,
    const uint8_t **image) {
    //TODO: open/create component_id with mmap
    return 0;
}

/* -------------------------------------------------------------------------
   [4. 하드웨어 액션 구역 (단순 로그 하드코딩) - do_fetch & do_run]
   * 하드코딩 이유: 실제로 플래시에 펌웨어를 쓰고(Fetch) MCU를 리셋(Run)하는 로직까지
     구현하면 연구 환경이 너무 복잡해짐. "정상적으로 여기까지 도달했음"을 알리는
     시리얼 프린트문으로 대체하여 공격 성공 여부를 모니터링하기 위함.
   ------------------------------------------------------------------------- */

// 펌웨어 이미지 다운로드
int suit_platform_do_fetch(suit_reference_t *component_id,
						   int digest_type,
						   const uint8_t *digest_bytes,
						   size_t digest_len,
						   size_t image_size,
						   const uint8_t* uri,
						   size_t uri_len)
{
//    printf("Fetching ");  // 원본
//    if (component_id == NULL) {
//        printf("<only component>\n");
//    } else {
//        //TODO
//    }
//    printf("  Target digest type %i\n", digest_type);
//    printf("  Target digest bytes: ");
//    x_print(digest_bytes, digest_len);
//    printf("\n");
//    printf("  Source: ");
//    s_print((char*) uri, uri_len);
//    printf("\n");

    // 나중에 TOCTOU 테스트 시, 이 페치(다운로드) 과정 중간에 메모리를 가로채는 시나리오를 구성함
	printf("[Fetch Simulation] Downloading image size: %u bytes\r\n", image_size);
    return 0;
}

// 펌웨어 실행
int suit_platform_do_run(const uint8_t *component_id) {
//    printf("booted\n");

	// 권한 상승 및 임의 코드 실행 성공 시 도달하는 최종 목적지
	printf("[Run Simulation] Booting new firmware payload!\r\n");
    //TODO
    return 0;
}
