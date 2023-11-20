
#pragma once

#include <JuceHeader.h>
#include "DJAudioPlayer.h"
#include "DeckGUI.h"
#include "WaveformDisplay.h"
#include <vector>
#include <string>

//==============================================================================
/*
*/
class PlaylistComponent  : public Component,
                           public TableListBoxModel,
                           public Button::Listener,
                           public TextEditor::Listener
{
public:
    PlaylistComponent(DJAudioPlayer* player, 
        DeckGUI* _deckGUI1, DeckGUI* _deckGUI2);
    ~PlaylistComponent() override;

    void paint (juce::Graphics&) override;
    void resized() override;

    //public virtual void functions, we must override and implement
    void buttonClicked(Button* button) override;
    int getNumRows() override;


    void textEditorTextChanged(TextEditor&);
    //to paint the ROW in the box model
    void paintRowBackground(Graphics&, int rowNumber, int width, int height,
                            bool rowIsSelected) override;
    //to paint the CELL in the box model
    void paintCell(Graphics& ,
                    int rowNumber,
                    int columnId,
                    int width,
                    int height,
                    bool rowIsSelected
                    ) override;

    //to allow custome components INSIDE box cell
    Component* refreshComponentForCell(int rowNumber, int columnId,
                    bool isRowSelected, Component* existingComponentToUpdate);

    //function to convert time to string
    std::string timeConvert(double rawTime);

    void removeTrackFromPlaylist();

    //load song from playlist into selected deck
    void sendToDeck(DeckGUI* deckGUI);

    
    void savePlaylist_Contents_NAME();//save the playlist contents to file reader
    void savePlaylist_Contents_LENGTH();//save the playlist contents to file reader

    
    void fetch_savedPlaylist_Name();//read the file when app is run
    void fetch_savedPlaylist_Length();//read the file when app is run
    

private:

    double trackbuttonID;
    Array<File>tracks;
    String trackName;
    Array<String> trackNameList;
    Array<double> trackLength;

    std::unique_ptr<AudioFormatReaderSource> readerSource; //smart pointer, from the passed in audio from format reader, to get blocks of audio
    AudioFormatManager formatManager;
    FileChooser fChooser{ "Select a file..." };

    TableListBox tableComponent;
    TextButton playlistLoadButton{"Add to Library"};
    TextEditor searchLibrary{ "Search" };

    //these are pointers so that we can send the event listeners
    //from button clicks to these classes
    DJAudioPlayer* player;
    DeckGUI* deckGUI1;
    DeckGUI* deckGUI2;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PlaylistComponent)
};
