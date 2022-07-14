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
#include <cstring>

#include "../include/rapidxml.hpp"
#include "config.h"
#include "helper.h"
#include "utils.h"

using namespace std;
using namespace rapidxml;
namespace fs = std::experimental::filesystem;

// "/Users/vishal/Downloads/iitd_things/7th_Sem/info retrieval/assignments/assignment_1/source/data/large_data/coll" ../indexfile ../data/small_data/stopwordfile.txt 2 ../data/small_data/xml_tags_info.txt
// ../data/small_data/coll ../indexfile ../data/small_data/stopwordfile.txt 1 ../data/small_data/xml_tags_info.txt
int main(int argc, char **argv) {
    if (argc != 6) {
        cout << "error: expected 6 arguments" << endl;
        return 1;
    }

    string coll_path(argv[1]), index_file(argv[2]), stop_word_file(argv[3]), xml_tags_info(argv[5]);
    int compression_method = atoi(argv[4]);

    if (compression_method >= 5) {
        cout << "not implemented" << endl;
        return 0;
    }

    // generating stop words
    string delimiters = DELIMITERS;
    vector<string> stop_words_vector = get_words(stop_word_file, true);
    unordered_set<string> stop_words(stop_words_vector.begin(), stop_words_vector.end());
    vector<string> xml_tags = get_words(xml_tags_info);
    unordered_set<string> indexable;
    for (size_t i = 1; i < xml_tags.size(); i++) {
        string s(xml_tags[i]);
        // transform(s.begin(), s.end(), s.begin(), ::tolower);
        indexable.insert(s);
    }

    if (DEBUG) {
        assert(xml_tags.size() > 0);
    }


    dictionary dict;
    document_collection documents;
    unordered_set<string> all_terms;
    for (const auto &entry : fs::directory_iterator(coll_path)) {
        if (DEBUG) {
            cout << "Parsing: " << endl;
            cout << entry.path() << endl;
        }
        // Parsing an XML document
        xml_document<> doc;
        ifstream theFile(entry.path());
        vector<char> buffer((istreambuf_iterator<char>(theFile)), istreambuf_iterator<char>());
        buffer.push_back('\0');
        doc.parse<0>(&buffer[0]);
        for (xml_node<> *DOC_node = doc.first_node("DOC"); DOC_node; DOC_node = DOC_node->next_sibling()) {
            xml_node<> *doc_id_node = DOC_node->first_node(xml_tags[0].c_str());
            if (doc_id_node != NULL) {
                string doc_id(doc_id_node->value());
                doc_id = trim(doc_id);
                doc_int doc_index = documents.insert(doc_id);
                for (xml_node<> *child_node = DOC_node->first_node(); child_node; child_node = child_node->next_sibling()) {
                    if (child_node != NULL) {
                        string node_name = child_node->name();
                        if (indexable.count(node_name)) {
                            vector<string> terms = get_terms(child_node->value(), stop_words, delimiters);
                            all_terms.insert(terms.begin(), terms.end());
                            for (string term: terms) {
                                // if (DEBUG) {cout << doc_index << ": " << term << endl;}
                                dict.insert(term, doc_index);
                            }
                        }
                    }
                    // xml_node<> *child_node = DOC_node->first_node(xml_tags[i].c_str());
                    // printf("%s: %s\n", child_node->name(), child_node->value());
                }
            }

            // for (xml_node<> *child_node = DOC_node->first_node(); child_node; child_node = child_node->next_sibling()) {
            //     printf("%s: %s\n", child_node->name(), child_node->value());
            // }
            // cout << endl;
        }
    }

    // fs::path dir(INDEX_FILES_DIR);
    // fs::path dict_file_path = dir / fs::path(index_file + ".dict");
    fs::path dict_file_path = fs::path(index_file + ".dict");
    ofstream fout(dict_file_path, ios::out | ios::binary);
    if (fout.is_open()) {

        serialize_vector_of_strings(fout, stop_words_vector);

        documents.serialize_to_disk(fout);

        // fs::path index_file_path = dir / fs::path(index_file + ".idx");
        fs::path index_file_path = fs::path(index_file + ".idx");
        ofstream fout2(index_file_path, ios::out | ios::binary);
        cached_dictionary cached_dict(compression_method);
        if (fout2.is_open()) {
            for (string term: all_terms) {
                posting_list<doc_int> post_list = dict.find(term); // since it is a copy it does not affect the original dictionary. It will still be in memory
                post_list.load_full_posting_list();
                cached_dict.insert_in_disk(fout2, term, post_list.posting_list_items);
                // if (DEBUG) {
                //     cout << term << "->";
                //     for (doc_int d: post_list.posting_list_items) {
                //         cout << d << ",";
                //     }
                //     cout << endl;
                // }
                post_list.clear();
            }
            fout2.close();
        } else {
            cout << "can't open " << index_file_path << endl;
        }
        cached_dict.serialize_to_disk(fout);
        fout.close();
    } else {
        cout << "can't open " << dict_file_path << endl;
    }
    return 0;
}