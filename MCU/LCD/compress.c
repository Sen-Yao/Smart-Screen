#include <stdint.h>
#include <stddef.h>

// RLE Ñ¹Ëõº¯Êý
size_t rle_compress(const uint8_t* input, size_t input_size, uint8_t* output) {
    size_t in_idx = 0, out_idx = 0;
    while (in_idx < input_size) {
        uint8_t value = input[in_idx];
        size_t count = 1;
        while (in_idx + count < input_size && input[in_idx + count] == value && count < 255) {
            count++;
        }
        output[out_idx++] = value;
        output[out_idx++] = count;
        in_idx += count;
    }
    return out_idx;
}

// RLE ½âÑ¹Ëõº¯Êý
size_t rle_decompress(const uint8_t* input, size_t input_size, uint8_t* output) {
    size_t in_idx = 0, out_idx = 0;
    while (in_idx < input_size) {
        uint8_t value = input[in_idx++];
        uint8_t count = input[in_idx++];
        for (size_t i = 0; i < count; i++) {
            output[out_idx++] = value;
        }
    }
    return out_idx;
}
