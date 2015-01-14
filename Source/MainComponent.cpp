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
}

MainContentComponent::~MainContentComponent()
{
  if (_profileEditorWindow != nullptr) {
    _profileEditorWindow.deleteAndZero();
  }
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
    MainWindow::addPatch
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
