#include <stdlib.h>

#define R_MIN    30
#define R_MAX    80
#define R_DELTA (R_MAX - R_MIN)

#define DEBUG_DROP 0

static int DROP_ID = 0;

class Drop {

private:
    // window
    int width;
    int height;

    float thick; // [0, 1] thick1 = (r- r.min) / (r.max - r.min)
                 // consider the shape thick = thick1/((spreadX + spreadY)*0.5 + 1)

    // Open GL part
    GLuint Program;
    GLuint VBO;
    GLuint VAO;
    GLuint EBO;
    //texture
    GLuint alphaMap;
    GLuint colorMap;
    GLuint shineMap;
    GLuint fgMap;
    GLuint bgMap;
    int id;

public:
    // location
    int x;
    int y;

    // shape
    float r;
    float spreadX;
    float spreadY;

    // velocity
    float momentum;
    float momentumX;

    // Collision
    bool isKilled;
    float shrink;

    // trails
    float trailRate;
    float lastSpawn;
    float nextSpawn;

    // parent
    int parent;

    Drop(int x, int y, int r, int width, int height, GLuint Program);
    Drop(int width, int height, GLuint Program);
    void DropCleanUp();
    void setTexture(GLuint x, GLuint y, GLuint z, GLuint w1, GLuint w2) {
        alphaMap = x;
        colorMap = y;
        shineMap = z;
        fgMap = w1;
        bgMap = w2;

         int i=0;
        // Bind Textures using texture units
        glUseProgram(this->Program);
        glUniform1i(glGetUniformLocation(this->Program, "fgTexture"), i++);
        glUniform1i(glGetUniformLocation(this->Program, "bgTexture"), i++); 
        glUniform1i(glGetUniformLocation(this->Program, "alphaTexture"), i++);
        glUniform1i(glGetUniformLocation(this->Program, "colorTexture"), i++);
        glUniform1i(glGetUniformLocation(this->Program, "shineTexture"), i++);
    }

    void draw_init();
    void draw();
    int getX() {
        return this->x;
    }
    int getID() {
        return this->id;
    }
    int getY() {
        return this->y;
    }
    int getWidth() {
        return this->width;
    }
    int getHeight() {
        return this->height;
    }

    void updatePosition();
    void CreepDown();
    void UpdateTrail(vector<Drop> &result);
};

void Drop::DropCleanUp() {
    // Properly de-allocate all resources once they've outlived their purpose
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
}

Drop::Drop(int x, int y, int r, int width, int height, GLuint Program) {
    this->x = x;
    this->y = y;
    this->r = r;
    this->width = width;
    this->height = height;
    this->Program = Program;
    this->spreadX = 1;
    // 1.2 - 3
    this->spreadY = 1.2 + ((float)(rand() % 100)) / 100.0 * 1.8;

    // velocity
    this->momentum = 0;

    // Collision
    this->isKilled = false;
    this->shrink = 0;

    // trails
    this->trailRate = 1;
    this->lastSpawn = 0;
    this->nextSpawn = R_MAX - this->momentum * 2 + (R_MAX - this->r);

    // drop id
    this->id = DROP_ID++;
    this->parent = -1;

    // thick
    this->thick = this->r < R_MIN ? 0 : (this->r - R_MIN) / R_DELTA;
    this->thick *= 1/((this->spreadX+this->spreadY)*0.5);
}

Drop::Drop(int width, int height, GLuint Program) {
    this->width = width;
    this->height = height;
    this->Program = Program;

    this->x = rand() % width;
    this->y = rand() % height;

    this->r = rand() % R_DELTA + R_MIN;

#if DEBUG_DROP
    printf("this->r is %f 11111111\n", this->r);
#endif
    this->spreadX = 1;
    // 1.2 - 3
    this->spreadY = 1.1 + ((float)(rand() % 100)) / 100.0 * 1.0;

    // velocity
    this->momentum = 0;

    // Collision
    this->isKilled = false;
    this->shrink = 0;

    // trails
    this->trailRate = 1;
    this->lastSpawn = 0;
    this->nextSpawn = R_MAX - this->momentum * 2 + (R_MAX - this->r);

    // drop id
    this->id = DROP_ID++;
    this->parent = -1;
}

void Drop::updatePosition() {
    if(isKilled) {
        return;
    }

    // slowdown momentum
    this->momentum *= 0.9;
    this->momentumX *= 0.9;

    this->y += momentum;
    this->x += momentumX;

    if(this->y > height - r)
        isKilled = true;
}

void Drop::draw_init() {
    // Set up vertex data (and buffer(s)) and attribute pointers
    GLfloat vertices[] = {
        // Positions          // Colors           // Texture Coords
         1.0f,  1.0f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f, // Top Right
         1.0f, -1.0f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f, // Bottom Right
        -1.0f, -1.0f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f, // Bottom Left
        -1.0f,  1.0f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f  // Top Left 
    };
    GLuint indices[] = {  // Note that we start from 0!
        0, 1, 3, // First Triangle
        1, 2, 3  // Second Triangle
    };

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);
    // Color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);
    // TexCoord attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0); // Unbind VAO

    // set resolution
    GLfloat w = (GLfloat)this->width;
    GLfloat h = (GLfloat)this->height;
    glUniform1f(glGetUniformLocation(this->Program, "imageWidth"), w);
    glUniform1f(glGetUniformLocation(this->Program, "imageHeight"), h);
    glUniform1f(glGetUniformLocation(this->Program, "thick"), thick);
}

void Drop::draw() {
    glm::mat4 model;
    model = glm::mat4();
    model = glm::translate(model, glm::vec3(2.0*x/width - 1, 1.0 - 2.0*y/height, 0));
    model = glm::scale(model, glm::vec3(r*spreadX/width, r*spreadY/height, 1.0));

    glUseProgram(this->Program);
    glUniformMatrix4fv(glGetUniformLocation(this->Program, "model"), 1, GL_FALSE, glm::value_ptr(model));

    int i=0;
    glActiveTexture(GL_TEXTURE0 + i);
    i++;
    glBindTexture(GL_TEXTURE_2D, fgMap);

    glActiveTexture(GL_TEXTURE0 + i);
    i++;
    glBindTexture(GL_TEXTURE_2D, bgMap);

    glActiveTexture(GL_TEXTURE0 + i);
    i++;
    glBindTexture(GL_TEXTURE_2D, alphaMap);

    glActiveTexture(GL_TEXTURE0 + i);
    i++;
	glBindTexture(GL_TEXTURE_2D, colorMap);

    glActiveTexture(GL_TEXTURE0 + i);
    i++;
	glBindTexture(GL_TEXTURE_2D, shineMap);

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);  
}

#define CREEPDOWN_RATE 0.3
#define CREEPDOWN_VELOCITY 3
#define SHRINK_RATE 0.5

#define DEBUG_SHRINK 0

void Drop::CreepDown() {
    if((float)(rand() % R_MAX) / R_MAX * this->r / R_MAX > CREEPDOWN_RATE) {
        this->momentum += (float)(rand() % ((int)(this->r))) / R_MAX * CREEPDOWN_VELOCITY;
    }

    if(this->r < R_MIN/2 && ((float)(rand() % R_MAX) / R_MAX > SHRINK_RATE)) {
        this->shrink += 0.01;
#if DEBUG_SHRINK
        printf("current shrink %f  %f \n", this->shrink, this->r);
#endif
    }

    this->r -= this->shrink;
    if(this->r <= 0) {
        this->isKilled = true;
    }
}

#define TRAIL_OFFSET     0.1
#define TRAIL_SIZE_MIN   0.3
#define TRAIL_SIZE_MAX   0.6
#define DEBUG_TRAIL 0

void Drop::UpdateTrail(vector<Drop> &result) {
    this->lastSpawn += this->momentum * this->trailRate;
    if(this->lastSpawn > this->nextSpawn && this->r > R_MIN) {
        // create trail drop
        int randX = (((float)(rand() % R_MAX) / R_MAX) - 0.5) * this->r * TRAIL_OFFSET;
        int randY = ((float)(rand() % R_MAX) / R_MAX) * this->r * TRAIL_OFFSET;
        float randR = ((float)(rand() % R_MAX) / R_MAX) * (TRAIL_SIZE_MAX - TRAIL_SIZE_MIN) + TRAIL_SIZE_MIN;
        //float randR = 1;
        Drop trailDrop(this->x+randX, this->y-randY, this->r * randR, this->width, this->height, this->Program);
        trailDrop.setTexture(alphaMap, colorMap, shineMap, fgMap, bgMap);
        trailDrop.draw_init();

        // set the parent
        if(this->parent == -1)
            trailDrop.parent = this->id;
        else
            trailDrop.parent = this->parent;

        trailDrop.spreadY = this->momentum * 0.1;
        result.push_back(trailDrop);

        this->r *= 0.97;
        this->lastSpawn = 0;
        this->nextSpawn = rand() % R_DELTA + R_MIN + this->momentum * 2 + (R_MAX - this->r) + 10;
#if DEBUG_TRAIL
        printf("create a trail position %d %d radius %f, parent drop %d %d radius %f \n", trailDrop.getX(), trailDrop.getY(), trailDrop.r, this->x, this->y, this->r);
        printf("lastSpawn %f nextSpawn %f \n", this->lastSpawn, this->nextSpawn);
#endif
    }
}
