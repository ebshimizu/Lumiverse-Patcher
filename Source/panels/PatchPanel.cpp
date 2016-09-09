/*
  ==============================================================================

    PatchPanel.cpp
    Created: 6 Jan 2015 3:22:02pm
    Author:  eshimizu

  ==============================================================================
*/

#include "../../JuceLibraryCode/JuceHeader.h"
#include "PatchPanel.h"
#include "../Main.h"
#include "../components/ArnoldAnimationPatchViewer.h"

//==============================================================================
PatchPanel::PatchPanel()
{
  // In your constructor, you should add any child components, and
  // initialise any special settings that your component needs.

  addAndMakeVisible(_tc = new TabbedComponent(TabbedButtonBar::Orientation::TabsAtTop));
  reload();
}

PatchPanel::~PatchPanel()
{
  _tc = nullptr;
}

void PatchPanel::reload() {
  _tc->clearTabs();
  
  auto& rigRef = MainWindow::getRig();

  for (const auto& p : rigRef->getPatches()) {
    if (p.second->getType() == "DMXPatch") {
      _tc->addTab(p.first, Colour(0xff000040), new DMXPatchViewer(p.first), true);
    }
    else if (p.second->getType() == "ArnoldAnimationPatch") {
      _tc->addTab(p.first, Colour(0xff000040), new ArnoldAnimationPatchViewer(p.first), true);
    }
  }
}

void PatchPanel::paint (Graphics& g)
{
    /* This demo code just fills the component's background and
       draws some placeholder text to get you started.

       You should replace everything in this method with your own
       drawing code..
    */

    g.fillAll (Colour(0xff0c0c0c));   // clear the background
}

void PatchPanel::resized()
{
  // This method is where you should set the bounds of any child
  // components that your component contains..
  auto area = getLocalBounds();
  
  _tc->setBoundsInset(BorderSize<int>(0));
}
