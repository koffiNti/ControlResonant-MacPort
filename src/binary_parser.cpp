#include "nl_api_blueprint.h"
#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <algorithm>

namespace NLBridge {

    /**
     * BinaryParser: Handles extraction of JS/Data from original game containers
     */
    class BinaryParser {
    public:
        struct ExtractionResult {
            bool success;
            std::vector<uint8_t> payload;
            std::string metadata;
        };

        static ExtractionResult Extract(const std::string& filePath) {
            std::ifstream file(filePath, std::ios::binary | std::ios::ate);
            if (!file.is_open()) {
                std::cerr << "[BinaryParser] Error: Could not open file " << filePath << std::endl;
                return {false, {}, ""};
            }

            std::streamsize size = file.tellg();
            file.seekg(0, std::ios::beg);

            // 1. Read Header
            BinaryContainerHeader header;
            if (!file.read(reinterpret_cast<char*>(&header), sizeof(BinaryContainerHeader))) {
                std::cerr << "[BinaryParser] Error: Header too small in " << filePath << std::endl;
                return {false, {}, ""};
            }

            // 2. Validate Magic (assuming 0x4C554121 - 'LUA!' in little endian)
            if (header.magic != 0x4C554121) {
                std::cerr << "[BinaryParser] Warning: Unexpected magic "
                          << std::hex << header.magic << " in " << filePath << std::endl;
                // We continue anyway as version shifts might occur
            }

            // 3. Extract Payload
            if (header.data_offset + header.data_size > size) {
                std::cerr << "[BinaryParser] Error: Payload bounds exceed file size in " << filePath << std::endl;
                return {false, {}, ""};
            }

            std::vector<uint8_t> payload(header.data_size);
            file.seekg(header.data_offset, std::ios::beg);
            file.read(reinterpret_cast<char*>(payload.data()), header.data_size);

            return {true, payload, "Extracted " + std::to_string(header.data_size) + " bytes"};
        }
    };
}
