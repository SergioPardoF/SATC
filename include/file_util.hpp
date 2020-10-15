//
// Created by adrian on 15/11/18.
//

#ifndef FILE_UTIL_HPP
#define FILE_UTIL_HPP

#include <vector>
#include <iostream>
#include <fstream>
#include <cctype>
#include <sys/stat.h>
#include <config_util.hpp>


namespace util {


    namespace file {

        uint64_t file_size(const std::string& file){
            struct stat fs;
            if(stat(file.c_str(), &fs) != 0) {
                return 0;
            };
            return fs.st_size;
        }

        bool file_exists(const std::string &file){
            struct stat fs;
            return (stat(file.c_str(), &fs) == 0);
        }

        bool remove_file(const std::string &file){
            return (remove(file.c_str())==0);
        }

        std::string remove_path(const std::string &file){
            auto pos_last_slash = file.find_last_of("\\/");
            if(pos_last_slash != -1) {
                return file.substr(pos_last_slash+1);
            }
            return file;
        }

        std::string remove_extension(const std::string &file){
            auto pos_last_slash = file.find_last_of("\\/");
            auto pos_last_point = file.find_last_of('.');
            if(pos_last_point != -1 && (pos_last_slash == -1 || pos_last_slash < pos_last_point)){
                return file.substr(0, pos_last_point);
            }
            return file;
        }

        std::string index_file(const std::string &index_name, const char* argv[], const size_t length){
            std::string result = index_name;
            if(length > 1){
                std::string dataset_name = remove_extension(remove_path(argv[1]));
                result += "_" + dataset_name;
            }
            for(size_t i = 2; i < length; ++i){
                result += "_" + std::string(argv[i]);
            }
            return result;
        }

        template<class t_value>
        void read_from_file(const std::string& file, std::vector<t_value> &container){
            std::ifstream in(file, std::ios::in | std::ios::binary );
            uint64_t size = file_size(file);
            container.resize(size / sizeof(t_value));
            const uint64_t block_size = config::BUFFER_BLOCK_SIZE * sizeof(t_value);
            uint64_t pos = 0;
            do {
                uint64_t read_bytes = std::min(block_size, container.size() * sizeof(t_value) - pos);
                in.read((char*) &container[pos / sizeof(t_value)], read_bytes);
                pos += read_bytes;
            }while(pos < container.size()*sizeof(t_value));
            in.close();
        }


        template<class t_value>
        void write_to_file(const std::string& file, std::vector<t_value> &container){
            std::ofstream out(file, std::ios::out | std::ios::binary );
            const uint64_t block_size = config::BUFFER_BLOCK_SIZE * sizeof(t_value);
            uint64_t pos = 0;
            do {
                uint64_t write_bytes = std::min(block_size, container.capacity() * sizeof(t_value) - pos);
                out.write((char*) &container[pos / sizeof(t_value)], write_bytes);
                pos += write_bytes;
            }while(pos < container.size()*sizeof(t_value));
            out.close();
        }

    }



}


#endif
