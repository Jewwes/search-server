#pragma once

#include <vector>
#include <deque>

#include "search_server.h"

class RequestQueue {
public:
    explicit RequestQueue(const SearchServer& search_server);

    template<typename DocumentPredicate>
    std::vector<Document> AddFindRequest(const std::string& raw_query, DocumentPredicate document_predicate) {

        UpdateRequests();

        auto result = search_server_.FindTopDocuments(raw_query, document_predicate);
        if (result.empty()) {
            requests_with_no_results_ += 1;
        }

        requests_.push_back({ !result.empty(), minutes_ });

        return result;
    }

    std::vector<Document> AddFindRequest(const std::string& raw_query, DocumentStatus status);

    std::vector<Document> AddFindRequest(const std::string& raw_query);

    int GetNoResultRequests() const;
private:
    struct QueryResult {
        bool found;
        int timestamp;
    };

    const SearchServer& search_server_;
    std::deque<QueryResult> requests_;
    const static int min_in_day_ = 1440;
    int minutes_ = 0;
    int requests_with_no_results_ = 0;

    void UpdateRequests();
};