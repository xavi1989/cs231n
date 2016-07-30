#include <stdlib.h>
#include <vector>
#include <algorithm> // sort

#include <drop.h>

struct _compareDrop {
    bool operator() (Drop x, Drop y) {
        int width = x.getWidth();
        int index1 = x.getY() * width + x.getX();
        int index2 = y.getY() * width + y.getX();
        return (index1 < index2);
    }
} compareDrop

class Rain {
    public:
        Rain(int width, int height, int init_counter, GLuint Program);

        // fresh with more rain drops
        void AddMoreRainDrops();

        void UpdateRain();
    private:
        Vector<Drop> rainDrops;
        int counter;
        int width;
        int height;

        // droprate: drop count = dropRate * width * height
        float dropRate;

        // shader program
        GLuint Program
};

// init
Rain::Rain(int width, int height, int init_counter, GLuint Program) {
    this->width = width;
    this->height = height;
    this->counter = init_counter;
    this->Program = Program;

    for(int i=0; i<init_counter; i++) {
        Drop tmp(width, height, Program);
        rainDrops.push_back(tmp);
    }
}

void Rain::AddMoreRainDrops() {
    int count = dropRate * this->width * this->height;

    for(int i=0; i<count; i++) {
        Drop tmp(width, height, Program);
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
