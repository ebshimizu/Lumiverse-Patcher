/*
  ==============================================================================

    DMXPatchViewer.cpp
    Created: 7 Jan 2015 5:15:35pm
    Author:  eshimizu

  ==============================================================================
*/

#include "../../JuceLibraryCode/JuceHeader.h"
#include "DMXPatchViewer.h"
#include "../Main.h"

//==============================================================================
DMXPatchViewer::DMXPatchViewer(string patchID) : _patchID(patchID)
{
  addAndMakeVisible(&_universeList);
  _universeList.addListener(this);
  addAndMakeVisible(_dm = new DMXMap(patchID, 0));

  reload();
}

DMXPatchViewer::~DMXPatchViewer()
{
  _dm = nullptr;
}

void DMXPatchViewer::reload() {
  _universeList.clear(dontSendNotification);

  auto& rigRef = MainWindow::getRig();

  auto patch = rigRef->getPatch(_patchID);
  assert(patch != nullptr && patch->getType() == "DMXPatch");
  DMXPatch* p = dynamic_cast<DMXPatch*>(patch);
  
  // We only show you stuff if there's an actual interface assigned to it.
  for (auto i : p->getInterfaceInfo()) {
    _universeList.addItem(String(i.second + 1) + ": " + i.first, i.second + 1);
  }

  _universeList.setSelectedItemIndex(0);
}

void DMXPatchViewer::paint (Graphics& g)
{
  g.fillAll (Colour(0xff0c0c0c));   // clear the background
}

void DMXPatchViewer::resized()
{
  auto area = getLocalBounds();
  auto top = area.removeFromTop(30);
  _universeList.setBounds(top.removeFromRight(240).reduced(3));
  _dm->setBounds(area);
}

void DMXPatchViewer::comboBoxChanged(ComboBox* box) {
  int uni = box->getSelectedId();
  changeUniverse(uni - 1);
}

void DMXPatchViewer::changeUniverse(int uni) {
  _dm->setUniverse(uni);
  _dm->reinit();
}