/***
BSD 2-Clause License

Copyright (c) 2018, <author_name>
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

#include <repair_sampling.hpp>

using namespace cds;

int main(){

   std::vector<uint32_t> vec;
   for(uint32_t i = 0; i < 1000; ++i){
       vec.push_back((uint32_t) (rand() % 10)); //0-19
   }
   for(auto &v : vec){
       std::cout << v << ", ";
   }
   std::cout << std::endl;
   repair_sampling<> m_structure(vec,100);
   std::cout << "Built" << std::endl;
   auto sol = m_structure.decompress();
   std::cout << "Decompressed" << std::endl;
   for(uint i = 0; i < vec.size(); ++i){
       if(vec[i] != sol[i]){
           std::cout << "Error en i=" <<i << std::endl;
           exit(0);
       }
   }
   std::cout << "Everything is OK!" << std::endl;

   auto r5 = m_structure.access(270,321);
   for(const auto &v : r5){
       std::cout << v << ", ";
   }
    std::cout << std::endl;
   for(uint i = 270; i <= 321; ++i){
       std::cout << vec[i] << ", ";
   }
   std::cout << std::endl;
   for(const auto &v : vec){
       std::cout << v << ", ";
   }
   std::cout << std::endl;

   auto p = m_structure.extremes(0, 999);
   std::cout << "min: " << p.first << ", max: " << p.second << std::endl;

   std::cout << "Array: " << (sizeof(uint32_t) * 1000) << " (bytes)" << std::endl;
   std::cout << "Structure: " << sdsl::size_in_bytes(m_structure) << " (bytes)" << std::endl;
   sdsl::write_structure<sdsl::format_type::HTML_FORMAT>(m_structure, "repair_sampling.html");

}