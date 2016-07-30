#include <stdlib.h>
#include <vector>
#include <algorithm> // sort

#include "drop.h"

#define MAX_COUNT 100

struct _compareDrop {
    bool operator() (Drop x, Drop y) {
        int width = x.getWidth();
        int index1 = x.getY() * width + x.getX();
        int index2 = y.getY() * width + y.getX();
        return (index1 < index2);
    }
} compareDrop;

class Rain {
    public:
        Rain(int width, int height, int init_counter, GLuint Program, GLuint x, GLuint y, GLuint z, GLuint w1, GLuint w2);

        void Draw();
        // fresh with more rain drops
        void AddMoreRainDrops();

        void UpdateRain();
    private:
        vector<Drop> rainDrops;
        int counter;
        int width;
        int height;

        // droprate: drop count = dropRate * width * height
        float dropRate;

        // shader program
        GLuint Program;
        //texture
        GLuint alphaMap;
        GLuint colorMap;
        GLuint shineMap;
        GLuint fgMap;
        GLuint bgMap;
};

// init
Rain::Rain(int width, int height, int init_counter, GLuint Program, GLuint x, GLuint y, GLuint z, GLuint w1, GLuint w2) {
    this->width = width;
    this->height = height;
    this->counter = init_counter;
    this->Program = Program;
    alphaMap = x;
    colorMap = y;
    shineMap = z;
    fgMap = w1;
    bgMap = w2;

    for(int i=0; i<init_counter; i++) {
        Drop tmp(width, height, Program);
        tmp.setTexture(alphaMap, colorMap, shineMap, fgMap, bgMap);
        tmp.draw_init();

        rainDrops.push_back(tmp);
    }

    dropRate = 0.0001;
}

void Rain::AddMoreRainDrops() {
    int count = dropRate * this->width * this->height;
    if(count > MAX_COUNT)
        count = MAX_COUNT;

    for(int i=0; i<count; i++) {
        Drop tmp(width, height, Program);
        tmp.setTexture(alphaMap, colorMap, shineMap, fgMap, bgMap);
        tmp.draw_init();

        rainDrops.push_back(tmp);
        this->counter++;
    }
}

void Rain::UpdateRain() {
    // Add more rain drops
    // sort the rain drop, scan the drop from top to bottom
    /*
        for each drop
            creep down
            shrink
            create trail rate drop
            Update Position
            Check collision
                Create new drop
            
    */
    AddMoreRainDrops();
    sort(this->rainDrops.begin(), this->rainDrops.end(), compareDrop);

    int length = this->rainDrops.size();
    for(int i=0; i<length; i++) {
        if(rainDrops[i].isKilled) {
            continue;
        }

        // Creep Down
        
        //shrink

        // Update Trail
        // Update position
        rainDrops[i].updatePosition();
    }


}

void Rain::Draw() {
    UpdateRain();

    for(int i=0; i<rainDrops.size(); i++) {
        if(!rainDrops[i].isKilled) {
            rainDrops[i].draw();
        }
    }
}
