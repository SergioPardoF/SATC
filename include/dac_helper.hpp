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

#ifndef REPAIR_SAMPLING_DAC_HELPER_HPP
#define REPAIR_SAMPLING_DAC_HELPER_HPP

#include <limits>
#include <vector>
#include "repair_sampling_offsets_helper.hpp"

namespace cds {

    namespace dac_helper {

        template<class Container>
        std::pair<int, int> extremes(Container &c, uint64_t i, uint64_t j){
            int min = std::numeric_limits<int>::max();
            int max = 0;
            for(auto k = i ; k <= j; ++k){
                auto val = c[k];
                if(val < min) min = val;
                if(val > max) max = val;
            }
            return {min, max};
        }

        template<class Container>
        std::vector<int> access(Container &c, uint64_t i, uint64_t j){
            std::vector<int> res;
            for(auto k = i ; k <= j; ++k){
                res.push_back(c[k]);
            }
            return res;
        }

        template<class Container>
        double similarity(Container &c1, Container &c2, uint64_t i, uint64_t j){
            double r = 0;
            for(auto k = i ; k <= j; ++k){
                if(c1[k]>c2[k]){
                    r += c1[k] - c2[k];
                }else{
                    r += c2[k] - c1[k];
                }
            }
            return r;
        }



        template<class Container, class t_similarity>
        double similarity_old(Container &c1, Container &c2, uint64_t window, uint64_t i, uint64_t j, const t_similarity &similarity_function){
            double result = 0;
            uint64_t blocks = 0;
            auto lower_bound = i;
            while(i <= j){
                int min1 = std::numeric_limits<int>::max();
                int max1 = -1;
                uint64_t t_min1, t_max1;
                for(uint64_t k = i; k <= std::min(i + window-1, j); ++k){
                    auto v = static_cast<int32_t >(c1[k]);
                    if(min1 > v) {
                        min1 = v;
                        t_min1 = k;
                    }
                    if(max1 < v){
                        max1 = v;
                        t_max1 = k;
                    }
                }
                int min2 = std::numeric_limits<int>::max();
                int max2 = -1;
                uint64_t t_min2, t_max2;
                for(uint64_t k = i; k <= std::min(i + window-1, j); ++k){
                    auto v = static_cast<int32_t >(c2[k]);
                    if(min2 > v) {
                        min2 = v;
                        t_min2 = k;
                    }
                    if(max2 < v){
                        max2 = v;
                        t_max2 = k;
                    }
                }
                auto min_dis = cds::euclidean_distance({min1, t_min1-lower_bound}, {min2, t_min2-lower_bound});
                auto max_dis = cds::euclidean_distance({max1, t_max1-lower_bound}, {max2, t_max2-lower_bound});
                result += similarity_function(min_dis, max_dis);
                i += window;
                ++blocks;
            }
            return result / (double) blocks;

        }



    }
}

#endif //REPAIR_SAMPLING_DAC_HELPER_HPP
