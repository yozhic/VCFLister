// Parser checks for quoted-printable charset and unindented vCard 2.1 base64.
// Build: cl /nologo /std:c++17 /EHsc /utf-8 /W3 test_parser.cpp vcf_parser.cpp vcf_utils.cpp
#include "vcf_parser.hpp"
#include <cstdio>

static int fail(const char* msg) {
    std::fprintf(stderr, "FAIL %s\n", msg);
    return 1;
}

int main() {
    {
        const auto cs = ParseVCard(
            L"BEGIN:VCARD\r\nVERSION:2.1\r\n"
            L"FN;CHARSET=ISO-8859-1;ENCODING=QUOTED-PRINTABLE:Jos\u00E9\r\n"
            L"END:VCARD\r\n");
        if (cs.size() != 1 || cs[0].fn != L"Jos\u00E9") return fail("latin1 literal");
    }
    {
        const auto cs = ParseVCard(
            L"BEGIN:VCARD\r\nVERSION:2.1\r\n"
            L"FN;CHARSET=ISO-8859-1;ENCODING=QUOTED-PRINTABLE:Jos=E9\r\n"
            L"END:VCARD\r\n");
        if (cs.size() != 1 || cs[0].fn != L"Jos\u00E9") return fail("latin1 =E9");
    }
    {
        const auto cs = ParseVCard(
            L"BEGIN:VCARD\r\nVERSION:2.1\r\n"
            L"FN;CHARSET=WINDOWS-1251;ENCODING=QUOTED-PRINTABLE:\u0418\u0432\u0430\u043D\r\n"
            L"END:VCARD\r\n");
        if (cs.size() != 1 || cs[0].fn != L"\u0418\u0432\u0430\u043D") return fail("1251 literal");
    }
    {
        const auto cs = ParseVCard(
            L"BEGIN:VCARD\r\nVERSION:2.1\r\n"
            L"FN:Photo\r\n"
            L"PHOTO;ENCODING=BASE64;JPEG:TW\r\n"
            L"Fu\r\n"
            L"END:VCARD\r\n");
        if (cs.size() != 1 || !cs[0].photo || cs[0].photo->bytes.size() != 3) return fail("photo size");
        const auto& b = cs[0].photo->bytes;
        if (b[0] != 'M' || b[1] != 'a' || b[2] != 'n') return fail("photo bytes");
    }
    std::printf("ok\n");
    return 0;
}
