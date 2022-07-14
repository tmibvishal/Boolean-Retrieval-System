#include <vector>
#include <iostream>
#include <cmath>
#include <bitset>
#include <climits>
#include <cstring>
#include <cassert>
#include <snappy.h>
#include "compression.h"
#include "config.h"

using namespace std;

vector<u_ll> reverse_lookup_table = {0, 128, 64, 192, 32, 160, 96, 224, 16, 144, 80, 208, 48, 176, 112, 240, 8, 136, 72, 200, 40, 168, 104, 232, 24, 152, 88, 216, 56, 184, 120, 248, 4, 132,
                                              68, 196, 36, 164, 100, 228, 20, 148, 84, 212, 52, 180, 116, 244, 12, 140, 76, 204, 44, 172, 108, 236, 28, 156, 92, 220, 60, 188, 124, 252, 2, 130, 66,
                                              194, 34, 162, 98, 226, 18, 146, 82, 210, 50, 178, 114, 242, 10, 138, 74, 202, 42, 170, 106, 234, 26, 154, 90, 218, 58, 186, 122, 250, 6, 134, 70, 198, 38,
                                              166, 102, 230, 22, 150, 86, 214, 54, 182, 118, 246, 14, 142, 78, 206, 46, 174, 110, 238, 30, 158, 94, 222, 62, 190, 126, 254, 1, 129, 65, 193, 33, 161,
                                              97, 225, 17, 145, 81, 209, 49, 177, 113, 241, 9, 137, 73, 201, 41, 169, 105, 233, 25, 153, 89, 217, 57, 185, 121, 249, 5, 133, 69, 197, 37, 165, 101, 229,
                                              21, 149, 85, 213, 53, 181, 117, 245, 13, 141, 77, 205, 45, 173, 109, 237, 29, 157, 93, 221, 61, 189, 125, 253, 3, 131, 67, 195, 35, 163, 99, 227, 19, 147,
                                              83, 211, 51, 179, 115, 243, 11, 139, 75, 203, 43, 171, 107, 235, 27, 155, 91, 219, 59, 187, 123, 251, 7, 135, 71, 199, 39, 167, 103, 231, 23, 151, 87,
                                              215, 55, 183, 119, 247, 15, 143, 79, 207, 47, 175, 111, 239, 31, 159, 95, 223, 63, 191, 127, 255};

u_ll length(u_ll x) {
    if (x == 0) {
        return 0;
    } else {
        return (64 - __builtin_clzll(x));
    }
}

u_ll reverse_bits(u_ll num, u_ll size_bits) {
    // inspiration from article: https://www.geeksforgeeks.org/reverse-bits-using-lookup-table-in-o1-time/
    u_ll reverse_num = 0;
    if (size_bits <= 8) {
        reverse_num = reverse_lookup_table.at(num);
        reverse_num >>= (8 - size_bits);
        return reverse_num;
    } else if (size_bits <= 16) {
        reverse_num = reverse_lookup_table[num & 0xff] << 8 |
                      reverse_lookup_table[(num >> 8) & 0xff];
        reverse_num >>= (16 - size_bits);
        return reverse_num;
    } else if (size_bits <= 32) {
        reverse_num = reverse_lookup_table[num & 0xff] << 24 |
                      reverse_lookup_table[(num >> 8) & 0xff] << 16 |
                      reverse_lookup_table[(num >> 16) & 0xff] << 8 |
                      reverse_lookup_table[(num >> 24) & 0xff];
        reverse_num >>= (32 - size_bits);
        return reverse_num;
    } else if (size_bits <= 64) {
        reverse_num = reverse_lookup_table[num & 0xff] << 56 |
                      reverse_lookup_table[(num >> 8) & 0xff] << 48 |
                      reverse_lookup_table[(num >> 16) & 0xff] << 40 |
                      reverse_lookup_table[(num >> 24) & 0xff] << 32 |
                      reverse_lookup_table[(num >> 32) & 0xff] << 24 |
                      reverse_lookup_table[(num >> 40) & 0xff] << 16 |
                      reverse_lookup_table[(num >> 48) & 0xff] << 8 |
                      reverse_lookup_table[(num >> 56) & 0xff];
        reverse_num >>= (64 - size_bits);
        return reverse_num;
    }
    return reverse_num;
}

class bit_stream {
    size_t i = 0;
    size_t j = 0;
    vector<u_ll> vec;
    size_t ele_size = 64;

public:
    bit_stream() {
        vec.push_back(0LL);
    }

    void append(u_ll x, int l) {
        u_ll b2 = x;
        b2 >>= (ele_size - j);
        x <<= j;
        vec[i] |= x;
        j += l;
        if (j >= ele_size) {
            i += 1;
            j = j % ele_size;
            vec.push_back(0LL);
            vec[i] |= b2;
        }
    }

    void fill_compressed_list(vector<char>& compressed_list) {
        size_t e = (u_ll) ele_size / 8ULL;
        size_t n_c = vec.size() * e;
        compressed_list.resize(n_c);
        memcpy(&compressed_list[0], &vec[0], n_c);
        size_t k = e * i, l = j;
        while (l >= 8) {
            k += 1;
            l -= 8;
        }
        if (l > 0) k += 1;
        while (compressed_list.size() > k) {
            compressed_list.pop_back();
        }
    }

    void print_bit_stream() {
        for (u_ll e: vec) {
            bitset<64> b(e);
            for (int k = 0; k < (64 / 2); k++) {
                bool temp_b = b[k];
                b[k] = b[64 - k - 1];
                b[64 - k - 1] = temp_b;
            }
            cout << b;
        }
        cout << endl;
    }
};

pair<u_ll, int> compress_num_c2(u_ll x) {
    u_ll lx = length(x);
    u_ll llx = length(lx);

    int l = (int) (llx + (llx-1) + (lx-1));

    x = reverse_bits(x, lx);
    lx = reverse_bits(lx, llx);

    u_ll inv_zero = 1;
    inv_zero = ~inv_zero;

    x &= inv_zero;
    x <<= (llx + llx - 2);

    lx &= inv_zero;
    lx <<= (llx - 1);

    u_ll b1 = (1LL << (llx - 1)) - 1;

    b1 |= lx;
    b1 |= x;

    return make_pair(b1, l);
}

vector<char> compress_list(vector<doc_int> &post_list, int compression_method) {
    // gap encoding
    size_t n = post_list.size();
    for (size_t i = n - 1; i >= 1; i--) {
        post_list[i] -= post_list[i - 1];
    }

    vector<char> compressed_list;
    if (compression_method == 0) {
        ll num_bytes = (ll) n * sizeof(doc_int);
        compressed_list.resize(num_bytes);
        memcpy(&compressed_list[0], (char *) &post_list[0], num_bytes);
        return compressed_list;
    } else if (compression_method == 1) {
        for (size_t i = 0; i < n; i++) {
            u_ll a = (u_ll) post_list[i];
            assert(a != 0);
            int n_7 = (int) ceil(((double) (64 - __builtin_clzll(a))) / 7);
            vector<char> arr(n_7);
            u_ll mask = (1ULL << 7) - 1;
            for (int j = n_7 - 1; j >= 0; j--) {
                arr[j] = (char) (a & mask);
                a >>= 7;
            }
            char MSB_MASK_1 = (char) (1 << 7);
            for (int j = 0; j < n_7 - 1; j++) {
                arr[j] |= MSB_MASK_1;
            }
            arr[n_7 - 1] &= (char) (~MSB_MASK_1);
            compressed_list.insert(compressed_list.end(), arr.begin(), arr.end());
        }
    } else if (compression_method == 2) {
        bit_stream bit_s;
        for (size_t i = 0; i < n; i++) {
            u_ll x = (u_ll) post_list[i];
            pair<u_ll, int> p = compress_num_c2(x);
            u_ll b1 = p.first;
            int l = p.second;

            bit_s.append(b1, l);
        }
        bit_s.fill_compressed_list(compressed_list);
        if (DEBUG) {
            bit_s.print_bit_stream();
        }
    } else if (compression_method == 3) {
        size_t n_i = post_list.size() * sizeof(doc_int);
        string output;
        snappy::Compress((char*) &post_list[0], n_i, &output);
        compressed_list.resize(output.size());
        memcpy(&compressed_list[0], &output[0], output.size());
    } else if (compression_method == 4) {
        bit_stream bit_s;
        for (size_t i = 0; i < n; i++) {
            u_ll x = (u_ll) post_list[i];

            u_ll min_size = ULLONG_MAX;
            int k_for_min_size = 1;
            for (int k=1; k<=32; k++) {
                u_ll q = (x-1) >> k;
                u_ll cur_size = q + 1 + k;
                if (cur_size < min_size) {
                    min_size = cur_size;
                    k_for_min_size = k;
                }
            }

            u_ll k = k_for_min_size;
            u_ll mask = (1LL << k) - 1;
            u_ll q = (x-1) >> k;
            u_ll r = (x-1) & mask;

            if (DEBUG) {
                cout << "k: " << k << ", q: " << q << ", r: " << r << endl;
            }
            
            u_ll b1 = (1LL << (q+1)) - 1;
            b1 >>= 1;
            if (DEBUG) {
                assert(length(b1) == q);
                assert(length(r) <= k);
            }
            r = reverse_bits(r, k);
            b1 |= (r << (q+1));

            pair<u_ll, int> p = compress_num_c2(k);
            u_ll b2 = p.first;
            int l = p.second;

            bit_s.append(b2, l);
            if (DEBUG) {
                assert(length(b1) <= min_size);
            }
            bit_s.append(b1, (int)min_size);
        }
        bit_s.fill_compressed_list(compressed_list);
    } else {
        cout << "compression_method " << compression_method << " is not implemented" << endl;
    }

    return compressed_list;
}

vector<doc_int> decompress_list(vector<char> &compressed_list, int compression_method, size_t p_n) {
    size_t n_c = compressed_list.size();
    vector<doc_int> post_list;
    if (compression_method == 0) {
        if (DEBUG) {
            assert(n_c % sizeof(doc_int) == 0);
        }
        size_t n = n_c / sizeof(doc_int);
        post_list.resize(n);
        memcpy(&post_list[0], &compressed_list[0], n_c);
    } else if (compression_method == 1) {
        size_t i = 0;
        unsigned char MSB_MASK_1 = (unsigned char) (1 << 7);
        unsigned char inc_MSB_MASK_1 = (unsigned char) (~MSB_MASK_1);
        while (i < n_c) {
            u_ll num = 0;
            while ((MSB_MASK_1 & compressed_list[i]) != 0) {
                num <<= 7;
                num |= (compressed_list[i++] & inc_MSB_MASK_1);
            }
            num <<= 7;
            num |= compressed_list[i++];
            doc_int a = (doc_int) num;
            post_list.push_back((doc_int)a);
        }
    } else if (compression_method == 2) {
        if (n_c != 0) {
            size_t i = 0, j = 0;
            while (true) {
                int llx = 0;
                while (true) {
                    bool t = compressed_list[i] & (1 << j);
                    j += 1;
                    if (j >= 8) {
                        j = 0;
                        i += 1;
                    }
                    llx += 1;
                    if (!t) break;
                }

                int lx = 1;
                llx -= 1;
                while (llx--) {
                    bool t = compressed_list[i] & (1 << j);
                    j += 1;
                    if (j >= 8) {
                        j = 0;
                        i += 1;
                    }
                    lx = (lx << 1) | (t ? 1 : 0);
                }

                u_ll x = 1;
                lx -= 1;
                while (lx--) {
                    bool t = compressed_list[i] & (1 << j);
                    j += 1;
                    if (j >= 8) {
                        j = 0;
                        i += 1;
                    }
                    x = (x << 1) | (t ? 1 : 0);
                }
                post_list.push_back((doc_int)x);
                if (post_list.size() >= p_n) {
                    break;
                }
            }
        }
    } else if (compression_method == 3) {
        size_t n_i = compressed_list.size();
        string output;
        snappy::Uncompress(&compressed_list[0], n_i, &output);
        post_list.resize(output.size() / sizeof(doc_int));
        memcpy(&post_list[0], &output[0], output.size());
    } else if (compression_method == 4) {
        if (n_c != 0) {
            size_t i = 0, j = 0;
            while (true) {
                int llx = 0;
                while (true) {
                    bool t = compressed_list[i] & (1 << j);
                    j += 1;
                    if (j >= 8) {
                        j = 0;
                        i += 1;
                    }
                    llx += 1;
                    if (!t) break;
                }

                int lx = 1;
                llx -= 1;
                while (llx--) {
                    bool t = compressed_list[i] & (1 << j);
                    j += 1;
                    if (j >= 8) {
                        j = 0;
                        i += 1;
                    }
                    lx = (lx << 1) | (t ? 1 : 0);
                }

                u_ll x = 1;
                lx -= 1;
                while (lx--) {
                    bool t = compressed_list[i] & (1 << j);
                    j += 1;
                    if (j >= 8) {
                        j = 0;
                        i += 1;
                    }
                    x = (x << 1) | (t ? 1 : 0);
                }

                u_ll k = x;
                int q = 0;
                while (true) {
                    bool t = compressed_list[i] & (1 << j);
                    j += 1;
                    if (j >= 8) {
                        j = 0;
                        i += 1;
                    }
                    q += 1;
                    if (!t) break;
                }
                q -= 1;

                u_ll r = 0;
                u_ll k_dup = k;
                while (k--) {
                    bool t = compressed_list[i] & (1 << j);
                    j += 1;
                    if (j >= 8) {
                        j = 0;
                        i += 1;
                    }
                    r = (r << 1) | (t ? 1 : 0);
                }

                u_ll num = (q << k_dup) + r + 1;

                if (DEBUG) {
                    cout << "k: " << k_dup << ", q: " << q << ", r: " << r << ", num: " << num << endl;
                }

                post_list.push_back((doc_int)num);
                if (post_list.size() >= p_n) {
                    break;
                }
            }
        }
    } else {
        cout << "compression_method " << compression_method << " is not implemented" << endl;
    }

    // resetting gap encoding
    for (size_t i = 1; i < post_list.size(); i++) {
        post_list[i] += post_list[i - 1];
    }

    if (DEBUG) {
        assert(p_n == post_list.size());
    }

    return post_list;
}