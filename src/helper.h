#ifndef HELPER_H
#define HELPER_H
#include <string>
using namespace std;
string trim(const string &s);
void serialize_vector_of_strings(ofstream& fout, vector<string>& vec);
vector<string> deserialize_vector_of_strings(ifstream& fin);
vector<string> get_words(const string& file_path, bool lower_case = false);
vector<string> get_terms(const string& s_text, string& delimiters, bool porter_stemming);
vector<string> get_terms(const string& s_text, unordered_set<string>& stop_words, string& delimiters, bool porter_stemming = true);
vector<string> get_terms(char* text, unordered_set<string>& stop_words, string& delimiters, bool porter_stemming = true);
#endif
