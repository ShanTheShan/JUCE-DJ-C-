/*
  ==============================================================================

    This file was auto-generated!

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "DJAudioPlayer.h"
#include "PlaylistComponent.h"
#include "DeckGUI.h"


//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
class MainComponent   : public AudioAppComponent //this is inheritance, inherit AudioC
{
public:
    //==============================================================================
    MainComponent();
    ~MainComponent();

    //==============================================================================
    void prepareToPlay (int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock (const AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;

    //==============================================================================
    void paint (Graphics& g) override;
    void resized() override;

private:
    //==============================================================================
    // Your private member variables go here...

    AudioFormatManager formatManager; 
    AudioThumbnailCache thumbCache{100}; 

    DJAudioPlayer player1{formatManager};
    DeckGUI deckGUI1{&player1, formatManager,thumbCache};

    DJAudioPlayer player2{formatManager};
    DeckGUI deckGUI2{&player2, formatManager,thumbCache};


    MixerAudioSource mixerSource; //to play multiple tracks at the same time
    
    //track library
    PlaylistComponent playlistComponent{&player1,&deckGUI1,&deckGUI2};;



    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
