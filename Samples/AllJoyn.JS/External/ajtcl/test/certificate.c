/**
 * @file
 */
/******************************************************************************
 * Copyright AllSeen Alliance. All rights reserved.
 *
 *    Permission to use, copy, modify, and/or distribute this software for any
 *    purpose with or without fee is hereby granted, provided that the above
 *    copyright notice and this permission notice appear in all copies.
 *
 *    THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 *    WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 *    MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 *    ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 *    WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 *    ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 *    OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 ******************************************************************************/
#define AJ_MODULE TEST_CERTIFICATE

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "aj_debug.h"
#include "alljoyn.h"
#include "aj_cert.h"
#include "aj_crypto.h"
#include "aj_crypto_sha2.h"

uint8_t dbgTEST_CERTIFICATE = 1;

static const char pem_x509_self[] = {
    "-----BEGIN CERTIFICATE-----"
    "MIIBszCCAVmgAwIBAgIJAILNujb37gH2MAoGCCqGSM49BAMCMFYxKTAnBgNVBAsM"
    "IDdhNDhhYTI2YmM0MzQyZjZhNjYyMDBmNzdhODlkZDAyMSkwJwYDVQQDDCA3YTQ4"
    "YWEyNmJjNDM0MmY2YTY2MjAwZjc3YTg5ZGQwMjAeFw0xNTAyMjYyMTUxMjNaFw0x"
    "NjAyMjYyMTUxMjNaMFYxKTAnBgNVBAsMIDdhNDhhYTI2YmM0MzQyZjZhNjYyMDBm"
    "NzdhODlkZDAyMSkwJwYDVQQDDCA3YTQ4YWEyNmJjNDM0MmY2YTY2MjAwZjc3YTg5"
    "ZGQwMjBZMBMGByqGSM49AgEGCCqGSM49AwEHA0IABGEkAUATvOE4uYmt/10vkTcU"
    "SA0C+YqHQ+fjzRASOHWIXBvpPiKgHcINtNFQsyX92L2tMT2Kn53zu+3S6UAwy6yj"
    "EDAOMAwGA1UdEwQFMAMBAf8wCgYIKoZIzj0EAwIDSAAwRQIgKit5yeq1uxTvdFmW"
    "LDeoxerqC1VqBrmyEvbp4oJfamsCIQDvMTmulW/Br/gY7GOP9H/4/BIEoR7UeAYS"
    "4xLyu+7OEA=="
    "-----END CERTIFICATE-----"
};

static const char pem_prv_1[] = {
    "-----BEGIN EC PRIVATE KEY-----"
    "MHcCAQEEIAqN6AtyOAPxY5k7eFNXAwzkbsGMl4uqvPrYkIj0LNZBoAoGCCqGSM49"
    "AwEHoUQDQgAEvnRd4fX9opwgXX4Em2UiCMsBbfaqhB1U5PJCDZacz9HumDEzYdrS"
    "MymSxR34lL0GJVgEECvBTvpaHP2bpTIl6g=="
    "-----END EC PRIVATE KEY-----"
};

static const char pem_x509_1[] = {
    "-----BEGIN CERTIFICATE-----"
    "MIIBtDCCAVmgAwIBAgIJAMlyFqk69v+OMAoGCCqGSM49BAMCMFYxKTAnBgNVBAsM"
    "IDdhNDhhYTI2YmM0MzQyZjZhNjYyMDBmNzdhODlkZDAyMSkwJwYDVQQDDCA3YTQ4"
    "YWEyNmJjNDM0MmY2YTY2MjAwZjc3YTg5ZGQwMjAeFw0xNTAyMjYyMTUxMjVaFw0x"
    "NjAyMjYyMTUxMjVaMFYxKTAnBgNVBAsMIDZkODVjMjkyMjYxM2IzNmUyZWVlZjUy"
    "NzgwNDJjYzU2MSkwJwYDVQQDDCA2ZDg1YzI5MjI2MTNiMzZlMmVlZWY1Mjc4MDQy"
    "Y2M1NjBZMBMGByqGSM49AgEGCCqGSM49AwEHA0IABL50XeH1/aKcIF1+BJtlIgjL"
    "AW32qoQdVOTyQg2WnM/R7pgxM2Ha0jMpksUd+JS9BiVYBBArwU76Whz9m6UyJeqj"
    "EDAOMAwGA1UdEwQFMAMBAf8wCgYIKoZIzj0EAwIDSQAwRgIhAKfmglMgl67L5ALF"
    "Z63haubkItTMACY1k4ROC2q7cnVmAiEArvAmcVInOq/U5C1y2XrvJQnAdwSl/Ogr"
    "IizUeK0oI5c="
    "-----END CERTIFICATE-----"
};

static const char pem_prv_2[] = {
    "-----BEGIN EC PRIVATE KEY-----"
    "MHcCAQEEIIHvXKVlMAUG8NOeJ9SqQg3Op5kXIBRvoHowaLtySxhToAoGCCqGSM49"
    "AwEHoUQDQgAE79HKpErGIZVLzKvc1gPoCkKQtuc1JP9N9AGXGrvQWOQOSwzg3E82"
    "4DqEWkvOFEP1GHeagPFIINl6IUvcgISwLA=="
    "-----END EC PRIVATE KEY-----"
};

static const char pem_x509_2[] = {
    "-----BEGIN CERTIFICATE-----"
    "MIIBWzCCAQCgAwIBAgIJAN1+gCpX2RyfMAoGCCqGSM49BAMCMCsxKTAnBgNVBAMM"
    "IGE2NzgyNWUwZjZlYzZmZDlhMWVlYWJkNWMyNTg5Y2Q1MB4XDTE1MDMwMjE0NDYx"
    "N1oXDTE2MDMwMTE0NDYxN1owKzEpMCcGA1UEAwwgYTY3ODI1ZTBmNmVjNmZkOWEx"
    "ZWVhYmQ1YzI1ODljZDUwWTATBgcqhkjOPQIBBggqhkjOPQMBBwNCAATv0cqkSsYh"
    "lUvMq9zWA+gKQpC25zUk/030AZcau9BY5A5LDODcTzbgOoRaS84UQ/UYd5qA8Ugg"
    "2XohS9yAhLAsow0wCzAJBgNVHRMEAjAAMAoGCCqGSM49BAMCA0kAMEYCIQCLChlN"
    "IoHhS7jbhbV96uyIthGEyJ62YvM+438VFMEHTwIhAOpxvefi7VFHQXhWpNE5KmG5"
    "zhXQwrpn6D0rMylIZ5/v"
    "-----END CERTIFICATE-----"
};

static const char pem_prv_3[] = {
    "-----BEGIN EC PRIVATE KEY-----"
    "MDECAQEEIICSqj3zTadctmGnwyC/SXLioO39pB1MlCbNEX04hjeioAoGCCqGSM49"
    "AwEH"
    "-----END EC PRIVATE KEY-----"
};

static const char pem_x509_3[] = {
    "-----BEGIN CERTIFICATE-----"
    "MIIBWjCCAQGgAwIBAgIHMTAxMDEwMTAKBggqhkjOPQQDAjArMSkwJwYDVQQDDCAw"
    "ZTE5YWZhNzlhMjliMjMwNDcyMGJkNGY2ZDVlMWIxOTAeFw0xNTAyMjYyMTU1MjVa"
    "Fw0xNjAyMjYyMTU1MjVaMCsxKTAnBgNVBAMMIDZhYWM5MjQwNDNjYjc5NmQ2ZGIy"
    "NmRlYmRkMGM5OWJkMFkwEwYHKoZIzj0CAQYIKoZIzj0DAQcDQgAEP/HbYga30Afm"
    "0fB6g7KaB5Vr5CDyEkgmlif/PTsgwM2KKCMiAfcfto0+L1N0kvyAUgff6sLtTHU3"
    "IdHzyBmKP6MQMA4wDAYDVR0TBAUwAwEB/zAKBggqhkjOPQQDAgNHADBEAiAZmNVA"
    "m/H5EtJl/O9x0P4zt/UdrqiPg+gA+wm0yRY6KgIgetWANAE2otcrsj3ARZTY/aTI"
    "0GOQizWlQm8mpKaQ3uE="
    "-----END CERTIFICATE-----"
};

/**
 * the basic constraints is marked as critical.
 */
static const char pem_x509_bcCritical[] = {
    "-----BEGIN CERTIFICATE-----"
    "MIIBVDCB/KADAgECAhC+Ci4hDqaWuEWj2eDd0zrfMAoGCCqGSM49BAMCMCQxIjAgBgNVBAMMGUFsbEpveW5UZXN0U2VsZlNpZ25lZE5hbWUwHhcNMTUwMzMxMTc0MTQwWhcNMTYwMzMwMTc0MTQwWjAkMSIwIAYDVQQDDBlBbGxKb3luVGVzdFNlbGZTaWduZWROYW1lMFkwEwYHKoZIzj0CAQYIKoZIzj0DAQcDQgAE5nmP2qHqZ6N67jdoVxSA64U+Y+rThK+oAwgR6DNezFKMSgVMA1Snn4qsc1Q+KbaYAMj7hWs6xDUIbz6XTOJBvaMQMA4wDAYDVR0TAQH/BAIwADAKBggqhkjOPQQDAgNHADBEAiBJpmVQof40vG9qjWgBTMkETUT0d1kGADBjQK162bUCygIgAtHmpfRztbtr5hgXYdjx4W3Kw0elmnuIfsvrY86ONZs="
    "-----END CERTIFICATE-----"
};

/**
 * the basic constraints has a path len field.
 */
static const char pem_x509_pathLen[] = {
    "-----BEGIN CERTIFICATE-----"
    "MIIBTDCB86ADAgECAhDNAwko47UUmUcr+HFVMJj1MAoGCCqGSM49BAMCMB4xHDAaBgNVBAMME0FsbEpveW5UZXN0Um9vdE5hbWUwHhcNMTUwMzMxMjMyODU2WhcNMTYwMzMwMjMyODU2WjAeMRwwGgYDVQQDDBNBbGxKb3luVGVzdFJvb3ROYW1lMFkwEwYHKoZIzj0CAQYIKoZIzj0DAQcDQgAEwmq2CF9Q1Lh/RfE9ejHMGb+AkgKljRgh3D2uOVCGCvxpMtH4AR+QzAPKwYOHvKewsZIBtC41N5Fb4wFbR3kaSaMTMBEwDwYDVR0TBAgwBgEB/wIBADAKBggqhkjOPQQDAgNIADBFAiAyIj1kEli20k2jRuhmSqyjHJ1rlv0oyLOXpgI5f5P0nAIhALIV4i9VG6+DiL7VgNQ1LQswZMgjEUMuPWL6UyuBDe3z"
    "-----END CERTIFICATE-----"
};

/**
 * the basic constraints has no CA field.
 */
static const char pem_x509_no_CA[] = {
    "-----BEGIN CERTIFICATE-----"
    "MIIBRTCB66ADAgECAhAIrQyeRPmaj0tCzYi1kc1LMAoGCCqGSM49BAMCMB4xHDAaBgNVBAMME0FsbEpveW5UZXN0Um9vdE5hbWUwHhcNMTUwMzMxMjMyODU2WhcNMTYwMzMwMjMyODU2WjAcMRowGAYDVQQDDBFDZXJ0U2lnbkxpYkNsaWVudDBZMBMGByqGSM49AgEGCCqGSM49AwEHA0IABDrQE+EUBFzwtXq/vlG6IYYEpVxEndizIvaysExCBML5uYovNVLfWEqFmEDGLvv3rJkZ0I0xhzSyzLD+Zo4xzU+jDTALMAkGA1UdEwQCMAAwCgYIKoZIzj0EAwIDSQAwRgIhAJ++iDjgYeje0kmJ3cdYTwen1V92Ldz4m0NInbpPX3BOAiEAvUTLYd83T4uXNh6P+JL4Phj3zxVBo2mSvwnuFSyeSOg="
    "-----END CERTIFICATE-----"
};

int AJ_Main(int ac, char** av)
{
    AJ_Status status = AJ_OK;
    X509Certificate certificate;
    DER_Element der;
    ecc_privatekey prv;
    ecc_signature sig;
    uint8_t digest[SHA256_DIGEST_LENGTH];

    status = AJ_X509DecodeCertificatePEM(&certificate, pem_x509_self);
    AJ_ASSERT(AJ_OK == status);
    der.size = certificate.der.size;
    der.data = certificate.der.data;
    status = AJ_X509DecodeCertificateDER(&certificate, &der);
    AJ_ASSERT(AJ_OK == status);
    status = AJ_X509SelfVerify(&certificate);
    AJ_Printf("Verify: %s\n", AJ_StatusText(status));
    AJ_Free(certificate.der.data);

    memset(digest, 1, sizeof (digest));

    status = AJ_X509DecodeCertificatePEM(&certificate, pem_x509_1);
    AJ_ASSERT(AJ_OK == status);
    der.size = certificate.der.size;
    der.data = certificate.der.data;
    status = AJ_X509DecodeCertificateDER(&certificate, &der);
    AJ_ASSERT(AJ_OK == status);
    status = AJ_DecodePrivateKeyPEM(&prv, pem_prv_1);
    AJ_ASSERT(AJ_OK == status);
    status = AJ_DSASignDigest(digest, &prv, &sig);
    AJ_ASSERT(AJ_OK == status);
    status = AJ_DSAVerifyDigest(digest, &sig, &certificate.tbs.publickey);
    AJ_Printf("Verify: %s\n", AJ_StatusText(status));
    AJ_Free(certificate.der.data);

    status = AJ_X509DecodeCertificatePEM(&certificate, pem_x509_2);
    AJ_ASSERT(AJ_OK == status);
    der.size = certificate.der.size;
    der.data = certificate.der.data;
    status = AJ_X509DecodeCertificateDER(&certificate, &der);
    AJ_ASSERT(AJ_OK == status);
    status = AJ_X509SelfVerify(&certificate);
    AJ_Printf("Verify: %s\n", AJ_StatusText(status));
    status = AJ_DecodePrivateKeyPEM(&prv, pem_prv_2);
    AJ_ASSERT(AJ_OK == status);
    status = AJ_DSASignDigest(digest, &prv, &sig);
    AJ_ASSERT(AJ_OK == status);
    status = AJ_DSAVerifyDigest(digest, &sig, &certificate.tbs.publickey);
    AJ_Printf("Verify: %s\n", AJ_StatusText(status));
    AJ_Free(certificate.der.data);

    status = AJ_X509DecodeCertificatePEM(&certificate, pem_x509_3);
    AJ_ASSERT(AJ_OK == status);
    der.size = certificate.der.size;
    der.data = certificate.der.data;
    status = AJ_X509DecodeCertificateDER(&certificate, &der);
    AJ_ASSERT(AJ_OK == status);
    status = AJ_X509SelfVerify(&certificate);
    AJ_Printf("Verify: %s\n", AJ_StatusText(status));
    status = AJ_DecodePrivateKeyPEM(&prv, pem_prv_3);
    AJ_ASSERT(AJ_OK == status);
    status = AJ_DSASignDigest(digest, &prv, &sig);
    AJ_ASSERT(AJ_OK == status);
    status = AJ_DSAVerifyDigest(digest, &sig, &certificate.tbs.publickey);
    AJ_Printf("Verify: %s\n", AJ_StatusText(status));
    AJ_Free(certificate.der.data);

    status = AJ_X509DecodeCertificatePEM(&certificate, pem_x509_bcCritical);
    AJ_ASSERT(AJ_OK == status);
    der.size = certificate.der.size;
    der.data = certificate.der.data;
    status = AJ_X509DecodeCertificateDER(&certificate, &der);
    AJ_ASSERT(AJ_OK == status);
    AJ_Printf("Parse cert with basicConstraints marked as critical: %s\n", AJ_StatusText(status));
    AJ_Free(certificate.der.data);

    status = AJ_X509DecodeCertificatePEM(&certificate, pem_x509_pathLen);
    AJ_ASSERT(AJ_OK == status);
    der.size = certificate.der.size;
    der.data = certificate.der.data;
    status = AJ_X509DecodeCertificateDER(&certificate, &der);
    AJ_ASSERT(AJ_OK == status);
    AJ_Printf("Parse pathLen: %s\n", AJ_StatusText(status));
    AJ_Free(certificate.der.data);

    status = AJ_X509DecodeCertificatePEM(&certificate, pem_x509_no_CA);
    AJ_ASSERT(AJ_OK == status);
    der.size = certificate.der.size;
    der.data = certificate.der.data;
    status = AJ_X509DecodeCertificateDER(&certificate, &der);
    AJ_ASSERT(AJ_OK == status);
    AJ_Printf("Parse no CA: %s\n", AJ_StatusText(status));
    AJ_Free(certificate.der.data);

    return 0;
}

#ifdef AJ_MAIN
int main(int ac, char** av)
{
    return AJ_Main(ac, av);
}
#endif
