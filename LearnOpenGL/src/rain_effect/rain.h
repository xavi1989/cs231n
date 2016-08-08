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

        void RainDropCollision(int index, int length);

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

    dropRate = 0.000001;
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

#if DEBUG_DROP
    for(int i=0; i<rainDrops.size(); i++) {
        printf("%d rain size %f \n", i, rainDrops[i].r);
    }
#endif
    int length = this->rainDrops.size();
    for(int i=0; i<length; i++) {
        if(rainDrops[i].isKilled) {
            continue;
        }

        // Creep Down
        rainDrops[i].CreepDown();

        // Update Trail
        vector<Drop> trailDrop;
        rainDrops[i].UpdateTrail(trailDrop);

        if(trailDrop.size()) {
            for(int j=0; j<trailDrop.size(); j++) {
                rainDrops.push_back(trailDrop[j]);
            }
        }

        // Update position
        rainDrops[i].updatePosition();

        // Check Collision
        RainDropCollision(i, length);
    }


}

#define RADIUS_RATE 0.3
#define DEBUG_COLLISION 0

void Rain::RainDropCollision(int index, int length) {
    Drop drop = rainDrops[index];

    if(drop.isKilled)
        return;

    for(int i=index; i<length; i++) {
        if(index == i || rainDrops[i].isKilled == true)
            continue;

        if(drop.getID() == rainDrops[i].parent || drop.parent == rainDrops[i].getID())
            continue;

        float dx = drop.getX() - rainDrops[i].getX();
        float dy = drop.getY() - rainDrops[i].getY();
        float distance = sqrt(dx * dx + dy * dy);

        if(distance < (drop.r + rainDrops[i].r) * RADIUS_RATE) {
#if DEBUG_COLLISION
        printf("current drop index %d position %d %d radius %f id %d parent %d\n", index, drop.x, drop.y, drop.r, drop.getID(), drop.parent);
        printf("collide drop index %d position %d %d radius %f id %d parent %d \n", i, rainDrops[i].x, rainDrops[i].y, rainDrops[i].r, rainDrops[i].getID(), rainDrops[i].parent);
#endif

            // merge the two rain drops
            float r1 = drop.r;
            float r2 = rainDrops[i].r;

            float targetR = sqrt(r1 * r1 + r2 * r2);
            if(targetR > R_MAX)
                targetR = R_MAX;

            drop.momentumX += dx * 0.1;
            drop.momentum += targetR;

            rainDrops[i].isKilled = true;
        }
    }
}


void Rain::Draw() {
    UpdateRain();

#if DEBUG_TRAIL
    int counter = 0;
    printf("rainDrops.size() %d \n", rainDrops.size());
#endif

    for(int i=0; i<rainDrops.size(); i++) {
        if(!rainDrops[i].isKilled) {
#if DEBUG_TRAIL
            printf("counter is %d \n", counter++);
#endif
            rainDrops[i].draw();
        }
    }
}
