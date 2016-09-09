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

  addAndMakeVisible(_editInterface = new TextButton("Edit Interface", "Edit an existing Interface"));
  _editInterface->addListener(this);
  _editInterface->setName("editInterface");

  addAndMakeVisible(_deleteInterface = new TextButton("Delete Interface", "Delete an existing Interface"));
  _deleteInterface->addListener(this);
  _deleteInterface->setName("deleteInterface");

  addAndMakeVisible(_addUniverse = new TextButton("Add Universe", "Add a Universe"));
  _addUniverse->addListener(this);
  _addUniverse->setName("addUniverse");

  addAndMakeVisible(_editUniverse = new TextButton("Edit Universe", "Edit a Universe"));
  _editUniverse->addListener(this);
  _editUniverse->setName("editUniverse");

  addAndMakeVisible(_deleteUniverse = new TextButton("Delete Universe", "Deletes a Universe"));
  _deleteUniverse->addListener(this);
  _deleteUniverse->setName("deleteUniverse");

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
  
  if (patch == nullptr || patch->getType() != "DMXPatch")
    return;

  DMXPatch* p = dynamic_cast<DMXPatch*>(patch);
  
  // We only show you stuff if there's an actual interface assigned to it.
  for (auto i : p->getInterfaceInfo()) {
    String text = String(i.second + 1) + ": " + i.first;

    if (p->getInterface(i.first)->getInterfaceType() == "DMXPro2Interface") {
      DMXPro2Interface* iface = (DMXPro2Interface*)p->getInterface(i.first);
      if (iface->getOut1Universe() == i.second) {
        text = text + " (Output 1)";
      }
      else if (iface->getOut2Universe() == i.second) {
        text = text + " (Output 2)";
      }
      else {
        text = text + " (Error: Not Enough Outputs)";
      }
    }

    _universeList.addItem(text, i.second + 1);
  }

  _universeList.setSelectedItemIndex(0);
  changeUniverse(-1);
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
  _addInterface->setBounds(top.removeFromLeft(100).reduced(3));
  _editInterface->setBounds(top.removeFromLeft(100).reduced(3));
  _deleteInterface->setBounds(top.removeFromLeft(100).reduced(3));
  _addUniverse->setBounds(top.removeFromLeft(100).reduced(3));
  _editUniverse->setBounds(top.removeFromLeft(100).reduced(3));
  _deleteUniverse->setBounds(top.removeFromLeft(100).reduced(3));

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
  else if (b->getName() == "editInterface") {
    int res = getInterfaceList().showMenu(PopupMenu::Options().withTargetComponent(b));
    vector<string> ids = MainWindow::getRig()->getPatchAsDMXPatch(_patchID)->getInterfaceIDs();
    
    if (res != 0) {
      string id = ids[res - 1];
      DMXInterface* iface = MainWindow::getRig()->getPatchAsDMXPatch(_patchID)->getInterface(id);
      string type = iface->getInterfaceType();
      if (type == "DMXPro2Interface") {
        editDMXPro2Interface((DMXPro2Interface*)iface);
      }
      else if (type == "KiNetInterface") {
        editKiNetInterface((KiNetInterface*)iface);
      }
      else if (type == "ArtNetInterface") {
        editArtNetInterface((ArtNetInterface*)iface);
      }
    }
  }
  else if (b->getName() == "deleteInterface") {
    int res = getInterfaceList().showMenu(PopupMenu::Options().withTargetComponent(b));
    vector<string> ids = MainWindow::getRig()->getPatchAsDMXPatch(_patchID)->getInterfaceIDs();

    if (res != 0) {
      string id = ids[res - 1];

      juce::AlertWindow w("Delete Interface Field",
        "Are you sure you want to delete the interface " + id + "?",
        juce::AlertWindow::WarningIcon);

      w.addButton("Delete", 1, KeyPress(KeyPress::returnKey, 0, 0));
      w.addButton("Cancel", 0, KeyPress(KeyPress::escapeKey, 0, 0));

      if (w.runModalLoop() != 0) // is they picked 'delete'
      {
        MainWindow::getRig()->stop();

        MainWindow::getRig()->getPatchAsDMXPatch(_patchID)->deleteInterface(id);

        MainWindow::getRig()->init();
        MainWindow::getRig()->run();
        reload();
      }
    }
  }
  else if (b->getName() == "addUniverse") {
    addUniverse();
  }
  else if (b->getName() == "editUniverse") {
    int res = getUniverseList().showMenu(PopupMenu::Options().withTargetComponent(b));

    int index = 1;
    for (auto i : MainWindow::getRig()->getPatchAsDMXPatch(_patchID)->getInterfaceInfo()) {
      if (index == res) {
        editUniverse(i.first, i.second);
        return;
      }
      index++;
    }
  }
  else if (b->getName() == "deleteUniverse") {
    int res = getUniverseList().showMenu(PopupMenu::Options().withTargetComponent(b));

    if (res != 0) {
      string id = "";
      int univ = -1;
      
      int index = 1;
      for (auto i : MainWindow::getRig()->getPatchAsDMXPatch(_patchID)->getInterfaceInfo()) {
        if (index == res) {
          id = i.first;
          univ = i.second;
          break;
        }
        index++;
      }

      juce::AlertWindow w("Delete Universe " + String(univ + 1),
        "Are you sure you want to delete this Universe?",
        juce::AlertWindow::WarningIcon);

      w.addButton("Delete", 1, KeyPress(KeyPress::returnKey, 0, 0));
      w.addButton("Cancel", 0, KeyPress(KeyPress::escapeKey, 0, 0));

      if (w.runModalLoop() != 0) // is they picked 'delete'
      {
        MainWindow::getRig()->stop();

        MainWindow::getRig()->getPatchAsDMXPatch(_patchID)->removeInterface(univ, id);

        MainWindow::getRig()->init();
        MainWindow::getRig()->run();
        reload();
      }
    }
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

void DMXPatchViewer::addUniverse() {
  vector<string> ids = MainWindow::getRig()->getPatchAsDMXPatch(_patchID)->getInterfaceIDs();
  if (ids.size() == 0) {
    juce::AlertWindow::showMessageBoxAsync(juce::AlertWindow::WarningIcon,
      "Unable to add Universe",
      "At least one interface must be defined to add a universe.",
      "OK");
    return;
  }

  juce::AlertWindow w("Add Universe",
    "Add an Interface to the patch",
    juce::AlertWindow::QuestionIcon);

  StringArray interfaces;
  for (const auto& id : ids) {
    interfaces.add(id);
  }

  w.addTextEditor("number", "", "Universe Number");
  w.addComboBox("id", interfaces, "Interface Name");

  w.addButton("Add", 1, KeyPress(KeyPress::returnKey, 0, 0));
  w.addButton("Cancel", 0, KeyPress(KeyPress::escapeKey, 0, 0));

  if (w.runModalLoop() != 0) // is they picked 'add'
  {
    MainWindow::getRig()->stop();

    // this is the item they chose in the drop-down list..
    int ifaceIndex = w.getComboBoxComponent("id")->getSelectedItemIndex();
    int universe = w.getTextEditor("number")->getText().getIntValue() - 1;

    DMXInterface* iface = MainWindow::getRig()->getPatchAsDMXPatch(_patchID)->getInterface(ids[ifaceIndex]);

    if (iface->getInterfaceType() == "DMXPro2Interface") {
      // Additional options for PRO2
      StringArray output;
      output.add("1");
      output.add("2");

      juce::AlertWindow w2("DMX Pro 2 Settings", "Select an output for the universe.", juce::AlertWindow::QuestionIcon);
      w2.addComboBox("out", output, "Output Number");
      w2.addButton("Add", 1, KeyPress(KeyPress::returnKey, 0, 0));
      w2.addButton("Cancel", 0, KeyPress(KeyPress::escapeKey, 0, 0));

      if (w2.runModalLoop() != 0)
      {
        int out = w2.getComboBoxComponent("out")->getSelectedItemIndex();
        DMXPro2Interface* piface = (DMXPro2Interface*)iface;
        if (out == 0) {
          piface->setOut1Universe(universe);
        }
        else {
          piface->setOut2Universe(universe);
        }
      }
      else {
        return;
      }
    }

    MainWindow::getRig()->getPatchAsDMXPatch(_patchID)->assignInterface(iface, universe);

    MainWindow::getRig()->init();
    MainWindow::getRig()->run();

    reload();
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

void DMXPatchViewer::editArtNetInterface(ArtNetInterface* iface) {
  juce::AlertWindow w("Edit Art-Net Interface",
    "Edit Art-Net Settings",
    juce::AlertWindow::QuestionIcon);

  w.addTextEditor("ip", iface->getIP(), "IP Address");
  w.addTextEditor("broadcast", iface->getBroadcast(), "Broadcast IP (optional)");

  w.addButton("Save", 1, KeyPress(KeyPress::returnKey, 0, 0));
  w.addButton("Cancel", 0, KeyPress(KeyPress::escapeKey, 0, 0));

  if (w.runModalLoop() != 0) // is they picked 'add'
  {
    MainWindow::getRig()->stop();

    iface->closeInt();

    string ip = w.getTextEditor("ip")->getText().toStdString();
    string broadcast = w.getTextEditor("broadcast")->getText().toStdString();

    iface->setIP(ip);
    iface->setBroadcast(broadcast);

    MainWindow::getRig()->init();
    MainWindow::getRig()->run();
  }
}

void DMXPatchViewer::editKiNetInterface(KiNetInterface* iface) {
  juce::AlertWindow w("Edit KiNet Interface",
    "KiNet Settings",
    juce::AlertWindow::QuestionIcon);

  w.addTextEditor("ip", iface->getIP(), "Power Supply IP");
  w.addTextEditor("port", String(iface->getPort()), "Port (default: 6038)");

  w.addButton("Save", 1, KeyPress(KeyPress::returnKey, 0, 0));
  w.addButton("Cancel", 0, KeyPress(KeyPress::escapeKey, 0, 0));

  if (w.runModalLoop() != 0) // is they picked 'add'
  {
    MainWindow::getRig()->stop();

    iface->closeInt();

    string ip = w.getTextEditor("ip")->getText().toStdString();
    int port = w.getTextEditor("port")->getText().getIntValue();

    iface->setIP(ip);
    iface->setPort(port);

    MainWindow::getRig()->init();
    MainWindow::getRig()->run();
  }
}

void DMXPatchViewer::editDMXPro2Interface(DMXPro2Interface* iface) {
  juce::AlertWindow w("Edit ENTTEC USB DMX PRO MK 2 Interface",
    "ENTTEC USB Settings",
    juce::AlertWindow::QuestionIcon);

  w.addTextEditor("pronum", String(iface->getProNum()), "Device Number");

  w.addButton("Save", 1, KeyPress(KeyPress::returnKey, 0, 0));
  w.addButton("Cancel", 0, KeyPress(KeyPress::escapeKey, 0, 0));

  if (w.runModalLoop() != 0) // is they picked 'add'
  {
    MainWindow::getRig()->stop();

    iface->closeInt();

    int proNum = w.getTextEditor("pronum")->getText().getIntValue();

    iface->setProNum(proNum);

    MainWindow::getRig()->init();
    MainWindow::getRig()->run();
  }
}

void DMXPatchViewer::editUniverse(string name, int univ) {
  vector<string> ids = MainWindow::getRig()->getPatchAsDMXPatch(_patchID)->getInterfaceIDs();

  juce::AlertWindow w("Edit Universe " + String(univ + 1),
    "Change interface assigned to this universe",
    juce::AlertWindow::QuestionIcon);

  StringArray interfaces;
  for (const auto& id : ids) {
    interfaces.add(id);
  }

  w.addComboBox("id", interfaces, "Interface Name");
  w.getComboBoxComponent("id")->setSelectedId(interfaces.indexOf(String(name)) + 1);

  w.addButton("Save", 1, KeyPress(KeyPress::returnKey, 0, 0));
  w.addButton("Cancel", 0, KeyPress(KeyPress::escapeKey, 0, 0));

  if (w.runModalLoop() != 0) // is they picked 'add'
  {
    MainWindow::getRig()->stop();

    // this is the item they chose in the drop-down list..
    int ifaceIndex = w.getComboBoxComponent("id")->getSelectedItemIndex();
    DMXInterface* iface = MainWindow::getRig()->getPatchAsDMXPatch(_patchID)->getInterface(ids[ifaceIndex]);

    if (iface->getInterfaceType() == "DMXPro2Interface") {
      // Additional options for PRO2
      StringArray output;
      output.add("1");
      output.add("2");

      juce::AlertWindow w2("DMX Pro 2 Settings", "Select an output for the universe.", juce::AlertWindow::QuestionIcon);
      w2.addComboBox("out", output, "Output Number");
      w2.addButton("Save", 1, KeyPress(KeyPress::returnKey, 0, 0));
      w2.addButton("Cancel", 0, KeyPress(KeyPress::escapeKey, 0, 0));

      if (w2.runModalLoop() != 0)
      {
        int out = w2.getComboBoxComponent("out")->getSelectedItemIndex();
        DMXPro2Interface* piface = (DMXPro2Interface*)iface;
        if (out == 0) {
          piface->setOut1Universe(univ);
        }
        else {
          piface->setOut2Universe(univ);
        }
      }
      else {
        return;
      }
    }

    // Remove old interface from universe
    MainWindow::getRig()->getPatchAsDMXPatch(_patchID)->removeInterface(univ, name);
    // Assign new interface to universe
    MainWindow::getRig()->getPatchAsDMXPatch(_patchID)->assignInterface(iface, univ);

    MainWindow::getRig()->init();
    MainWindow::getRig()->run();

    reload();
  }
}

PopupMenu DMXPatchViewer::getInterfaceList() {
  vector<string> ids = MainWindow::getRig()->getPatchAsDMXPatch(_patchID)->getInterfaceIDs();
  PopupMenu m;
  int i = 1;
  for (const auto& id : ids) {
    m.addItem(i, id);
    i++;
  }

  return m;
}

PopupMenu DMXPatchViewer::getUniverseList() {
  StringArray univ;
  PopupMenu univMenu;
  
  int index = 1;
  for (auto i : MainWindow::getRig()->getPatchAsDMXPatch(_patchID)->getInterfaceInfo()) {
    univMenu.addItem(index, String(i.second + 1) + ": " + i.first);
    index++;
  }

  return univMenu;
}

void DMXPatchViewer::changeUniverse(int uni) {
  _dm->setUniverse(uni);
  _dm->reinit();
}