/*
  ==============================================================================

    DMXMap.cpp
    Created: 7 Jan 2015 3:48:06pm
    Author:  eshimizu

  ==============================================================================
*/

#include "../../JuceLibraryCode/JuceHeader.h"
#include "DMXMap.h"
#include "../Main.h"

//==============================================================================
DMXWidget::DMXWidget(int addr) : _addr(addr) {
  _status = FREE;
}

DMXWidget::~DMXWidget() {

}

void DMXWidget::paint(Graphics& g) {
  switch (_status) {
  case (FREE) :
    g.fillAll(Colour(0xff0c0c0c));
    g.setColour(Colours::white);
    break;
  case (USED) :
    g.fillAll(Colour(0xfffffbdb));
    g.setColour(Colours::black);
    break;
  case (CONFLICT) :
    g.fillAll(Colour(0xffff4d4d));
    g.setColour(Colours::black);
    break;
  }

  g.setFont(10);
  g.drawText(String(_addr + 1), getLocalBounds(), Justification::centred, false);
}

void DMXWidget::resized() {

}

void DMXWidget::setUsed() {
  if (_status == USED) {
    _status = CONFLICT;
  }
  else if (_status == FREE) {
    _status = USED;
  }
}

void DMXWidget::setFree() {
  _status = FREE;
}

void DMXWidget::setConflict() {
  _status = CONFLICT;
}

//==============================================================================
DMXMap::DMXMap(string patchID, int universe) : _patchID(patchID), _uni(universe)
{
  // In your constructor, you should add any child components, and
  // initialise any special settings that your component needs.

  for (int i = 0; i < 512; i++) {
    auto w = new DMXWidget(i);
    w->setFree();
    addAndMakeVisible(w);
    _addrs.push_back(w);
  }

  reinit();
}

DMXMap::~DMXMap()
{
  for (auto d : _addrs) {
    delete d;
  }
  _addrs.clear();
}

void DMXMap::reinit() {
  for (auto a : _addrs) {
    a->setFree();
  }

  auto& rigRef = MainWindow::getRig();

  // Find the DMX info associated with the selected universe.
  auto patch = rigRef->getPatch(_patchID);

  if (patch == nullptr) return;

  assert(patch->getType() == "DMXPatch");

  DMXPatch* p = dynamic_cast<DMXPatch*>(patch);

  DeviceSet allDevices = rigRef->getAllDevices();
  for (auto d : allDevices.getDevices()) {
    DMXDevicePatch* data = p->getDevicePatch(d->getId());
    if (data == nullptr) continue;

    if (data->getUniverse() == _uni) {
      int addrStart = data->getBaseAddress();
      int addrEnd = addrStart + p->sizeOfDeviceMap(data->getDMXMapKey()); // Exclusive upper bound on used addresses.

      for (int i = addrStart; i < addrEnd; i++) {
        _addrs[i]->setUsed();
      }
    }
  }

  repaint();
}

void DMXMap::paint (Graphics& g)
{
    /* This demo code just fills the component's background and
       draws some placeholder text to get you started.

       You should replace everything in this method with your own
       drawing code..
    */

    g.fillAll (Colour(0xff0c0c0c));   // clear the background
}

void DMXMap::resized()
{
  int gridWidth = 32;
  int gridHeight = 16;

  auto area = getLocalBounds();
  float width = (float)area.getWidth() / (float)gridWidth;
  float height = (float)area.getHeight() / (float)gridHeight;

  // DMX widgets are in a 32 x 16 grid  
  for (int x = 0; x < 32; x++) {
    for (int y = 0; y < 16; y++) {
      _addrs[y * gridWidth + x]->setBounds(x * width, y * height, width, height);
    }
  }
}
