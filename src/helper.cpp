#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <unordered_set>
#include <algorithm>
#include <cstring>
#include "../include/porter.h"
#include "helper.h"

using namespace std;

// string triming help taken from https://www.techiedelight.com/trim-string-cpp-remove-leading-trailing-spaces/

const string WHITESPACE = " \n\r\t\f\v";

string left_trim(const string &s) {
    size_t start = s.find_first_not_of(WHITESPACE);
    return (start == string::npos) ? "" : s.substr(start);
}

string right_trim(const string &s) {
    size_t end = s.find_last_not_of(WHITESPACE);
    return (end == string::npos) ? "" : s.substr(0, end + 1);
}

string trim(const string &s) {
    return right_trim(left_trim(s));
}

void serialize_vector_of_strings(ofstream& fout, vector<string>& vec) {
    size_t n = vec.size();
    fout.write((char*)&n, sizeof(n));
    for (string s: vec) {
        size_t s_size = s.size();
        fout.write((char*)&s_size, sizeof(s_size));
        fout.write((char*)&s[0], s_size);
    }
}

vector<string> deserialize_vector_of_strings(ifstream& fin) {
    size_t n;
    fin.read((char*)&n, sizeof(n));
    vector<string> vec(n);
    while (n--) {
        size_t s_size;
        fin.read((char*)&s_size, sizeof(s_size));
        string s(s_size, '0');
        fin.read((char*)&s[0], s_size);
        vec.push_back(s);
    }
    return vec;
}

vector<string> get_words(const string& file_path, bool lower_case) {
    vector<string> words;
    fstream fin;

    fin.open(file_path, ios::in);
    if (fin.is_open()) {
        string s;
        while (fin >> s) {
            if (lower_case) {
                transform(s.begin(), s.end(), s.begin(), ::tolower);
            }
            words.push_back(s);
        }
        fin.close(); //close the file object.
    }

    return words;
}

vector<string> get_terms(const string& s_text, string& delimiters, bool porter_stemming) {
    char *token;
    char *text = new char[s_text.size()+1];
    strcpy(text, s_text.c_str());
    token = strtok(text, delimiters.c_str());
    vector<string> words;

    while( token != NULL ) {
        string s = string(token);
        transform(s.begin(), s.end(), s.begin(), ::tolower);
        if (porter_stemming) {
            words.push_back(stemfile(s));
        } else {
            words.push_back(s);
        }
        token = strtok(NULL, delimiters.c_str());
    }

    delete[] text;

    return words;
}

vector<string> get_terms(const string& s_text, unordered_set<string>& stop_words, string& delimiters, bool porter_stemming) {
    char* token;
    char *text = new char[s_text.size()+1];
    strcpy(text, s_text.c_str());
    token = strtok(text, delimiters.c_str());
    vector<string> words;

    while( token != NULL ) {
        string s = string(token);
        transform(s.begin(), s.end(), s.begin(), ::tolower);
        if (stop_words.find(s) == stop_words.end()) {
            if (porter_stemming) {
                words.push_back(stemfile(s));
            } else {
                words.push_back(s);
            }
        }
        token = strtok(NULL, delimiters.c_str());
    }

    delete[] text;

    return words;
}

vector<string> get_terms(char* text, unordered_set<string>& stop_words, string& delimiters, bool porter_stemming) {
    char* token;
    token = strtok(text, delimiters.c_str());
    vector<string> words;

    while( token != NULL ) {
        string s = string(token);
        transform(s.begin(), s.end(), s.begin(), ::tolower);
        if (stop_words.find(s) == stop_words.end()) {
            if (porter_stemming) {
                words.push_back(stemfile(s));
            } else {
                words.push_back(s);
            }
        }
        token = strtok(NULL, delimiters.c_str());
    }

    return words;
}