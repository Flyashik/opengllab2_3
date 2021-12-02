// ����������� �����������

#include <gl/glew.h>
#include <SFML/OpenGL.hpp>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>

#include <iostream>


// ���������� � ����������������� ID
// ID ��������� ���������
GLuint Program;
// ID �������� ������
GLint Attrib_vertex;
// ID �������� �����
GLint Attrib_color;
// ID ������� ���������� �����
GLint Unif_color;
// ID VBO ������
GLuint VBO_position;
// ID VBO �����
GLuint VBO_color;
GLint Unif_scale;
// �������
struct Vertex
{
    GLfloat x;
    GLfloat y;
};

float scale[2] = { 1.0f, 1.0f };
// �������� ��� ���������� �������
const char* VertexShaderSource = R"(
    #version 330 core
    in vec2 coord;
    in vec4 colors;
    uniform vec2 scale;
    out vec4 vert_color;

    void main() {
        vec3 scale_position = vec3(coord, 0.0) * mat3(
						scale[0], 0, 0,
						0, scale[1], 0,
						0, 0, 1
						);		
        gl_Position = vec4(scale_position, 1.0);
        vert_color = colors;
    }
)";

// �������� ��� ������������ �������
const char* FragShaderSource = R"(
    #version 330 core
    in vec4 vert_color;

    out vec4 colors;
    void main() {
        colors = vert_color;
    }
)";


void Init();
void Draw();
void Release();

void scale_change(float x, float y)
{
    scale[0] += x;
    scale[1] += y;
}

int main() {
    sf::Window window(sf::VideoMode(600, 600), "My OpenGL window", sf::Style::Default, sf::ContextSettings(24));
    window.setVerticalSyncEnabled(true);

    window.setActive(true);

    // ������������� glew
    glewInit();

    Init();

    

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
            else if (event.type == sf::Event::Resized) {
                glViewport(0, 0, event.size.width, event.size.height);
            }
            else if (event.type == sf::Event::KeyPressed)
            {
                switch (event.key.code) {
                case (sf::Keyboard::Right): scale_change(0.01f, 0.0f); break;
                case (sf::Keyboard::Left): scale_change(-0.01f, 0.0f); break;
                case (sf::Keyboard::Up): scale_change(0.0f, 0.01f); break;
                case (sf::Keyboard::Down): scale_change(0.0f, -0.01f); break;
                default: break;
                }
            }
        }

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        Draw();

        window.display();
    }

    Release();
    return 0;
}


// �������� ������ OpenGL, ���� ���� �� ����� � ������� ��� ������
void checkOpenGLerror() {
    GLenum errCode;
    // ���� ������ ����� �������� ���
    // https://www.khronos.org/opengl/wiki/OpenGL_Error
    if ((errCode = glGetError()) != GL_NO_ERROR)
        std::cout << "OpenGl error!: " << errCode << std::endl;
}

// ������� ������ ���� �������
void ShaderLog(unsigned int shader)
{
    int infologLen = 0;
    int charsWritten = 0;
    char* infoLog;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infologLen);
    if (infologLen > 1)
    {
        infoLog = new char[infologLen];
        if (infoLog == NULL)
        {
            std::cout << "ERROR: Could not allocate InfoLog buffer" << std::endl;
            exit(1);
        }
        glGetShaderInfoLog(shader, infologLen, &charsWritten, infoLog);
        std::cout << "InfoLog: " << infoLog << "\n\n\n";
        delete[] infoLog;
    }
}

void HSVtoRGB(float H, float S, float V, float& R, float& G, float& B) {

    float s = S / 100;
    float v = V / 100;
    float C = s * v;
    float X = C * (1 - abs(fmod(H / 60.0, 2) - 1));
    float m = v - C;
    float r, g, b;
    if (H >= 0 && H < 60) {
        r = C, g = X, b = 0;
    }
    else if (H >= 60 && H < 120) {
        r = X, g = C, b = 0;
    }
    else if (H >= 120 && H < 180) {
        r = 0, g = C, b = X;
    }
    else if (H >= 180 && H < 240) {
        r = 0, g = X, b = C;
    }
    else if (H >= 240 && H < 300) {
        r = X, g = 0, b = C;
    }
    else {
        r = C, g = 0, b = X;
    }
    R = (r + m);
    G = (g + m);
    B = (b + m);
}

void InitVBO()
{
    glGenBuffers(1, &VBO_position);
    glGenBuffers(1, &VBO_color);
    // ������� ������������
    float R = 1.0f;
    float G = 1.0f;
    float B = 1.0f;
    float r = 0.3f;
    const int amountSegments = 361;
    float colors[amountSegments][4];
    Vertex triangle[amountSegments];
    triangle[0].x = 0.0f;
    triangle[0].y = 0.0f;
    colors[0][0] = R;
    colors[0][1] = G;
    colors[0][2] = B;
    colors[0][3] = 1.0f;
    for (int i = 1; i < amountSegments; i++)
    {
        float angle = 2.0f * 3.1415926f * float(i) / float(amountSegments);
        float dx = r * cosf(angle);
        float dy = r * sinf(angle);
        triangle[i].x = 0.0f + dx;
        triangle[i].y = 0.0f + dy;
        HSVtoRGB(i, 100, 100, R, G, B);
        colors[i][0] = R;
        colors[i][1] = G;
        colors[i][2] = B;
        colors[i][3] = 1.0f;
    }
    // �������� ������� � �����
    glBindBuffer(GL_ARRAY_BUFFER, VBO_position);
    glBufferData(GL_ARRAY_BUFFER, sizeof(triangle), triangle, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_color);
    glBufferData(GL_ARRAY_BUFFER, sizeof(colors), colors, GL_STATIC_DRAW);
    checkOpenGLerror();
}


void InitShader() {
    // ������� ��������� ������
    GLuint vShader = glCreateShader(GL_VERTEX_SHADER);
    // �������� �������� ���
    glShaderSource(vShader, 1, &VertexShaderSource, NULL);
    // ����������� ������
    glCompileShader(vShader);
    std::cout << "vertex shader \n";
    ShaderLog(vShader);

    // ������� ����������� ������
    GLuint fShader = glCreateShader(GL_FRAGMENT_SHADER);
    // �������� �������� ���
    glShaderSource(fShader, 1, &FragShaderSource, NULL);
    // ����������� ������
    glCompileShader(fShader);
    std::cout << "fragment shader \n";
    ShaderLog(fShader);

    // ������� ��������� � ����������� ������� � ���
    Program = glCreateProgram();
    glAttachShader(Program, vShader);
    glAttachShader(Program, fShader);

    // ������� ��������� ���������
    glLinkProgram(Program);
    // ��������� ������ ������
    int link_ok;
    glGetProgramiv(Program, GL_LINK_STATUS, &link_ok);
    if (!link_ok)
    {
        std::cout << "error attach shaders \n";
        return;
    }

    // ���������� ID �������� ������ �� ��������� ���������
    Attrib_vertex = glGetAttribLocation(Program, "coord");
    if (Attrib_vertex == -1)
    {
        std::cout << "could not bind attrib coord" << std::endl;
        return;
    }

    // ���������� ID �������� �����
    Attrib_color = glGetAttribLocation(Program, "colors");
    if (Attrib_color == -1)
    {
        std::cout << "could not bind attrib colors" << std::endl;
        return;
    }

    Unif_color = glGetUniformLocation(Program, "scale");
    if (Unif_color == -1)
    {
        std::cout << "could not bind uniform scale" << std::endl;
        return;
    }

    checkOpenGLerror();
}

void Init() {
    InitShader();
    InitVBO();
}


void Draw() {
    // ������������� ��������� ��������� �������
    glUseProgram(Program);
    // �������� ������� ���������
    glEnableVertexAttribArray(Attrib_vertex);
    glEnableVertexAttribArray(Attrib_color);

    // ���������� VBO_position
    glBindBuffer(GL_ARRAY_BUFFER, VBO_position);
    glVertexAttribPointer(Attrib_vertex, 2, GL_FLOAT, GL_FALSE, 0, 0);

    // ���������� VBO_color
    glBindBuffer(GL_ARRAY_BUFFER, VBO_color);
    glVertexAttribPointer(Attrib_color, 4, GL_FLOAT, GL_FALSE, 0, 0);

    // ��������� VBO
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glUniform2fv(Unif_scale, 1, scale);

    // �������� ������ �� ����������(������)
    glDrawArrays(GL_TRIANGLE_FAN, 0, 361);

    // ��������� ������� ���������
    glDisableVertexAttribArray(Attrib_vertex);
    glDisableVertexAttribArray(Attrib_color);

    glUseProgram(0);
    checkOpenGLerror();
}


// ������������ ��������
void ReleaseShader() {
    // ��������� ����, �� ��������� �������� ���������
    glUseProgram(0);
    // ������� ��������� ���������
    glDeleteProgram(Program);
}

// ������������ ������
void ReleaseVBO()
{
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glDeleteBuffers(1, &VBO_position);
    glDeleteBuffers(1, &VBO_color);
}

void Release() {
    ReleaseShader();
    ReleaseVBO();
}

