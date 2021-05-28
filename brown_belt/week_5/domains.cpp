#include <algorithm>
#include <iostream>
#include <string>
#include <string_view>
#include <vector>
#include "test_runner.h"

using namespace std;

vector<string> ReadDomains(istream& in_stream = cin) {
    size_t count;
    in_stream >> count;

    vector<string> domains(count);
    for (auto& domain : domains) {
        in_stream >> domain;
    }
    return domains;
}

bool CompareReverseString(string str1, string str2) {
    reverse(begin(str1), end(str1));
    reverse(begin(str2), end(str2));
    return str1 < str2;
}

void ReverseSortDomains(vector<string>& domains) {
    sort(begin(domains), end(domains), CompareReverseString);
}

bool IsSubdomain(string_view subdomain, string_view domain) {
    int i = subdomain.size() - 1;
    int j = domain.size() - 1;
    while (i >= 0 && j >= 0) {
        if (subdomain[i--] != domain[j--]) {
            return false;
        }
    }
    return (i < 0 && j < 0) ||
           (j < 0 && subdomain[i] == '.');
}

void FilterDomainDerivatives(vector<string>& domains) {
    size_t insert_pos = 0;
    for (string& domain : domains) {
        if (insert_pos == 0 || !IsSubdomain(domain, domains[insert_pos - 1])) {
            swap(domains[insert_pos++], domain);
        }
    }
    domains.resize(insert_pos);
}

bool IsDomainBanned(string_view domain, const vector<string>& banned_domains) {
//    if (const auto it = upper_bound(begin(banned_domains), end(banned_domains), domain);
//            it != begin(banned_domains) && IsSubdomain(domain, *prev(it))) {
//        return false;
//    } else {
//        return true;
//    }
//    for (const auto & banned_domain : banned_domains) {
//        if (IsSubdomain(banned_domain, domain)) {
//            return true;
//        }
//    }
//    return false;

    const auto it  = upper_bound(begin(banned_domains),
                                  end(banned_domains),
                                  string(domain),
                                  CompareReverseString
                                  );

    if ((it != begin(banned_domains) && IsSubdomain(domain, *prev(it))) /*||
        (it != end(banned_domains) && IsSubdomain(*it, domain))*/) {
        return true;
    } else {
        return false;
    }
}

void CheckAndWriteDomains(const vector<string>& domains_to_check,
                          const vector<string>& banned_domains,
                          ostream& out_stream = cout) {
    for (const string_view domain : domains_to_check) {
        if (IsDomainBanned(domain, banned_domains)) {
            out_stream << "Bad" << endl;
        } else {
            out_stream << "Good" << endl;
        }
    }
}

void TestReadDomains() {
    istringstream input("4\n"
                        "ya.ru\n"
                        "maps.me\n"
                        "m.ya.ru\n"
                        "com"
    );
    vector<string> result = {"ya.ru", "maps.me", "m.ya.ru", "com"};
    const vector<string> domains = ReadDomains(input);
    ASSERT_EQUAL(domains.size(), 4u);
    ASSERT_EQUAL(domains, result);
}

void TestCompareReverseStrings() {
    ASSERT(CompareReverseString("ma", "mb"));
    ASSERT(CompareReverseString("122", "123"));
    ASSERT(!CompareReverseString("ma", "ma"));
    ASSERT(!CompareReverseString("20", "10"));
}

void TestReverseSortDomains() {
    vector<string> domains = {"ya.ru", "maps.me", "m.ya.ru", "com"};
    ReverseSortDomains(domains);
    vector<string> result = {"maps.me", "com", "ya.ru", "m.ya.ru"};
    ASSERT_EQUAL(domains.size(), 4u);
    ASSERT_EQUAL(domains, result);
}

void TestFilterDerivatives() {
    {
        vector<string> domains = {"ya.ru", "maps.me", "m.ya.ru", "com"};
        ReverseSortDomains(domains);
        FilterDomainDerivatives(domains);
        vector<string> result = {"maps.me", "com", "ya.ru"};
        ASSERT_EQUAL(domains.size(), 3u);
        ASSERT_EQUAL(domains, result);
    }
}

void TestIsSubdomain() {
    ASSERT(IsSubdomain("maps.me", "maps.me"));
    ASSERT(IsSubdomain("m.maps.me", "maps.me"));
    ASSERT(IsSubdomain("m.ya.ru", "ya.ru"));
    ASSERT(IsSubdomain("moscow.m.ya.ru", "ya.ru"));
    ASSERT(IsSubdomain("ya.com", "com"));
    ASSERT(IsSubdomain("ha.ya.ya", "ya.ya"));
    ASSERT(IsSubdomain("d.b.c", "b.c"));
    ASSERT(!IsSubdomain("haya.ya", "ya.ya"));
    ASSERT(!IsSubdomain("ya.ya", "ya.ru"));
    ASSERT(!IsSubdomain("com.ru", "com"));
    ASSERT(!IsSubdomain("ru.com.en", "com"));
    ASSERT(!IsSubdomain("vk.com", "m.vk.com"));
}

void TestIsDomainBanned() {
    {
        vector<string> banned_domains = {"maps.me", "com", "ya.ru"};
        ASSERT(IsDomainBanned("ya.ru", banned_domains));
        ASSERT(IsDomainBanned("ya.com", banned_domains));
        ASSERT(IsDomainBanned("m.maps.me", banned_domains));
        ASSERT(IsDomainBanned("moscow.m.ya.ru", banned_domains));
        ASSERT(IsDomainBanned("maps.com", banned_domains));
        ASSERT(!IsDomainBanned("maps.ru", banned_domains));
        ASSERT(!IsDomainBanned("ya.ya", banned_domains));
    }
    {
        vector<string> banned_domains = {"b.c", "a.b.c"};
        FilterDomainDerivatives(banned_domains);
        ASSERT(IsDomainBanned("d.b.c", banned_domains));
    }
    {
        vector<string> banned_domains = {"1.ru", "2.ru", "3.ru", "4.ru"};
        ReverseSortDomains(banned_domains);
        ASSERT(IsDomainBanned("1.m.1.ru", banned_domains));
        ASSERT(IsDomainBanned("moscow.1.ru", banned_domains));
        ASSERT(IsDomainBanned("moscow.2.ru", banned_domains));
        ASSERT(IsDomainBanned("moscow.3.ru", banned_domains));
        ASSERT(IsDomainBanned("moscow.4.ru", banned_domains));
    }
    {
        vector<string> banned_domains = {"1.1.ru"};
        ReverseSortDomains(banned_domains);
        ASSERT(!IsDomainBanned("1.ru", banned_domains));
    }
}

void TestPrintBannedDomains() {
    vector<string> domains_to_check = {"ya.ru", "ya.com", "m.maps.me", "moscow.m.ya.ru",
                                       "maps.com", "maps.ru", "ya.ya"};
    vector<string> banned_domains = {"maps.me", "com", "ya.ru"};

    ostringstream output;
    CheckAndWriteDomains(domains_to_check, banned_domains, output);

    ASSERT_EQUAL(output.str(), "Bad\n"
                               "Bad\n"
                               "Bad\n"
                               "Bad\n"
                               "Bad\n"
                               "Good\n"
                               "Good\n"
    );
}

int main() {
#ifdef LOCAL_BUILD
    TestRunner tr;
    RUN_TEST(tr, TestReadDomains);
    RUN_TEST(tr, TestCompareReverseStrings);
    RUN_TEST(tr, TestIsSubdomain);
    RUN_TEST(tr, TestReverseSortDomains);
    RUN_TEST(tr, TestFilterDerivatives);
    RUN_TEST(tr, TestIsDomainBanned);
    RUN_TEST(tr, TestPrintBannedDomains);
    return 0;
#endif

    const vector<string> banned_domains = []() {
        vector<string> domains = ReadDomains();
        ReverseSortDomains(domains);
        FilterDomainDerivatives(domains);
        return move(domains);
    }();
    const vector<string> domains_to_check = ReadDomains();

    CheckAndWriteDomains(domains_to_check, banned_domains);
    return 0;
}
