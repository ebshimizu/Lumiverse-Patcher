/*
  ==============================================================================

    DMXPatchViewer.h
    Created: 7 Jan 2015 5:15:35pm
    Author:  eshimizu

  ==============================================================================
*/

#ifndef DMXPATCHVIEWER_H_INCLUDED
#define DMXPATCHVIEWER_H_INCLUDED

#include "../../JuceLibraryCode/JuceHeader.h"
#include "DMXMap.h"

//==============================================================================
/*
*/
class DMXPatchViewer : public Component, private ComboBoxListener, private ButtonListener
{
public:
  DMXPatchViewer(string patchID);
  ~DMXPatchViewer();

  void reload();
  void changeUniverse(int uni);

  void paint (Graphics&);
  void resized();

  // Callback for the combob box.
  void comboBoxChanged(ComboBox* box);

  void buttonClicked(Button* b) override;

  void addInterface();

private:
  void addArtNetInterface(string name);
  void addKiNetInterface(string name);
  void addDMXPro2Interface(string name);

  ComboBox _universeList;
  string _patchID;

  ScopedPointer<DMXMap> _dm;
  ScopedPointer<TextButton> _addUniverseButton;
  ScopedPointer<TextButton> _editUniverse;
  ScopedPointer<TextButton> _deleteUniverse;
  ScopedPointer<TextButton> _addInterface;
  ScopedPointer<TextButton> _editInterface;
  ScopedPointer<TextButton> _deleteInterface;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DMXPatchViewer)
};


#endif  // DMXPATCHVIEWER_H_INCLUDED
