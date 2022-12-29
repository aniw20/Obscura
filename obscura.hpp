#ifndef OBSCURA_H
#define OBSCURA_H

#include <iostream>
#include <fstream>
#include <vector>
#include <math.h>
#include <filesystem>
#include <fstream>
#include <unistd.h>
#include "lodePNG.cpp"

namespace fs = std::filesystem;

float map(float v, float a1, float a2, float b1, float b2){
    return b1 + (v - a1) * ((b2 - b1) / (a2 - a1));
}

uint32_t Color(uint8_t r, uint8_t g, uint8_t b){
    uint32_t color = ((r & 0xFF) << 16) + ((g & 0xFF) << 8) + (b & 0xFF);
    return color;
};

struct Surface{
    int width;
    int height;
    int cX;
    int cY;
    uint32_t* pixels;
    Surface(int w = 0, int h = 0){
        width = w;
        height = h;
        pixels = (uint32_t *) malloc(sizeof(uint32_t) * width * height);
        for ( int y = 0; y < height; y++ ){
            for ( int x = 0; x < width; x++ ){
                pixels[y*width + x] = 0;
            }
        }
        cX = width / 2;
        cY = height / 2;
    };
    void set(int x, int y, uint32_t color){
        x = cX + x;
        y = cY + y;
        if ( y > -1 and y < height and x > -1 and x < width ){
            pixels[y * width + x] = color;
        }
    };
    void fill(uint32_t color){
        for ( int y = 0; y < height; y++ ){
            for ( int x = 0; x < width; x++ ){
                pixels[y*width + x] = color;
            }
        }
    }
    void saveToPPM(std::string filename){
        std::ofstream file(filename);
        file << "P6\n";
        file << width << " " << height << " \n";
        file << "255\n";
        for ( int y = 0; y < height; y++ ){
            for ( int x = 0; x < width; x++ ){
                uint32_t color = pixels[y * width + x];
                file << uint8_t((color >> 16) & 0xFF);
                file << uint8_t((color >> 8) & 0xFF);
                file << uint8_t((color) & 0xFF);
            }
        }
        file.close();
    };
    void saveToPNG(std::string filename){
        std::vector<uint8_t> imageBuffer;
        int size = sizeof(pixels) / sizeof(uint32_t);
        std::vector<uint8_t> pngData;
        for ( int y = 0; y < height; y++ ){
            for ( int x = 0; x < width; x++ ){
                uint32_t color = pixels[y * width + x];
                pngData.push_back((color >> 16) & 0xFF);
                pngData.push_back((color >> 8) & 0xFF);
                pngData.push_back((color) & 0xFF);
                pngData.push_back(255);
            }
        }
        lodepng::encode(imageBuffer, pngData, width, height);
        lodepng::save_file(imageBuffer, filename);
    }
    void fillRect(int x, int y, int rwidth, int rheight, uint32_t color){
        x = x - rwidth / 2;
        y = y - rheight / 2;
        for ( int j = y; j <  y + rheight; j++){
            for ( int i = x; i < x + rwidth; i++ ){
                set(i, j, color);
            }
        }
    }
    void drawLine(int x1, int y1, int x2, int y2, uint32_t color){
        float dx = abs(x2 - x1);
        float sx = x1 < x2 ? 1 : -1;
        float dy = -abs(y2 - y1);
        float sy = y1 < y2 ? 1 : -1;
        float error = dx + dy;
        while ( true ){
            set(x1, y1, color);
            if ( x1 == x2  && y1 == y2){
                break;
            }
            float e2 = 2 * error;
            if ( e2 >= dy ){
                if (x1 == x2) break;
                error += dy;
                x1 = x1 + sx;
            }
            if (e2 <= dx) {
                if ( y1 == y2 ) break;
                error += dx;
                y1 = y1 + sy;
            }
        }
    };
    void fillTriangle(int x1, int y1, int x2, int y2, int x3, int y3, uint32_t color){

    };
    bool isPointInsideTriangle(int px, int py, int x1, int y1, int x2, int y2, int x3, int y3){
        double w1 = (x1*(y3 - y1) + (py - y1) * (x3 - x1) - px * (y3 - y1)) / ((y2 - y1) * (y3 - y1) );
	double w2 = (py - y1 - w1 * (y2 - y1)) / (y3 - y1);
        return true; 
    }
    void drawArc(float cx, float cy, float radius, double startAngle, double endAngle, uint32_t color){
        for(float t = startAngle; t <= endAngle; t += 0.001) 
        {
            int x = cos(t) * radius + cx; 
            int y = sin(t) * radius + cy;
            set(x, y, color);
        }
        if (radius > 0){
            drawArc(cx, cy, radius - 1, startAngle, endAngle, color);
        }
    }
};

struct Scene{
    int width;
    int height;
    Surface window;
    Scene(int w, int h){
        width = w;
        height = h;
        window = Surface(width, height);
    }
    virtual void render(){};
    void record(std::string fprefix, int frames = 5, int frameRate = 15){
        fs::remove_all("src/");
        mkdir("src/");
        for ( int frame = 1; frame < frames + 1; frame++ ){
            render();
            std::string cwd = (fprefix + std::to_string(frame) + ".png");
            window.saveToPNG("src\\" + cwd);
        }
        std::string size = std::to_string(width) + "x" + std::to_string(height);
        std::string cmd = "ffmpeg -r " + std::to_string(frameRate) + " -start_number 1 -s " + size + " -i src/" + fprefix + "\%d.png -c:v libx264 -pix_fmt yuv420p " + fprefix + ".mp4";
        std::system(cmd.c_str());
    };
};

#endif