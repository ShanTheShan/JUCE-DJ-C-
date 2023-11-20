

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

class DJAudioPlayer : public AudioSource {
  public:

    DJAudioPlayer(AudioFormatManager& _formatManager);
    ~DJAudioPlayer();

    void prepareToPlay (int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock (const AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;

    void loadURL(URL audioURL); //to load file from system
    void setGain(double gain);
    void setSpeed(double ratio);
    void setPosition(double posInSecs);
    void setPositionRelative(double pos);
    

    void start();
    void stop();

    /** get the relative position of the playhead */
    double getPositionRelative();

private:
    AudioFormatManager& formatManager; //self explanatory, checks file format
    std::unique_ptr<AudioFormatReaderSource> readerSource; //smart pointer, from the passed in audio from format reader, to get blocks of audio
    AudioTransportSource transportSource; //control audio, stop, play
    ResamplingAudioSource resampleSource{&transportSource, false, 2}; //to control speed

};




