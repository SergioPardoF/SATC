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
// Created by Adrián on 17/10/2020.
//

#include <iostream>
#include <dac_helper.hpp>
#include <repair_sampling_offsets.hpp>

int main(int argc, char** argv) {

    if(argc != 5){
        std::cout << "Usage: " << argv[0] << " index_file1 directory i j" << std::endl;
        return 1;
    }
    std::string index_file1 = argv[1];
    std::string directory = argv[2];
    uint64_t i = std::atoll(argv[3]);
    uint64_t j = std::atoll(argv[4]);
    if(!::util::file::end_slash(directory)){
        directory = directory + "/";
    }

    cds::dac_vector_dp_v2<> m_s1, m_s2;
    sdsl::load_from_file(m_s1, index_file1);
    auto files = ::util::file::read_directory(directory);
    std::multimap<uint64_t , std::string> map;
    for(const auto &f : files){
        auto path_file = directory + f;
        sdsl::load_from_file(m_s2, path_file);
        auto similarity = cds::dac_helper::similarity(m_s1, m_s2, i, j);
        //std::cout << "File: " << f << " similarity: " << similarity << std::endl;
        map.insert({similarity, f});
    }

    for(const auto &it : map) {
        std::cout << it.second << std::endl;
    }



}