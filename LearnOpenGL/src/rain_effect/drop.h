#include <stdlib.h>

#define R_MIN    10
#define R_MAX    50
#define R_DELTA (R_MAX - R_MIN)

class Drop {

private:
    // window
    int width;
    int height;

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
    GLuint bgMap;
    

public:
    Drop(int x, int y, int r, int width, int height, GLuint Program);
    Drop(int width, int height, GLuint Program);
    void setTexture(GLuint x, GLuint y, GLuint z, GLuint w) {
        alphaMap = x;
        colorMap = y;
        shineMap = z;
        bgMap = w;

        // Bind Textures using texture units
        glUseProgram(this->Program);
        glUniform1i(glGetUniformLocation(this->Program, "background"), 0); 
        glUniform1i(glGetUniformLocation(this->Program, "alphaTexture"), 1);
        glUniform1i(glGetUniformLocation(this->Program, "colorTexture"), 2);
        glUniform1i(glGetUniformLocation(this->Program, "shineTexture"), 3);
    }

    void draw_init();
    void draw();
    
};

Drop::Drop(int x, int y, int r, int width, int height, GLuint Program) {
    this->x = x;
    this->y = y;
    this->r = r;
    this->width = width;
    this->height = height;
    this->Program = Program;
    this->spreadX = 1;
    this->spreadY = 1;
}

Drop::Drop(int width, int height, GLuint Program) {
    this->Program = Program;
    this->width = width;
    this->height = height;  

    this->x = rand() % width;
    this->y = rand() % height;

    this->r = rand() % R_DELTA + R_MIN;
    this->spreadX = 1;
    this->spreadY = 1;
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
}

void Drop::draw() {
    glm::mat4 model;
    model = glm::mat4();
    model = glm::translate(model, glm::vec3(2.0*x/width - 1, 1.0 - 2.0*y/height, 0));
    model = glm::scale(model, glm::vec3(r*spreadX/width, r*spreadY/height, 1.0));

    glUseProgram(this->Program);
    glUniformMatrix4fv(glGetUniformLocation(this->Program, "model"), 1, GL_FALSE, glm::value_ptr(model));

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, bgMap);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, alphaMap);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, colorMap);
	  glActiveTexture(GL_TEXTURE3);
	  glBindTexture(GL_TEXTURE_2D, shineMap);

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);  
}
