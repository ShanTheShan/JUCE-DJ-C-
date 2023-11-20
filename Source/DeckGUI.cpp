
#include "../JuceLibraryCode/JuceHeader.h"
#include "DeckGUI.h"

//==============================================================================
DeckGUI::DeckGUI(DJAudioPlayer* _player,
                AudioFormatManager & 	formatManagerToUse,
                AudioThumbnailCache & 	cacheToUse
           ) : player(_player),
               waveformDisplay(formatManagerToUse, cacheToUse)
{

    addAndMakeVisible(playButton);
    addAndMakeVisible(stopButton);
    addAndMakeVisible(loadButton);
    addAndMakeVisible(muteButton);
    addAndMakeVisible(unmuteButton);

   
    auto playbuttonImage = ImageCache::getFromMemory(BinaryData::start_button_png,
                           BinaryData::start_button_pngSize);
    auto stopbuttonImage = ImageCache::getFromMemory(BinaryData::stop_button_png,
        BinaryData::stop_button_pngSize);

    //set the image to the  appropriate button
    playButton.setImages(true, true, true, 
        playbuttonImage, 0.6f, Colours::transparentBlack, 
        playbuttonImage, 1.0f, Colours::transparentBlack, 
        playbuttonImage, 0.6f, Colours::transparentBlack, 0.0f);

    stopButton.setImages(true, true, true,
        stopbuttonImage, 0.6f, Colours::transparentBlack,
        stopbuttonImage, 1.0f, Colours::transparentBlack,
        stopbuttonImage, 0.6f, Colours::transparentBlack, 0.0f);

    //settings for rotary slider
    setLookAndFeel(&otherLookAndFeel);
    volume_dial.setSliderStyle(juce::Slider::Rotary);
    volume_dial.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    speed_dial.setSliderStyle(juce::Slider::Rotary);
    speed_dial.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    pos_dial.setSliderStyle(juce::Slider::Rotary);
    pos_dial.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);


    setSize(300, 200);

    addAndMakeVisible(volume_dial);
    addAndMakeVisible(speed_dial);
    addAndMakeVisible(pos_dial);


    addAndMakeVisible(waveformDisplay);


    playButton.addListener(this);
    stopButton.addListener(this);
    loadButton.addListener(this);
    muteButton.addListener(this);
    unmuteButton.addListener(this);

    //make texts visible and text
    addAndMakeVisible(volumeText);
    addAndMakeVisible(posText);
    addAndMakeVisible(speedText);
    volumeText.setText("VOLUME", dontSendNotification);
    volumeText.attachToComponent(&volume_dial, true);
    speedText.setText("SPEED", dontSendNotification);
    speedText.attachToComponent(&speed_dial, true);
    posText.setText("POS", dontSendNotification);
    posText.attachToComponent(&pos_dial, true);

    //dial listner and range
    volume_dial.addListener(this);
    speed_dial.addListener(this);
    pos_dial.addListener(this);

    volume_dial.setRange(0.0, 1.0);
    speed_dial.setRange(0.0, 15);
    pos_dial.setRange(0.0, 1.0);

    //dial default value
    volume_dial.setValue(0.185);

    startTimer(500);

}

DeckGUI::~DeckGUI()
{
    stopTimer();
}

void DeckGUI::paint (Graphics& g)
{
    //this paints gets overriden from
    //class RotaryLookAndFeel : public juce::LookAndFeel_V4

    g.fillAll (Colours::black);   // clear the background

    loadButton.setColour(TextButton::buttonColourId, Colours::mediumblue);
    muteButton.setColour(TextButton::buttonColourId, Colours::orange);
    unmuteButton.setColour(TextButton::buttonColourId, Colours::orange);

}

void DeckGUI::drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height, float sliderPos, const float rotaryStartAngle, const float rotaryEndAngle, juce::Slider&)
{

}

void DeckGUI::resized()
{
    double rowH = getHeight() / 8; 
    playButton.setBounds(0, 0, getWidth()/5, rowH);
    stopButton.setBounds(0, rowH, getWidth() / 5, rowH);
    muteButton.setBounds(getWidth() / 1.4, rowH/4, getWidth()/6, rowH/2);
    unmuteButton.setBounds(getWidth() / 1.4, rowH*0.9, getWidth() / 4, rowH / 2);

    posSlider.setBounds(0, rowH * 4, getWidth(), rowH);
    waveformDisplay.setBounds(0, rowH * 5, getWidth(), rowH * 2);
    loadButton.setBounds(0, rowH * 7, getWidth(), rowH);

    volume_dial.setBounds(getWidth() / 2.9, 0, getWidth() / 3, getHeight() / 3);
    speed_dial.setBounds(getWidth() / 10, getHeight() / 3.5, getWidth() / 3, getHeight() / 3);
    pos_dial.setBounds(getWidth() / 1.7, getHeight() / 3.5, getWidth() / 3, getHeight() / 3);

}

void DeckGUI::buttonClicked(Button* button)
{
    if (button == &playButton)
    {
        std::cout << "Play button was clicked " << std::endl;
        player->start();
    }
     if (button == &stopButton)
    {
        std::cout << "Stop button was clicked " << std::endl;
        player->stop();

    }
    if (button == &loadButton)
    {
        auto fileChooserFlags = 
        FileBrowserComponent::canSelectFiles;
        fChooser.launchAsync(fileChooserFlags, [this](const FileChooser& chooser)
        {

                auto cout = URL{ chooser.getResult() };
                auto stringcout = cout.toString(false);
    
            player->loadURL(URL{chooser.getResult()});
            // and now the waveformDisplay as well
            waveformDisplay.loadURL(URL{chooser.getResult()}); 
        });
    }
    if (button == &muteButton)
    {
        player->setGain(0);
    }
    if (button == &unmuteButton)
    {
        auto fetchGain = volume_dial.getValue();
        player->setGain(fetchGain);
    }
}

void DeckGUI::fromPlaylistToDeck(URL _url) //to access both methods
{
    player->loadURL(_url);
    waveformDisplay.loadURL(_url);
}

void DeckGUI::sliderValueChanged (Slider *slider)
{
    if (slider == &volume_dial)
    {
        player->setGain(slider->getValue());
    }

    if (slider == &speed_dial)
    {
        player->setSpeed(slider->getValue());
    }
    
    if (slider == &pos_dial)
    {
        player->setPositionRelative(slider->getValue());
    }
    
}

bool DeckGUI::isInterestedInFileDrag (const StringArray &files)
{
  std::cout << "DeckGUI::isInterestedInFileDrag" << std::endl;
  return true; 
}

void DeckGUI::filesDropped (const StringArray &files, int x, int y)
{
  std::cout << "DeckGUI::filesDropped" << std::endl;
  if (files.size() == 1)
  {
    player->loadURL(URL{File{files[0]}});
    waveformDisplay.loadURL(URL{ File{files[0]} });
  }
}

void DeckGUI::timerCallback()
{

    //std::cout << "DeckGUI::timerCallback" << std::endl;
    waveformDisplay.setPositionRelative(
            player->getPositionRelative());
}