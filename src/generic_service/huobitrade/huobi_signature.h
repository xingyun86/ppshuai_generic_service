#pragma once

#include <string>
#include <iostream>
#include <openssl/hmac.h>

namespace Huobi {

    class ApiSignature {
    public:

        static std::string gmtNow() {
            time_t t = time(nullptr);
            struct tm* local = gmtime(&t);
            char timeBuf[128] = { 0 };
            sprintf(timeBuf, "%04d-%02d-%02dT%02d:%02d:%02d",
                local->tm_year + 1900,
                local->tm_mon + 1,
                local->tm_mday,
                local->tm_hour,
                local->tm_min,
                local->tm_sec);
            return std::move(timeBuf);
        }

        static char dec2hexChar(short int n) {
            if (0 <= n && n <= 9) {
                return char(short('0') + n);
            }
            else if (10 <= n && n <= 15) {
                return char(short('A') + n - 10);
            }
            else {
                return char(0);
            }
        }

        static std::string escapeURL(const std::string& URL) {
            std::string result = "";
            for (unsigned int i = 0; i < URL.size(); i++) {
                char c = URL[i];
                if (
                    ('0' <= c && c <= '9') ||
                    ('a' <= c && c <= 'z') ||
                    ('A' <= c && c <= 'Z') ||
                    c == '/' || c == '.'
                    ) {
                    result += c;
                }
                else {
                    int j = (short int)c;
                    if (j < 0) {
                        j += 256;
                    }
                    int i1, i0;
                    i1 = j / 16;
                    i0 = j - i1 * 16;
                    result += '%';
                    result += dec2hexChar(i1);
                    result += dec2hexChar(i0);
                }
            }
            return std::move(result);
        }

        static std::string CreateSignature(const std::string& host, const std::string& accessKey, const std::string& secretKey,
            const std::string& address, const std::string& method, const std::string& timeBuf, const std::string& params = "") {
            if (accessKey.empty() || secretKey.empty()) {
                std::cout << "API key and secret key are required" << std::endl;
            }

            std::string cre = method + "\n" + host + "\n" + address + "\n"
                + "AccessKeyId=" + accessKey + "&SignatureMethod=HmacSHA256"
                + "&SignatureVersion=2&Timestamp=" + timeBuf;

            if (!params.empty()) {
                cre += "&" + params;
            }

            const EVP_MD* engine = EVP_sha256();
            unsigned char output[1024] = { 0 };
            uint32_t len = 1024;

#if !defined(OPENSSL_VERSION_1_1)
            //openssl 1.0.x
            HMAC_CTX ctx;
            HMAC_CTX_init(&ctx);
            HMAC_Init_ex(&ctx, secretKey.data(), secretKey.size(), engine, NULL);
            HMAC_Update(&ctx, (unsigned char*)cre.data(), cre.size());
            HMAC_Final(&ctx, output, &len);
            HMAC_CTX_cleanup(&ctx);
#else
            //openssl 1.1.x
            HMAC_CTX* ctx = HMAC_CTX_new();
            HMAC_Init_ex(ctx, secretKey.data(), secretKey.size(), engine, NULL);
            HMAC_Update(ctx, (unsigned char*)cre.data(), cre.size());
            HMAC_Final(ctx, output, &len);
            HMAC_CTX_free(ctx);
#endif
            return std::move(base64_encode(output, 32));
        }
        static std::string CreateSignature(const std::string& host, const std::string& accessKey, const std::string& secretKey,
            const std::string& address, const std::string& method, const std::string& timeBuf, const std::map<std::string, std::string>& params = {}) {
            if (accessKey.empty() || secretKey.empty()) {
                std::cout << "API key and secret key are required" << std::endl;
            }

            std::string cre = method + "\n" + host + "\n" + address + "\n"
                + "AccessKeyId=" + accessKey + "&SignatureMethod=HmacSHA256"
                + "&SignatureVersion=2&Timestamp=" + timeBuf;

            if (!params.empty()) {
                for (auto it : params)
                {
                    cre += "&" + it.first + "=" + it.second;
                }                
            }

            const EVP_MD* engine = EVP_sha256();
            unsigned char output[1024] = { 0 };
            uint32_t len = 1024;

#if !defined(OPENSSL_VERSION_1_1)
            //openssl 1.0.x
            HMAC_CTX ctx;
            HMAC_CTX_init(&ctx);
            HMAC_Init_ex(&ctx, secretKey.data(), secretKey.size(), engine, NULL);
            HMAC_Update(&ctx, (unsigned char*)cre.data(), cre.size());
            HMAC_Final(&ctx, output, &len);
            HMAC_CTX_cleanup(&ctx);
#else
            //openssl 1.1.x
            HMAC_CTX* ctx = HMAC_CTX_new();
            HMAC_Init_ex(ctx, secretKey.data(), secretKey.size(), engine, NULL);
            HMAC_Update(ctx, (unsigned char*)cre.data(), cre.size());
            HMAC_Final(ctx, output, &len);
            HMAC_CTX_free(ctx);
#endif
            return std::move(base64_encode(output, 32));
        }

        static std::string CreateV2Signature(const std::string& host, const std::string& accessKey, const std::string& secretKey,
            const std::string& address, const std::string& method, const std::string& timeBuf = escapeURL(gmtNow()), const std::string& params = "") {
            if (accessKey.empty() || secretKey.empty()) {
                std::cout << "API key and secret key are required" << std::endl;
            }

            std::string cre = method + "\n" + host + "\n" + address + "\n"
                + "accessKey=" + accessKey + "&signatureMethod=HmacSHA256"
                + "&signatureVersion=2.1&timestamp=" + timeBuf;

            if (!params.empty()) {
                cre += "&" + params;
            }

            const EVP_MD* engine = EVP_sha256();
            unsigned char output[1024] = { 0 };
            uint32_t len = 1024;

#if !defined(OPENSSL_VERSION_1_1)
            //openssl 1.0.x
            HMAC_CTX ctx;
            HMAC_CTX_init(&ctx);
            HMAC_Init_ex(&ctx, secretKey.data(), secretKey.size(), engine, NULL);
            HMAC_Update(&ctx, (unsigned char*)cre.data(), cre.size());
            HMAC_Final(&ctx, output, &len);
            HMAC_CTX_cleanup(&ctx);
#else
            //openssl 1.1.x
            HMAC_CTX* ctx = HMAC_CTX_new();
            HMAC_Init_ex(ctx, secretKey.data(), secretKey.size(), engine, NULL);
            HMAC_Update(ctx, (unsigned char*)cre.data(), cre.size());
            HMAC_Final(ctx, output, &len);
            HMAC_CTX_free(ctx);
#endif
            return std::move(base64_encode(output, 32));
        }
        static std::string CreateV2Signature(const std::string& host, const std::string& accessKey, const std::string& secretKey,
            const std::string& address, const std::string& method, const std::string& timeBuf = escapeURL(gmtNow()), const std::map<std::string, std::string>& params = {}) {
            if (accessKey.empty() || secretKey.empty()) {
                std::cout << "API key and secret key are required" << std::endl;
            }

            std::string cre = method + "\n" + host + "\n" + address + "\n"
                + "accessKey=" + accessKey + "&signatureMethod=HmacSHA256"
                + "&signatureVersion=2.1&timestamp=" + timeBuf;

            if (!params.empty()) {
                for (auto it : params)
                {
                    cre += "&" + it.first + "=" + it.second;
                }
            }

            const EVP_MD* engine = EVP_sha256();
            unsigned char output[1024] = { 0 };
            uint32_t len = 1024;

#if !defined(OPENSSL_VERSION_1_1)
            //openssl 1.0.x
            HMAC_CTX ctx;
            HMAC_CTX_init(&ctx);
            HMAC_Init_ex(&ctx, secretKey.data(), secretKey.size(), engine, NULL);
            HMAC_Update(&ctx, (unsigned char*)cre.data(), cre.size());
            HMAC_Final(&ctx, output, &len);
            HMAC_CTX_cleanup(&ctx);
#else
            //openssl 1.1.x
            HMAC_CTX* ctx = HMAC_CTX_new();
            HMAC_Init_ex(ctx, secretKey.data(), secretKey.size(), engine, NULL);
            HMAC_Update(ctx, (unsigned char*)cre.data(), cre.size());
            HMAC_Final(ctx, output, &len);
            HMAC_CTX_free(ctx);
#endif
            return std::move(base64_encode(output, 32));
        }

        static std::string buildSignaturePath(const std::string& host, const std::string& accessKey, const std::string& secretKey,
            const std::string& address, const std::string& method, const std::string& params = "") {
            time_t t = time(NULL);
            struct tm* local = gmtime(&t);
            char timeBuf[128] = { 0 };
            sprintf(timeBuf, "%04d-%02d-%02dT%02d%%3A%02d%%3A%02d",
                local->tm_year + 1900,
                local->tm_mon + 1,
                local->tm_mday,
                local->tm_hour,
                local->tm_min,
                local->tm_sec);
            std::string code = escapeURL(CreateSignature(host, accessKey, secretKey, address, method, timeBuf, params));
            std::string res = "";
            res +=
                "AccessKeyId=" + accessKey + "&SignatureMethod=HmacSHA256"
                + "&SignatureVersion=2&Timestamp=" + timeBuf;
            if (!params.empty())
            {
                res += "&" + params;
            }
            res += "&Signature=" + code;

            return std::move(res);

        }
        static std::string buildSignaturePath(const std::string& host, const std::string& accessKey, const std::string& secretKey,
            const std::string& address, const std::string& method, const std::map<std::string, std::string>& params = {}) {
            time_t t = time(NULL);
            struct tm* local = gmtime(&t);
            char timeBuf[128] = { 0 };
            sprintf(timeBuf, "%04d-%02d-%02dT%02d%%3A%02d%%3A%02d",
                local->tm_year + 1900,
                local->tm_mon + 1,
                local->tm_mday,
                local->tm_hour,
                local->tm_min,
                local->tm_sec);
            std::string code = escapeURL(CreateSignature(host, accessKey, secretKey, address, method, timeBuf, params));
            std::string res = "";
            res +=
                "AccessKeyId=" + accessKey + "&SignatureMethod=HmacSHA256"
                + "&SignatureVersion=2&Timestamp=" + timeBuf;
            if (!params.empty())
            {
                for (auto it : params)
                {
                    res += "&" + it.first + "=" + it.second;
                }
            }
            res += "&Signature=" + code;

            return std::move(res);
        }

    };
}