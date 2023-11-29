#include <algorithm>
#include <iostream>
#include <set>
#include <string>
#include <utility>
#include <vector>
#include <map>
#include <cmath>
using namespace std;

const int MAX_RESULT_DOCUMENT_COUNT = 5;

string ReadLine() {
    string s;
    getline(cin, s);
    return s;
}

int ReadLineWithNumber() {
    int result = 0;
    cin >> result;
    ReadLine();
    return result;
}


vector<string> SplitIntoWords(const string& text) {
    vector<string> words;
    string word;
    for (const char c : text) {
        if (c == ' ') {
            if (!word.empty()) {
                words.push_back(word);
                word.clear();
            }
        } else {
            word += c;
        }
    }
    if (!word.empty()) {
        words.push_back(word);
    }

    return words;
}

struct Document {
    int id;
    double relevance;
};

class SearchServer {
public:
    void SetStopWords(const string& text) {
        if (!text.empty()) {
            vector<string> stop_words = SplitIntoWords(text);
            stop_words_.insert(stop_words.begin(), stop_words.end());
        }
    }

    void AddDocument(int document_id, const string& document) {
        ++document_count_;
        const vector<string> words = SplitIntoWordsNoStop(document);
        const double inv_word_count = 1.0 / words.size();
        for (const string& word : words) {
            word_to_document_freqs_[word][document_id] += inv_word_count;
        }
    }

    vector<Document> FindTopDocuments(const string& raw_query) const {
        const Query query_words = ParseQuery(raw_query);
        auto matched_documents = FindAllDocuments(query_words);

        sort(matched_documents.begin(), matched_documents.end(),
             [](const Document& lhs, const Document& rhs) {
                 return lhs.relevance > rhs.relevance;
             });
        if (matched_documents.size() > MAX_RESULT_DOCUMENT_COUNT) {
            matched_documents.resize(MAX_RESULT_DOCUMENT_COUNT);
        }
        return matched_documents;
    }

private:
    double document_count_ = 0.0;

    struct Query {
        set<string> minus_words;
        set<string> plus_words;
    };

    map<string, map<int, double>> word_to_document_freqs_;
    set<string> stop_words_;

    bool IsStopWord(const string& word) const {
        return stop_words_.count(word) > 0;
    }

    vector<string> SplitIntoWordsNoStop(const string& text) const {
        vector<string> words;
        for (const string& word : SplitIntoWords(text)) {
            if (!IsStopWord(word)) {
                words.push_back(word);
            }
        }
        return words;
    }

    Query ParseQuery(const string& text) const {
        Query query_words;
        for (const string& word : SplitIntoWordsNoStop(text)) {
            if (word[0] == '-') {
                if (!word.empty() && !stop_words_.count(word.substr(1))) {
                    query_words.minus_words.insert(word.substr(1));
                }
            } else {
                query_words.plus_words.insert(word);
            }
        }
        return query_words;
    }

    double Calidf(const string& word) const {
        int sizedoc = word_to_document_freqs_.at(word).size();
        return log(static_cast<double>(document_count_) / sizedoc);
    }

    vector<Document> FindAllDocuments(const Query& query) const {
        map<int, double> relev;
        vector<Document> matched_documents;
        for (const auto& word : query.plus_words) {
            if (word_to_document_freqs_.count(word) > 0) {
                auto& wordid = word_to_document_freqs_.at(word);
                double idf = Calidf(word);
                for (const auto& [doc_id, tf] : wordid) {
                    relev.insert({doc_id, 0});
                    relev[doc_id] += tf * idf;
                }
            }
        }

        for (const auto& word : query.minus_words) {
            if (word_to_document_freqs_.count(word) > 0) {
                const auto& Minus_document = word_to_document_freqs_.at(word);
                for (const auto& [document_id, tf] : Minus_document) {
                    relev.erase(document_id);
                }
            }
        }

        for (const auto& [id, rele] : relev) {
            Document document;
            document.id = id;
            document.relevance = rele;
            matched_documents.push_back(document);
        }
        return matched_documents;
    }
};

SearchServer CreateSearchServer() {
    SearchServer search_server;
    search_server.SetStopWords(ReadLine());

    const int document_count = ReadLineWithNumber();
    for (int document_id = 0; document_id < document_count; ++document_id) {
        search_server.AddDocument(document_id, ReadLine());
    }

    return search_server;
}

int main() {
    const SearchServer search_server = CreateSearchServer();

    const string query = ReadLine();
    for (const auto& [document_id, relevance] : search_server.FindTopDocuments(query)) {
        cout << "{ document_id = "s << document_id << ", "
             << "relevance = "s << relevance << " }"s << endl;
    }
}
