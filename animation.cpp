#include "obscura.hpp"
#include <iostream>

struct World: Scene{
    int max_iterations;
    int side = 0;
    World(int w, int h): Scene(w, h){
       max_iterations = 1;
    };
    void render(){
        window.fill(0);
        //int color = map(side, 0, 120, 0, 0xFF);
        //window.fillRect(0, 0, side, side, Color(color, color, color));
        //window.drawArc(0, 0, 60, 0, beginAngle, 0xFF0000);
        //beginAngle += 2 * 3.1459 / 60;
        //side += 2;
        int colors[max_iterations];
        int range = 9;
        for ( int i = 0; i < max_iterations; i++ ){
            if ( i > range ){
                colors[i] = rand() % (0xFFFFFF + 1);
            }else{
                colors[i] = 0;
            }
        }
        for ( int j = -height / 2; j < height / 2; j++ ) {
            for ( int i = -width / 2; i < width / 2; i++ ) {
                float x0 = map(i, -width/2, width / 2, -2.0, 0.47);
                float y0 = map(j, -height / 2, height / 2, -1.12, 1.12);
                float x = 0;
                float y = 0;
                int iteration = 0;
                while (x*x + y*y <= 2*2 and iteration < max_iterations) {
                    float xtemp = x*x - y*y + x0;
                    y = 2 * x * y + y0;
                    x = xtemp;
                    iteration += 1;
                }
                int color = map(iteration, 0, max_iterations, 0, 255);
                //int color = colors[iteration];
                window.set(i, j, Color(color, color, color));
            }
        }
        if ( max_iterations < 20 and max_iterations < 70 ){
            max_iterations += 1;
        }else if( max_iterations < 70 ){
            max_iterations += 4;
        }
    }
};

int main(){
    std::filesystem::path src = std::filesystem::current_path();
    World w = World(800, 600);
    w.record("anime2", 100, 25);
}