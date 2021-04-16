#pragma once

#include "iterator_range.h"

#include <istream>
#include <ostream>
#include <set>
#include <list>
#include <vector>
#include <map>
#include <string>
#include <string_view>
#include <mutex>

using namespace std;

#include <iostream>

class InvertedIndex {
public:
    struct Entry {
        size_t docid, hitcount;
    };

    InvertedIndex() = default;
    explicit InvertedIndex(istream& document_input);
    const vector<Entry>& Lookup(const string_view& word) const;

    size_t GetDocsCount() {
        return docs.size();
    }
private:
    map<string_view, vector<Entry>> index;
    list<string> docs;

    //    mutex m;
};

class SearchServer {
public:
    SearchServer() = default;
    explicit SearchServer(istream& document_input);
    void UpdateDocumentBase(istream& document_input);
    void AddQueriesStream(istream& query_input, ostream& search_results_output);

private:
    InvertedIndex index;
    mutex index_access_mutex;

    inline static const size_t MAX_DOCUMENT_COUNT = 50000;
    inline static const size_t MAX_THREAD_COUNT = 10;

    vector<string> ProcessQueriesRange(IteratorRange<vector<string>::iterator> range);
};

