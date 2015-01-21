/*
  ==============================================================================

    This file was auto-generated!

  ==============================================================================
*/

#include "MainComponent.h"
#include "Main.h"
#include "panels/ProfileEditor.h"

#include "LumiverseCore.h"
using namespace Lumiverse;

 //==============================================================================
MainContentComponent::MainContentComponent()
{
  addAndMakeVisible(m_dp = new DevicePanel());
  addAndMakeVisible(m_pp = new PatchPanel());
  addAndMakeVisible(m_rb = new StretchableLayoutResizerBar(&m_layout, 1, false));

  m_layout.setItemLayout(0, -0.25, -0.75, -0.65);
  m_layout.setItemLayout(1, 3, 3, 3);
  m_layout.setItemLayout(2, -0.25, -0.75, -0.35);

  setSize (1024, 768);

  loadProfiles();
}

MainContentComponent::~MainContentComponent()
{
  if (_profileEditorWindow != nullptr) {
    _profileEditorWindow.deleteAndZero();
  }

  deleteProfiles();
}

void MainContentComponent::paint (Graphics& g)
{
  g.fillAll (Colour (0xff0c0c0c));
}

void MainContentComponent::resized()
{
  // This is called when the MainContentComponent is resized.
  // If you add any child components, this is where you should
  // update their positions.
  juce::Rectangle<int> area(getLocalBounds());

  Component* comps[] = { m_dp, m_rb, m_pp };
  m_layout.layOutComponents(comps, 3, 0, 0, area.getWidth(), area.getHeight(), true, true);
}

//================================================================================
//Application Command Manager overrides

ApplicationCommandTarget* MainContentComponent::getNextCommandTarget()
{
  // this will return the next parent component that is an ApplicationCommandTarget (in this
  // case, there probably isn't one, but it's best to use this method in your own apps).
  return findFirstTargetParentComponent();
}

void MainContentComponent::getAllCommands(Array<CommandID>& commands)
{
  // this returns the set of all commands that this target can perform..
  const CommandID ids[] = {
    MainWindow::open, MainWindow::save, MainWindow::saveAs, MainWindow::openProfileEditor,
    MainWindow::addPatch, MainWindow::deletePatch, MainWindow::loadProfiles, MainWindow::setProfileLocation,
    MainWindow::addDevices, MainWindow::updateSelection, MainWindow::deleteDevices
  };

  commands.addArray(ids, numElementsInArray(ids));
}

void MainContentComponent::getCommandInfo(CommandID commandID, ApplicationCommandInfo& result)
{
  const String generalCategory("General");
  const String deviceCategory("Device");
  const String patchCategory("Patch");

  switch (commandID)
  {
  case MainWindow::open:
    result.setInfo("Open...", "Open a Lumiverse Rig.", generalCategory, 0);
    result.addDefaultKeypress('o', ModifierKeys::commandModifier);
    break;
  case MainWindow::save:
    result.setInfo("Save", "Saves the current Rig.", generalCategory, 0);
    result.addDefaultKeypress('s', ModifierKeys::commandModifier);
    break;
  case MainWindow::saveAs:
    result.setInfo("Save As...", "Saves the current Rig under a new name.", generalCategory, 0);
    result.addDefaultKeypress('s', ModifierKeys::commandModifier | ModifierKeys::shiftModifier);
    break;
  case MainWindow::openProfileEditor:
    result.setInfo("Open Profile Editor", "Opens the Profile Editor window.", deviceCategory, 0);
    result.addDefaultKeypress(KeyPress::F2Key, ModifierKeys::noModifiers);
    break;
  case MainWindow::addPatch:
    result.setInfo("Add Patch...", "Adds a new patch to the rig.", patchCategory, 0);
    result.addDefaultKeypress('p', ModifierKeys::ctrlModifier);
    break;
  case MainWindow::deletePatch:
    result.setInfo("Delete Patch...", "Deletes a Patch from the Rig", patchCategory, 0);
    break;
  case MainWindow::loadProfiles:
    result.setInfo("Reload Profiles", "Reloads the profiles from the profiles directory", generalCategory, 0);
    result.addDefaultKeypress(KeyPress::F5Key, ModifierKeys::noModifiers);
    break;
  case MainWindow::setProfileLocation:
    result.setInfo("Set Profiles Folder", "Sets the location to read profiles from", generalCategory, 0);
    break;
  case MainWindow::addDevices:
    result.setInfo("Add Devices", "Adds devices to the Rig.", deviceCategory, 0);
    result.addDefaultKeypress('a', ModifierKeys::noModifiers);
    break;
  case MainWindow::deleteDevices:
    result.setInfo("Delete Devices", "Deletes the selected devices from the Rig", deviceCategory, 0);
    result.addDefaultKeypress('d', ModifierKeys::commandModifier);
    break;
  case MainWindow::updateSelection:
    result.setInfo("Update Selection", "updates the current set of selected devices.", "internal", 0);
    break;
  default:
    break;
  }
}

bool MainContentComponent::perform(const InvocationInfo& info)
{
  switch (info.commandID)
  {
  case MainWindow::open:
    open();
    break;
  case MainWindow::save:
    save();
    break;
  case MainWindow::saveAs:
    saveAs();
    break;
  case MainWindow::openProfileEditor:
    openProfileEditor();
    break;
  case MainWindow::addPatch:
    addPatch();
    break;
  case MainWindow::deletePatch:
    deletePatch();
    break;
  case MainWindow::loadProfiles:
    loadProfiles();
    break;
  case MainWindow::setProfileLocation:
    setProfileLocation();
    break;
  case MainWindow::addDevices:
    addDevices();
    break;
  case MainWindow::updateSelection:
    updateSelection();
    break;
  case MainWindow::deleteDevices:
    deleteDevices();
    break;
  default:
    return false;
  }

  return true;
}

//================================================================================
// Other MainContentComponent Functions
void MainContentComponent::reload() {
  m_dp->reload();
  m_pp->reload();
}

//================================================================================
//Application Command Manager callbacks

void MainContentComponent::open() {
  // Open file browser

  FileChooser fc("Load Lumiverse Rig",
    File::getCurrentWorkingDirectory(),
    "*.json",
    true);

  if (fc.browseForFileToOpen())
  {
    String chosen;
    chosen << fc.getResults().getReference(0).getFullPathName();

    File selected = fc.getResult();
    String fileName = selected.getFileName();
    fileName = fileName.upToFirstOccurrenceOf(".", false, false);
    m_parentDir = selected.getParentDirectory();

    bool res = MainWindow::getRig()->load(chosen.toStdString());

    if (res) {
      MainWindow::getRig()->init();
      MainWindow::getRig()->run();

      m_rigName = fileName.toStdString();
      MainWindow::getMainWindow()->setName("Lumiverse Patcher - " + m_rigName);
    }
    else
    {
      AlertWindow::showMessageBoxAsync(AlertWindow::WarningIcon,
        "Failed to Load selected rig file.",
        "File data may be in invalid format.",
        "OK");
    }

    // Reload various GUI elements
    reload();
  }
}

void MainContentComponent::save() {
  if (m_rigName == "") {
    // Redirect to save as if we don't have a show name...
    saveAs();
    return;
  }

  File rigFile = m_parentDir.getChildFile(String(m_rigName) + ".rig.json");
  bool rigRes = MainWindow::getRig()->save(rigFile.getFullPathName().toStdString(), true);

  if (!rigRes)
  {
    AlertWindow::showMessageBoxAsync(AlertWindow::WarningIcon, "Failed to save Rig.", "Try again or save under a different file name.", "OK");
  }
}

void MainContentComponent::saveAs() {
  FileChooser fc("Save as...",
    File::getCurrentWorkingDirectory(),
    "*.rig.json",
    true);

  if (fc.browseForFileToSave(true))
  {
    File selected = fc.getResult();
    String fileName = selected.getFileName();
    fileName = fileName.upToFirstOccurrenceOf(".", false, false);

    m_parentDir = selected.getParentDirectory();

    File rigFile = m_parentDir.getChildFile(fileName + ".rig.json");

    bool rigRes = MainWindow::getRig()->save(rigFile.getFullPathName().toStdString(), true);

    if (rigRes) {
      m_rigName = fileName.toStdString();
      MainWindow::getMainWindow()->setName("Lumiverse Patcher - " + m_rigName);
    }
    else {
      AlertWindow::showMessageBoxAsync(AlertWindow::WarningIcon, "Failed to save Rig.", "Try again or save under a different file name.", "OK");
    }
  }
}

void MainContentComponent::addPatch() {
  juce::AlertWindow w("Add Patch",
    "Add a Patch to the Rig",
    juce::AlertWindow::QuestionIcon);

  StringArray types;
  types.add("DMX");

  w.addTextEditor("name", "", "Patch ID");
  w.addComboBox("type", types, "Type");

  w.addButton("Add", 1, KeyPress(KeyPress::returnKey, 0, 0));
  w.addButton("Cancel", 0, KeyPress(KeyPress::escapeKey, 0, 0));

  if (w.runModalLoop() != 0) {
    string id = w.getTextEditor("name")->getText().toStdString();
    int type = w.getComboBoxComponent("type")->getSelectedItemIndex();

    // Check for duplicate patch name
    if (MainWindow::getRig()->getPatch(id) != nullptr) {
      juce::AlertWindow::showMessageBoxAsync(juce::AlertWindow::WarningIcon,
        "Unable to add Patch",
        "An Patch with the name \"" + id + "\" already exists.",
        "OK");
      return;
    }

    MainWindow::getRig()->stop();

    if (type == 0) {
      // DMX Patch
      DMXPatch* p = new DMXPatch();
      MainWindow::getRig()->addPatch(id, (Patch*)p);
    }

    MainWindow::getRig()->init();
    MainWindow::getRig()->run();
    reload();
  }
}

void MainContentComponent::deletePatch() {
  juce::AlertWindow w("Delete Patch",
    "Delete a Patch from the Rig",
    juce::AlertWindow::WarningIcon);

  StringArray patches;
  for (const auto& p : MainWindow::getRig()->getPatches()) {
    patches.add(p.first);
  }

  w.addComboBox("patch", patches, "Patch");

  w.addButton("Delete", 1, KeyPress(KeyPress::returnKey, 0, 0));
  w.addButton("Cancel", 0, KeyPress(KeyPress::escapeKey, 0, 0));

  if (w.runModalLoop() != 0) {
    int patch = w.getComboBoxComponent("patch")->getSelectedItemIndex();

    juce::AlertWindow w("Delete Patch",
      "Are you sure you want to delete this Patch? All Patch information will be deleted from the Rig.\nThis action is not undoable.",
      juce::AlertWindow::WarningIcon);

    w.addButton("Delete", 1, KeyPress(KeyPress::returnKey, 0, 0));
    w.addButton("Cancel", 0, KeyPress(KeyPress::escapeKey, 0, 0));

    if (w.runModalLoop() != 0) // is they picked 'delete'
    {
      MainWindow::getRig()->stop();

      string patchID = patches[patch].toStdString();
      MainWindow::getRig()->deletePatch(patchID);

      MainWindow::getRig()->init();
      MainWindow::getRig()->run();
      reload();
    }
  }
}

void MainContentComponent::loadProfiles() {
  deleteProfiles();
  (new ProfileLoader(this))->launchThread();
}

void MainContentComponent::deleteProfiles() {
  for (const auto& kvp : _deviceProfiles) {
    delete kvp.second;
  }
  _deviceProfiles.clear();
  _dmxProfiles.clear();
}

bool MainContentComponent::loadProfile(string filename) {
  // Check to see if we can load the file.
  ifstream data;
  data.open(filename, ios::in | ios::binary | ios::ate);

  if (data.is_open()) {
    // "+ 1" for the ending
    streamoff size = data.tellg();
    char* memblock = new char[(unsigned int)size + 1];

    data.seekg(0, ios::beg);

    data.read(memblock, size);
    data.close();

    // It's not guaranteed that the following memory after memblock is blank.
    // C-style string needs an end.
    memblock[size] = '\0';

    JSONNode n = libjson::parse(memblock);

    // Load
    // Profiles have two fields: template and dmxMap. Template is the device, dmxMap is the DMX map
    auto d = n.find("template");
    if (d == n.end()) {
      return false;
    }

    // load DMX map
    auto i = n.find("dmxMap");
    if (i == n.end()) {
      return false;
    }

    map <string, patchData> dmxMapData;
    auto j = i->begin();
    while (j != i->end()) {
      string paramName = j->name();

      // This assumes the next piece of data is arranged in a [ int, string ] format
      unsigned int addr = (*j)[0].as_int();
      string conversion = (*j)[1].as_string();

      dmxMapData[paramName] = patchData(addr, conversion);

      ++j;
    }

    // Make new device
    Device* newDevice = new Device("template", *d);

    // assign to proper location if not exist already
    if (_deviceProfiles.count(newDevice->getType()) != 0) {
      delete newDevice;
      delete memblock;
      return false;
    }

    _deviceProfiles[newDevice->getType()] = newDevice;
    _dmxProfiles[newDevice->getType()] = dmxMapData;

    delete memblock;
  }
  else {
    return false;
  }

  return true;
}

void MainContentComponent::setProfileLocation() {
  FileChooser fc("Select Profiles Folder",
    File(MainWindow::getPropertiesFile()->getValue("profilePath")),
    "", true);

  if (fc.browseForDirectory()) {
    String chosen = fc.getResults().getReference(0).getFullPathName();
    MainWindow::getPropertiesFile()->setValue("profilePath", chosen);
    loadProfiles();
    reload();
  }
}

void MainContentComponent::addDevices() {
  if (MainWindow::getRig()->getPatches().size() == 0) {
    juce::AlertWindow::showMessageBoxAsync(juce::AlertWindow::WarningIcon,
      "Unable to add Devices",
      "No patches are defined for the Rig",
      "OK");
    return;
  }

  juce::AlertWindow w("Add Devices",
    "Add devices to the Rig",
    juce::AlertWindow::QuestionIcon);

  StringArray profiles;
  for (const auto& kvp : _deviceProfiles) {
    profiles.add(kvp.first);
  }

  StringArray patches;
  for (const auto& kvp : MainWindow::getRig()->getPatches()) {
    patches.add(kvp.first);
  }

  w.addTextEditor("name", "", "Device Base ID");
  w.addTextEditor("channel", "", "Channel");
  w.addTextEditor("number", "1", "Quantity");
  w.addTextEditor("universe", "", "Universe");
  w.addTextEditor("addr", "", "Address");
  w.addComboBox("type", profiles, "Device Type");
  w.addComboBox("patch", patches, "Patch");

  w.addButton("Add", 1, KeyPress(KeyPress::returnKey, 0, 0));
  w.addButton("Cancel", 0, KeyPress(KeyPress::escapeKey, 0, 0));

  if (w.runModalLoop() != 0) {
    string id = w.getTextEditor("name")->getText().toStdString();
    string profileName = profiles[w.getComboBoxComponent("type")->getSelectedItemIndex()].toStdString();
    Patch* patch = MainWindow::getRig()->getPatch(patches[w.getComboBoxComponent("patch")->getSelectedItemIndex()].toStdString());
    int quantity = w.getTextEditor("number")->getText().getIntValue();
    int universe = w.getTextEditor("universe")->getText().getIntValue() - 1;
    int addr = w.getTextEditor("addr")->getText().getIntValue() - 1;
    int channel = w.getTextEditor("channel")->getText().getIntValue();

    MainWindow::getRig()->stop();

    // retrieve the profiles
    Device* p = _deviceProfiles[profileName];
    auto d = _dmxProfiles[profileName];

    for (int i = 0; i < quantity; i++) {
      // Add the device
      String newId = id + ((quantity > 1) ? String(i) : "");

      if (MainWindow::getRig()->getDevice(newId.toStdString()) != nullptr) {
        juce::AlertWindow::showMessageBoxAsync(juce::AlertWindow::WarningIcon,
          "Unable to add Device " + newId,
          "Device with this ID already exists in the Rig.",
          "OK");
        return;
      }
      
      Device* newDevice = new Device(newId.toStdString(), p);
      newDevice->setChannel(channel + i);
      MainWindow::getRig()->addDevice(newDevice);

      // Patch the device
      if (patch->getType() == "DMXPatch" && !w.getTextEditor("universe")->isEmpty() && !w.getTextEditor("addr")->isEmpty()) {
        DMXPatch* p = (DMXPatch*)patch;
        p->addDeviceMap(profileName, _dmxProfiles[profileName]);

        // Check to see if device fits in universe
        if (addr + p->sizeOfDeviceMap(profileName) > 512) {
          // wrap around to new universe.
          // note that we don't check conflicts. User is supposed to do that.
          universe++;
          addr = 0;
        }

        p->patchDevice(newId.toStdString(), new DMXDevicePatch(profileName, addr, universe));
        addr += p->sizeOfDeviceMap(profileName);
      }
    }

    MainWindow::getRig()->init();
    MainWindow::getRig()->run();
    reload();
  }
}

void MainContentComponent::updateSelection() {
  _currentSelection = m_dp->getSelectedDevices();
  m_dp->updateSelectedDevices(_currentSelection);
}

void MainContentComponent::deleteDevices() {
  juce::AlertWindow w("Delete Devices",
    "Are you sure you want to delete the selected Devices?",
    juce::AlertWindow::WarningIcon);

  w.addButton("Delete", 1, KeyPress(KeyPress::returnKey, 0, 0));
  w.addButton("Cancel", 0, KeyPress(KeyPress::escapeKey, 0, 0));

  if (w.runModalLoop() != 0) {
    MainWindow::getRig()->stop();

    for (const auto& d : _currentSelection.getDevices()) {
      MainWindow::getRig()->deleteDevice(d->getId());
    }

    reload();
    updateSelection();
    MainWindow::getRig()->init();
    MainWindow::getRig()->run();
  }
}

void MainContentComponent::openProfileEditor() {
  if (_profileEditorWindow == nullptr) {
    ProfileEditor* profileEditorWindow = new ProfileEditor("Profile Editor", Colours::white, DocumentWindow::allButtons);
    profileEditorWindow->centreWithSize(800, 600);
    profileEditorWindow->setResizable(true, false);
    profileEditorWindow->setUsingNativeTitleBar(false);
    profileEditorWindow->setVisible(true);

    _profileEditorWindow = profileEditorWindow;
  }
}

//==============================================================================

ProfileLoader::ProfileLoader(MainContentComponent* mc) : 
  ThreadWithProgressWindow("Loading Profiles", true, true), _mc(mc)
{
  setStatusMessage("Getting ready...");
}

void ProfileLoader::run()
{
  setProgress(-1.0); // setting a value beyond the range 0 -> 1 will show a spinning bar..

  // scan the number of files in the profiles directory.
  File profDir(MainWindow::getPropertiesFile()->getValue("profilePath"));
  Array<File> profiles;
  profDir.findChildFiles(profiles, File::TypesOfFileToFind::findFiles, true, "*.profile.json");

  const int profilesFound = profiles.size();
  int _profilesLoaded = 0;

  for (int i = 0; i < profilesFound; ++i)
  {
    // must check this as often as possible, because this is
    // how we know if the user's pressed 'cancel'
    if (threadShouldExit())
      return;

    // this will update the progress bar on the dialog box
    setProgress(i / (double)profilesFound);
    setStatusMessage("Loading " + profiles[i].getFileName());

    // Load the profile.
    bool res = _mc->loadProfile(profiles[i].getFullPathName().toStdString());
    if (res) _profilesLoaded++;

    wait(5);
  }

  setProgress(-1.0); // setting a value beyond the range 0 -> 1 will show a spinning bar..
  setStatusMessage("Loaded " + String(_profilesLoaded) + " profiles successfully. (found " + String(profilesFound) + ")");
  wait(3000);
}

// This method gets called on the message thread once our thread has finished..
void ProfileLoader::threadComplete(bool userPressedCancel)
{
  if (userPressedCancel)
  {
    AlertWindow::showMessageBoxAsync(AlertWindow::WarningIcon,
      "Load Profiles",
      "Profile load interrupted. Not all profiles have been loaded.");
  }

  // ..and clean up by deleting our thread object..
  delete this;
}