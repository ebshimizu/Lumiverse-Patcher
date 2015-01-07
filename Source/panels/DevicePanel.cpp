/*
  ==============================================================================

    DevicePanel.cpp
    Created: 6 Jan 2015 3:21:40pm
    Author:  eshimizu

  ==============================================================================
*/

#include "../../JuceLibraryCode/JuceHeader.h"
#include "DevicePanel.h"

//==============================================================================
DevicePanel::DevicePanel()
{
  // In your constructor, you should add any child components, and
  // initialise any special settings that your component needs.
  addAndMakeVisible(_dt = new DeviceTable());
  //addAndMakeVisible(_dp = new PropertiesPanel());
  //addAndMakeVisible(_rb = new StretchableLayoutResizerBar(&_layout, 1, true));

  _layout.setItemLayout(0, -0.25, -0.75, -0.75);
  //_layout.setItemLayout(1, 3, 3, 3);
  //_layout.setItemLayout(2, -0.25, -0.75, -0.35);
}

DevicePanel::~DevicePanel()
{
  _dt = nullptr;
  _rb = nullptr;
}

void DevicePanel::paint (Graphics& g)
{
    /* This demo code just fills the component's background and
       draws some placeholder text to get you started.

       You should replace everything in this method with your own
       drawing code..
    */

    g.fillAll (Colour(0xff0c0c0c));   // clear the background
}

void DevicePanel::resized()
{
  juce::Rectangle<int> area(getLocalBounds());

  //Component* comps[] = { _dt, _rb/*, _dp */};
  //_layout.layOutComponents(comps, 3, 0, 0, area.getWidth(), area.getHeight(), true, true);
  _dt->setBoundsInset(BorderSize<int>(0));
}

void DevicePanel::reload() {
  _dt->reinit();
}