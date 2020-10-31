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
// Created by Adrián on 30/10/2020.
//
#include <iostream>
#include <file_util.hpp>
#include <unordered_map>

int main(int argc, char** argv) {

    if(argc != 3){
        std::cout << "Usage: " << argv[0] << " file1 file2" << std::endl;
        return 1;
    }
    std::string file1 = argv[1];
    std::string file2 = argv[2];

    std::ifstream input1(file1);
    std::ifstream input2(file2);
    std::string line;
    std::vector<std::string> lines1, lines2;
    while(input1 >> line){
        lines1.push_back(::util::file::remove_extension(line));
    }
    input1.close();
    while(input2 >> line){
        lines2.push_back(::util::file::remove_extension(line));
    }
    input2.close();
    uint64_t oks = 0;
    std::unordered_map<std::string, bool> map_files;
    uint64_t pos1 = 0, pos2 = 0;
    while(pos1 < lines1.size() && pos2 < lines2.size()){
        if(map_files.find(lines2[pos2]) != map_files.end()){
            ++pos2;
        }else if(lines1[pos1] == lines2[pos2]){
            ++oks;
            ++pos1;
            ++pos2;
        }else{
            map_files.insert({lines1[pos1], true});
            ++pos1;
        }
    }
    std::cout << oks << " aciertos de " << lines1.size() << " elementos (" << (oks/ (double) lines1.size() * 100) <<"%)"<< std::endl;


}