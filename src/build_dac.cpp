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
// Created by Adrián on 20/10/2020.
//

#include <iostream>
#include <dac.hpp>
#include <file_util.hpp>

int main(int argc, char** argv) {

    if(argc != 3){
        std::cout << "Usage: " << argv[0] << " input_file index_file" << std::endl;
        return 1;
    }
    std::string input_file = argv[1];
    std::string index_file = argv[2];

    std::vector<int> input_vector;
    ::util::file::read_from_file(input_file, input_vector);
    cds::dac_vector_dp_v2<> m_dac(input_vector);
    for(uint64_t i = 0; i < input_vector.size(); ++i){
        std::cout << m_dac[i] << std::endl;
    }
    sdsl::store_to_file(m_dac, index_file);
    sdsl::write_structure<sdsl::format_type::HTML_FORMAT>(m_dac, ::util::file::remove_extension(index_file) + ".html");

}