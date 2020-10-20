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
// Created by Adrián on 19/10/2020.
//

#include <iostream>
#include <repair_sampling_offsets.hpp>
#include <random>
#include <time_util.hpp>

std::vector<std::pair<uint64_t, uint64_t>> ranges(uint64_t numbers, uint64_t length, uint64_t last){
    std::vector<std::pair<uint64_t, uint64_t>> res;
    while(res.size() < numbers){
        uint64_t pos = rand() % last;
        if(pos + length < last){
            res.emplace_back(pos, pos + length);
        }
    }
    return res;
}


int main(int argc, char** argv) {

    if(argc != 5){
        std::cout << "Usage: " << argv[0] << " index_file length number operation" << std::endl;
        std::cout << "operation: access or extremes" << std::endl;
        return 1;
    }
    std::string index_file = argv[1];
    uint64_t length = std::atoll(argv[2]);
    uint64_t number = std::atoll(argv[3]);
    cds::repair_sampling_offset<> m_structure;
    sdsl::load_from_file(m_structure, index_file);
    auto operations = ranges(number, length, m_structure.last_t);
    if(strcmp(argv[4], "access")== 0){
       auto t0 = ::util::time::user::now();
       for(const auto &op : operations){
           m_structure.access(op.first, op.second);
       }
       auto t1 = ::util::time::user::now();
       auto micr = ::util::time::duration_cast<::util::time::microseconds>(t1-t0);
       std::cout << "Access operations took: " << micr << " (microseconds)." << std::endl;
       std::cout << "Each query took: " << micr/(double) operations.size() << " (microseconds)." << std::endl;
    }else if (strcmp(argv[4], "extremes") == 0){
        auto t0 = ::util::time::user::now();
        for(const auto &op : operations){
            m_structure.extremes(op.first, op.second);
        }
        auto t1 = ::util::time::user::now();
        auto micr = ::util::time::duration_cast<::util::time::microseconds>(t1-t0);
        std::cout << "Extremes operations took: " << micr << " (microseconds)." << std::endl;
        std::cout << "Each query took: " << micr/(double) operations.size() << " (microseconds)." << std::endl;

    }else{
        std::cout << "Operation " << argv[4] << " is not supported." << std::endl;
    }


}