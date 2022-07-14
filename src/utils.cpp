#include <string>
#include <unordered_map>
#include <fstream>
#include <iostream>
#include <cassert>
#include <algorithm>
#include "config.h"
#include "compression.h"

#include "utils.h"

using namespace std;


template<typename T>
ll posting_list<T>::load_full_posting_list() {
    ll size_bytes_added = 0LL;
    if (flushed_to_disk.size() > 0) {
        vector<T> backup_current_list = posting_list_items;
        posting_list_items.clear();

        for (auto[file_name, position] : flushed_to_disk) {
            fstream fin(file_name, ios::in | ios::binary);
            if (fin.is_open()) {
                fin.seekg(position, fin.beg);
                size_t n = 0;
                fin.read((char *) &n, sizeof(size_t));
                vector<T> temp_list(n);
                if (DEBUG) {
                    // get length of file
                    fin.seekg(0, fin.end);
                    u_ll length = fin.tellg();
                    fin.seekg(position + sizeof(size_t), fin.beg);

                    assert((position + (u_ll)sizeof(size_t) + (u_ll) n * sizeof(T)) <= length);
                }
                fin.read((char *) &temp_list[0], n * sizeof(T));
                fin.close();
                posting_list_items.insert(posting_list_items.end(), temp_list.begin(), temp_list.end());
            } else {
                cout << "error: can't read file file_path" << endl;
            }
        }
        size_bytes_added = (ll) sizeof(T) * (ll) posting_list_items.size();
        posting_list_items.insert(posting_list_items.end(), backup_current_list.begin(), backup_current_list.end());
    }
    return size_bytes_added;
}

template<typename T>
ll posting_list<T>::flush(string file_path, fstream &fout, ll threshold_size_bytes) {
    size_t n = posting_list_items.size();
    ll list_size = (ll) n * sizeof(T);
    if (list_size > threshold_size_bytes) {
        // flush the current list to the end of the file at file_path
        flushed_to_disk.push_back(make_pair(file_path, fout.tellp()));
        fout.write((char *) &n, sizeof(size_t));
        fout.write((char *) &posting_list_items[0], list_size);
        posting_list_items.clear();
        return list_size - threshold_size_bytes;
    }
    return 0LL;
}

template<typename T>
vector<T> posting_list<T>::get_full_posting_list_items() {
    load_full_posting_list();
    return posting_list_items;
}

template<typename T>
void posting_list<T>::print_full_posting_list() {
    load_full_posting_list();
    for (T e: posting_list_items) {
        cout << e << ", ";
    }
}

template<typename T>
ll posting_list<T>::insert(T t, ll threshold_size_bytes) {
    if (df == 0 || last_entry != t) {
        posting_list_items.push_back(t);
        last_entry = t;
        df += 1;
        if ((ll) posting_list_items.size() * (ll) sizeof(T) > threshold_size_bytes) {
            return (ll) sizeof(T);
        }
    }
    return 0ULL;
}

template<typename T>
void posting_list<T>::clear() {
    df = 0;
    posting_list_items.clear();
    flushed_to_disk.clear();
    last_entry = -1;
}

// I am declaring all definitions above with doc_int since it is one of the easy solution to separate template class definitions with declarations.
// https://isocpp.org/wiki/faq/templates#templates-defn-vs-decl
template ll posting_list<doc_int>::load_full_posting_list();
template ll posting_list<doc_int>::flush(string file_path, fstream &fout, ll threshold_size_bytes);
template vector<doc_int> posting_list<doc_int>::get_full_posting_list_items();
template void posting_list<doc_int>::print_full_posting_list();
template ll posting_list<doc_int>::insert(doc_int t, ll threshold_size_bytes);
template void posting_list<doc_int>::clear();

void dictionary::flush() {
    string file_path = partial_file_path + to_string(file_num++);

    fstream fout(file_path, ios::out | ios::binary);
    if (fout.is_open()) {
        for (auto it = vocabulary.begin(); it != vocabulary.end(); it++) {
            total_extra_size_bytes -= it->second.flush(file_path, fout, threshold_size_bytes);
        }
        fout.close();
    } else {
        cout << "error: can't write to file " << file_path << endl;
    }
    if (DEBUG) {
        assert(total_extra_size_bytes >= 0);
    }
}

void dictionary::insert(string term, doc_int doc_index) {
    // insert a new entry for term
    total_extra_size_bytes += vocabulary[term].insert(doc_index, threshold_size_bytes);
    if (total_extra_size_bytes > MAX_EXTRA_SIZE_POSTING_LISTS) {
        flush();
    }
    if (DEBUG) {
        assert(total_extra_size_bytes <= MAX_EXTRA_SIZE_POSTING_LISTS);
    }
}

posting_list<doc_int> dictionary::find(string term) {
    // find and return the entry for term T if present
    return vocabulary[term];
}

size_t document_collection::size() {
    return doc_ids.size();
}

doc_int document_collection::insert(string doc_id) {
    auto it = doc_indices.find(doc_id);
    if (it == doc_indices.end()) {
        doc_int ind = (doc_int) doc_ids.size() + 1;
        doc_indices[doc_id] = ind;
        doc_ids.push_back(doc_id);
        return ind;
    }
    return it->second;
}

string document_collection::get_doc_id(int doc_index) {
    return doc_ids.at(doc_index - 1);
}

void document_collection::serialize_to_disk(ostream& fout) {
    size_t n = doc_ids.size();
    fout.write((char*)&n, sizeof(size_t));
    for (size_t i=0; i<n; i++) {
        string doc_id = doc_ids[i];
        size_t doc_id_size = doc_id.size();
        fout.write((char*)&doc_id_size, sizeof(size_t));
        fout.write((char*)&doc_id[0], doc_id_size);
    }
}

void document_collection::deserialize_from_disk(ifstream &fin) {
    size_t n;
    fin.read((char*)&n, sizeof(n));
    for (size_t i=0; i<n; i++) {
        size_t doc_id_size;
        fin.read((char*)&doc_id_size, sizeof(size_t));
        string doc_id(doc_id_size, '0');
        fin.read((char*)&doc_id[0], doc_id_size);
        insert(doc_id);
    }
}

cached_dictionary::cached_dictionary() {}
cached_dictionary::cached_dictionary(int compression_method_): compression_method(compression_method_) {}

void cached_dictionary::serialize_to_disk(ostream& fout) {
    fout.write((char*)&compression_method, sizeof(compression_method));
    size_t size_vocabulary = flushed_vocabulary.size();
    fout.write((char*)&size_vocabulary, sizeof(size_vocabulary));
    for (auto [term, tup]: flushed_vocabulary) {
        size_t term_size = term.size();
        fout.write((char*)&term_size, sizeof(term_size));
        fout.write((const char*)&term[0], term_size);
        auto [p_n, c_n, position] = tup;
        fout.write((char*)&p_n, sizeof(p_n));
        fout.write((char*)&c_n, sizeof(c_n));
        fout.write((char*)&position, sizeof(position));
    }
}

void cached_dictionary::deserialize_from_disk(ifstream& fin) {
    fin.read((char*)&compression_method, sizeof(compression_method));
    size_t size_vocabulary;
    fin.read((char*)&size_vocabulary, sizeof(size_vocabulary));
    while (size_vocabulary--) {
        size_t term_size;
        fin.read((char*)&term_size, sizeof(term_size));
        string term(term_size, '0');
        fin.read((char*)&term[0], term_size);
        size_t p_n;
        fin.read((char*)&p_n, sizeof(p_n));
        size_t c_n;
        fin.read((char*)&c_n, sizeof(c_n));
        ll position;
        fin.read((char*)&position, sizeof(position));
        flushed_vocabulary[term] = make_tuple(p_n, c_n, position);
    }
}

void cached_dictionary::insert_in_disk(ofstream& fout, string term, vector<doc_int> &posting_list_items) {
    vector<char> compressed_list = compress_list(posting_list_items, compression_method);
    flushed_vocabulary[term] = make_tuple(posting_list_items.size(), compressed_list.size(), fout.tellp());
    // not storing the list size since it is in flushed_vocabulary which will be fetched from disk from the indexfile.dict first
    fout.write((char *) &compressed_list[0], (ll) compressed_list.size() * sizeof(char));
}

vector<doc_int>& cached_dictionary::get_post_list(ifstream& fin, string term) {

    if (cur_size_bytes > capacity_bytes) {
        if (DEBUG) {
            cout << "Deleting elements from dictionary. cur_size = " << cur_size_bytes/(1024*1024) << "MB, capacity = " << capacity_bytes/(1024*1024) << "MB" << endl;
        }
        vector<pair<int, string>> v;
        for (auto it = frequency.begin(); it != frequency.end(); it++) {
            v.push_back({it->second, it->first});
        }
        sort(v.begin(), v.end());
        size_t i = 0;
        while (i < v.size() && cur_size_bytes > capacity_bytes) {
            auto it = cached_post_lists.find(v[i].second);
            cur_size_bytes -= (u_ll) (it->second.size()) * (u_ll) sizeof(doc_int) * 8ULL;
            cached_post_lists.erase(it);
            frequency.erase(v[i].second);
            i += 1;
        }

        size_t l = (v.size() + i)/2;
        while (i<l) {
            auto it = cached_post_lists.find(v[i].second);
            cur_size_bytes -= (u_ll) (it->second.size()) * (u_ll) sizeof(doc_int) * 8ULL;
            cached_post_lists.erase(it);
            frequency.erase(v[i].second);
            i += 1;
        }
    }

    auto it = cached_post_lists.find(term);
    if (it != cached_post_lists.end()) {
        return it->second;
    } else {
        vector<doc_int> posting_list_items;
        auto it_2 = flushed_vocabulary.find(term);
        if (it_2 != flushed_vocabulary.end()) {
            auto [p_n, c_n, position] = it_2->second;
            fin.seekg(position);
            vector<char> compressed_list(c_n);
            fin.read((char *) &compressed_list[0], (ll) c_n * sizeof(char));
            posting_list_items = decompress_list(compressed_list, compression_method, p_n);
        }

        cached_post_lists[term] = posting_list_items;
        frequency[term] += 1;
        cur_size_bytes += (u_ll) (posting_list_items.size()) * (u_ll) sizeof(doc_int) * 8ULL;

        return cached_post_lists[term];
    }
}