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

  addAndMakeVisible(_addInterface = new TextButton("Add Interface", "Add a new Interface to the Patch"));
  _addInterface->addListener(this);
  _addInterface->setName("addInterface");

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
  _addInterface->setBounds(top.removeFromLeft(75).reduced(3));

  _dm->setBounds(area);
}

void DMXPatchViewer::comboBoxChanged(ComboBox* box) {
  int uni = box->getSelectedId();
  changeUniverse(uni - 1);
}

void DMXPatchViewer::buttonClicked(Button* b) {
  if (b->getName() == "addInterface") {
    addInterface();
  }
}

void DMXPatchViewer::addInterface() {
  juce::AlertWindow w("Add Interface",
    "Add an Interface to the patch",
    juce::AlertWindow::QuestionIcon);

  StringArray types;
  types.add("Art-Net");
  types.add("KiNet");
  types.add("ENTTEC USB DMX PRO Mk II");

  w.addTextEditor("name", "", "Name");
  w.addComboBox("types", types, "Interface Type");

  w.addButton("Add", 1, KeyPress(KeyPress::returnKey, 0, 0));
  w.addButton("Cancel", 0, KeyPress(KeyPress::escapeKey, 0, 0));

  if (w.runModalLoop() != 0) // is they picked 'add'
  {
    MainWindow::getRig()->stop();

    // this is the item they chose in the drop-down list..
    const int typeChosen = w.getComboBoxComponent("types")->getSelectedItemIndex();
    string name = w.getTextEditor("name")->getText().toStdString();

    if (MainWindow::getRig()->getPatchAsDMXPatch(_patchID)->getInterface(name) != nullptr) {
      juce::AlertWindow::showMessageBoxAsync(juce::AlertWindow::WarningIcon,
        "Unable to add Interface",
        "An interface with the name \"" + name + "\" already exists.",
        "OK");
      return;
    }

    if (typeChosen == 0) {
      // Art-Net
      addArtNetInterface(name);
    }
    else if (typeChosen == 1) {
      // Kinet
      addKiNetInterface(name);
    }
    else if (typeChosen == 2) {
      // ENTTEC USB DMX PRO
      addDMXPro2Interface(name);
    }

    MainWindow::getRig()->init();
    MainWindow::getRig()->run();
  }
}

void DMXPatchViewer::addArtNetInterface(string name) {
  juce::AlertWindow w("Add Art-Net Interface",
    "Additional Art-Net Settings",
    juce::AlertWindow::QuestionIcon);

  w.addTextEditor("ip", "", "IP Address");
  w.addTextEditor("broadcast", "", "Broadcast IP (optional)");

  w.addButton("Add", 1, KeyPress(KeyPress::returnKey, 0, 0));
  w.addButton("Cancel", 0, KeyPress(KeyPress::escapeKey, 0, 0));

  if (w.runModalLoop() != 0) // is they picked 'add'
  {
    string ip = w.getTextEditor("ip")->getText().toStdString();
    string broadcast = w.getTextEditor("broadcast")->getText().toStdString();

    ArtNetInterface* iface = new ArtNetInterface(name, ip, broadcast);

    // This should succeed since the check from addInterface already checked for duplicate interfaces.
    MainWindow::getRig()->getPatchAsDMXPatch(_patchID)->addInterface((DMXInterface*)iface);
  }
}

void DMXPatchViewer::addKiNetInterface(string name) {
  juce::AlertWindow w("Add KiNet Interface",
    "Additional KiNet Settings",
    juce::AlertWindow::QuestionIcon);

  w.addTextEditor("ip", "", "Power Supply IP");
  w.addTextEditor("port", "6038", "Port (default: 6038)");

  w.addButton("Add", 1, KeyPress(KeyPress::returnKey, 0, 0));
  w.addButton("Cancel", 0, KeyPress(KeyPress::escapeKey, 0, 0));

  if (w.runModalLoop() != 0) // is they picked 'add'
  {
    string ip = w.getTextEditor("ip")->getText().toStdString();
    int port = w.getTextEditor("port")->getText().getIntValue();

    KiNetInterface* iface = new KiNetInterface(name, ip, port);

    // This should succeed since the check from addInterface already checked for duplicate interfaces.
    MainWindow::getRig()->getPatchAsDMXPatch(_patchID)->addInterface((DMXInterface*)iface);
  }
}

void DMXPatchViewer::addDMXPro2Interface(string name) {
  juce::AlertWindow w("Add ENTTEC USB DMX PRO MK 2 Interface",
    "Additional ENTTEC USB Settings",
    juce::AlertWindow::QuestionIcon);

  w.addTextEditor("pronum", "0", "Device Number");

  w.addButton("Add", 1, KeyPress(KeyPress::returnKey, 0, 0));
  w.addButton("Cancel", 0, KeyPress(KeyPress::escapeKey, 0, 0));

  if (w.runModalLoop() != 0) // is they picked 'add'
  {
    int proNum = w.getTextEditor("pronum")->getText().getIntValue();

    DMXPro2Interface* iface = new DMXPro2Interface(name, proNum);

    // This should succeed since the check from addInterface already checked for duplicate interfaces.
    MainWindow::getRig()->getPatchAsDMXPatch(_patchID)->addInterface((DMXInterface*)iface);
  }
}

void DMXPatchViewer::changeUniverse(int uni) {
  _dm->setUniverse(uni);
  _dm->reinit();
}