#include "MemoryManager.h"

int main(int argc, char* argv[]) {
    try {
        if (argc > 1) {
            std::cout << "First Argument: " << argv[1] << std::endl;
        }
        Timing::timestart("minmax_encode");
        MemoryManager::minmax_encode("../../../data/tvdata/"+std::string(argv[1])+".csv");
        Timing::timeend("minmax_encode");
        // MemoryManager mm(argv[1]);
        // Timing::timestart("load_minmax_data");
        // mm.load_minmax_data();
        // Timing::timeend("load_minmax_data");
        // mm.print_variables_status();
        // std::vector<double> min_list = {-2.0, -7.0, -7.0, -6.0, 4.0};
        // std::vector<double> max_list = {7.0, 9.0, 7.0, 8.0, 7.0};
        // std::vector<size_t> index_list = {10, 1, 2, 6, 40};
        // for(size_t i=0; i<min_list.size(); ++i) {
        //     auto agg_pair = mm.minmax_decode_index(min_list[i], max_list[i], index_list[i], 0);
        //     std::cout << "Index: " << index_list[i] << "; Left Child:" << agg_pair.first.first << ", " << agg_pair.first.second 
        //                 << "; Right Child:" << agg_pair.second.first << ", " << agg_pair.second.second << std::endl;
        // }
        // size_t data_size = 4733280;
        // size_t data_size = 56;
        // size_t original_level = BitScanReverse(data_size);
        // VariableManager vm(0);
        // Timing::timestart("load_minmax_data");
        // vm.load_minmax_data("../om3_data/"+std::string(argv[1])+ "_om3_v1", data_size);
        // Timing::timeend("load_minmax_data");
        // vm.print_decoded_levels();
        // // auto agg_pair = vm.minmax_decode_index(-2.0, 7.0, 10, original_level); // expect {1.0, 7.0}, {-2.0, 6.0}
        // // auto agg_pair = vm.minmax_decode_index(-7.0, 9.0, 1, original_level); // expect {-7.0, 7.0}, {-6.0, 9.0}
        // // auto agg_pair = vm.minmax_decode_index(-7.0, 7.0, 2, original_level); // expect {-7.0, 6.0}, {-2.0, 7.0}
        // // auto agg_pair = vm.minmax_decode_index(-6.0, 8.0, 6, original_level); // expect {1.0, 4.0}, {-6.0, 8.0}
        // // auto agg_pair = vm.minmax_decode_index(4.0, 7.0, 40, original_level); // expect {7.0, 4.0}, {7.0, 4.0}
        // std::vector<double> min_list = {-2.0, -7.0, -7.0, -6.0, 4.0};
        // std::vector<double> max_list = {7.0, 9.0, 7.0, 8.0, 7.0};
        // std::vector<size_t> index_list = {10, 1, 2, 6, 40};
        // for(size_t i=0; i<min_list.size(); ++i) {
        //     auto agg_pair = vm.minmax_decode_index(min_list[i], max_list[i], index_list[i], original_level);
        //     std::cout << "Index: " << index_list[i] << "; Left Child:" << agg_pair.first.first << ", " << agg_pair.first.second 
        //                 << "; Right Child:" << agg_pair.second.first << ", " << agg_pair.second.second << std::endl;
        // }
        // min_list = {-2.0, -7.0, -7.0, -6.0, 4.0};
        // max_list = {7.0, 9.0, 7.0, 8.0, 7.0};
        // index_list = {10, 1, 2, 6, 40};
        // for(size_t i=0; i<min_list.size(); ++i) {
        //     auto agg_pair = vm.minmax_decode_index(min_list[i], max_list[i], index_list[i], original_level);
        //     std::cout << "Index: " << index_list[i] << "; Left Child:" << agg_pair.first.first << ", " << agg_pair.first.second 
        //                 << "; Right Child:" << agg_pair.second.first << ", " << agg_pair.second.second << std::endl;
        // }
        // for(size_t i=0; i<1000000; ++i) {
        //     Timing::timestart("direct_access");
        //     auto agg_pair = mm.minmax_decode_index(-8.0, 20, 1024, 0);
        //     Timing::timeend("direct_access");
        // }
        // for(size_t i=0; i<1000000; ++i) {
        //     Timing::timestart("om3_coeff_access");
        //     auto agg_pair = mm.minmax_decode_index(-8.0, 20, 10000, 0);
        //     Timing::timeend("om3_coeff_access");
        // }
        // for(size_t i=0; i<1000000; ++i) {
        //     Timing::timestart("uncompressed_access");
        //     auto agg_pair = mm.minmax_decode_index(-8.0, 20, 1048576, 0);
        //     Timing::timeend("uncompressed_access");
        // }
    } 
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    Timing::timetotal("minmax_encode");
    Timing::timetotal("load_minmax_data");
    Timing::timetotal("direct_access");
    Timing::timetotal("om3_coeff_access");
    Timing::timetotal("uncompressed_access");
    return 0;
}