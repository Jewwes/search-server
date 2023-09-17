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
    struct Query {
        set<string> minus;
        set<string> plus;
    };
    
struct Document {
    int id;
    double relevance;
};

class SearchServer {
public:
    void SetStopWords(const string& text) {
        for (const string& word : SplitIntoWords(text)) {
            stop_words_.insert(word);
        }
    }

    void AddDocument(int document_id, const string& document) {
        const vector<string> words = SplitIntoWordsNoStop(document);
        double TF = 0;
        for (const auto& word : words) {
            TF = count(words.begin(), words.end(), word);
            TF /= words.size();
            word_to_document_freqs_[word].insert({ document_id,TF });
        }
        document_count_++;
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
    map<string, map<int, double>> word_to_document_freqs_;
    double document_count_ = 0;
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
        for ( string& word : SplitIntoWordsNoStop(text)) {
            if (word[0] == '-'){
                word = word.substr(1);
                if (!IsStopWord(word)){
                    query_words.minus.insert(word);
                }                
            } else {
                query_words.plus.insert(word);
            }
        }
        return query_words;
    }

    vector<Document> FindAllDocuments(const Query& query_words) const {
        map<int,double> document_to_relevance_;
        vector<Document> resulted_elements_;
        for (const auto& iterator : query_words.plus){
            if (word_to_document_freqs_.count(iterator)){
                for (auto& [document_id, TF] : word_to_document_freqs_.at(iterator)){
                    document_to_relevance_[document_id] += TF * log((document_count_)/word_to_document_freqs_.at(iterator).size());
                }
            }
        }
        for (const auto& minus : query_words.minus){
            if (word_to_document_freqs_.count(minus)){
                for (auto& [minus_element,element] : word_to_document_freqs_.at(minus)){
                    document_to_relevance_.erase(minus_element);
                }
            }
        }
        for (const auto& [element,number]: document_to_relevance_){
            resulted_elements_.push_back({element,number});
        }
        return resulted_elements_;
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
