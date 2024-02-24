#include "request_queue.h"

RequestQueue::RequestQueue(const SearchServer& search_server) : search_server_(search_server) {}

std::vector<Document> RequestQueue::AddFindRequest(const std::string& raw_query, DocumentStatus status) {
    return AddFindRequest(
        raw_query, [status](int document_id, DocumentStatus document_status, int rating) {
            return document_status == status;
        });
}

std::vector<Document> RequestQueue::AddFindRequest(const std::string& raw_query) {
    return AddFindRequest(raw_query, DocumentStatus::ACTUAL);
}

int RequestQueue::GetNoResultRequests() const {
    return requests_with_no_results_;
}

void RequestQueue::UpdateRequests() {
    minutes_ += 1;
    while (!requests_.empty() && minutes_ - requests_.front().timestamp >= min_in_day_) {
        if (!requests_.front().found) {
            requests_with_no_results_ -= 1;
        }

        requests_.pop_front();
    }
}