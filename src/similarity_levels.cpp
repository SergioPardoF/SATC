/***
BSD 2-Clause License

Copyright (c) 2018, Adrián
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice, this
  list of conditions and the following disclaimer.

* Redistributions in binary form must reproduce the above copyright notice,
  this list of conditions and the following disclaimer in the documentation
  and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
**/


//
// Created by Adrián on 29/10/2020.
//
#include <string>
#include <iostream>
#include <repair_sampling_offsets.hpp>
#include <repair_sampling_offsets_helper.hpp>

int main(int argc, char** argv) {

    if(argc != 5){
        std::cout << "Usage: " << argv[0] << " index_file1 index_file2 length window_size" << std::endl;
        return 1;
    }
    std::string index_file1 = argv[1];
    std::string index_file2 = argv[2];
    uint64_t length = std::atoll(argv[3]);
    uint64_t window_size = std::atoll(argv[4]);
    cds::repair_sampling_offset<> m_s1, m_s2;
    sdsl::load_from_file(m_s1, index_file1);
    sdsl::load_from_file(m_s2, index_file2);
    auto last = std::min(m_s1.last_t, m_s2.last_t);
    for(uint64_t i = 0;  i + length < last; i = i + length){
        auto similarity = cds::compute_similarity(m_s1, m_s2, window_size, i, i + length -1, cds::sum_similarity());
        std::cout << "Similarity value for [" << i << ", " << i + length -1 << "]: " << similarity << std::endl;
    }



}