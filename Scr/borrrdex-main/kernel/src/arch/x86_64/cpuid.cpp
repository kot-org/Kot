#include "cpuid.h"
#include "string.h"

constexpr size_t CPUID_VENDOR_SIZE = sizeof(uint32_t) * 3 + 1;
constexpr size_t CPUID_BRAND_SIZE = sizeof(uint32_t) * 12 + 1;

inline void reg_to_str(char* buf, uint64_t reg) {
    buf[0] = (char)(reg & 0xff);
    buf[1] = (char)((reg & 0xff00) >> 8);
    buf[2] = (char)((reg & 0xff0000) >> 16);
    buf[3] = (char)((reg & 0xff000000) >> 24);
}

const char* cpuid_vendor_name(char* buf, size_t size) {
    if(size < CPUID_VENDOR_SIZE) {
        return NULL;
    }

    uint64_t a = 0, b, c, d;
    _cpuid(&a, &b, &c, &d);
    reg_to_str(buf, b);
    reg_to_str(buf + sizeof(uint32_t), d);
    reg_to_str(buf + sizeof(uint32_t) * 2, c);
    buf[CPUID_VENDOR_SIZE - 1] = 0;

    return buf;
}

#define BIT_IS_SET(val, bit) ((val) & (1 << (bit)))

char sVendor[CPUID_VENDOR_SIZE];
char sBrand[CPUID_VENDOR_SIZE];
uint32_t sF1ECX, sF1EDX, sF7EBX, sF7ECX, sF81ECX, sF81EDX;
bool sIsIntel, sIsAMD;

bool CPUIDFeatures::SSE3(void) { return BIT_IS_SET(sF1ECX, 0); }
bool CPUIDFeatures::PCLMULQDQ(void) { return BIT_IS_SET(sF1ECX, 1); }
bool CPUIDFeatures::MONITOR(void) { return BIT_IS_SET(sF1ECX, 3); }
bool CPUIDFeatures::SSSE3(void) { return BIT_IS_SET(sF1ECX, 9); }
bool CPUIDFeatures::FMA(void) { return BIT_IS_SET(sF1ECX, 12); }
bool CPUIDFeatures::CMPXCHG16B(void) { return BIT_IS_SET(sF1ECX, 13); }
bool CPUIDFeatures::SSE41(void) { return BIT_IS_SET(sF1ECX, 19); }
bool CPUIDFeatures::SSE42(void) { return BIT_IS_SET(sF1ECX, 20); }
bool CPUIDFeatures::MOVBE(void) { return BIT_IS_SET(sF1ECX, 22); }
bool CPUIDFeatures::POPCNT(void) { return BIT_IS_SET(sF1ECX, 23); }
bool CPUIDFeatures::AES(void) { return BIT_IS_SET(sF1ECX, 25); }
bool CPUIDFeatures::XSAVE(void) { return BIT_IS_SET(sF1ECX, 26); }
bool CPUIDFeatures::OSXSAVE(void) { return BIT_IS_SET(sF1ECX, 27); }
bool CPUIDFeatures::AVX(void) { return BIT_IS_SET(sF1ECX, 28); }
bool CPUIDFeatures::F16C(void) { return BIT_IS_SET(sF1ECX, 29); }
bool CPUIDFeatures::RDRAND(void) { return BIT_IS_SET(sF1ECX, 30); }

bool CPUIDFeatures::MSR(void) { return BIT_IS_SET(sF1EDX, 5); }
bool CPUIDFeatures::CX8(void) { return BIT_IS_SET(sF1EDX, 8); }
bool CPUIDFeatures::SEP(void) { return BIT_IS_SET(sF1EDX, 11); }
bool CPUIDFeatures::CMOV(void) { return BIT_IS_SET(sF1EDX, 15); }
bool CPUIDFeatures::CLFSH(void) { return BIT_IS_SET(sF1EDX, 19); }
bool CPUIDFeatures::MMX(void) { return BIT_IS_SET(sF1EDX, 23); }
bool CPUIDFeatures::FXSR(void) { return BIT_IS_SET(sF1EDX, 24); }
bool CPUIDFeatures::SSE(void) { return BIT_IS_SET(sF1EDX, 25); }
bool CPUIDFeatures::SSE2(void) { return BIT_IS_SET(sF1EDX, 26); }

bool CPUIDFeatures::FSGSBASE(void) { return BIT_IS_SET(sF7EBX, 0); }
bool CPUIDFeatures::BMI1(void) { return BIT_IS_SET(sF7EBX, 3); }
bool CPUIDFeatures::HLE(void) { return sIsIntel && BIT_IS_SET(sF7EBX, 4); }
bool CPUIDFeatures::AVX2(void) { return BIT_IS_SET(sF7EBX, 5); }
bool CPUIDFeatures::BMI2(void) { return BIT_IS_SET(sF7EBX, 8); }
bool CPUIDFeatures::ERMS(void) { return BIT_IS_SET(sF7EBX, 9); }
bool CPUIDFeatures::INVPCID(void) { return BIT_IS_SET(sF7EBX, 10); }
bool CPUIDFeatures::RTM(void) { return sIsIntel && BIT_IS_SET(sF7EBX, 11); }
bool CPUIDFeatures::AVX512F(void) { return BIT_IS_SET(sF7EBX, 16); }
bool CPUIDFeatures::RDSEED(void) { return BIT_IS_SET(sF7EBX, 18); }
bool CPUIDFeatures::ADX(void) { return BIT_IS_SET(sF7EBX, 19); }
bool CPUIDFeatures::AVX512PF(void) { return BIT_IS_SET(sF7EBX, 26); }
bool CPUIDFeatures::AVX512ER(void) { return BIT_IS_SET(sF7EBX, 27); }
bool CPUIDFeatures::AVX512CD(void) { return BIT_IS_SET(sF7EBX, 28); }
bool CPUIDFeatures::SHA(void) { return BIT_IS_SET(sF7EBX, 29); }

bool CPUIDFeatures::PREFETCHWT1(void) { return BIT_IS_SET(sF7ECX, 0); }

bool CPUIDFeatures::LAHF(void) { return BIT_IS_SET(sF81ECX, 0); }
bool CPUIDFeatures::LZCNT(void) { return sIsIntel && BIT_IS_SET(sF81ECX, 5); }
bool CPUIDFeatures::ABM(void) { return sIsAMD && BIT_IS_SET(sF81ECX, 5); }
bool CPUIDFeatures::SSE4a(void) { return sIsAMD && BIT_IS_SET(sF81ECX, 6); }
bool CPUIDFeatures::XOP(void) { return sIsAMD && BIT_IS_SET(sF81ECX, 11); }
bool CPUIDFeatures::TBM(void) { return sIsAMD && BIT_IS_SET(sF81ECX, 21); }

bool CPUIDFeatures::SYSCALL(void) { return sIsIntel && BIT_IS_SET(sF81ECX, 11); }
bool CPUIDFeatures::MMXEXT(void) { return sIsAMD && BIT_IS_SET(sF81ECX, 22); }
bool CPUIDFeatures::RDTSCP(void) { return sIsIntel && BIT_IS_SET(sF81ECX, 27); }
bool CPUIDFeatures::_3DNOWEXT(void) { return sIsAMD && BIT_IS_SET(sF81ECX, 30); }
bool CPUIDFeatures::_3DNOW(void) { return sIsAMD && BIT_IS_SET(sF81ECX, 31); }

const char* CPUIDFeatures::vendor() const {
    return sVendor;
}

const char* CPUIDFeatures::brand() const {
    return sBrand;
}

CPUIDFeatures::CPUIDFeatures() {
    if(sBrand[0]) {
        return;
    }

    uint64_t a = 0, b, c, d;
    _cpuid(&a, &b, &c, &d);
    reg_to_str(sVendor, b);
    reg_to_str(sVendor + sizeof(uint32_t), d);
    reg_to_str(sVendor + sizeof(uint32_t) * 2, c);
    sIsIntel = memcmp("GenuineIntel", sVendor, 12) == 0;
    if(!sIsIntel) {
        sIsAMD = memcmp("AuthenticAMD", sVendor, 12) == 0;
    }

    uint64_t max = a;
    if(max >= 1) {
        a = 1;
        _cpuid(&a, &b, &c, &d);
        sF1ECX = (uint32_t)c;
        sF1EDX = (uint32_t)d;
    }

    if(max >= 7) {
        a = 7;
        _cpuid(&a, &b, &c, &d);
        sF7EBX = b;
        sF7ECX = c;
    }

    a = 0x80000000;
    _cpuid(&a, &b, &c, &d);
    max = a;
    if(a >= 0x80000001) {
        a = 0x80000001;
        _cpuid(&a, &b, &c, &d);
        sF81ECX = c;
        sF81EDX = d;
    }

    if(max >= 0x80000004) {
        for(uint32_t i = 0; i < 3; i++) {
            a = 0x80000002 + i;
            _cpuid(&a, &b, &c, &d);
            reg_to_str(sBrand + i * 16, a);
            reg_to_str(sBrand + i * 16 + sizeof(uint32_t), b);
            reg_to_str(sBrand + i * 16 + sizeof(uint32_t) * 2, c);
            reg_to_str(sBrand + i * 16 + sizeof(uint32_t) * 3, d);
        }
    }
}