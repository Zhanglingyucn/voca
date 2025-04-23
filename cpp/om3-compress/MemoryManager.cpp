#include "MemoryManager.h"
#include <unistd.h>
#include <sys/stat.h> // for stat
bool fileExists(const std::string& filePath) {
    struct stat buffer;
    return (stat(filePath.c_str(), &buffer) == 0);
  }

// Helper function to create directory
bool create_directory(const std::string& path) {
    // Using system command to create directory
    std::string cmd = "mkdir -p " + path;
    return system(cmd.c_str()) == 0;
}

// Helper function to check if directory exists
bool directory_exists(const std::string& path) {
    struct stat buffer;
    return (stat(path.c_str(), &buffer) == 0 && S_ISDIR(buffer.st_mode));
}

// Helper function to get filename without extension
std::string get_filename_without_extension(const std::string& path) {
    size_t last_slash = path.find_last_of("/\\");
    size_t last_dot = path.find_last_of(".");
    
    if (last_dot == std::string::npos || 
        (last_slash != std::string::npos && last_dot < last_slash)) {
        return path.substr(last_slash + 1);
    }
    
    if (last_slash == std::string::npos) {
        return path.substr(0, last_dot);
    }
    
    return path.substr(last_slash + 1, last_dot - last_slash - 1);
}

/**
 * @brief Encodes CSV data using MinMax hierarchical compression
 * 
 * This function:
 * 1. Reads CSV file using rapidcsv
 * 2. Processes each column (except first 't' column)
 * 3. Applies MinMax encoding to each column
 * 4. Saves encoded data to binary files
 * 5. Creates metadata file with column/row counts
 *
 * File Structure:
 * - ../om3_data/                    (directory)
 *   |- {data_name}_om3.bin         (metadata file)
 *   |- {data_name}_om3_v{id}.bin   (encoded data)
 *   |- {data_name}_om3_v{id}.flagz (order flags)
 *
 * @param path_string Path to input CSV file
 * @return true if encoding and saving succeeds, false on any error
 * @throws std::runtime_error If first column is not named 't'
 */
bool MemoryManager::minmax_encode(const std::string &csv_file_path)
{
        char cwd[500];
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
      std::cout << "当前路径: " << cwd << std::endl;
    } else {
      std::cerr << "获取当前路径失败" << std::endl;
      return 1;
    }
    // 也可以直接使用相对路径，前提是程序运行的目录是期望的目录
    if (fileExists(csv_file_path)) {
        std::cout << "文件存在 (相对路径): " << csv_file_path << std::endl;
      } else {
        std::cout << "文件不存在 (相对路径): " << csv_file_path << std::endl;
      }
    rapidcsv::Document doc(csv_file_path);
    std::vector<std::string> headers;
    MemoryManager::print_csv_info(doc, headers); // Print CSV info

    std::string data_name = get_filename_without_extension(csv_file_path);

    // Check if the path exists, create if it doesn't
    try {
        if (!directory_exists("../om3_data/")) {
            if (!create_directory("../om3_data/")) {
                std::cerr << "Error: Failed to create data directory." << std::endl;
                return false;
            }
            std::cout << "Created directory: ../om3_data/" << std::endl;
        }
    } catch (const std::exception& e) {
        std::cerr << "Error creating directory: " << e.what() << std::endl;
        return false;
    }

    std::string path_string = "../om3_data/"+data_name + "_om3.bin";
    std::ofstream outfile(path_string, std::ios::binary | std::ios::trunc);
    if (!outfile) {
        std::cerr << "Error: Cannot open file for writing: " << data_name << std::endl;
        return false;
    }

    if(!(headers[0] == "t" || headers[0] == "\"t\"" || headers[0] == "\'t\"")){
        throw std::runtime_error("Error: First column is not t");
    }

    for (size_t i = 1; i < headers.size(); ++i) {
        std::vector<double> values = doc.GetColumn<double>(headers[i]); // 直接获取整列数据

        auto result = VariableManager::minmax_encode(values);
        // VariableManager::print_encoded_result(result);
        VariableManager::save_minmax_data(data_name, i, result);
    }

    size_t data_size = doc.GetRowCount();
    size_t col_count = headers.size()-1;
    outfile.write(reinterpret_cast<const char*>(&col_count), sizeof(col_count));
    outfile.write(reinterpret_cast<const char*>(&data_size), sizeof(data_size));

    outfile.close();
    std::cout << "Successfully wrote col/row to " << path_string << std::endl;
    return outfile.good(); // Check if any errors occurred during writing/closing
}

std::pair<CoeffPair, CoeffPair> MemoryManager::minmax_decode_index(double agg_min, double agg_max, size_t index, size_t variable_id)
{
    return variables[variable_id].minmax_decode_index(agg_min, agg_max, index, this->original_level);
}

/**
 * @brief Loads MinMax encoded data from binary files
 * 
 * This function:
 * 1. Reads metadata file to get column count and data size
 * 2. Loads each variable's encoded data
 * 3. Stores loaded variables in member vector
 *
 * File Structure:
 * - Metadata (.bin): 
 *   - Column count (size_t)
 *   - Data size (size_t)
 * - Variable data: Loaded through VariableManager::load_minmax_data
 *
 * @return true if all data loaded successfully, false on any error
 * 
 * @note Sets data_size to 0 on read failure
 * @note Reserves space in variables vector before loading
 * @note Performs integrity checks on loaded data
 */
bool MemoryManager::load_minmax_data()
{
    // char cwd[500];
    // if (getcwd(cwd, sizeof(cwd)) != NULL) {
    //   std::cout << "当前路径: " << cwd << std::endl;
    // } else {
    //   std::cerr << "获取当前路径失败" << std::endl;
    //   return 1;
    // }
    std::string path_string = "../om3_data/" + this->data_name + "_om3.bin";
    // 也可以直接使用相对路径，前提是程序运行的目录是期望的目录
    // if (fileExists(path_string)) {
    //     std::cout << "文件存在 (相对路径): " << path_string << std::endl;
    //   } else {
    //     std::cout << "文件不存在 (相对路径): " << path_string << std::endl;
    //   }
    std::ifstream infile(path_string, std::ios::binary);
    if (!infile)
    {
        std::cerr << "Error: Cannot open file for reading: " << path_string << std::endl;
        this->data_size = 0; return false;
    }
    
    size_t col_count;
    // 1. Read original variable num & data size
    infile.read(reinterpret_cast<char*>(&col_count), sizeof(col_count));
    variables.reserve(col_count);
    if (!infile) { this->data_size = 0; return false; } // Basic check
    infile.read(reinterpret_cast<char*>(&this->data_size), sizeof(this->data_size));
    if (!infile) { this->data_size = 0; return false; } // Basic check
    this->original_level = BitScanReverse(this->data_size);

    // 2. Read each variable's data
    for (size_t i = 0; i < col_count; ++i)
    {
        auto &vm=variables.emplace_back(i);
        // VariableManager vm(i);
        if (!vm.load_minmax_data("../om3_data/"+this->data_name + "_om3_v" + std::to_string(i+1), this->data_size))
        {
            std::cerr << "Error: Failed to load variable data for variable ID: " << i+1 << std::endl;
            this->data_size = 0; return false;
        }
        // vm.print_decoded_levels();
        // variables.emplace_back(std::move(vm));
    }

    // Basic check if we reached EOF unexpectedly or read failed
    if (infile.peek() != EOF && !infile.good())
    {
        std::cerr << "Warning: File read might be incomplete or corrupted: " << path_string << std::endl;
        this->data_size = 0; return false;
    }

    return true;
}

void MemoryManager::print_variables_status() const {
    std::cout << "\n=== Memory Manager Status ===" << std::endl;
    std::cout << "Data Name: " << data_name << std::endl;
    std::cout << "Data Size: " << data_size << std::endl;
    std::cout << "Number of Variables: " << variables.size() << std::endl;
    
    // Print status for each variable
    for (const auto& var : variables) {
        var.print_details();
    }
}

void MemoryManager::print_csv_info(const rapidcsv::Document& doc, std::vector<std::string>& out_headers) {
    std::cout << "\n=== CSV File Information ===" << std::endl;
    
    // Get and print headers
    out_headers = doc.GetColumnNames();
    std::cout << "Headers (" << out_headers.size() << "): ";
    for (const auto& h : out_headers) std::cout << h << " ";
    std::cout << std::endl;
    
    // Print row count
    size_t row_count = doc.GetRowCount();
    std::cout << "Number of Rows: " << row_count << std::endl;
    
    // Print first few rows as preview
    // std::cout << "\nData Preview (first 5 rows):" << std::endl;
    // for (size_t i = 0; i < std::min(row_count, size_t(5)); ++i) {
    //     for (const auto& h : headers) {
    //         std::cout << doc.GetCell<double>(h, i) << "\t";
    //     }
    //     std::cout << std::endl;
    // }
}

/**
 * @brief Performs MinMax hierarchical encoding without padding.
 *
 * This function implements a hierarchical compression scheme that:
 * 1. Creates base level pairs from input data
 * 2. Iteratively merges pairs upward, storing detail coefficients
 * 3. Records comparison order at bottom level
 * 4. Produces a final top-level aggregate
 *
 * Algorithm Steps:
 * 1. Initialize: Convert input values to (min,max) pairs
 * 2. Iterate until single pair remains:
 *    - Merge adjacent pairs
 *    - Store detail coefficients (D^{j-1}_{2i}, D^{j-1}_{2i+1})
 *    - Record comparison order at bottom level
 * 3. Reverse detail coefficients for top-down order
 *
 * @param input_data Vector of input data points to encode
 *
 * @return Tuple containing:
 *         - first: Top-level aggregate pair (global min, max)
 *         - second: Min detail coefficients by level (top-down)
 *         - third: Max detail coefficients by level (top-down)
 *         - fourth: Bottom level comparison order flags
 *
 * @throws std::runtime_error If final aggregation fails
 *
 * @note Detail coefficients are stored in top-down order
 * @note Empty input returns ({0,0}, {}, {}, {})
 * @note Single value input returns ({val,val}, {}, {}, {})
 *
 * @example
 *   std::vector<double> data = {1.0, 2.0, 3.0, 4.0, 5.0};
 *   auto [top_agg, min_details, max_details, order] = minmax_encode(data);
 */
std::tuple<CoeffPair, CoeffArr, CoeffArr, OrderArr>
VariableManager::minmax_encode(const std::vector<double>& input_data) {
    if (input_data.empty()) {
        // Return default/empty values
        return {{0.0, 0.0}, {}, {}, {}};
    }

    if (input_data.size() == 1) {
         // Special case: single data point is the global min/max
         // No detail coefficients are generated.
        return {{input_data[0], input_data[0]}, {}, {}, {}};
    }

    // 1. Initialization: Create base level aggregate pairs
    std::vector<CoeffPair> current_aggregates;
    current_aggregates.reserve(input_data.size());
    for (double val : input_data) {
        current_aggregates.push_back({val, val});
    }

    // Store all detail coefficients level by level, bottom-up initially.
    CoeffArr all_min_details_bottom_up;
    CoeffArr all_max_details_bottom_up;
    OrderArr bottom_order;

    // 2. Iterative Computation: From level j down to level 1 (calculating j-1)
    while (current_aggregates.size() > 1) {
        size_t current_level_pair_count = current_aggregates.size();
        size_t num_pairs_to_process = current_level_pair_count / 2;
        size_t next_level_pair_count = (current_level_pair_count + 1) / 2;
        bool is_bottom_level = (current_level_pair_count == input_data.size());
        size_t order_num = nextPowerOfTwo(current_level_pair_count)/2;

        std::vector<CoeffPair> next_aggregates(next_level_pair_count);
        std::vector<double> current_level_min_details; // For D^{j-1}_{2i}
        std::vector<double> current_level_max_details; // For D^{j-1}_{2i+1}
        current_level_min_details.reserve(num_pairs_to_process);
        current_level_max_details.reserve(num_pairs_to_process);
        OrderArr current_level_min_order(order_num, false); // For min_order

        for (size_t i = 0; i < num_pairs_to_process; ++i)
        {
            size_t left_pair_idx = 2 * i;
            size_t right_pair_idx = 2 * i + 1;

            // Use const& to avoid unnecessary copies if CoeffPair is complex,
            // but since it's std::pair<double, double>, value might be fine too.
            // Stick with const& for consistency with the original intent.
            const auto &left_pair = current_aggregates[left_pair_idx];
            const auto &right_pair = current_aggregates[right_pair_idx];

            // Calculate next level aggregate pair
            double next_agg_min = std::min(left_pair.first, right_pair.first);
            double next_agg_max = std::max(left_pair.second, right_pair.second);
            next_aggregates[i] = {next_agg_min, next_agg_max};

            if (is_bottom_level)
            {
                current_level_min_order[i] = left_pair.first < right_pair.first;
            }
            else
            {
                // Calculate detail coefficients (using subtraction as in the target code)
                double detail_min = left_pair.first - right_pair.first;   // D^{j-1}_{2i}
                double detail_max = left_pair.second - right_pair.second; // D^{j-1}_{2i+1}
                current_level_min_details.push_back(detail_min);
                current_level_max_details.push_back(detail_max);
            }
        }

        // Handle odd number of pairs in the current level
        if (current_level_pair_count % 2 != 0) {
            next_aggregates[next_level_pair_count - 1] = current_aggregates.back();
            // Note: The original code didn't explicitly handle order for the odd element.
            // This version maintains that behavior for bottom_order.
        }

        // Store the details calculated for this level (which corresponds to level j-1)
        // We store them bottom-up first.
        // Discard the details generated from the initial base level (level l)
        if (!current_level_min_details.empty()) // If num_pairs_to_process > 0
        {
            // std::cout << "current level detail size: " << current_level_min_details.size() << std::endl;
            // This is not the bottom level, store the details
            all_min_details_bottom_up.push_back(std::move(current_level_min_details));
            all_max_details_bottom_up.push_back(std::move(current_level_max_details));
        }
        else if(is_bottom_level)
        {
            bottom_order = std::move(current_level_min_order);
        }

        // Move to the next level
        current_aggregates = std::move(next_aggregates);
    }

    // 3. Final Result Preparation
    if (current_aggregates.size() != 1) {
         throw std::runtime_error("Internal error: Final aggregates size is not 1 pair.");
    }

    CoeffPair top_level_aggregate = current_aggregates[0];

    // Reverse the order of levels in details to be top-down
    std::reverse(all_min_details_bottom_up.begin(), all_min_details_bottom_up.end());
    std::reverse(all_max_details_bottom_up.begin(), all_max_details_bottom_up.end());

    return {top_level_aggregate, std::move(all_min_details_bottom_up), std::move(all_max_details_bottom_up), std::move(bottom_order)};
}

CoeffPair VariableManager::minmax_decode_pair(double agg_parent, double detail, bool is_min)
{
    double child_left, child_right;

    if(is_min) {
        // --- Decode the 'min' components (first elements) ---
        if (detail < 0) { // D < 0
            child_left = agg_parent;             // A^j_{4i} = A
            child_right = agg_parent - detail; // A^j_{4i+2} = A - D
        } else { // D >= 0
            child_right = agg_parent;            // A^j_{4i+2} = A
            child_left = agg_parent + detail; // A^j_{4i} = A + D
        }
    } else {
        // --- Decode the 'max' components (second elements) ---
        if (detail < 0) { // D < 0
            child_right = agg_parent;             // A^j_{4i+2} = A
            child_left = agg_parent + detail; // A^j_{4i} = A + D
        } else { // D >= 0
            child_left = agg_parent;            // A^j_{4i} = A
            child_right = agg_parent - detail; // A^j_{4i+2} = A - D
        }
    }

    return {child_left, child_right};
}

std::pair<CoeffPair, CoeffPair>
VariableManager::minmax_decode_index(double agg_min, double agg_max, size_t index, size_t original_level)
{
    if(index == 0) throw std::runtime_error("Error: index 0 is not valid for decoding");

    size_t level = BitScanReverse(index);
    size_t level_index = index - (1 << level);
    if(level > original_level) {
        throw std::runtime_error("index out of range ("+std::to_string(index)+")");
    } else if(level == original_level) {
        // is the last level
        CoeffPair last_pair = this->bottom_order[level_index] ? 
                                std::make_pair(agg_min, agg_max) :
                                std::make_pair(agg_max, agg_min);
        return {last_pair, last_pair};
    } else {
        if(index < L1_SIZE) {
            // L1 cache
            return {{L1_min[2*index], L1_max[2*index]}, {L1_min[2*index+1], L1_max[2*index+1]}};
        } else if(index < L2_SIZE) {
            // L2 cache
            CoeffPair min_pair = minmax_decode_pair(agg_min, L2_min[index-L1_SIZE], true);
            CoeffPair max_pair = minmax_decode_pair(agg_max, L2_max[index-L1_SIZE], false);
            return {{min_pair.first, max_pair.first}, {min_pair.second, max_pair.second}};
        } else {
            // L3 cache
            #ifdef COMPRESS_ALP
            auto compressed_data = L3_min[level-L2_LEVEL];
            CoeffPair min_pair = minmax_decode_pair(agg_min, get_decoded_value(compressed_data, level_index), true);
            compressed_data = L3_max[level-L2_LEVEL];
            CoeffPair max_pair = minmax_decode_pair(agg_max, get_decoded_value(compressed_data, level_index), false);
            #else
            CoeffPair min_pair = minmax_decode_pair(agg_min, L3_min[level-L2_LEVEL][level_index], true);
            CoeffPair max_pair = minmax_decode_pair(agg_max, L3_max[level-L2_LEVEL][level_index], false);
            #endif
            return {{min_pair.first, max_pair.first}, {min_pair.second, max_pair.second}};
        }
    }
}

/**
 * @brief Saves MinMax encoded data to binary files.
 *
 * This function writes the MinMax encoded data to two files:
 * 1. A .bin file containing coefficient data
 * 2. A .flagz file containing order flags
 *
 * File Format (.bin):
 * 1. top_aggregate.first (double)
 * 2. top_aggregate.second (double)
 * 3. Min detail coefficients for each level (doubles)
 * 4. Max detail coefficients for each level (doubles)
 *
 * @param data_name Base name for the output files
 * @param id Variable identifier (appended to filename)
 * @param data Tuple containing:
 *            - first: Top level aggregate pair (global min, max)
 *            - second: Min detail coefficients by level
 *            - third: Max detail coefficients by level
 *            - fourth: Bottom level order flags
 * @return true if data was successfully written, false on any error
 *
 * @note Files are created in ../om3_data/ directory
 * @note Order flags are written to a separate .flagz file
 * @note All files are written in binary mode
 */
bool VariableManager::save_minmax_data(const std::string& data_name, const size_t id, const std::tuple<CoeffPair, CoeffArr, CoeffArr, OrderArr>& data) {
    std::string path_string = "../om3_data/" + data_name + "_om3_v" + std::to_string(id);
    std::ofstream outfile(path_string + ".bin", std::ios::binary | std::ios::trunc);
    if (!outfile) {
        std::cerr << "Error: Cannot open file for writing: " << data_name << std::endl;
        return false;
    }

    const auto& top_aggregate = std::get<0>(data);
    const auto& min_details = std::get<1>(data);
    const auto& max_details = std::get<2>(data);
    const auto& bottom_order = std::get<3>(data);

    // std::cout << "\n=== Encoding Result ===" << std::endl;
    // std::cout << "Top Aggregate: {" << top_aggregate.first << ", " 
    //           << top_aggregate.second << "}" << std::endl;

    // // Print detail coefficients by level
    // for (size_t i = 0; i < min_details.size(); ++i) {
    //     std::cout << "Level " << i << " Details:" << std::endl;
    //     std::cout << "  Min: ";
    //     for (double d : min_details[i]) std::cout << d << " ";
    //     std::cout << "\n  Max: ";
    //     for (double d : max_details[i]) std::cout << d << " ";
    //     std::cout << std::endl;
    // }

    // // Print bottom order flags
    // std::cout << "Bottom Order: ";
    // for (bool b : bottom_order) std::cout << (b ? "1" : "0") << " ";
    // std::cout << std::endl;

    // 2. Save top aggregate
    outfile.write(reinterpret_cast<const char*>(&top_aggregate.first), sizeof(double));
    outfile.write(reinterpret_cast<const char*>(&top_aggregate.second), sizeof(double));

    // 3. Save Min Details
    size_t num_min_levels = min_details.size();
    for (const auto& level_details : min_details) {
        outfile.write(reinterpret_cast<const char*>(level_details.data()), level_details.size() * sizeof(double));
    }

    // 4. Save Max Details
    size_t num_max_levels = max_details.size();
    for (const auto& level_details : max_details) {
        outfile.write(reinterpret_cast<const char*>(level_details.data()), level_details.size() * sizeof(double));
    }

    // 5. Bottom order boolean vector
    VariableManager::save_order_data(path_string, bottom_order);
    
    outfile.close();
    std::cout << "Successfully wrote minmax data to " << path_string << ".bin" << std::endl;
    return outfile.good(); // Check if any errors occurred during writing/closing
}

/**
 * @brief Calculates the size of each level reducing from n down to 1.
 *
 * Non-powers of 2 are conceptually padded to the next power of 2
 * to determine the number of levels, but the size only counts the
 * original 'n' elements spread across those levels using floor division.
 * The reduction continues until the padded size reaches 1.
 *
 * @param n A positive integer representing the initial size
 * @return A vector of integers representing the effective size of each level
 * @throws std::invalid_argument If n is not a positive integer
 *
 * @example
 * calculate_level_sizes(9)  // returns {4, 2, 1, 1}
 * calculate_level_sizes(10) // returns {5, 2, 1, 1}
 * calculate_level_sizes(16) // returns {8, 4, 2, 1}
 * calculate_level_sizes(18) // returns {9, 4, 2, 1, 1}
 * calculate_level_sizes(28) // returns {14, 7, 3, 2, 1}
 */
std::vector<size_t> calculate_level_sizes(size_t n) {
    if (n < 1) {
        throw std::invalid_argument("Input 'n' must be a positive integer.");
    }
    if (n == 1) {
        return {1};
    }
    
    std::vector<size_t> level_sizes;
    level_sizes.reserve(32);  // Pre-allocate space for reasonable depth

    size_t current_size = n;
    while (current_size > 1) {
        size_t num_pairs = current_size / 2;  // 实际合并的对数
        level_sizes.push_back(num_pairs);     // details 的数量
        current_size = (current_size + 1) / 2;  // 下一层的元素数量
    }

    return level_sizes;
}

/**
 * @brief Loads MinMax encoded data from a binary file.
 *
 * This function reads the MinMax encoded data previously saved by save_minmax_data().
 * The data is read in the following order:
 * 1. Top level aggregate pair (global min, global max)
 * 2. Min detail coefficients for each level
 * 3. Max detail coefficients for each level
 * 4. Bottom level comparison order flags
 *
 * File Format:
 * 1. top_aggregate.first (double)
 * 2. top_aggregate.second (double)
 * 3. For each level (determined by calculate_level_sizes):
 *    a. min detail coefficients (num_coeffs_in_level * double)
 *    b. max detail coefficients (num_coeffs_in_level * double)
 * 4. Bottom order flags (in separate .flagz file)
 *
 * @param path_string Base path for the data files (without extension)
 * @param original_data_size Original size of the data before encoding
 * @return true if the data was successfully loaded, false otherwise
 *
 * @note The function expects both a .bin file for coefficient data and 
 *       a .flagz file for order flags at the specified path
 * @note The function verifies data integrity through EOF checks and read operations
 */
bool VariableManager::load_minmax_data(const std::string& path_string, const size_t original_data_size) {
    std::ifstream infile(path_string + ".bin", std::ios::binary);
    if (!infile) {
        std::cerr << "Error: Cannot open file for reading: " << path_string + ".bin" << std::endl;
        return false;
    }

    CoeffPair top_aggregate;
    CoeffArr min_details;
    CoeffArr max_details;

    // 1. Read top aggregate
    infile.read(reinterpret_cast<char*>(&top_aggregate.first), sizeof(double));
    if (!infile) { return false; }
    infile.read(reinterpret_cast<char*>(&top_aggregate.second), sizeof(double));
    if (!infile) { return false; }

    // 2. Calculate size of each level
    size_t bottom_details_num = original_data_size % 2 == 0 ? original_data_size / 2 : original_data_size / 2 + 1;
    std::vector<size_t> level_sizes = calculate_level_sizes(bottom_details_num);
    std::reverse(level_sizes.begin(), level_sizes.end()); // Reverse to match top-down order
    
    // 3. Read Min Details
    min_details.resize(level_sizes.size()); // Pre-allocate levels
    for (size_t i = 0; i < level_sizes.size(); ++i) {
        size_t num_coeffs_in_level = level_sizes[i];
        min_details[i].resize(num_coeffs_in_level);
        infile.read(reinterpret_cast<char*>(min_details[i].data()), num_coeffs_in_level * sizeof(double));
        if (!infile) { return false; }
    }

    // 4. Read Max Details
    max_details.resize(level_sizes.size()); // Pre-allocate levels
    for (size_t i = 0; i < level_sizes.size(); ++i) {
        size_t num_coeffs_in_level = level_sizes[i];
        max_details[i].resize(num_coeffs_in_level);
        infile.read(reinterpret_cast<char*>(max_details[i].data()), num_coeffs_in_level * sizeof(double));
        if (!infile) { return false; }
    }

    // 5. Bottom order boolean vector
    this->load_order_data(path_string, this->bottom_order);

    // Basic check if we reached EOF unexpectedly or read failed
    if (infile.peek() != EOF && !infile.good()) {
        std::cerr << "Warning: File read might be incomplete or corrupted: " << path_string << std::endl;
        return false;
    }

    build_multilevel_cache(top_aggregate, min_details, true);
    build_multilevel_cache(top_aggregate, max_details, false);
    
    return true;
}

void VariableManager::build_multilevel_cache(const CoeffPair& top_aggregate, const CoeffArr &input_data, bool is_min)
{
    auto& target_L1 = is_min ? this->L1_min : this->L1_max;
    auto& target_L2 = is_min ? this->L2_min : this->L2_max;
    auto& target_L3 = is_min ? this->L3_min : this->L3_max;
    target_L1[1] = is_min ? top_aggregate.first : top_aggregate.second;

    size_t offset = 0, i=0, current_aggregates_num=1;
    for(; (i < L1_LEVEL) && (i<input_data.size()); offset+=(1<<i), ++i) {
        auto& details = input_data[i];
        size_t next_aggregates_num=0;
        for(size_t j = 0; j < current_aggregates_num; ++j) {
            size_t idx = offset+j+1;
            if(j < details.size()) {
                CoeffPair children = minmax_decode_pair(target_L1[idx], details[j], is_min);
                target_L1[2*idx] = children.first;
                target_L1[2*idx+1] = children.second;
                next_aggregates_num += 2;
            } else {
                // std::cout << "Carrying over unpaired element:" << target_L1[idx] << std::endl;
                next_aggregates_num++;
                target_L1[2*idx] = target_L1[idx];
            }
        }
        current_aggregates_num = next_aggregates_num;
    }
    size_t L2_offest = offset; // L1_SIZE-1
    for(; (i < L2_LEVEL) && (i<input_data.size()); offset+=(1<<i), ++i) {
        for(size_t j = 0; j < input_data[i].size(); ++j) {
            target_L2[offset+j-L2_offest] = input_data[i][j];
        }
    }
    for(; (i<input_data.size()); ++i) {
        #ifdef COMPRESS_ALP
        // compressed detail coefficients
        target_L3.push_back(tree_level_encode(input_data[i]));
        std::cout << "compression ratio at Level (" << std::to_string(i) << ", " + std::string(is_min?"min":"max") + "):"
                    << (1.0-target_L3.back()->calc_compression_ratio()/64.0)*100 << '%' << std::endl;
        #else
        target_L3.emplace_back(input_data[i]);
        #endif
    }
}


/**
 * @brief Saves boolean order flags to a binary file
 *
 * This function writes an array of boolean flags to a .flagz file,
 * where each bool is stored as two identical bytes for OM3 compatibility.
 *
 * File Format (.flagz):
 * - Each bool is stored as two consecutive identical bytes:
 *   - true  -> {1,1}
 *   - false -> {0,0}
 *
 * @param path_string Base path for the output file (without extension)
 * @param data Vector of boolean flags to write
 * @return true if data was successfully written, false on any error
 *
 * @note Writes to {path_string}.flagz in binary mode
 * @note Each bool is written twice for OM3 compatibility
 */
bool VariableManager::save_order_data(const std::string &path_string, const OrderArr &data)
{
    std::ofstream outfile(path_string+".flagz", std::ios::binary);
    if (!outfile.is_open()) {
        std::cerr << "Error opening file: " << path_string << ".flagz" << std::endl;
        return false;
    }

    for (bool b : data) {
        char byte = b ? 1 : 0; // Convert bool to a byte (1 or 0)
        // using two times to compatible with om3
        outfile.write(&byte, 1);  
        outfile.write(&byte, 1);
    }

    outfile.close();
    std::cout << "Successfully wrote bool vector to " << path_string << ".flagz" << std::endl;
    return outfile.good();
}

/**
 * @brief Loads boolean order flags from a binary file
 *
 * This function reads an array of boolean flags from a .flagz file,
 * where each bool was stored as two identical bytes.
 *
 * File Format (.flagz):
 * - File size must be even (2 bytes per bool)
 * - Each bool is stored as two consecutive bytes:
 *   - {1,1} -> true
 *   - {0,0} -> false
 *
 * @param path_string Base path for the input file (without extension)
 * @param[out] data Vector to store the loaded boolean flags
 * @return true if data was successfully loaded, false on any error
 *
 * @note Reads from {path_string}.flagz in binary mode
 * @note Resizes output vector based on file size
 * @note Only first byte of each pair is used for bool value
 */
bool VariableManager::load_order_data(const std::string &path_string, OrderArr &data) {
    std::ifstream infile(path_string + ".flagz", std::ios::binary);
    if (!infile.is_open()) {
        std::cerr << "Error opening file: " << path_string << ".flagz" << std::endl;
        return false;
    }

    // Get file size
    infile.seekg(0, std::ios::end);
    std::streamsize file_size = infile.tellg();
    infile.seekg(0, std::ios::beg);

    // Each bool is stored as two bytes in the file
    size_t num_bools = file_size / 2;
    data.resize(num_bools);

    // Read bool values
    for (size_t i = 0; i < num_bools; ++i) {
        char byte1, byte2;
        infile.read(&byte1, 1);
        infile.read(&byte2, 1);
        data[i] = (byte1 != 0);
    }

    infile.close();
    return infile.good();
}

void VariableManager::print_details() const {
    std::cout << "\n=== Variable " << variable_id << " Details ===" << std::endl;
    
    // Print L1 cache info
    std::cout << "L1 Cache Status:" << std::endl;
    std::cout << "MinMax Pairs: " << L1_min.size()-1 << "/" << L1_SIZE_TIMES2 << " elements" << std::endl;
    
    // Print L2 cache info
    std::cout << "L2 Cache Status:" << std::endl;
    std::cout << "  OM3 coefficients: " << L2_min.size() << "/" << (L2_SIZE-L1_SIZE) << " elements" << std::endl;
    
    // Print detail coefficients info
    std::cout << "L3 Cache Status:" << std::endl;
    std::cout << "  Compressed Levels: " << L3_min.size()  << " Levels" << std::endl;
    
    // Print bottom order info
    std::cout << "Bottom Order Flags: " << bottom_order.size() << " flags" << std::endl;
}

void VariableManager::print_encoded_result(
    const std::tuple<CoeffPair, CoeffArr, CoeffArr, OrderArr>& result) {
    
    const auto& top_aggregate = std::get<0>(result);
    const auto& min_details = std::get<1>(result);
    const auto& max_details = std::get<2>(result);
    const auto& order = std::get<3>(result);

    std::cout << "\n=== Encoding Result ===" << std::endl;
    std::cout << "Top Aggregate: {" << top_aggregate.first << ", " 
              << top_aggregate.second << "}" << std::endl;

    // Print detail coefficients by level
    for (size_t i = 0; i < min_details.size(); ++i) {
        std::cout << "Level " << i << " Details:" << std::endl;
        std::cout << "  Min: ";
        for (double d : min_details[i]) std::cout << d << " ";
        std::cout << "\n  Max: ";
        for (double d : max_details[i]) std::cout << d << " ";
        std::cout << std::endl;
    }

    // Print bottom order flags
    std::cout << "Bottom Order: ";
    for (bool b : order) std::cout << (b ? "1" : "0") << " ";
    std::cout << std::endl;
}

void VariableManager::print_decoded_levels() const {
    std::cout << "\n=== Decoding Comparison ===" << std::endl;
    
    // Print L1 cache
    std::cout << "L1 Cache:" << std::endl;
    std::cout << "  Min: ";
    for (size_t i = 0; i < std::min(L1_min.size(), (size_t)20); ++i)
        std::cout << L1_min[i] << " ";
    std::cout << "\n  Max: ";
    for (size_t i = 0; i < std::min(L1_max.size(), (size_t)20); ++i)
        std::cout << L1_max[i] << " ";
    std::cout << std::endl;

    // Print L2 cache
    std::cout << "L2 Cache:" << std::endl;
    std::cout << "  Min: ";
    for (size_t i = 0; i < std::min(L2_min.size(), (size_t)20); ++i)
        std::cout << L2_min[i] << " ";
    std::cout << "\n  Max: ";
    for (size_t i = 0; i < std::min(L2_min.size(), (size_t)20); ++i)
        std::cout << L2_max[i] << " ";
    std::cout << std::endl;
}