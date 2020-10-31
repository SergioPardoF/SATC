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
#include <repair_sampling_offsets.hpp>

int main(int argc, char** argv) {

    if(argc != 2){
        std::cout << "Usage: " << argv[0] << " directory" << std::endl;
        return 1;
    }
    std::string directory = argv[1];
    if(!::util::file::end_slash(directory)){
        directory = directory + "/";
    }
    std::string repair_directory = directory + "dac/";
    ::util::file::create_directory(repair_directory);
    size_t min_length = INT64_MAX;
    auto files = ::util::file::read_directory(directory);
    for(const auto &f : files){
        auto path_file = directory + f;
        auto index_file = repair_directory + ::util::file::remove_extension(f)+".dac";
        std::vector<int> input_vector;
        ::util::file::read_from_file(path_file, input_vector);
        if(input_vector.size() < min_length) min_length = input_vector.size();
        cds::dac_vector_dp_v2<> m_dac(input_vector);
        sdsl::store_to_file(m_dac, index_file);
    }
    std::cout << "Min length: " << min_length << std::endl;
}