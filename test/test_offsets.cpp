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
#include <repair_sampling_offsets.hpp>
#include <repair_sampling_offsets_helper.hpp>
#include <vector>
#include <dac_helper.hpp>


std::string vector_int_file = "181201_181202_I_DXR_RSQESL_PS4EK1.sal";
std::string index_file = "repair/181201_181202_I_DXR_RSQESL_PS4EK1.repair";
uint64_t period = 7000;

TEST (RepairSamplingTest, Construction) {
    cds::repair_sampling_offset<> m_structure(vector_int_file, period);
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
    cds::repair_sampling_offset<> m_structure;
    sdsl::load_from_file(m_structure, index_file);

    std::vector<uint32_t> orig, sol;
    ::util::file::read_from_file(vector_int_file, orig);
    sol = m_structure.decompress();
    ASSERT_EQ(sol.size(), orig.size());
    for(uint64_t i = 0; i < orig.size(); ++i){
        ASSERT_EQ (sol[i], orig[i]);
    }

}

TEST (RepairSamplingTest, Access1) {
    cds::repair_sampling_offset<> m_structure;
    sdsl::load_from_file(m_structure, index_file);

    std::vector<uint32_t> orig;
    ::util::file::read_from_file(vector_int_file, orig);
    for(uint64_t i = 0; i < orig.size(); ++i){
        auto sol = m_structure.access(i,i);
        ASSERT_EQ( 1, sol.size());
        ASSERT_EQ (sol[0], orig[i]);
    }
}

TEST (RepairSamplingTest, AccessOffset) {
    uint64_t offset = 500;
    cds::repair_sampling_offset<> m_structure;
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
    uint64_t offset = 500;
    cds::repair_sampling_offset<> m_structure;
    sdsl::load_from_file(m_structure, index_file);

    std::vector<uint32_t> orig;
    ::util::file::read_from_file(vector_int_file, orig);
    uint64_t limit = 1;
    if(offset < orig.size() ){
        limit = orig.size()-offset;
    }
    for(uint64_t i = 0; i < limit; ++i){
        std::cout << "i: " << i << std::endl;
        auto sol = m_structure.extremes(i,std::min(i + offset, (uint64_t) (orig.size()-1)));
        int32_t min = INT32_MAX, max = 0;
        for(uint64_t j = i; j <= std::min(i + offset, (uint64_t) (orig.size()-1)); ++j){
            if(min > orig[j]) min = orig[j];
            if(max < orig[j]) max = orig[j];
        }
        ASSERT_EQ(sol.first, min);
        ASSERT_EQ(sol.second, max);
    }
}

TEST (RepairSamplingTest, SimilarityFunction) {
    uint64_t window = 1;
    cds::repair_sampling_offset<> m_structure;
    sdsl::load_from_file(m_structure, index_file);
    std::vector<uint32_t> orig;
    ::util::file::read_from_file(vector_int_file, orig);
    auto t_s = 0;
    int32_t min = INT32_MAX, max=-1;
    uint64_t t_min, t_max;
    auto min_max_s1 = m_structure.first(window, 0, m_structure.last_t);
    for(uint64_t i = t_s; i < std::min(t_s + window, m_structure.last_t+1); ++i){
        auto v = static_cast<int32_t >(orig[i]);
        if(min > v) {
            min = v;
            t_min = i;
        }
        if(max < v){
            max = v;
            t_max = i;
        }
    }
    ASSERT_EQ(min, min_max_s1.min);
    ASSERT_EQ(max, min_max_s1.max);
    ASSERT_EQ(t_min, min_max_s1.t_min);
    ASSERT_EQ(t_max, min_max_s1.t_max);
    while(m_structure.exists()){
        t_s += window;
        min_max_s1 = m_structure.next();
        min = INT32_MAX, max=-1;
        for(uint64_t i = t_s; i < std::min(m_structure.last_t+1, t_s + window); ++i){
            auto v = static_cast<int32_t >(orig[i]);
            if(min > v) {
                min = v;
                t_min = i;
            }
            if(max < v){
                max = v;
                t_max = i;
            }
        }
        ASSERT_EQ(min, min_max_s1.min);
        ASSERT_EQ(max, min_max_s1.max);
        ASSERT_EQ(t_min, min_max_s1.t_min);
        ASSERT_EQ(t_max, min_max_s1.t_max);

    }

}

TEST (RepairSamplingTest, Similarity) {
    uint64_t window = 500;
    std::vector<uint32_t > s1_values = {1,1,1,1,1,1,1,1,1,2,2,2,2,2,2,2,3,3,3,3,3,3,3,3,3,3,4,4,4,4,4,4};
    std::vector<uint32_t > s2_values, s3_values;
    s2_values.resize(s1_values.size());
    s3_values.resize(s1_values.size());
    for(uint64_t i = 0; i < s1_values.size(); ++i){
        s2_values[i] = s1_values[i]+2;
        s3_values[i] = s1_values[i] + rand() % 10 + 2;
    }
    cds::repair_sampling_offset<> s1(s1_values, period);
    cds::repair_sampling_offset<> s2(s2_values, period);
    cds::repair_sampling_offset<> s3(s3_values, period);

    auto v1 = cds::compute_similarity(s1, s2, 4, 0, s1_values.size()-1, cds::sum_similarity());
    auto v2 = cds::compute_similarity(s1, s3, 4, 0, s1_values.size()-1, cds::sum_similarity());
    std::cout << "v1: " << v1 << std::endl;
    std::cout << "v2: " << v2 << std::endl;

    ASSERT_GT(v2, v1);


}

TEST (DACTest, Similarity) {
    uint64_t window = 500;
    std::vector<uint32_t > s1_values = {1,1,1,1,1,1,1,1,1,2,2,2,2,2,2,2,3,3,3,3,3,3,3,3,3,3,4,4,4,4,4,4};
    std::vector<uint32_t > s2_values, s3_values;
    s2_values.resize(s1_values.size());
    s3_values.resize(s1_values.size());
    for(uint64_t i = 0; i < s1_values.size(); ++i){
        s2_values[i] = s1_values[i]+2;
        s3_values[i] = s1_values[i] + rand() % 10 + 2;
    }
    cds::dac_vector_dp_v2<> s1(s1_values);
    cds::dac_vector_dp_v2<> s2(s2_values);
    cds::dac_vector_dp_v2<> s3(s3_values);

    auto v1 = cds::dac_helper::similarity(s1, s2, 0, s1.size()-1);
    auto v2 = cds::dac_helper::similarity(s2, s1, 0, s1.size()-1);
    int32_t r1 = 0;
    for(uint64_t i = 0; i < s1_values.size(); ++i){
        r1 += std::abs(static_cast<int32_t>(s1_values[i]) - static_cast<int32_t>(s2_values[i]));
    }
    int32_t r2 = 0;
    for(uint64_t i = 0; i < s1_values.size(); ++i){
        r2 += std::abs(static_cast<int32_t>(s2_values[i]) - static_cast<int32_t>(s1_values[i]));
    }
    ASSERT_EQ(r1, r2);
    ASSERT_EQ(v2, v1);
    ASSERT_EQ(v1, r1);

    v1 = cds::dac_helper::similarity(s1, s3, 0, s1.size()-1);
    v2 = cds::dac_helper::similarity(s3, s1, 0, s1.size()-1);
    r1 = 0, r2 = 0;
    for(uint64_t i = 0; i < s1_values.size(); ++i){
        r1 += std::abs(static_cast<int32_t>(s1_values[i]) - static_cast<int32_t>(s3_values[i]));
    }
    for(uint64_t i = 0; i < s1_values.size(); ++i){
        r2 += std::abs(static_cast<int32_t>(s3_values[i]) - static_cast<int32_t>(s1_values[i]));
    }
    ASSERT_EQ(r1, r2);
    ASSERT_EQ(v2, v1);
    ASSERT_EQ(v1, r1);

}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);

    return RUN_ALL_TESTS();
}