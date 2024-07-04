//
// Created by sergio on 3/11/23.
//

#include <repair_sampling.hpp>
#include "BPlusTree.h"

#define EDATE "2012-12-31 00:00:00" // Earliest trip starting date

// Future arguments
#define ID 10982//8695
#define SDATE 1275960//1240920
#define FDATE 1276047//1241464
// pos in c for 1st travel is 212588 in csv line 170142
// pos in c dor 2nd travel is 360731 in csv line 285157

using namespace cds;

// Returns a time_t with the seconds since the epoc from a string date
std::time_t convertStringToTimePoint(const std::string& dateString) {

    std::tm tm = {};
    std::istringstream iss(dateString);
    iss >> std::get_time(&tm, "%Y-%m-%d %H:%M:%S");

    return std::mktime(&tm);
}

// Convert string date to seconds since defined EDATE
int dateToInt(const std::string dateString, std::chrono::_V2::system_clock::time_point edateTP) {

    std::time_t dateTT = convertStringToTimePoint(dateString);
    auto seconds = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::from_time_t(dateTT) - edateTP);

    return static_cast<int>(seconds.count());
}

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

// Function to manually find the intersection of two sorted vectors
std::vector<int> intersectTwoSortedVectors(const std::vector<int>& v1, const std::vector<int>& v2) {
    std::vector<int> intersection;
    size_t i = 0, j = 0;
    while (i < v1.size() && j < v2.size()) {
        if (v1[i] < v2[j]) {
            ++i;
        } else if (v1[i] > v2[j]) {
            ++j;
        } else {
            intersection.push_back(v1[i]);
            ++i;
            ++j;
        }
    }
    return intersection;
}

// Function to find the intersection of three vectors
std::vector<int> intersectThreeVectors(const std::vector<int>& v1, const std::vector<int>& v2, const std::vector<int>& v3) {
    // Copy and sort the input vectors
    std::vector<int> sorted_v1 = v1;
    std::vector<int> sorted_v2 = v2;
    std::vector<int> sorted_v3 = v3;

    std::sort(sorted_v1.begin(), sorted_v1.end());
    std::sort(sorted_v2.begin(), sorted_v2.end());
    std::sort(sorted_v3.begin(), sorted_v3.end());

    // Find the intersection of the first two sorted vectors
    std::vector<int> temp_intersection = intersectTwoSortedVectors(sorted_v1, sorted_v2);

    // Find the intersection of the temporary intersection vector with the third sorted vector
    std::vector<int> final_intersection = intersectTwoSortedVectors(temp_intersection, sorted_v3);

    return final_intersection;
}

int main(int argc, const char *argv[]) {
    if (argc < 9) {
        std::cerr << "Usage: " << argv[0] << " starting_date ending_date object_id starting_date_tree ending_date_tree "
                                             "object_id_tree date_position.csv repair_struct" << std::endl;
    }

    //time_t edateTT = convertStringToTimePoint(EDATE);
    //uint32_t starting_date = static_cast<uint32_t>(dateToInt(argv[1], std::chrono::system_clock::from_time_t(edateTT)));
    //uint32_t ending_date = static_cast<uint32_t>(dateToInt(argv[2], std::chrono::system_clock::from_time_t(edateTT)));
    //uint32_t object_id = static_cast<uint32_t>(argv[3]);

    BPlusTree<uint32_t, int> starting_date_tree(100);
    BPlusTree<uint32_t, int> ending_date_tree(100);
    BPlusTree<uint32_t, int> id_object_tree(100);

    starting_date_tree.readFromFile(argv[4]);
    ending_date_tree.readFromFile(argv[5]);
    id_object_tree.readFromFile(argv[6]);

    std::ifstream csv(argv[7]);
    if (!csv.is_open()) {
        std::cerr << "Error opening the file." << std::endl;
        return 1;
    }

    std::ifstream repair(argv[8], std::ios::binary);
    if (!repair.is_open()) {
        std::cerr << "Error opening the repair file." << std::endl;
        return 1;
    }

    std::vector<uint32_t> result_id_keys;
    std::vector<int> result_id_values;
    std::vector<uint32_t> result_sdate_keys;
    std::vector<int> result_sdate_values;
    std::vector<uint32_t> result_fdate_keys;
    std::vector<int> result_fdate_values;
    int result_id_size, result_sdate_size, result_fdate_size;

    time_t edateTT = convertStringToTimePoint(EDATE);

    result_sdate_size = starting_date_tree.range_search(atoi(argv[1])-1, atoi(argv[1])+1, result_sdate_keys, result_sdate_values);
    result_fdate_size = ending_date_tree.range_search(atoi(argv[2])-1, atoi(argv[2])+1, result_fdate_keys, result_fdate_values);
    result_id_size = id_object_tree.range_search(atoi(argv[3]), atoi(argv[3]), result_id_keys, result_id_values);

    /*if (!id_object_tree.search(ID) || !starting_date_tree.search(SDATE) || !ending_date_tree.search(FDATE)) {
        std::cout << "Keys not on trees" << std::endl;
    }*/

    std::cout << "ID vector sizes: " << result_id_size << ", SDate vector sizes: " << result_sdate_size << ", FDate vector sizes: " << result_fdate_size << std::endl;

    std::vector<int> csv_lines = intersectThreeVectors(result_id_values, result_sdate_values, result_fdate_values);

    std::cout << "Printing lines in intersection: " << std::endl;
    for (auto entry:csv_lines) {
        std::cout << entry << ", ";
    }
    std::cout << std::endl;

    if (csv_lines.size() != 1) {
        std::cerr << "Result contains more than one trip" << std::endl;
    }

    std::string line;
    /*for (int i = 0; i < csv_lines.at(0); i++) {
        if (!std::getline(csv, line)) {
            std::cerr << "Error: File has fewer than " + std::to_string(csv_lines.at(0)) + " lines." << std::endl;
            return 1;
        }
    }*/

    repair_sampling<> m_structure;
    m_structure.load(repair);

    std::vector<uint32_t> decompressed_numbers;

    if (!std::getline(csv, line) || !std::getline(csv, line)) {
        std::cerr << "Error: File has fewer than 1 line." << std::endl;
        return 1;
    }

    std::vector<std::string> vline = splitString(line, ',');
    int pos_c = std::stoi(vline[3]);
    int pos_c2 = 0;

    // Decompression
    if (csv_lines.at(0) <= 2) {
        std::cout << "Position in c to decompress is " << pos_c << std::endl;
        for (int i = 0; i < pos_c+1; i++){
            m_structure.decompress_entry(m_structure.compressed()[i], decompressed_numbers);
        }
    } else {
        for (int i = 2; i < csv_lines.at(0); i++){
            if (std::getline(csv, line)) {
                pos_c2 = pos_c;
                vline.clear();
                vline = splitString(line, ',');
                pos_c = std::stoi(vline[3]);
            }
        }
        if (pos_c == pos_c2){
            std::cout << "Position in c to decompress is " << pos_c << std::endl;
            m_structure.decompress_entry(m_structure.compressed()[pos_c], decompressed_numbers);
        } else {
            std::cout << "Positions in c to decompress are from " << pos_c2 << " to " << pos_c << std::endl;
            for(int i = pos_c2; i < pos_c+1; i++) {
                m_structure.decompress_entry(m_structure.compressed()[i], decompressed_numbers);
            }
        }
    }

    std::cout << "Size of decompressed vector: " << decompressed_numbers.size() << std::endl;
    std::cout << "Printing decompressed result: " << std::endl;
    for (auto entry:decompressed_numbers) {
        std::cout << entry << " ";
    }
    std::cout << std::endl;

    csv.close();
    repair.close();

    return 0;
}