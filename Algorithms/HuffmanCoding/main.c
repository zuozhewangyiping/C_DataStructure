#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ds_huffmancoding.h"

int main(void)
{
    unsigned char input[] = "hello huffman";

    DS_HuffmanCoding *hc = ds_huffmancoding_create();

    ds_huffmancoding_build(hc, input, (int)strlen((const char *)input));

    unsigned char *compressed, *decompressed;
    int compressed_len, decompressed_len;

    if (!ds_huffmancoding_encode(hc, input, (int)strlen((const char *)input),
                                 &compressed, &compressed_len))
    {
        printf("encode failed\n");
        ds_huffmancoding_destroy(hc);
        return 1;
    }
    printf("original: %d bytes, compressed: %d bytes\n",
           (int)strlen((const char *)input), compressed_len);

    if (!ds_huffmancoding_decode(hc, compressed, compressed_len,
                                 &decompressed, &decompressed_len))
    {
        printf("decode failed\n");
        free(compressed);
        ds_huffmancoding_destroy(hc);
        return 1;
    }
    printf("decompressed: %d bytes\n", decompressed_len);

    printf("round-trip: %s\n", memcmp(input, decompressed, decompressed_len) == 0 ? "OK" : "FAIL");

    free(compressed);
    free(decompressed);
    ds_huffmancoding_destroy(hc);
    return 0;
}
