//
// Created by DELL on 2025/4/14.
//

#include "AlpEncoder.h"
#include <alp.hpp>
#include <stdexcept>
#include <iostream>

#include "utils.h"


AlpEncoder::AlpEncoder()
    : bit_width(0)
{
    sample_arr = new PT[BLOCK_SIZE];
    exc_arr = new PT[BLOCK_SIZE];
    encoded_arr = new ST[BLOCK_SIZE];
    ffor_base_pt = new ST[BLOCK_SIZE];
    ffor_arr = new ST[BLOCK_SIZE];
    right_arr = new UT[BLOCK_SIZE];
    ffor_right_arr = new UT[BLOCK_SIZE];
    left_arr = new u16[BLOCK_SIZE];
    ffor_left_arr = new u16[BLOCK_SIZE];
    rd_exc_arr = new u16[BLOCK_SIZE];
    exc_pos_arr = new u16[BLOCK_SIZE];
    exc_cnt_pt = new u16[BLOCK_SIZE];
}

AlpEncoder::~AlpEncoder()
{
    delete[] sample_arr;
    delete[] exc_arr;
    delete[] encoded_arr;
    delete[] ffor_base_pt;
    delete[] ffor_arr;
    delete[] right_arr;
    delete[] ffor_right_arr;
    delete[] left_arr;
    delete[] ffor_left_arr;
    delete[] rd_exc_arr;
    delete[] exc_pos_arr;
    delete[] exc_cnt_pt;
}

const CompressedBlock* AlpEncoder::compress(const PT* input_arr)
{
    constexpr u64 column_offset = 0;
    const u64 tuples_count = BLOCK_SIZE;
    alp::state<PT> stt;

    alp::encoder<PT>::init(input_arr, column_offset, tuples_count, sample_arr, stt);
    if (stt.scheme == alp::Scheme::ALP)
    {
        // Encode
        alp::encoder<PT>::encode(input_arr, exc_arr, exc_pos_arr,
                                 exc_cnt_pt, encoded_arr, stt);
        alp::encoder<PT>::analyze_ffor(encoded_arr, bit_width, ffor_base_pt);
        ffor::ffor(encoded_arr, ffor_arr, bit_width, ffor_base_pt);


        return new CompressedBlock_Alp(ffor_base_pt[0], ffor_arr, exc_arr,
                                       exc_pos_arr, exc_cnt_pt[0], bit_width, stt.fac, stt.exp);
    }
    if (stt.scheme == alp::Scheme::ALP_RD)
    {
        alp::rd_encoder<PT>::init(input_arr, column_offset, tuples_count, sample_arr, stt);

        alp::rd_encoder<PT>::encode(input_arr, rd_exc_arr, exc_pos_arr,
                                    exc_cnt_pt, right_arr, left_arr, stt);
        ffor::ffor(right_arr, ffor_right_arr, stt.right_bit_width, &stt.right_for_base);
        ffor::ffor(left_arr, ffor_left_arr, stt.left_bit_width, &stt.left_for_base);

        return new CompressedBlock_AlpRd(ffor_right_arr, ffor_left_arr,
                                         rd_exc_arr, exc_pos_arr, exc_cnt_pt[0], stt);
    }
    throw std::runtime_error("INVALID scheme");
}

CompressedData* tree_level_encode(const std::vector<PT>& data)
{
    static AlpEncoder encoder;
    // std::cout<<"Compressing data of size: " << data.size() << std::endl;
    auto* compressed_data = new CompressedData(ceil(data.size(), BLOCK_SIZE));
    auto* buf = new double[BLOCK_SIZE];
    int cnt = 0;
    for (const auto& val : data)
    {
        buf[cnt++] = val;
        if (cnt == BLOCK_SIZE)
        {
            compressed_data->push(encoder.compress(buf));
            cnt = 0;
        }
    }
    if (cnt)
        compressed_data->push(encoder.compress(buf));
    delete[] buf;
    return compressed_data;
}
