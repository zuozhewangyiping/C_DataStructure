#ifndef DS_HUFFMANCODING_H
#define DS_HUFFMANCODING_H

typedef struct DS_HuffmanCoding DS_HuffmanCoding;

/* return pointer if success, NULL if malloc failed */
DS_HuffmanCoding *ds_huffmancoding_create(void);

void ds_huffmancoding_destroy(DS_HuffmanCoding *hc);

/* return 1 if success, 0 if hc / user_data NULL, or malloc failed */
int ds_huffmancoding_build(DS_HuffmanCoding *hc, const unsigned char *user_data, int user_data_length);
/*
    unsigned char *sentence = "Hello world!";

    if(ds_huffmancoding_build(hc, sentence, strlen((const char *)sentence)))
    {
        // 可以开始压缩/解压
    }
*/

/* return 1 if success, 0 if hc / uncompressed_data / compressed_data / compressed_data_length NULL, tree not built, or malloc failed */
int ds_huffmancoding_encode(DS_HuffmanCoding *hc,
                            const unsigned char *uncompressed_data, int uncompressed_data_length,
                            unsigned char **compressed_data, int *compressed_data_length);
/*
    unsigned char *compressed_data;
    int compressed_data_length;

    if(ds_huffmancoding_encode(hc, uncompressed_data, strlen((const char *)uncompressed_data), &compressed_data, &compressed_data_length))
    {
        // compressed_data 指向压缩后的数据，compressed_data_length 是其长度
        // compressed_data 指向堆内存，调用者使用完毕后需 free(compressed_data);
    }
*/

/* return 1 if success, 0 if hc / compressed_data / uncompressed_data / uncompressed_data_length NULL, tree not built, or malloc failed */
int ds_huffmancoding_decode(DS_HuffmanCoding *hc,
                            const unsigned char *compressed_data, int compressed_data_length,
                            unsigned char **uncompressed_data, int *uncompressed_data_length);
/*
    unsigned char *uncompressed_data;
    int uncompressed_data_length;

    if(ds_huffmancoding_decode(hc, compressed_data, compressed_data_length, &uncompressed_data, &uncompressed_data_length))
    {
        // uncompressed_data 指向解压缩后的数据，uncompressed_data_length 是其长度
        // uncompressed_data 指向堆内存，调用者使用完毕后需 free(uncompressed_data);
    }
*/

#endif

/* !WARNING!
 * build 必须先于 encode / decode 调用。
 * encode 和 decode 返回的 out_data 是 malloc 分配的新堆块，调用者用完必须 free。
 * 三个函数操作的 hc 必须是同一个实例——build 生成的树和编码表只存在于该实例中。
 * 编码值用 unsigned long long（64 位）存储，树深不得超过 64 层。
 * 256 种符号在正常频率分布下深度不超过 ~30，此限制不影响正常使用。
 */
