/*
  ==============================================================================

    ProfileEditor.h
    Created: 8 Jan 2015 2:29:43pm
    Author:  eshimizu

  ==============================================================================
*/

#ifndef PROFILEEDITOR_H_INCLUDED
#define PROFILEEDITOR_H_INCLUDED

#include "../../JuceLibraryCode/JuceHeader.h"
#include "LumiverseCore.h"

using namespace Lumiverse;
using namespace std;

//==============================================================================
/*
Component that allows users to setup profiles (or device templates might be a 
better way of saying that). Essentially you get to set all the defaults and
data needed to create a device from this particular file.
*/
class ProfileEditorComponent : public Component, private ButtonListener
{
public:
  ProfileEditorComponent();
  ~ProfileEditorComponent();

  void paint(Graphics& g);
  void resized();

  void close();

  void buttonClicked(Button* b) override;

  void reloadParameters();
  void reloadMetadata();
  void reloadDMXMap();
private:
  Device* _device;
  
  // stores the data used in the DMX Device Map.
  map<string, patchData> _dmxMapData;

  map < string, Array<PropertyComponent* > > _paramGUIs;
  Array<PropertyComponent*> _metadataProperties;

  File _profile;

  ScopedPointer<TextButton> _browse;
  ScopedPointer<TextButton> _addParam;
  ScopedPointer<TextButton> _addMetadata;
  ScopedPointer<PropertyPanel> _params;
  ScopedPointer<PropertyPanel> _metadata;
  ScopedPointer<PropertyPanel> _dmxMap;

  void addParam();
  void addMetadata();
  void reloadAll(string paramName);

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ProfileEditorComponent);
};

//==============================================================================
/*
*/
class ProfileEditor : public DocumentWindow
{
public:
  ProfileEditor(const String& name, Colour backgroundColour, int buttonsNeeded);
  ~ProfileEditor();

  void closeButtonPressed();

private:
  ScopedPointer<ProfileEditorComponent> _pec;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ProfileEditor)
};


#endif  // PROFILEEDITOR_H_INCLUDED
