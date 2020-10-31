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
// Created by Adrián on 28/10/2020.
//

#ifndef REPAIR_SAMPLING_REPAIR_SAMPLING_OFFSETS_HELPER_HPP
#define REPAIR_SAMPLING_REPAIR_SAMPLING_OFFSETS_HELPER_HPP

#include <repair_sampling_offsets.hpp>

namespace cds {

    inline double euclidean_distance(const std::pair<uint32_t , uint64_t > &p, const std::pair<uint32_t , uint64_t > &p1){
        auto x = std::abs((int32_t) (p1.first - p.first));
        auto y = std::abs((int64_t) (p1.second - p.second));
        return std::sqrt(x * x + y * y);
    }

   //
    typedef struct {

        template<class t_value>
        t_value operator ()(const t_value& v1, const t_value& v2) const {
            return v1 + v2;
        }
    } sum_similarity;

    template <class t_value, class t_similarity>
    double compute_similarity( repair_sampling_offset<t_value> &s1,  repair_sampling_offset<t_value> &s2,
                              const typename repair_sampling_offset<t_value>::size_type window_size,
                              const typename repair_sampling_offset<t_value>::size_type i,
                              const typename repair_sampling_offset<t_value>::size_type j,
                              const t_similarity &similarity_function){


        //TODO: Precondition i and j between 0 and last_t
        auto min_max_s1 = s1.first(window_size, i, j);
        auto min_max_s2 = s2.first(window_size, i, j);
        auto min_distance = euclidean_distance({min_max_s1.min, min_max_s1.t_min}, {min_max_s2.min, min_max_s2.t_min});
        auto max_distance = euclidean_distance({min_max_s1.max, min_max_s1.t_max}, {min_max_s2.max, min_max_s2.t_max});
        uint64_t blocks = 1;
        double result = similarity_function(min_distance, max_distance);
        while(s1.exists()){
            min_max_s1 = s1.next();
            min_max_s2 = s2.next();
            min_distance = euclidean_distance({min_max_s1.min, min_max_s1.t_min}, {min_max_s2.min, min_max_s2.t_min});
            max_distance = euclidean_distance({min_max_s1.max, min_max_s1.t_max}, {min_max_s2.max, min_max_s2.t_max});
            result += similarity_function(min_distance, max_distance);
            ++blocks;
        }
        return result / (double) blocks;
    }

}
#endif //REPAIR_SAMPLING_REPAIR_SAMPLING_OFFSETS_HELPER_HPP
