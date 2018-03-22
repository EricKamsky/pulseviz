#include <iostream>
#include "visualizerwindow.h"
#include "visualizer/waveform.h"
#include "visualizer/spectrum.h"
#include "visualizer/octavebands.h"
#include "visualizer/spectrogram.h"
#include "visualizer/spectrogram3d.h"

VisualizerWindow::VisualizerWindow(std::map<std::string, ColorScheme> colorschemes)
    :
    Window(),
    visualizer(nullptr),
    visualizer_num(static_cast<unsigned int>(-1)),
    palettes(colorschemes),
    palette_it(palettes.begin())
{
    for (auto& colorscheme: this->palettes)
        colorscheme.second.createPaletteTexture();

    this->cycleThroughVisualizers();
}

void VisualizerWindow::onFramebufferSizeChange(int width, int height)
{
    glViewport(0, 0, width, height);
    if (this->visualizer)
    {
        this->visualizer->onFramebuffersizeChanged(
            static_cast<unsigned int>(width),
            static_cast<unsigned int>(height)
        );
    }
}

void VisualizerWindow::onKeyPress(int key, int /* scancode */, int action, int /* mods */)
{
    if (action == GLFW_RELEASE)
        return;

    switch (key)
    {
        case GLFW_KEY_Q: this->setShouldClose(); break;
        case GLFW_KEY_F: this->toggleFullScreen(); break;
        case GLFW_KEY_C: this->cycleThroughVisualizers(); break;
        case GLFW_KEY_P: this->cycleThroughPalettes(); break;
    }
}

void VisualizerWindow::render()
{
    glfwMakeContextCurrent(this->window_handle);

    auto background_color = this->visualizer->colorscheme->getBackgroundColor();
    glClearColor(background_color.r, background_color.g, background_color.b, 1.0f);

    if (this->visualizer)
        this->visualizer->render();
}

template<class VisualizerClass>
void VisualizerWindow::createVisualizer()
{
    this->visualizer.reset();

    this->visualizer = std::unique_ptr<Visualizer>(
        dynamic_cast<Visualizer*>(new VisualizerClass())
    );

    glfwSetWindowTitle(this->window_handle, this->visualizer->getTitle());

    int width, height;
    glfwGetWindowSize(this->window_handle, &width, &height);
    this->visualizer->onFramebuffersizeChanged(
        static_cast<unsigned int>(width),
        static_cast<unsigned int>(height)
    );

    this->visualizer->setColorScheme(palette_it->second);
}

void VisualizerWindow::cycleThroughVisualizers()
{
    this->visualizer_num = (this->visualizer_num + 1) % 5;
    switch (this->visualizer_num)
    {
        case 0: this->createVisualizer<WaveFormVisualizer>(); break;
        case 1: this->createVisualizer<SpectrumVisualizer>(); break;
        case 2: this->createVisualizer<OctavebandsVisualizer>(); break;
        case 3: this->createVisualizer<SpectrogramVisualizer>(); break;
        case 4: this->createVisualizer<Spectrogram3DVisualizer>(); break;
        default: throw "This should not happen!";
    }
}

void VisualizerWindow::cycleThroughPalettes()
{
    palette_it++;
    if (palette_it == palettes.end())
        palette_it = palettes.begin();

    std::cout << "Current palette: " << palette_it->first << std::endl;
    this->visualizer->setColorScheme(palette_it->second);
}

Visualizer& VisualizerWindow::getVisualizer()
{
    return *this->visualizer;
}
