#include <stdio.h>
#include <stdlib.h>

#include "pico/stdlib.h"
#include "pico/time.h"
#include "hardware/gpio.h"
#include "hardware/adc.h"

#include "fixed_point.hpp"
#include "fp_math.hpp"
#include "st7735.hpp"

#include "textures.hpp"

inline constexpr uint8_t SCREEN_WIDTH = 160;
inline constexpr uint8_t SCREEN_HEIGHT = 128;
inline constexpr uint8_t worldMap[10][10] = {
                                            {1,1,1,1,1,1,1,1,1,1},
                                            {1,0,0,0,0,0,0,0,0,1},
                                            {1,0,0,0,0,0,0,0,0,1},
                                            {1,0,0,0,0,0,0,0,0,1},
                                            {1,0,0,0,0,0,0,0,0,1},
                                            {1,0,0,0,0,1,1,1,0,1},
                                            {1,0,0,0,0,0,1,0,0,3},
                                            {1,0,0,0,0,2,2,0,0,3},
                                            {1,0,0,0,0,0,0,0,0,3},
                                            {1,1,1,1,2,2,2,3,3,3}};

inline constexpr uint8_t J_VRX_PIN = 28, J_VRY_PIN = 27;


inline constexpr Fixed15_16 MOVE_STEP = 0.05_fp;
inline constexpr uint32_t INPUT_DELAY = 15000;

inline constexpr Fixed15_16 rosin = sinfp(2); // sin(2 degrees)
inline constexpr Fixed15_16 rocos = cosfp(2); // cos(2 degrees)
inline constexpr Fixed15_16 FOV_SCALE = 0.66667_fp;


int main()
{
    stdio_init_all();
    adc_init();


    adc_gpio_init(J_VRX_PIN);
    adc_gpio_init(J_VRY_PIN);


    ST7735 tft(1, spi0, 18, 19, 17, 21, 20, 255);
    tft.initialize(ST7735::TFT_Type::GREEN_TAB);

    // validate texture data
    if (!TextureManager::isValid()) {
        printf("ERROR Texture data invalid!\n");
        tft.drawFillScreen(0xF800); // red screen

        while (true) tight_loop_contents(); // halt
    }

    // int worldMap[24][24]=
    // {
    //     {4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,7,7,7,7,7,7,7,7},
    //     {4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,7,0,0,0,0,0,0,7},
    //     {4,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,7},
    //     {4,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,7},
    //     {4,0,3,0,0,0,0,0,0,0,0,0,0,0,0,0,7,0,0,0,0,0,0,7},
    //     {4,0,4,0,0,0,0,5,5,5,5,5,5,5,5,5,7,7,0,7,7,7,7,7},
    //     {4,0,5,0,0,0,0,5,0,5,0,5,0,5,0,5,7,0,0,0,7,7,7,1},
    //     {4,0,6,0,0,0,0,5,0,0,0,0,0,0,0,5,7,0,0,0,0,0,0,8},
    //     {4,0,7,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,7,7,7,1},
    //     {4,0,8,0,0,0,0,5,0,0,0,0,0,0,0,5,7,0,0,0,0,0,0,8},
    //     {4,0,0,0,0,0,0,5,0,0,0,0,0,0,0,5,7,0,0,0,7,7,7,1},
    //     {4,0,0,0,0,0,0,5,5,5,5,0,5,5,5,5,7,7,7,7,7,7,7,1},
    //     {6,6,6,6,6,6,6,6,6,6,6,0,6,6,6,6,6,6,6,6,6,6,6,6},
    //     {8,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,4},
    //     {6,6,6,6,6,6,0,6,6,6,6,0,6,6,6,6,6,6,6,6,6,6,6,6},
    //     {4,4,4,4,4,4,0,4,4,4,6,0,6,2,2,2,2,2,2,2,3,3,3,3},
    //     {4,0,0,0,0,0,0,0,0,4,6,0,6,2,0,0,0,0,0,2,0,0,0,2},
    //     {4,0,0,0,0,0,0,0,0,0,0,0,6,2,0,0,5,0,0,2,0,0,0,2},
    //     {4,0,0,0,0,0,0,0,0,4,6,0,6,2,0,0,0,0,0,2,2,0,2,2},
    //     {4,0,6,0,6,0,0,0,0,4,6,0,0,0,0,0,5,0,0,0,0,0,0,2},
    //     {4,0,0,5,0,0,0,0,0,4,6,0,6,2,0,0,0,0,0,2,2,0,2,2},
    //     {4,0,6,0,6,0,0,0,0,4,6,0,6,2,0,0,5,0,0,2,0,0,0,2},
    //     {4,0,0,0,0,0,0,0,0,4,6,0,6,2,0,0,0,0,0,2,0,0,0,2},
    //     {4,4,4,4,4,4,4,4,4,4,1,1,1,2,2,2,2,2,2,3,3,3,3,3}
    // };

    Fixed15_16 pos_x = 6_fp;
    Fixed15_16 pos_y = 2_fp;
    
    // movement & rotation

    absolute_time_t last_move_time = 0;

    Fixed15_16 dir_x = -1_fp; 
    Fixed15_16 dir_y; 
    
    Fixed15_16 plane_x; 
    Fixed15_16 plane_y = 0.66666667_fp; 

    // current raycast screen coordinate
    uint8_t current_screen_x = 0;

    while (true) {
        uint64_t math_start, math_end;
        math_start = time_us_64();

        Fixed15_16 camera_x = (2 * Fixed15_16(current_screen_x) / Fixed15_16(SCREEN_WIDTH)) - 1;

        Fixed15_16 ray_dir_x = dir_x + (plane_x * camera_x);
        Fixed15_16 ray_dir_y = dir_y + (plane_y * camera_x);

        int16_t map_x = pos_x.toInt();
        int16_t map_y = pos_y.toInt();

        // DDA setup
        
        Fixed15_16 delta_dist_x = (ray_dir_x == 0) ? Fixed15_16::fromRaw(INT32_MAX) : abs(1 / ray_dir_x);
        Fixed15_16 delta_dist_y = (ray_dir_y == 0) ? Fixed15_16::fromRaw(INT32_MAX) : abs(1 / ray_dir_y);


        Fixed15_16 side_dist_x;
        Fixed15_16 side_dist_y;
        
        // dda step direction
        int8_t step_x;
        int8_t step_y;

        uint8_t hit = 0;
        uint8_t side;

        // sidedist is the distance to get to an int coordinate on the map after which we will start DDA with deltadist in step direction
        if (ray_dir_x < 0) {
            step_x = -1;
            side_dist_x = (pos_x - map_x) * delta_dist_x;
        } else {
            step_x = 1;
            side_dist_x = (map_x + 1 - pos_x) * delta_dist_x;
        }
        if (ray_dir_y < 0) {
            step_y = -1;
            side_dist_y = (pos_y - map_y) * delta_dist_y;
        } else {
            step_y = 1;
            side_dist_y = (map_y + 1 - pos_y) * delta_dist_y;
        }

        // finally start DDA loop
        while (hit == 0) {
            if (side_dist_x < side_dist_y) {
                side_dist_x += delta_dist_x;
                map_x += step_x;
                side = 0;
            } else {
                side_dist_y += delta_dist_y;
                map_y += step_y;
                side = 1;
            }

            if (worldMap[map_x][map_y] > 0) {
                hit = 1;
            }
        }

        Fixed15_16 wall_dist; // perpendicular distance to wall

        // this is same as calculating ((map_x - pos_x + (1 - step_x) / 2) / ray_dir_x) but can be simplified due to scaling of sidedist and deltadist by raydir magnitude
        if (side == 0) {
            wall_dist = side_dist_x - delta_dist_x;
        } else {
            wall_dist = side_dist_y - delta_dist_y;
        }

        // ---- drawing the actual raycaster line ----

        // this is larger than the actual line drawn so that textures close up to walls can be scaled properly.
        int16_t line_height = (SCREEN_HEIGHT / wall_dist).toInt();

        int16_t draw_start = (-line_height >> 1) + (SCREEN_HEIGHT >> 1);
        if (draw_start < 0) draw_start = 0; 

        int16_t draw_end = (line_height >> 1) + (SCREEN_HEIGHT >> 1);
        if (draw_end >= SCREEN_HEIGHT) draw_end = SCREEN_HEIGHT - 1;
        
        // exact position where wall was hit
        Fixed15_16 wall_x; 
        if (side == 0) {
            wall_x = pos_y + wall_dist * ray_dir_y;
        } else {
            wall_x = pos_x + wall_dist * ray_dir_x;
        }
        // normalize this position to [0,1]
        wall_x = fractional(wall_x);
        

        int16_t tex_x_coord = (wall_x << TEX_LOG2_SIZE).toInt();
        
        // mirror texture coordinate if needed
        if ((side == 0 && ray_dir_x > 0) || (side == 1 && ray_dir_y < 0)) {
            tex_x_coord = TEX_SIZE - tex_x_coord - 1;
        } 
        
        // step through texture for each screen pixel
        Fixed15_16 step = TEX_SIZE_FP / Fixed15_16(line_height);
        
        int16_t wall_top_coord = (SCREEN_HEIGHT - line_height) >> 1;
        
        // starting texture coordinate
        Fixed15_16 tex_pos = (draw_start - wall_top_coord) * step;
        
        // texture number, -1 to account for 0 indexing
        uint8_t tex_num = worldMap[map_x][map_y] - 1; 

        // buffer for the texture from this ray column -- init to the color that we want the background to be.
        uint16_t ray_column[SCREEN_HEIGHT] = {0};

        // pointer to the column of the texture we are sampling from
        // since textures are stored column major for cache efficiency
        // const uint16_t* tex_column = &texture_map[tex_num][tex_x_coord * TEX_SIZE];
        const uint16_t* tex_data_start = TextureManager::getTextureData(tex_num);
        const uint16_t* tex_column = &tex_data_start[tex_x_coord * TEX_SIZE];

        for (size_t y = draw_start; y < draw_end; y++) {
            int16_t tex_y_coord = tex_pos.toInt() & TEX_MASK;
            
            tex_pos += step;

            uint16_t color = tex_column[tex_y_coord];
            
            // make y-sides darker CHANGE THIS TO MAKE IT SO THAT TEXTURES THAT ARE DARKER ARE JUST A DIFFERENT TEXTURE
            // if (side == 1) {
                // simple bitmasking to darken color (5-6-5 format)
                // color = (color >> 1) & 0x39EF;
            // }

            ray_column[y] = color;
        }

        math_end = time_us_64();
        printf("Math calc time: %dus\n", (uint32_t)(math_end - math_start));

        uint64_t gfx_start, gfx_end;
        gfx_start = time_us_64();

        tft.drawRayColumnn(current_screen_x, ray_column, SCREEN_HEIGHT);

        gfx_end = time_us_64();
        printf("GFX draw time: %dus\n", (uint32_t)(gfx_end - gfx_start));



        current_screen_x++;
        if (current_screen_x >= SCREEN_WIDTH) {
            current_screen_x = 0;
        }
    
        adc_select_input(2); // VRX
        uint16_t vrx_reading = adc_read();
        adc_select_input(1); // VRY
        uint16_t vry_reading = adc_read();
        // printf("VRX: %d, VRY: %d\n", vrx_reading, vry_reading);

        if (get_absolute_time() - last_move_time > INPUT_DELAY) {
            last_move_time = get_absolute_time();
    
            if (vry_reading < 1000) {
                if (worldMap[(pos_x + dir_x * 10 * MOVE_STEP).toInt()][pos_y.toInt()] == 0) {
                    pos_x += dir_x * MOVE_STEP;
                }
                if (worldMap[pos_x.toInt()][(pos_y + dir_y * 10 * MOVE_STEP).toInt()] == 0) {
                    pos_y += dir_y * MOVE_STEP;
                }
    
            } else if (vry_reading > 3000) {
                if (worldMap[(pos_x - dir_x * 10 * MOVE_STEP).toInt()][pos_y.toInt()] == 0) {
                    pos_x -= dir_x * MOVE_STEP;
                }
                if (worldMap[pos_x.toInt()][(pos_y - dir_y * 10 * MOVE_STEP).toInt()] == 0) {
                    pos_y -= dir_y * MOVE_STEP;
                }
            }
            
            if (vrx_reading > 3000) {
                Fixed15_16 oldDirX = dir_x;
                Fixed15_16 oldDirY = dir_y;
    
                dir_x = oldDirX * rocos - oldDirY * rosin;
                dir_y = oldDirX * rosin + oldDirY * rocos;
    
                // recalibrate camera plane to account for drift (there shouldnt be a significant drift but just in case)
                plane_x = -dir_y * FOV_SCALE;
                plane_y = dir_x * FOV_SCALE;
    
            } else if (vrx_reading < 1000) {
                Fixed15_16 oldDirX = dir_x;
                Fixed15_16 oldDirY = dir_y;
    
                dir_x = oldDirX * rocos + oldDirY * rosin;
                dir_y = -oldDirX * rosin + oldDirY * rocos;
    
                // recalibrate camera plane to account for drift (there shouldnt be a significant drift but just in case)
                plane_x = -dir_y * FOV_SCALE;
                plane_y = dir_x * FOV_SCALE;
            }
        }
    }
}
