#include "search_server.h"
#include "iterator_range.h"
#include "parse.h"
#include "profile.h"

#include <algorithm>
#include <iterator>
#include <sstream>
#include <iostream>
#include <queue>
#include <numeric>
#include <future>

InvertedIndex::InvertedIndex(istream& document_input) {
    for (string current_document; getline(document_input, current_document); ) {
        docs.push_back(move(current_document));
        const size_t docid = docs.size() - 1;
        for (const auto& word : SplitIntoWords(docs.back())) {
            auto& frequency = index[word];
            if (!frequency.empty() && frequency.back().docid == docid) {
                ++frequency.back().hitcount;
            } else {
                frequency.push_back({docid, 1});
            }
        }
    }
}

const vector<InvertedIndex::Entry>& InvertedIndex::Lookup(const string_view& word) const {
    static const vector<Entry> empty;
    if (auto it = index.find(word); it != index.end()) {
        return it->second;
    } else {
        return empty;
    }
}

SearchServer::SearchServer(istream& document_input) {
    cin.tie(nullptr);
    ios_base::sync_with_stdio(false);

    UpdateDocumentBase(document_input);
}

void SearchServer::UpdateDocumentBase(istream& document_input) {
    LOG_DURATION("UpdateDocumentBase");

    index = InvertedIndex(document_input);
}

void SearchServer::AddQueriesStream(istream& query_input, ostream& search_results_output) {
//    TotalDuration docid_creation("Total docid creation");
//    TotalDuration sort_vec("Total sort");

//    vector<size_t> docid_count(index.GetDocsCount());

    vector<string> queries;
    queries.reserve(MAX_DOCUMENT_COUNT);
    for (string current_query; getline(query_input, current_query); ) {
        queries.push_back(current_query);
    }

    vector<future<vector<string>>> futures;
    size_t range_size = queries.size() / MAX_THREAD_COUNT;
    for (auto& range : Paginate(queries, range_size)) {
        futures.push_back(async([=]{
            return ProcessQueriesRange(range);
        }));
    }

    for (auto& f : futures) {
        for (const auto& s : f.get()) {
            search_results_output << s;
        }
    }
}

vector<string> SearchServer::ProcessQueriesRange(IteratorRange<vector<string>::iterator> range) {
    vector<string> result;
    result.reserve(range.size());

    vector<size_t> docid_count(index.GetDocsCount());
    vector<size_t> indexes(docid_count.size());
    for(string current_query : range) {
        docid_count.assign(docid_count.size(), 0);
        for (const string_view& word : SplitIntoWords(current_query)) {
            for (auto [docid, hit_count]  : index.Lookup(word)) {
                docid_count[docid] += hit_count;
            }
        }

        iota(indexes.begin(), indexes.end(), 0);
        IteratorRange top_5 = Head(indexes, 5);
        partial_sort(top_5.begin(),
                     top_5.end(),
                     indexes.end(),
                     [&docid_count](size_t i, size_t j){
                        return docid_count[i] > docid_count[j] ||
                              (docid_count[i] == docid_count[j] && i < j);
                     });


        ostringstream os;
        os << current_query << ':';
        for (size_t docid : top_5) {
            size_t hit_count = docid_count[docid];
            if (hit_count == 0) {
                break;
            }
            os << " {"
               << "docid: " << docid << ", "
               << "hitcount: " << hit_count << '}';
        }
        os << endl;
        result.push_back(os.str());
    }
    return result;
}


