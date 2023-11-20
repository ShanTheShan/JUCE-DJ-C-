
#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "DJAudioPlayer.h"
#include "WaveformDisplay.h"

//==============================================================================

/** RotaryLookAndFeel : public LookAndFeel_V4,
taken from the juce tutorial and integrated into my codes,
reference provided in the report as well
Getting started (no date) JUCE. 
Available at: https://docs.juce.com/master/tutorial_look_and_feel_customisation.html */
class RotaryLookAndFeel : public LookAndFeel_V4
{
public:
    RotaryLookAndFeel()
    {
        setColour(Slider::thumbColourId, Colours::red);
    }

    void drawRotarySlider(Graphics& g, int x, int y, int width, int height, float sliderPos,
        const float rotaryStartAngle, const float rotaryEndAngle, Slider&) override
    {
        float r = (float)jmin(width / 2, height / 2) - 4.0f;
        float midX = (float)x + (float)width * 0.5f;
        float midY = (float)y + (float)height * 0.5f;
        float radius_X = midX - r;
        float radius_Y = midY - r;
        float radius_Width = r * 2.0f;
        float angle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);

        // fill
        g.setColour(Colours::rebeccapurple);
        g.fillEllipse(radius_X, radius_Y, radius_Width, radius_Width);

        // outline
        g.setColour(Colours::grey);
        g.drawEllipse(radius_X, radius_Y, radius_Width, radius_Width, 4.0f);

        Path p;
        float pointerLength = r * 0.33f;
        float pointerThickness = 4.0f;
        p.addRectangle(-pointerThickness * 0.5f, -r, pointerThickness, pointerLength);
        p.applyTransform(AffineTransform::rotation(angle).translated(midX, midY));

        // pointer
        g.setColour(Colours::white);
        g.fillPath(p);
    }

    void drawButtonText(Graphics& g, TextButton& button, bool, bool isButtonDown) override
    {
        auto font = getTextButtonFont(button, button.getHeight());
        g.setFont(20.0f);
        g.setColour(button.findColour(button.getToggleState() ? TextButton::textColourOnId
            : TextButton::textColourOffId)
            .withMultipliedAlpha(button.isEnabled() ? 1.0f : 0.5f));

        int top_Indent = jmin(4, button.proportionOfHeight(0.3f));
        int edge = jmin(button.getHeight(), button.getWidth()) / 2;

        int fontHeight = roundToInt(font.getHeight() * 0.6f);
        int left_Indent = jmin(fontHeight, 2 + edge / (button.isConnectedOnLeft() ? 4 : 2));
        int right_Indent = jmin(fontHeight, 2 + edge / (button.isConnectedOnRight() ? 4 : 2));
        int textWidth = button.getWidth() - left_Indent - right_Indent;

        int edgeValue = 4;
        int offset = isButtonDown ? edgeValue / 2 : 0;

        if (textWidth > 0)
            g.drawFittedText(button.getButtonText(),
                left_Indent + offset, top_Indent + offset, 
                textWidth, button.getHeight() - top_Indent * 2 - edgeValue,
                juce::Justification::centred, 2);
    }

};

/*======================================================================*/


class DeckGUI    : public Component,
                   public LookAndFeel_V4,
                   public Button::Listener, 
                   public Slider::Listener, 
                   public FileDragAndDropTarget, 
                   public Timer
{
public:
    DeckGUI(DJAudioPlayer* player, 
           AudioFormatManager & 	formatManagerToUse,
           AudioThumbnailCache & 	cacheToUse );
    ~DeckGUI();

    void paint (Graphics&) override;
    void drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height, float sliderPos,
                          const float rotaryStartAngle, const float rotaryEndAngle, 
                          juce::Slider&) override;
    void resized() override;

     /** implement Button::Listener */
    void buttonClicked (Button *) override;

    /** implement Slider::Listener */
    void sliderValueChanged (Slider *slider) override;

    bool isInterestedInFileDrag (const StringArray &files) override;
    void filesDropped (const StringArray &files, int x, int y) override; 

    void timerCallback() override; 
    void fromPlaylistToDeck(URL _url);

private:

    ImageButton playButton{"PLAY"};
    ImageButton stopButton{"STOP"};
    TextButton loadButton{"LOAD"};
    TextButton muteButton{ "MUTE" };
    TextButton unmuteButton{ "UNMUTE" };
  
    Label volumeText;
    Label speedText;

    Slider posSlider;
    Label posText;

    Slider volume_dial;
    Slider speed_dial;
    Slider pos_dial;

    FileChooser fChooser{"Select a file..."};

    WaveformDisplay waveformDisplay;
    RotaryLookAndFeel otherLookAndFeel;


    DJAudioPlayer* player;


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DeckGUI)
};
