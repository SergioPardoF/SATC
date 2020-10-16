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
// Created by Adrián on 16/10/2020.
//

#include <gtest/gtest.h>
#include <repair_sampling.hpp>
#include <vector>


std::string vector_int_file = "vector_int_file.bin";
std::string index_file = "repair_sampling.idx";
uint64_t period = 100;

TEST (RepairSamplingTest, Construction) {
    cds::repair_sampling<> m_structure(vector_int_file, period);
    std::vector<uint32_t> orig, sol;
    ::util::file::read_from_file(vector_int_file, orig);
    sol = m_structure.decompress();
    ASSERT_EQ(sol.size(), orig.size());
    for(uint64_t i = 0; i < orig.size(); ++i){
        ASSERT_EQ (sol[i], orig[i]);
    }
    ASSERT_TRUE(sdsl::store_to_file(m_structure, index_file));
}

TEST (RepairSamplingTest, Load) {
    cds::repair_sampling<> m_structure;
    sdsl::load_from_file(m_structure, index_file);

    std::vector<uint32_t> orig, sol;
    ::util::file::read_from_file(vector_int_file, orig);
    sol = m_structure.decompress();
    ASSERT_EQ(sol.size(), orig.size());
    for(uint64_t i = 0; i < orig.size(); ++i){
        ASSERT_EQ (sol[i], orig[i]);
    }

}

TEST (RepairSamplingTest, Acesss1) {
    cds::repair_sampling<> m_structure;
    sdsl::load_from_file(m_structure, index_file);

    std::vector<uint32_t> orig;
    ::util::file::read_from_file(vector_int_file, orig);
    for(uint64_t i = 0; i < orig.size(); ++i){
        auto sol = m_structure.access(i,i);
        ASSERT_EQ( 1, sol.size());
        ASSERT_EQ (sol[0], orig[i]);
    }
}

TEST (RepairSamplingTest, AcesssOffset) {
    uint64_t offset = 40;
    cds::repair_sampling<> m_structure;
    sdsl::load_from_file(m_structure, index_file);

    std::vector<uint32_t> orig;
    ::util::file::read_from_file(vector_int_file, orig);
    for(uint64_t i = 0; i < orig.size()-offset; ++i){
        auto sol = m_structure.access(i,i+offset);
        ASSERT_EQ(offset+1, sol.size());
        for(uint64_t j = 0; j <= offset; ++j){
            ASSERT_EQ (sol[j], orig[i+j]);
        }
    }
}

TEST (RepairSamplingTest, Extremes) {

}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);

    return RUN_ALL_TESTS();
}