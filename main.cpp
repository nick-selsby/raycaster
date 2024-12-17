#include "main.h"

#include "map.hpp"

#include <numbers>

const int MAP_WIDTH = 20;
const int MAP_HEIGHT = 20;
const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;

const int RAY_EVERY_N_PIXELS = 2;

const int worldMap[MAP_WIDTH][MAP_HEIGHT] = {
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,1,1,1,1,1,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,1,0,0,0,1,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,1,0,0,0,1,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,1,0,0,0,1,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,1,1,0,1,1,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}
};

Map map;

class Player {
    double pos_x, pos_y;
    double dir_x, dir_y;
    double plane_x, plane_y;

public:
    Player(double x, double y, double dirX, double dirY, double planeX, double planeY)
        : pos_x(x), pos_y(y), dir_x(dirX), dir_y(dirY), plane_x(planeX), plane_y(planeY)
    {}

    void handle_input(const Uint8* keys, double move_speed, double rot_speed) {
        if (keys[SDL_SCANCODE_UP] || keys[SDL_SCANCODE_W]) {
            if (worldMap[int(pos_x + dir_x * move_speed)][int(pos_y)] == 0) 
                pos_x += dir_x * move_speed;
            if (worldMap[int(pos_x)][int(pos_y + dir_y * move_speed)] == 0) 
                pos_y += dir_y * move_speed;
        }
        if (keys[SDL_SCANCODE_DOWN] || keys[SDL_SCANCODE_S]) {
            if (worldMap[int(pos_x - dir_x * move_speed)][int(pos_y)] == 0) 
                pos_x -= dir_x * move_speed;
            if (worldMap[int(pos_x)][int(pos_y - dir_y * move_speed)] == 0) 
                pos_y -= dir_y * move_speed;
        }
        if (keys[SDL_SCANCODE_RIGHT] || keys[SDL_SCANCODE_D]) {
            double old_dir_x = dir_x;
            dir_x = dir_x * cos(rot_speed) - dir_y * sin(rot_speed);
            dir_y = old_dir_x * sin(rot_speed) + dir_y * cos(rot_speed);
            double old_plane_x = plane_x;
            plane_x = plane_x * cos(rot_speed) - plane_y * sin(rot_speed);
            plane_y = old_plane_x * sin(rot_speed) + plane_y * cos(rot_speed);
        }
        if (keys[SDL_SCANCODE_LEFT] || keys[SDL_SCANCODE_A]) {
            double old_dir_x = dir_x;
            dir_x = dir_x * cos(-rot_speed) - dir_y * sin(-rot_speed);
            dir_y = old_dir_x * sin(-rot_speed) + dir_y * cos(-rot_speed);
            double old_plane_x = plane_x;
            plane_x = plane_x * cos(-rot_speed) - plane_y * sin(-rot_speed);
            plane_y = old_plane_x * sin(-rot_speed) + plane_y * cos(-rot_speed);
        }
    }

    void cast_ray(SDL_Renderer* renderer, int x) const {
        double camera_x = 2 * x / double(SCREEN_WIDTH) - 1;
        double ray_dir_x = dir_x + plane_x * camera_x;
        double ray_dir_y = dir_y + plane_y * camera_x;
        float ray_dir_length = std::sqrt(ray_dir_x * ray_dir_x + ray_dir_y * ray_dir_y);

        RayCastResult result;
        map.cast(Vector2(pos_y, pos_x), Vector2(-ray_dir_y, -ray_dir_x), result);

        if (result.hit == true) {
            double perp_wall_dist = result.distance / ray_dir_length;

            int line_height = (int)(SCREEN_HEIGHT / perp_wall_dist);
            int draw_start = -line_height / 2 + SCREEN_HEIGHT / 2;
            if (draw_start < 0) draw_start = 0;
            int draw_end = line_height / 2 + SCREEN_HEIGHT / 2;
            if (draw_end >= SCREEN_HEIGHT) draw_end = SCREEN_HEIGHT - 1;

            float r = result.line->color.r;
            float g = result.line->color.g;
            float b = result.line->color.b;

            float brightness = 1.0f;

            float min_dist = 0.0f;
            float max_dist = 30.0f;
            float dist_b = (perp_wall_dist - min_dist) / (max_dist - min_dist);
            dist_b = 1.0f - std::sqrt(std::clamp(dist_b, 0.0f, 1.0f));
            brightness *= dist_b;

            r *= brightness; g *= brightness; b *= brightness;

            // draw walls
            SDL_SetRenderDrawColor(renderer,
                uint8_t(r * 255.0f),        // r
                uint8_t(g * 255.0f),        // g
                uint8_t(b * 255.0f),        // b
                255);                       // a

            SDL_Rect current_rect = {x - RAY_EVERY_N_PIXELS / 2, draw_start, RAY_EVERY_N_PIXELS, draw_end - draw_start};
            SDL_RenderFillRect(renderer, &current_rect);
        }
    }
    
};


void draw_ceiling_and_floor(SDL_Renderer* renderer) {
    // draw ceiling with gradient
    for (int y = 0; y < SCREEN_HEIGHT / 2; ++y) {
        float t = float(y) / (SCREEN_HEIGHT / 2);
        uint8_t r = uint8_t(100 * (1 - t));
        uint8_t g = uint8_t(100 * (1 - t));
        uint8_t b = uint8_t(255 * (1 - t));
        SDL_SetRenderDrawColor(renderer, r, g, b, 255);
        SDL_RenderDrawLine(renderer, 0, y, SCREEN_WIDTH, y);
    }

    // draw floor with gradient
    for (int y = SCREEN_HEIGHT / 2; y < SCREEN_HEIGHT; ++y) {
        float t = float(y - SCREEN_HEIGHT / 2) / (SCREEN_HEIGHT / 2);
        uint8_t r = uint8_t(0 * - t);
        uint8_t g = uint8_t(0 * - t);
        uint8_t b = uint8_t(255 * t);
        SDL_SetRenderDrawColor(renderer, r, g, b, 255);
        SDL_RenderDrawLine(renderer, 0, y, SCREEN_WIDTH, y);
    }
}


bool init(SDL_Window** window, SDL_Renderer** renderer) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        return false;
    }

    *window = SDL_CreateWindow(
        "raycaster",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        SCREEN_WIDTH,
        SCREEN_HEIGHT,
        SDL_WINDOW_SHOWN
    );

    if (*window == NULL) {
        SDL_Quit();
        return false;
    }

    *renderer = SDL_CreateRenderer(*window, -1, SDL_RENDERER_ACCELERATED);
    if (*renderer == NULL) {
        SDL_DestroyWindow(*window);
        SDL_Quit();
        return false;
    }

    map.add_tiles(MAP_WIDTH, MAP_HEIGHT, &worldMap[0][0]);
    map.add_line_segment({ 2.0f, 1.0f }, { 1.0f, 2.0f }, AQUA_BLUE);
    const int POINTS = 4;
    for (int i = 0; i < POINTS; i++) {
        float cx = 13.0f;
        float cy = 13.0f;
        float r = 1.0f;
        float p1x = cx + r * std::cos((i / (float)POINTS) * std::numbers::pi * 2);
        float p1y = cy + r * std::sin((i / (float)POINTS) * std::numbers::pi * 2);
        float p2x = cx + r * std::cos(((i + 1) / (float)POINTS) * std::numbers::pi * 2);
        float p2y = cy + r * std::sin(((i + 1) / (float)POINTS) * std::numbers::pi * 2);
        map.add_line_segment({ p1x, p1y }, { p2x, p2y }, WHITE);
    }

    return true;
}

int main(int argc, char* argv[]) {
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;
    
    if (!init(&window, &renderer)) {
        return 1;
    }

    Player player = {2.5, 1.5, 1.0, 0.0, 0.0, 0.66};
    Uint64 last_time = SDL_GetTicks();

    const int FPS = 60;
    const int FRAME_DELAY = 1000 / FPS;

    bool running = true;
    SDL_Event event;
    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            }        
        }

        Uint64 current_time = SDL_GetTicks64();
        double frame_time = (current_time - last_time) / 1000.0;
        last_time = current_time;
        
        const Uint8* keys = SDL_GetKeyboardState(NULL);
        double move_speed = frame_time * 5.0;
        double rot_speed = frame_time * 3.0;
        player.handle_input(keys, move_speed, rot_speed);

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        draw_ceiling_and_floor(renderer);
        for(int x = 0; x < SCREEN_WIDTH; x = x + RAY_EVERY_N_PIXELS) {
            player.cast_ray(renderer, x);
        }

        SDL_RenderPresent(renderer);

        // limit framerate
        Uint64 frame_time_ms = SDL_GetTicks64() - current_time;
        if (frame_time_ms < FRAME_DELAY) {
            //std::cout << frame_time_ms << "|" << FRAME_DELAY << std::endl;
            SDL_Delay(FRAME_DELAY - frame_time_ms);
        }
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    
    return 0;
}
