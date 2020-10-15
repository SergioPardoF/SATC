//
// Created by adrian on 26/12/17.
//

#ifndef CDS_SPLIT_REPAIR_HELPER_HPP
#define CDS_SPLIT_REPAIR_HELPER_HPP

#include <vector>
#include <map>

namespace cds {

    template <class sig_value = int64_t, class usig_value = uint64_t >
    class split_repair_helper{

    private:
        std::map<sig_value, usig_value> m_prev_map;
        std::map<sig_value, usig_value> m_succ_map;
        std::vector<sig_value>* m_repair_string;
        std::vector<sig_value>* m_terminals;
        std::map<sig_value, char>* m_map_terminals;
        typedef typename std::map<sig_value, usig_value>::iterator it_type;

    public:
        split_repair_helper(std::vector<sig_value>* repair_string, std::vector<sig_value>* terminals,
                            std::map<sig_value, char>* map_terminals){
            m_repair_string = repair_string;
            m_terminals = terminals;
            m_map_terminals = map_terminals;

        };

        void add_split_at(usig_value i){

            if(m_repair_string->size() == 0) return;

            bool exist_prev = (i > 0);
            bool exist_succ = (i < m_repair_string->size()-1);
            it_type it_prev = m_prev_map.end(), it_succ = m_succ_map.end();
            usig_value prev_split = 1, succ_split = 1;
            sig_value prev_value, succ_value;
            if(exist_prev){
                prev_value = m_repair_string->at(i-1);
                if((it_prev = m_prev_map.find(prev_value)) != m_prev_map.end()){
                    prev_split = it_prev->second + 1;
                }
            }
            if(exist_succ){
                succ_value = m_repair_string->at(i+1);
                if((it_succ = m_succ_map.find(succ_value)) != m_succ_map.end()){
                    succ_split = it_succ->second + 1;
                }
            }
            usig_value max = std::max(prev_split, succ_split);
            if(exist_prev){
                if(it_prev != m_prev_map.end()){
                    m_prev_map.erase(it_prev);
                }
                m_prev_map.insert(std::pair<sig_value, usig_value >(prev_value, max));

            }
            if(exist_succ){
                if(it_succ != m_succ_map.end()){
                    m_succ_map.erase(it_succ);
                }
                m_succ_map.insert(std::pair<sig_value, usig_value >(succ_value, max));
            }
            m_repair_string->at(i) = -max;
            if(m_map_terminals->find(-max) == m_map_terminals->end()){
                m_terminals->push_back(-max);
                m_map_terminals->insert(std::pair<sig_value, char>(-max, 'a'));
            }

        }
    };
}
#endif //CDS_SPLIT_REPAIR_HELPER_HPP
