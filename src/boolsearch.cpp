#include <fstream>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>
#include <string.h>
#include <unordered_set>
#include <sstream>
#include <experimental/filesystem>
#include <algorithm>
#include <map>

#include "../include/rapidxml.hpp"
#include "config.h"
#include "utils.h"
#include "helper.h"

using namespace std;
namespace fs = std::experimental::filesystem;

// "/Users/vishal/Downloads/iitd_things/7th_Sem/info retrieval/assignments/assignment_1/source/data/large_data/queries.txt" ../data/large_data/result.txt ../indexfile.idx ../indexfile.dict
// ../data/small_data/query_file.txt ../data/small_data/result.txt ../indexfile.idx ../indexfile.dict
int main(int argc, char **argv) {
    if (argc != 5) {
        cout << "error: expected 5 arguments" << endl;
        return 1;
    }

    string query_file_path(argv[1]), result_file_path(argv[2]), index_file_path(argv[3]), dict_file_path(argv[4]);

    ifstream fin_dict(dict_file_path, ios::out | ios::binary);
    if (!fin_dict.is_open()) {
        cout << "can't open " << dict_file_path << endl;
        return 1;
    }

    vector<string> stop_words_vector = deserialize_vector_of_strings(fin_dict);
    unordered_set<string> stop_words(stop_words_vector.begin(), stop_words_vector.end());
    if (DEBUG) {
        cout << "Stop Words are: ";
        for (string s: stop_words) {
            cout << s << ", ";
        }
        cout << endl;
    }

    document_collection documents;
    documents.deserialize_from_disk(fin_dict);

    cached_dictionary cached_dict;
    cached_dict.deserialize_from_disk(fin_dict);
    
    ifstream fin_idx(index_file_path, ios::out | ios::binary);
    if (!fin_idx.is_open()) {
        cout << "can't open " << index_file_path << endl;
        return 1;
    }
    ifstream fin_query(query_file_path);
    if (!fin_query.is_open()) {
        cout << "can't open " << query_file_path << endl;
        return 1;
    }
    ofstream fout_result(result_file_path);
    if (!fout_result.is_open()) {
        cout << "can't open " << result_file_path << endl;
        return 1;
    }

    string delimiters = DELIMITERS;
    string query;
    u_ll i = 0;
    while (getline(fin_query, query)) {
        vector<doc_int> result;
        vector<string> terms = get_terms(query, delimiters, true);
        bool first_term = true;
        for (string term: terms) {
            if (stop_words.find(term) == stop_words.end()) {
                vector<doc_int> &post_list_items = cached_dict.get_post_list(fin_idx, term);
                if (first_term) {
                    result = post_list_items;
                    first_term = false;
                } else {
                    vector<doc_int> temp;
                    set_intersection(result.begin(), result.end(), post_list_items.begin(), post_list_items.end(), back_inserter(temp));
                    result = temp;
                }
            }
        }
        if (DEBUG) {
            cout << "query: " << query << endl;
            for (doc_int d: result) {
                cout << d << ", ";
            }
            cout << endl;
        }
        for (doc_int d: result) {
            fout_result << "Q" << i << " ";
            fout_result << documents.get_doc_id(d) << " ";
            fout_result << "1.0" << endl;
        }
        i += 1;
        result.clear();
    }

    return 0;
}