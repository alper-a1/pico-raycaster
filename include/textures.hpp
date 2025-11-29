/**
 * @file textures.hpp
 */

#ifndef TEXTURES_H
#define TEXTURES_H

#include <cstdint>

#include "fixed_point.hpp"

// #include "raw_textures.hpp" // auto-generated texture data OLD

// texture based constants


inline constexpr uint8_t TEX_LOG2_SIZE = 6;  // log2 of texture size (64)
inline constexpr uint8_t TEX_SIZE = (1 << TEX_LOG2_SIZE);
inline constexpr Fixed15_16 TEX_SIZE_FP = Fixed15_16(TEX_SIZE);
inline constexpr uint8_t TEX_MASK = TEX_SIZE - 1; // for wrapping

struct TextureFileHeader {
    inline static constexpr uint32_t VALID_MAGIC = 0x30504958;

    uint32_t magic;
    uint32_t version;
    uint32_t tex_count;
    uint32_t reserved;
};

extern "C" {
    // defined in tools/textures.S
    // kept as byte array for pointer math
    extern const uint8_t textures_xip_blob[]; 
}

/**
 * @class TextureManager
 * @brief Manages access to textures stored in XIP memory.
 */
class TextureManager {
    public:
        /// @brief Retrieves the header of the textures data.
        static const TextureFileHeader* getHeader() {
            return reinterpret_cast<const TextureFileHeader*>(textures_xip_blob);
        }
        
        /**
         * @brief Retrieves pointer to texture data by index.
         * @param texIndex Index of the texture to retrieve.
         * @return Pointer to the texture data, or nullptr if index is out of bounds.
         */
        static const uint16_t* getTextureData(uint8_t texIndex) {
            const TextureFileHeader* const header = getHeader();
            
            // bounds check 
            if (texIndex >= header->tex_count) {
                return nullptr; 
            }
            
            // pointer to the location of the start of the offset array (after header ends)
            const uint32_t* offset_array_addr = reinterpret_cast<const uint32_t*>(textures_xip_blob + sizeof(TextureFileHeader));
    
            uint32_t offset = offset_array_addr[texIndex];

            // uint8 here because we want to move offset in bytes, not wider type
            const uint8_t* texture_start_addr = textures_xip_blob + offset;
            
            // we are at the correct position and can cast back to uint16 (color)
            return reinterpret_cast<const uint16_t*>(texture_start_addr);
        }

        /// @brief Checks if the texture data in XIP memory is valid.
        /// @note Check BEFORE attempting to access any textures! 
        static bool isValid() {
            return getHeader()->magic == TextureFileHeader::VALID_MAGIC;
        }
};


#endif // TEXTURES_H