#ifndef RENDERER_H
#define RENDERER_H

class Renderer {
public:
    Renderer();
    ~Renderer();

    bool init();
    void render();
    void clean();
};

#endif // RENDERER_H
