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



    }
}

#endif //REPAIR_SAMPLING_DAC_HELPER_HPP
