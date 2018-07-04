#ifndef OCTAVEBANDS_H
#define OCTAVEBANDS_H

#include <chrono>
#include <fftw3.h>
#include "../dsp/bands.h"
#include "../visualizer.h"
#include "../gfx/shader.h"
#include "../gfx/palette.h"

struct Bar
{
    float height;
    float tick_height;
};

class OctavebandsVisualizer : public Visualizer
{
public:
    static void loadConfig(const IniParser& ini);
    static const std::string name;

    OctavebandsVisualizer();
    ~OctavebandsVisualizer() override;

    const std::string& getTitle() const override;

    void attachSRC() override;
    void detatchSRC() override;
    void draw() override;
    void resize(int width, int height) override;

protected:
    void audioThreadFunc();
    bool quit_thread;
    std::thread audio_thread;

    void updateTicks();
    std::chrono::steady_clock::time_point last_render_tp;

    std::vector<Bar> bars;
    std::mutex render_mutex;

    Shader shader;
    PaletteTexture palette;
};

class OctavebandsVisualizerFactory : public VisualizerFactory
{
public:
    OctavebandsVisualizerFactory(const IniParser& ini);
    std::unique_ptr<Visualizer> create() const;
};

#endif // SPECTRUM_H
