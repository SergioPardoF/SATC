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
// Created by Adrián on 14/10/2020.
//

#ifndef CDS_REPAIR_SAMPLING_HPP
#define CDS_REPAIR_SAMPLING_HPP

#include <vector>
#include <dac.hpp>
#include "repair.hpp"
#include "split_repair_helper.hpp"
#include <util_mem.hpp>

namespace cds {



    template <class t_value = uint32_t>
    class repair_sampling {

    public:
        typedef uint64_t size_type;
        typedef t_value value_type;
        typedef struct {
            size_type entry;
            size_type t_b;
            size_type t_e;
        } slot_type;

        std::vector<value_type> compressed() {
            return m_values;
        }
        size_type m_alpha;

    private:
        size_type m_last_t;
        std::vector<value_type> m_values;
        std::vector<value_type> m_rules;
        std::vector<size_type> m_samples;
        dac_vector_dp_v2<> m_extra_info_length;
        dac_vector_dp_v2<> m_extra_info_values;

    public:
        const size_type &last_t = m_last_t;

    private:

        void copy(const repair_sampling& p){
            m_values = p.m_values;
            m_rules = p.m_rules;
            m_alpha = p.m_alpha;
            m_last_t = p.m_last_t;
            m_samples = p.m_samples;
            m_extra_info_length = p.m_extra_info_length;
            m_extra_info_values = p.m_extra_info_values;
        }

        void add_splits(std::vector<int32_t> &log, std::vector<int32_t> &terminals, std::map<int32_t, char> &map_terminals){
            split_repair_helper<int32_t, uint32_t> split_helper(&log, &terminals, &map_terminals);
            for(size_type i = 0; i < log.size(); i++){
                if(log[i]<0){
                    split_helper.add_split_at(i);
                }else{
                    if(map_terminals.find(log[i]) == map_terminals.end()){
                        terminals.push_back(log[i]);
                        map_terminals.insert(std::pair<int32_t, u_char>(log[i], 'a'));
                    }
                }
            }
        }

        uint32_t translate_log(const std::vector<int32_t> &log, const std::vector<int32_t> &terminals,
                                int* &log_repair){

            std::map<int32_t, int32_t> map_terminals;
            for(int32_t i = 0; i < terminals.size(); i++){
                map_terminals.insert(std::pair<int32_t, int32_t>(terminals[i], i));

            }

            std::map<int32_t, int32_t>::iterator it_map_terminals;
            for(size_type i = 0; i < log.size(); i++){
                it_map_terminals = map_terminals.find(log[i]);
                if(it_map_terminals != map_terminals.end()){
                    log_repair[i] = it_map_terminals->second;
                }else{
                    std::cout << "error1" << std::endl;
                    exit(20);
                }
            }

            uint32_t zero_value = 0;
            it_map_terminals = map_terminals.find(0);
            if(it_map_terminals != map_terminals.end()){
                zero_value = (uint32_t) it_map_terminals->second;
            }else{
                std::cout << "error2" << std::endl;
                exit(20);
            }
            return zero_value;
        }

        int rev_translate(const int value_to_translate, const int alpha, const int last, std::vector<int32_t> &terminals){
            if(value_to_translate >= alpha){
                return last + (value_to_translate-alpha+1);
            }else{
                return terminals[value_to_translate];
            }
        }

        uint32_t reverse_translation( repair &m_repair_algo, int zero, std::vector<int32_t> &terminals){
            auto last = terminals[terminals.size()-1];
            int idx = 0;
            for(uint i = 0; i < m_repair_algo.lenC; ++i){
                if(m_repair_algo.c[i] >= zero){
                    m_repair_algo.c[idx] = rev_translate(m_repair_algo.c[i], m_repair_algo.alpha, last, terminals);
                    ++idx;
                }
            }
            m_repair_algo.lenC = idx;
            for(uint i = 0; i < m_repair_algo.lenR; ++i){
                m_repair_algo.rules[i].left = rev_translate(m_repair_algo.rules[i].left, m_repair_algo.alpha, last, terminals);
                m_repair_algo.rules[i].right = rev_translate(m_repair_algo.rules[i].right, m_repair_algo.alpha, last, terminals);
            }
            m_repair_algo.alpha =  last+1;

        }

        void get_extra_info_rule(int i, const repair &m_repair_algo, size_type &t,
                value_type &min_value, value_type &max_value) {
            while (i >= m_alpha) {
                get_extra_info_rule(m_repair_algo.rules[i-m_alpha].left, m_repair_algo,
                                     t, min_value, max_value);
                i =  m_repair_algo.rules[i-m_alpha].right;
            }
            //Leaves
            if(i < m_alpha){
                //Increase time
                t++;
                //Compute minimum and maximum
                if(min_value > i){
                    min_value = i;
                }
                if(max_value < i){
                    max_value = i;
                }
            }
        }

        void add_extra_info_rules(const repair &m_repair_algo){

            std::vector<size_type > extra_info_time(m_repair_algo.lenR);
            std::vector<value_type > extra_info_pos(m_repair_algo.lenR * 2);
            m_rules.resize(m_repair_algo.lenR*2);
            for(uint32_t i = 0; i<m_repair_algo.lenR; i++){
                size_type t = 0;
                value_type min_value = INT32_MAX, max_value = 0;
                int32_t rule = i +  m_alpha;
                m_rules[2*i]= (int32_t) m_repair_algo.rules[i].left;
                m_rules[2*i+1] = (int32_t) m_repair_algo.rules[i].right;
                get_extra_info_rule(rule, m_repair_algo, t, min_value, max_value);
                extra_info_time[i] = t;
                extra_info_pos[2*i] = min_value;
                extra_info_pos[2*i+1] = max_value;
            }
            m_extra_info_length = dac_vector_dp_v2<>(extra_info_time);
            m_extra_info_values = dac_vector_dp_v2<>(extra_info_pos);
        }

        void construction(const std::vector<value_type> &log){
            m_last_t = log.size()-1;
            int32_t max_value = 0;
            std::vector<int32_t> aux_log, terminals;
            std::map<int32_t, char> map_terminals;
            for(size_type i = 0; i < log.size(); ++i){
                aux_log.push_back(log[i]);
                if(log[i] > max_value) max_value = log[i];
            }
            add_splits(aux_log, terminals, map_terminals);
            size_type length_log = aux_log.size();
            int* log_repair = new int[length_log];
            std::cout << "Numbers before compression: " << length_log << " numbers" << std::endl;
            sort(terminals.begin(), terminals.end());
            auto m_zero_value = translate_log(aux_log, terminals, log_repair);
            aux_log.clear();
            //3. We apply repair over the log
            repair m_repair_algo;
            int total_MB = ::util::memory::total_memory_megabytes() * 0.8;
            std::cout << "Memory to use: " << total_MB << "MB" << std::endl;
            m_repair_algo.run(log_repair, length_log, total_MB);
            reverse_translation(m_repair_algo, m_zero_value, terminals);
            std::cout << "Last in c: " << m_repair_algo.c[m_repair_algo.lenC-1] << std::endl;
            m_alpha = m_repair_algo.alpha;
            //6. We add extra information to the rules
            add_extra_info_rules(m_repair_algo);
            m_values = std::vector<value_type>(m_repair_algo.lenC);
            for(auto i = 0; i < m_values.size(); ++i){
                m_values[i] = m_repair_algo.c[i];
            }

            std::cout << "M_alpha: " << m_alpha << std::endl;
        }

        inline value_type left_rule(const value_type val){
            auto r_i = val - m_alpha;
            return m_rules[r_i*2];
        }

        inline value_type right_rule(const value_type val){
            auto r_i = val - m_alpha;
            return m_rules[r_i*2+1];
        }

        size_type length(value_type val){
            if(val < m_alpha){
                return 1;
            }else{
                return m_extra_info_length[val-m_alpha];
            }
        }

        std::pair<value_type, value_type> extremes(value_type val){
            if(val < m_alpha){
                return {val, val};
            }else{
                auto r_i = val - m_alpha;
                return {m_extra_info_values[2*r_i], m_extra_info_values[2*r_i+1]};
            }
        }

    public:

        void decompress_entry(value_type val, std::vector<value_type> &result){
            while(val >= m_alpha ){
                size_type cmin, cmax;
                std::tie(cmin, cmax) = extremes(val);
                if(cmin == cmax) {
                    std::fill_n(std::back_inserter(result), length(val), cmin);
                    return;
                }
                decompress_entry(left_rule(val), result);
                val = right_rule(val);
            }
            result.push_back(val);
        }
    private:
        void decompress_beg(value_type val, size_type t_b, size_type t_e, size_type t_i, size_type t_j, std::vector<value_type> &result){
            if(val >= m_alpha) {
                size_type cmin, cmax;
                std::tie(cmin, cmax) = extremes(val);
                if(cmin == cmax){
                    auto b = std::max(t_b, t_i);
                    auto e = std::min(t_e, t_j);
                    std::fill_n(std::back_inserter(result), e-b+1, cmin);
                }else{
                    auto l = left_rule(val);
                    auto len_l = length(l);
                    auto mid = t_b + len_l -1;
                    if (mid >= t_i) {
                        decompress_beg(l, t_b, mid, t_i, t_j, result);
                    }
                    if(mid+1 <= t_j){
                        decompress_beg(right_rule(val), mid+1, t_e, t_i, t_j, result);
                    }
                }
            }else if( t_i <= t_b && t_b <= t_j){
                result.push_back(val);
            }
        }

        void decompress_end(value_type val, size_type t_b, size_type t_e, size_type t_i, size_type t_j, std::vector<value_type> &result){
            if(val >= m_alpha) {
                size_type cmin, cmax;
                std::tie(cmin, cmax) = extremes(val);
                if(cmin == cmax) {
                    auto b = std::max(t_b, t_i);
                    auto e = std::min(t_e, t_j);
                    std::fill_n(std::back_inserter(result), e - b + 1, cmin);
                }else{
                    auto l = left_rule(val);
                    auto len_l = length(l);
                    auto mid = t_b + len_l -1;
                    if(mid >= t_i){
                        decompress_end(l, t_b, mid, t_i, t_j, result);
                    }
                    if(mid+1 <= t_j){
                        decompress_end(right_rule(val), t_b+len_l, t_e, t_i, t_j, result);
                    }
                }

            }else if(t_i <= t_b && t_b <= t_j){
                result.push_back(val);
            }
        }

        void update_extremes_beg(value_type val, size_type t_b, size_type t_e, size_type t_i, size_type t_j, value_type &min, value_type &max){
            if(val >= m_alpha) {
                size_type cmin, cmax;
                std::tie(cmin, cmax) = extremes(val);
                if (cmin > min && cmax < max) return;
                auto l = left_rule(val);
                auto len_l = length(l);
                auto mid = t_b + len_l - 1;
                if (mid >= t_i) {
                    update_extremes_beg(l, t_b, mid, t_i, t_j, min, max);
                }
                if (mid + 1 <= t_j){
                    update_extremes_beg(right_rule(val), mid + 1, t_e, t_i, t_j, min, max);
                }
            }else if(t_i <= t_b && t_b <= t_j){
                if(val < min) min = val;
                if(val > max) max = val;
            }
        }

        void update_extremes_end(value_type val, size_type t_b, size_type t_e, size_type t_i, size_type t_j, value_type &min, value_type &max){
            if(val >= m_alpha) {
                size_type cmin, cmax;
                std::tie(cmin, cmax) = extremes(val);
                if(cmin > min && cmax < max) return;
                auto l = left_rule(val);
                auto len_l = length(l);
                auto mid = t_b + len_l - 1;
                if (mid >= t_i) {
                    update_extremes_end(l, t_b, mid, t_i, t_j, min, max);
                }
                if (mid + 1 <= t_j){
                    update_extremes_end(right_rule(val), mid + 1, t_e, t_i, t_j, min, max);
                }
            }else if(t_i <= t_b && t_b <= t_j){
                if(val < min) min = val;
                if(val > max) max = val;
            }
        }



    public:

        repair_sampling(){};

        repair_sampling(const std::string &file){
            std::vector<value_type> log;
            ::util::file::read_from_file(file, log);
            construction(log);
        }

        repair_sampling(const std::vector<value_type> &log){
           construction(log);
        }





        std::vector<value_type> decompress(){
            std::vector<value_type> result;
            auto i = 0;
            for(const auto &c : m_values){
                decompress_entry(c, result);
                ++i;
            }
            return result;
        }

        //! Assignment move operation
        repair_sampling& operator=(repair_sampling&& p) {
            if (this != &p) {
                m_values = std::move(p.m_values);
                m_rules = std::move(p.m_rules);
                m_samples = std::move(p.m_samples);
                m_extra_info_length = std::move(p.m_extra_info_length);
                m_extra_info_values = std::move(p.m_extra_info_values);
                m_alpha = std::move(p.m_alpha);
                m_last_t = std::move(p.m_last_t);
            }
            return *this;
        }

        //! Assignment operator
        repair_sampling& operator=(const repair_sampling& p)
        {
            if (this != &p) {
                copy(p);
            }
            return *this;
        }

        //! Copy constructor
        repair_sampling(const repair_sampling& p)
        {
            copy(p);
        }

        //! Move constructor
        repair_sampling(repair_sampling&& p)
        {
            *this = std::move(p);
        }

        //! Swap method
        /*! Swaps the content of the two data structure.
         *  You have to use set_vector to adjust the supported bit_vector.
         *  \param bp_support Object which is swapped.
         */
        void swap(repair_sampling& p)
        {
            // m_bp.swap(bp_support.m_bp); use set_vector to set the supported bit_vector
            std::swap(m_values, p.m_values);
            std::swap(m_rules, p.m_rules);
            std::swap(m_samples, p.m_samples);
            m_extra_info_length.swap(p.m_extra_info_length);
            m_extra_info_values.swap(p.m_extra_info_values);
            std::swap(m_alpha, p.m_alpha);
            std::swap(m_last_t, p.m_last_t);

        }

        //! Serializes the snapshot to a stream.
        /*!
         * \param out The outstream to which the data structure is written.
         * \return The number of bytes written to out.
         */
        uint64_t serialize(std::ostream& out, sdsl::structure_tree_node* v=nullptr, std::string name="")const
        {
            sdsl::structure_tree_node* child = sdsl::structure_tree::add_child(v, name, sdsl::util::class_name(*this));
            uint64_t written_bytes = 0;
            written_bytes += sdsl::write_member(m_alpha, out, child, "alpha");
            written_bytes += sdsl::write_member(m_last_t, out, child, "last_t");
            written_bytes += sdsl::write_member(m_values.size(), out, child, "values_size");
            written_bytes += sdsl::serialize_vector(m_values, out, child, "values");
            written_bytes += sdsl::write_member(m_samples.size(), out, child, "samples_size");
            written_bytes += sdsl::serialize_vector(m_samples, out, child, "samples");
            written_bytes += sdsl::write_member(m_rules.size(), out, child, "rules_size");
            written_bytes += sdsl::serialize_vector(m_rules, out, child, "rules");
            written_bytes += m_extra_info_length.serialize(out, child, "extra_info_length");
            written_bytes += m_extra_info_values.serialize(out, child, "extra_info_values");
            //written_bytes += sdsl::serialize_vector(m_log_object, out, child, "log_objects");
            sdsl::structure_tree::add_size(child, written_bytes);
            return written_bytes;
        }

        void load(std::istream &in){
            uint64_t m_rules_size = 0, m_values_size=0, m_samples_size = 0;
            sdsl::read_member(m_alpha, in);
            sdsl::read_member(m_last_t, in);
            sdsl::read_member(m_values_size, in);
            m_values.resize(m_values_size);
            sdsl::load_vector(m_values, in);
            sdsl::read_member(m_samples_size, in);
            m_samples.resize(m_samples_size);
            sdsl::load_vector(m_samples, in);
            sdsl::read_member(m_rules_size, in);
            m_rules.resize(m_rules_size);
            sdsl::load_vector(m_rules, in);
            m_extra_info_length.load(in);
            m_extra_info_values.load(in);
        }



    };
}

#endif //NAME_PROJECT_REPAIR_SAMPLING_HPP
