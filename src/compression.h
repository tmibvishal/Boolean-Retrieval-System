#ifndef COMPRESSION_H
#define COMPRESSION_H

#include <vector>
#include "config.h"

using namespace std;

vector<char> compress_list(vector<doc_int>& post_list, int compression_method);
vector<doc_int> decompress_list(vector<char>& compressed_list, int compression_method, size_t p_n);

#endif
