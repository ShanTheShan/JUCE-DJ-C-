
#include <JuceHeader.h>
#include "PlaylistComponent.h"
#include <time.h>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <iostream>
#include <string.h>


//==============================================================================
PlaylistComponent::PlaylistComponent(DJAudioPlayer* _player, 
                                     DeckGUI* _deckGUI1, 
                                     DeckGUI* _deckGUI2)
                                    : player(_player),
                                      deckGUI1(_deckGUI1),
                                      deckGUI2(_deckGUI2)

{
    // In your constructor, you should add any child components, and
    // initialise any special settings that your component needs.


    //to register basic audio file formats
    formatManager.registerBasicFormats();


    //name column header
    tableComponent.getHeader().addColumn("Track Title", 1, 210);
    tableComponent.getHeader().setSortColumnId(1, true);
    tableComponent.getHeader().addColumn("Length", 2, 65);
    tableComponent.getHeader().addColumn("Deck 1", 3, 70);
    tableComponent.getHeader().addColumn("Deck 2", 4, 70);
    tableComponent.getHeader().addColumn("Remove", 5, 80);

    //box model for table, which is the playlist component
    tableComponent.setModel(this);

    //make components visible
    addAndMakeVisible(tableComponent);
    addAndMakeVisible(searchLibrary);
    addAndMakeVisible(playlistLoadButton);

    //event listener for add to library button
    searchLibrary.addListener(this);
    playlistLoadButton.addListener(this);

    searchLibrary.setTextToShowWhenEmpty("Search...",Colours::green);
    searchLibrary.setFont(40.0f);

    //load saved library when constructor is created
    fetch_savedPlaylist_Name();
    fetch_savedPlaylist_Length();
}

PlaylistComponent::~PlaylistComponent()
{
    //save playlist to library when destroyed
    savePlaylist_Contents_NAME();
    savePlaylist_Contents_LENGTH();
}

void PlaylistComponent::paint (juce::Graphics& g)
{
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));   // clear the background

    g.setColour (juce::Colours::grey);
    g.drawRect (getLocalBounds(), 1);   // draw an outline around the component

    g.setColour (juce::Colours::white);
    g.setFont (14.0f);

    //library text size
    g.drawText("PlaylistComponent", getLocalBounds(),
        juce::Justification::centred, true);   // draw some placeholder text

}

void PlaylistComponent::resized()
{
    // This method is where you should set the bounds of any child
    // components that your component contains..
    playlistLoadButton.setBounds(0, 0, getWidth(), getHeight()/20);
    searchLibrary.setBounds(0,
                            playlistLoadButton.getHeight(),
                            getWidth(),
                            getHeight() / 20);

    tableComponent.setBounds(0, getHeight()/5, getWidth(), getHeight());
}


int PlaylistComponent::getNumRows()
{
    return trackNameList.size();
}

void PlaylistComponent::paintRowBackground(Graphics& g, int rowNumber, int width, int height, bool rowIsSelected)
{
    if (rowIsSelected) 
    {
        g.fillAll(Colours::lightblue);
    }
    else
    {
        g.fillAll(Colours::white);
    }
}

void PlaylistComponent::paintCell(Graphics& g, int rowNumber, int columnId, int width, int height, bool rowIsSelected)
{
    //draw text of files loaded into the array, see buttonClicked for ref
    if (columnId == 1)
    {
        g.drawText(trackNameList[rowNumber], 5, 0, width-1, height, Justification::centredLeft, true);
    }

    g.fillRect(width - 1, 0, 1, height);


    //draw track length
    if (columnId == 2)
    {
        //refer to timeConvert function below
        String stringHHMMSS = timeConvert(trackLength[rowNumber]);
        g.drawText(stringHHMMSS, 5, 0, width, height, Justification::centredLeft, true);
    }
}

Component* PlaylistComponent::refreshComponentForCell(int rowNumber, int columnId, 
                                                      bool isRowSelected, Component* existingComponentToUpdate)
{
    //add load button for deck 1
    if (columnId == 3)
    {
        if (existingComponentToUpdate == nullptr)
        {
            TextButton* button = new TextButton{ "ADD" };
            //to be able to know which button,
            //get id of the string
            String trackID(std::to_string(rowNumber));
            button->setComponentID(trackID);
            button->addListener(this);

            button->onClick = [this] {sendToDeck(deckGUI1); };

            existingComponentToUpdate = button;
        }
    }

    //add load button for deck 2
    if (columnId == 4)
    {
        if (existingComponentToUpdate == nullptr)
        {
            TextButton* button = new TextButton{ "ADD" };
            //to be able to know which button,
            //get id of the string
            String trackID{ std::to_string(rowNumber) };
            button->setComponentID(trackID);

            button->addListener(this);
            existingComponentToUpdate = button;

            //call function to add to deck
            //using lamba, see reference, Juce methods accepts lambda
            // https://www.codespeedy.com/passing-and-storing-lambda-function-as-callbacks-in-cpp/
            //no arguements needed to pass for lamba, just capture button click "this"
            button->onClick = [this] {sendToDeck(deckGUI2);};
        }
    }

    //add delete button to remove from the library
    if (columnId == 5)
    {
        if (existingComponentToUpdate == nullptr)
        {
            ImageButton* button = new ImageButton{ "REMOVE" };
            //load image
            auto removeImage = ImageCache::getFromMemory(BinaryData::cross_png,
                                                        BinaryData::cross_pngSize);
            button->setImages(true, true, true,
                removeImage, 0.6f, Colours::transparentBlack,
                removeImage, 1.0f, Colours::transparentBlack,
                removeImage, 0.6f, Colours::transparentBlack, 0.0f);

            //to be able to know which button,
            //get id of the string
            String trackID(std::to_string(rowNumber));
            button->setComponentID(trackID);

            button->addListener(this);

            button->onClick = [this] {removeTrackFromPlaylist(); };

            existingComponentToUpdate = button;

        }
    }
    return existingComponentToUpdate;
}

std::string PlaylistComponent::timeConvert(double rawTime)
{

    /*==================================================================================================================*/
    // referenced from -> https://stackoverflow.com/questions/58695875/how-to-convert-seconds-to-hhmmss-millisecond-format-c
    // Converts the double value in trackLength array 
    /*==================================================================================================================*/
    auto trackArray_Time = rawTime;

    auto hours = std::chrono::duration_cast<std::chrono::hours>(std::chrono::duration<double>(trackArray_Time));
    trackArray_Time -= std::chrono::duration_cast<std::chrono::duration<double>>(hours).count();
    auto minutes = std::chrono::duration_cast<std::chrono::minutes>(std::chrono::duration<double>(trackArray_Time));
    trackArray_Time -= std::chrono::duration_cast<std::chrono::duration<double>>(minutes).count();
    auto seconds = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::duration<double>(trackArray_Time));

    std::ostringstream ss;
    ss << std::setfill('0') << std::setw(2) << hours.count() << ":"
        << std::setfill('0') << std::setw(2) << minutes.count() << ":"
        << std::setfill('0') << std::setw(2) << seconds.count();

    std::string trackTime_HHMMSS = ss.str();

    return trackTime_HHMMSS;
}

void PlaylistComponent::buttonClicked(Button* button)
{
    //to know which button we click
    trackbuttonID = button->getComponentID().getDoubleValue();

    if (button == &playlistLoadButton)
    {
        //taken from deckGUI, dr matthew's code version
        auto fileChooserFlags = FileBrowserComponent::canSelectMultipleItems;
        fChooser.launchAsync(fileChooserFlags, [this](const FileChooser& chooser)
            {
                //store the chosen tracks into tracks array
                auto chosen = chooser.getResults();
                for (int i = 0; i < chosen.size(); ++i)
                {
                    //push to our main track storring array
                    tracks.add(chosen[i]);
                }
                //loop thru the CHOSEN array to add the tracks to the table
                for (int i = 0; i < chosen.size(); ++i)
                {
                    //URL takes file parameter, tracks array is of type file
                    //then store filename as string with method into juce string
                    trackName = URL{ chosen[i] }.getFileName();
                    //now push to string array list
                    trackNameList.add(trackName);

                    //get total samples of the file, divide by sample rate = length in seconds
                    //code referenced to JUCE forum
                    // Available at: https://forum.juce.com/t/get-track-length-before-it-starts-playing/44838 
                    if (auto reader = formatManager.createReaderFor(chosen[i]))
                    {
                        auto lengthInSeconds = reader->lengthInSamples / reader->sampleRate;
                        // lengthInSeconds now contains the length of the audio file in seconds...
                        trackLength.add(lengthInSeconds);
                    }
                }
            //Call this when the number of rows in the list changes
            //becuz paintcell graphics uses tracknamelist, we must update it
            tableComponent.updateContent();
            });

    }
}


void PlaylistComponent::textEditorTextChanged(TextEditor& texteditor) { //from Juce doc

    for (String& userString : trackNameList) {
        if (userString.containsWholeWord(searchLibrary.getText())) {
            tableComponent.selectRow(std::distance(trackNameList.begin(), &userString), false, true);
        }
    }
}

void PlaylistComponent::sendToDeck(DeckGUI* deckGUI)
{
    for (int i = 0; i < tracks.size(); ++i) {
        if (trackbuttonID == i) 
        {
            //retrive path for selected file
            auto trackURL = URL{ tracks[i] };
            deckGUI->fromPlaylistToDeck(trackURL);//call function from DeckGui.h/.cpp
        }
    }
}

void PlaylistComponent::savePlaylist_Contents_NAME()
{
    std::ofstream file("library_NAME.txt");
    
    for (int i = 0; i < tracks.size(); ++i)
    {
        file << tracks[i].getFullPathName()  << "\n";

    }
}

void PlaylistComponent::savePlaylist_Contents_LENGTH()
{
    std::ofstream file("library_LENGTH.txt");

    for (int i = 0; i < trackLength.size(); ++i)
    {
        file << trackLength[i] << "\n";

    }
}

//===========================================================
/*a w3schools example regarding the use
of ofstream and fstream to read and write files
was referred to for the functions savePlaylist and fetch_playlist
See reference:
(no date) C++ files. Available at: https://www.w3schools.com/cpp/cpp_files.asp
*/

//===========================================================


void PlaylistComponent::fetch_savedPlaylist_Name()
{
   std::ifstream savedText("library_NAME.txt");
   std::string stringRead;


   if (savedText.is_open()) {
       while (getline(savedText, stringRead))
       {
        //get the string of the file
        //convert to URL
        //push the URL of the file to tracks
         File file(stringRead);
         tracks.add(file);
         //remove all characters in the URL except the file name and ext
         auto cut_String = stringRead.find_last_of("\\");
         auto fileName = stringRead.substr(cut_String + 1);
         trackNameList.add(fileName);
       }
   }
   savedText.close();
}

void PlaylistComponent::fetch_savedPlaylist_Length()
{
    std::ifstream savedText("library_LENGTH.txt");
    std::string stringRead;

    if (savedText.is_open()) {
        while (getline(savedText, stringRead))
        {
            auto stringTo_Double = std::stod(stringRead);
            trackLength.add(stringTo_Double);
        }
    }
    savedText.close();

}

void PlaylistComponent::removeTrackFromPlaylist()
{
    for (int i = 0; i < tracks.size(); ++i) {
        if (trackbuttonID == i)
        {
            AlertWindow test{ "Removal of Track", "Remove",
            MessageBoxIconType::InfoIcon, nullptr };
            test.showMessageBoxAsync(MessageBoxIconType::InfoIcon,
            "Removal of Track From Playlist", tracks[i].getFileName() + " has been removed", "DONE", nullptr);

            tracks.remove(i); 
            trackNameList.remove(i);
            trackLength.remove(i);
        }
    }
    //update the array and table
    tableComponent.updateContent();
}
