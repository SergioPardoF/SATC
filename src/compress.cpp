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
#include "BPlusTree.h"

using namespace cds;

// Function to split a string based on delimiter
std::vector<std::string> splitString(const std::string& line, char delimiter) {
    std::vector<std::string> tokens;
    std::stringstream ss(line);
    std::string token;
    while (std::getline(ss, token, delimiter)) {
        tokens.push_back(token.empty()?" ":token);
    }
    return tokens;
}

uint32_t convertStringto32(std::string key, std::map<std::string , uint32_t> &map, uint32_t &counter){
    auto it = map.find(key);
    if (it == map.end()) {
        map[key] = counter++;
        return map[key];
    } else {
        return it->second;
    }
}

int main(int argc, const char *argv[]){
    if (argc < 3)
    {
        std::cerr << "Usage: " << argv[0] << " trayectorias cabeceras" << std::endl;
        return EXIT_FAILURE;
    }

    std::ifstream input(argv[1]);
    if (!input.is_open()) {
        std::cout << "Error opening trayectories file." << std::endl;
        return EXIT_FAILURE;
    }

    // Header file
    std::ifstream cabeceras(argv[2]);
    if (!cabeceras.is_open()) {
        std::cout << "Error opening headers file." << std::endl;
        return EXIT_FAILURE;
    }

    std::ofstream date_position("fecha_posicion.csv");
    if (!date_position.is_open()) {
        std::cout << "Error opening result file." << std::endl;
        return EXIT_FAILURE;
    }

    std::string line;
    std::getline(cabeceras, line);  // headers line
    line = "hack_license,pickup_datetime,dropoff_datetime,c_pos";
    date_position << line << std::endl;
    /*
    if (std::getline(cabeceras, line)){
        line += ','+std::string("c_pos");
        cabeceras.seekg(0, std::ios::beg);
        cabeceras << line << std::endl << std::endl;
    } else {
        std::cerr << "Error reading header from CSV file." << std::endl;
        return EXIT_FAILURE;
    }
    */
    line.clear();

    std::vector<uint32_t> numbers;
    uint32_t number;
    std::cout << "Reading uncompressed file..." << std::endl;
    while(input.read(reinterpret_cast<char*>(&number), sizeof(uint32_t))){
        numbers.push_back(number);
        //std::cout << number << ", ";
    }
    //std::cout << std::endl;

   repair_sampling<> m_structure(numbers);
   std::cout << "Built" << std::endl;
   /*auto sol = m_structure.decompress();
   std::cout << "Decompressed" << std::endl;
   for(uint i = 0; i < numbers.size(); ++i){
       if(numbers[i] != sol[i]){
           std::cout << "Error en i=" <<i << std::endl;
           exit(0);
       }
   } */
   std::cout << "Everything is OK!" << std::endl;

   //std::cout << "Compresed numbers: " << std::endl;
   auto compressed = m_structure.compressed();
   /*for(int i=0; i < compressed.size(); i++)
       std::cout << compressed[i] << ", ";
   std::cout << std::endl;*/
/*
   std::cout << "Acess operation: " << std::endl;
   auto r5 = m_structure.access(3,9);   // i: 270, j:321
   for(const auto &v : r5){
       std::cout << v << ", ";
   }
    std::cout << std::endl;
   for(uint i = 3; i <= 9; ++i){    // i: 270, j:321
       std::cout << numbers[i] << ", ";
   }
   std::cout << std::endl;
   for(const auto &v : numbers){
       std::cout << v << ", ";
   }
   std::cout << std::endl;

   auto p = m_structure.extremes(0, 9);   // i: 30, j:999
   std::cout << "min: " << p.first << ", max: " << p.second << std::endl;
*/
   std::cout << "Array: " << (sizeof(uint32_t) * 1000) << " (bytes)" << std::endl;
   std::cout << "Structure: " << sdsl::size_in_bytes(m_structure) << " (bytes)" << std::endl;
   sdsl::write_structure<sdsl::format_type::HTML_FORMAT>(m_structure, "repair_sampling.html");

    std::vector<uint32_t> decompressed_entry;
    /*
    std::cout << "Decompressing entries: " << std::endl;
    for(int j=10; j<17; j++){
        std::cout << "Decompressing entry " << j << ": " << std::endl;
        m_structure.decompress_entry(j, decompressed_entry);
        std::cout << "Elements in " << j << " = " << decompressed_entry.size() << std::endl;
        for(int i=0; i<decompressed_entry.size();i++) {
            std::cout << decompressed_entry[i] << ", ";
        }
        std::cout << std::endl;
        decompressed_entry.clear();
    }
    std::cout << std::endl;
    */
    std::cout << "Compressed size: " << compressed.size() << std::endl;
    std::cout << "Decompressing c entry by entry and noting 0's..." << std::endl;
    BPlusTree<uint32_t, int> init_date_tree(100);
    BPlusTree<uint32_t, int> fin_date_tree(100);
    BPlusTree<uint32_t, int> id_tree(100);
    uint32_t sdate, fdate;
    uint32_t map_counter = 1;
    std::map<std::string, uint32_t> map;
    uint32_t converted_id;
    int csv_line = 2;
    for (int i = 0; i < compressed.size(); i++) {
        if (compressed[i] < m_structure.m_alpha) {
            if (compressed[i] == 0) {
                std::string newline;
                std::getline(cabeceras, line);
                std::vector<std::string> vline = splitString(line, ',');
                converted_id = convertStringto32(vline[0], map, map_counter);
                /*newline += std::to_string(converted_id) + ',' + vline[5] + ',' + vline[6] + ',' + std::to_string(i);
                date_position << newline << std::endl;*/
                std::stringstream ss1(vline[5]);
                std::stringstream ss2(vline[6]);
                ss1 >> sdate;
                ss2 >> fdate;
                newline += std::to_string(converted_id) + ',' + std::to_string(sdate) + ',' + std::to_string(fdate) + ',' + std::to_string(i);
                date_position << newline << std::endl;
                init_date_tree.insert(sdate, csv_line);
                id_tree.insert(converted_id, csv_line);
                fin_date_tree.insert(fdate, csv_line);
                csv_line++;
            }
        } else {
            m_structure.decompress_entry(compressed[i], decompressed_entry);
            for (auto entry : decompressed_entry) {
                if (entry == 0) {
                    std::string newline;
                    std::getline(cabeceras, line);
                    std::vector<std::string> vline = splitString(line, ',');
                    converted_id = convertStringto32(vline[0], map, map_counter);
                    std::stringstream ss1(vline[5]);
                    std::stringstream ss2(vline[6]);
                    ss1 >> sdate;
                    ss2 >> fdate;
                    newline += std::to_string(converted_id) + ',' + std::to_string(sdate) + ',' + std::to_string(fdate) + ',' + std::to_string(i);
                    date_position << newline << std::endl;
                    init_date_tree.insert(sdate, csv_line);
                    id_tree.insert(converted_id, csv_line);
                    fin_date_tree.insert(fdate, csv_line);
                    csv_line++;
                }
            }
            decompressed_entry.clear();
        }
    }

    std::cout << "csv lines: " << csv_line << std::endl;
    std::cout << "Writing trees to disk..." << std::endl;

    init_date_tree.writeToFile("init_date_btree.bin");
    fin_date_tree.writeToFile("fin_date_btree.bin");
    id_tree.writeToFile("id_btree.bin");

    std::ofstream repair_struct("repair_struct.bin");
    if (!repair_struct.is_open()) {
        std::cout << "Error opening repair file." << std::endl;
        return EXIT_FAILURE;
    }
    m_structure.serialize(repair_struct);
    /*
    std::cout << "Positions of decompressed 0 values in c :" << std::endl;
    for(auto pos : zero_pos){
        std::cout << pos << ", ";
    }
    std::cout << std::endl;
    */
    cabeceras.close();
    date_position.close();
    input.close();
    repair_struct.close();

    std::cout << "DONE" << std::endl;

    //init_date_tree.bpt_print();


    /*std::cout << "Reading tree..." << std::endl;
    BPlusTree<uint32_t, int> init_date_loaded_tree(100);

    init_date_loaded_tree.readFromFile("init_date_btree.bin");

    //init_date_loaded_tree.bpt_print();
    if (init_date_tree.search(1242840)){
        std::cout << "Key found" << std::endl;
    }

    if (init_date_loaded_tree.search(1242840)){
        std::cout << "Key found" << std::endl;
    }

    std::vector<uint32_t> sdatetree_result_keys, sdate_loaded_tree_result_keys;
    std::vector<int> sdatetree_result_values, sdate_loaded_tree_result_values;

    std::cout << "Comparing trees..." << std::endl;
    if (init_date_tree.equals(&init_date_loaded_tree)){
        std::cout << "Trees are equal" << std::endl;
    } else {
        std::cout << "Trees are not equal" << std::endl;
    }

    init_date_tree.range_search(1242840, 1243342, sdatetree_result_keys, sdatetree_result_values);
    std::cout << "Range search in constructed tree: " << std::endl;
    std::cout << "Size of result vectors: " << sdatetree_result_keys.size() << std::endl;
    std::cout << "Range search in serialized tree: " << std::endl;
    init_date_loaded_tree.range_search(1242840, 1243342, sdate_loaded_tree_result_keys, sdate_loaded_tree_result_values);
    std::cout << "Size of result vectors: " << sdate_loaded_tree_result_keys.size() << std::endl;
     */
}