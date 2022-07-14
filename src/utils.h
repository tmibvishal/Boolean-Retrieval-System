#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <unordered_map>
#include <fstream>
#include <iostream>
#include "config.h"

using namespace std;

template<typename T>
class posting_list {
    doc_int df = 0; // document frequency
    vector<pair<string, ll>> flushed_to_disk;
    T last_entry; // undefined is df = 0 o.w. is the last entry of the file


public:
    vector<T> posting_list_items;

    ll load_full_posting_list();

    ll flush(string file_path, fstream &fout, ll threshold_size_bytes);

    vector<T> get_full_posting_list_items();

    void print_full_posting_list();

    ll insert(T t, ll threshold_size_bytes = 0);

    void clear();
};

// dictionary should fit entirely in memory
class dictionary {
    unordered_map<string, posting_list<doc_int>> vocabulary;
    ll total_extra_size_bytes = 0;
    size_t threshold_size_bytes = SAFE_SIZE; // min size that i keep in each posting list safely
    string partial_file_path = TEMP_DIR_PATH;
    ll file_num = 0;

    void flush();

public:
    void insert(string term, doc_int doc_index);

    posting_list<doc_int> find(string term);
    /* find_prefix currently not supported
    vector<posting_list<doc_int>> find_prefix(string prefix) {
        // find and return the entries for all terms that start with a given prefix P
    }
    */
};

class document_collection {
    unordered_map<string, doc_int> doc_indices; // doc_id -> doc_index (when did the document occured first)
    vector<string> doc_ids;                     // doc_index -> doc_id
public:
    size_t size();

    doc_int insert(string doc_id);

    string get_doc_id(int doc_index);

    void serialize_to_disk(ostream &fout);
    void deserialize_from_disk(ifstream &fin);
};

class cached_dictionary {
public:
    int compression_method = 0;
    unordered_map<string, vector<doc_int>> cached_post_lists;
    unordered_map<string, int> frequency;
    unordered_map<string, tuple<size_t, size_t, ll>> flushed_vocabulary; // post_list size, compressed_list size, position in file
    u_ll cur_size_bytes = 0;
    u_ll capacity_bytes = CACHED_DICT_SIZE_BYTES;

    cached_dictionary();

    cached_dictionary(int compression_method);

    void serialize_to_disk(ostream &fout);

    void deserialize_from_disk(ifstream &fin);

    void insert_in_disk(ofstream &fout, string term, vector<doc_int> &posting_list_items);

    vector<doc_int>& get_post_list(ifstream& fin, string term);
};

#endif
