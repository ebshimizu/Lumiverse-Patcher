/*
  ==============================================================================

    DeviceTable.cpp
    Created: 6 Jan 2015 3:46:25pm
    Author:  eshimizu

  ==============================================================================
*/

#include "../../JuceLibraryCode/JuceHeader.h"
#include "DeviceTable.h"
#include "../Main.h"

//==============================================================================
DeviceTable::DeviceTable()
{
  addAndMakeVisible(_table);
  _table.setModel(this);
  _table.setColour(ListBox::backgroundColourId, Colour(0xff0c0c0c));

  init();
}

DeviceTable::~DeviceTable()
{
}

void DeviceTable::init() {
  // Load data from the rig
  auto& rigRef = MainWindow::getRig();

  DeviceSet allDevices = rigRef->getAllDevices();
  vector<std::string> allIds = allDevices.getIds();

  for (auto id : allIds) {
    _ids.add(id);
  }

  _numRows = _ids.size();

  auto paramNames = rigRef->getAllUsedParams();

  _cols.add("ID");
  _cols.add("Channel");
  _cols.add("Universe");
  _cols.add("Address");

  _numCols = _cols.size();

  for (int i = 0; i < _cols.size(); i++) {
    _table.getHeader().addColumn(_cols[i], i + 1, 50);
  }

  _table.getHeader().setSortColumnId(2, true); // sort forwards by the channel column be default right now
  _table.setMultipleSelectionEnabled(true);

  for (int i = 0; i < _cols.size(); i++) {
    _table.getHeader().setColumnWidth(i + 1, getColumnAutoSizeWidth(i + 1));
  }
}

void DeviceTable::reinit() {
  // Reset data to clean
  _table.getHeader().removeAllColumns();
  _ids.clear();
  _cols.clear();
  _numRows = _ids.size();
  _numCols = _cols.size();

  init();
  repaint();
}


void DeviceTable::paintRowBackground(Graphics& g, int rowNumber, int width, int height, bool rowIsSelected) {
  if (rowIsSelected)
  {
    g.fillAll(Colour(0xff2771f2));
  }
  else {
    g.fillAll(Colour(0xff0c0c0c));
  }
}

void DeviceTable::paintCell(Graphics& g, int rowNumber, int columnId, int width, int height, bool rowIsSelected) {
  Font font(14.0f);
  g.setFont(font);

  Device* device;
  auto& rigRef = MainWindow::getRig();

  device = rigRef->getDevice(_ids[rowNumber].toStdString());

  // There's some weird stuff going on when a resize of the window happens.
  // Abort if we don't actually have a device.
  if (device == nullptr)
    return;

  g.setColour(Colours::white);

  // ID, Channel, Universe, Address, Patch, Additional Patch Info
  if (columnId == 1) {
    g.drawText(_ids[rowNumber], 2, 0, width - 4, height, juce::Justification::left, true);
  }
  else if (columnId == 2) {
    g.drawText(String(device->getChannel()), 2, 0, width - 4, height, juce::Justification::left, true);
  }
  else if (columnId == 3) {
    int universe = getUniverse(_ids[rowNumber].toStdString());
    if (universe == -1) {
      g.drawText("-", 2, 0, width - 4, height, juce::Justification::left, true);
    }
    else {
      g.drawText(String(universe + 1), 2, 0, width - 4, height, juce::Justification::left, true);
    }
  }
  else if (columnId == 4) {
    int addr = getAddress(_ids[rowNumber].toStdString());
    if (addr == -1) {
      g.drawText("-", 2, 0, width - 4, height, juce::Justification::left, true);
    }
    else {
      g.drawText(String(addr + 1), 2, 0, width - 4, height, juce::Justification::left, true);
    }
  }
}

void DeviceTable::sortOrderChanged(int newSortColumnId, bool isForwards) {
  // Currently only IDs will get sorted while I see how this works.
  auto& rigRef = MainWindow::getRig();

  if (newSortColumnId == 1) {
    // Id
    _ids.sortNatural();
  }
  else if (newSortColumnId == 2) {
    // Channel
    // This is like some relational database nonsense here...
    // Because I don't want to spend the time to make a nice sorting thing (not the
    // point of this right now!) we're gonna stick things in multimaps and extract the order
    // from there.

    multimap<unsigned int, String> chanToId;

    // Don't have to check if using programmer or rig here since this part is identical between them.
    for (auto id : _ids) {
      unsigned int channel = rigRef->getDevice(id.toStdString())->getChannel();
      chanToId.insert(std::pair<unsigned int, String>(channel, id));
    }

    StringArray sortedIds;
    // Extract the sorted keys from the multimap.
    for (auto kvp : chanToId) {
      sortedIds.add(kvp.second);
    }

    _ids.swapWith(sortedIds);
  }
  else if (newSortColumnId == 3 || newSortColumnId == 4) {
    multimap<unsigned int, String> addrToId;

    // Don't have to check if using programmer or rig here since this part is identical between them.
    for (auto id : _ids) {
      unsigned int channel = getUniverse(id.toStdString()) * 512 + getAddress(id.toStdString()) + 1;
      addrToId.insert(std::pair<unsigned int, String>(channel, id));
    }

    StringArray sortedIds;
    // Extract the sorted keys from the multimap.
    for (auto kvp : addrToId) {
      sortedIds.add(kvp.second);
    }

    _ids.swapWith(sortedIds);
  }

  if (!isForwards)
  {
    StringArray revIds;
    // Reverse the ids
    for (int i = _ids.size() - 1; i >= 0; i--) {
      revIds.add(_ids[i]);
    }

    _ids.swapWith(revIds);
  }

  _table.updateContent();
}

void DeviceTable::selectedRowsChanged(int lastRowSelected) {
  // Get the list of things selected in the table.
  //StringArray ids = getSelectedIds();

  // Replace them in the device set.
  //DeviceSet newSelection(m_rig.get());

  //for (String id : _ids) {
    // Query system will pick this up as a single id.
  //  newSelection = newSelection.add(id.toStdString());
  //}

  //m_mc->setActiveSelection(newSelection);

  // Redirect to the main selection changed function after updating the selected set.
}

Component* DeviceTable::refreshComponentForCell(int rowNumber, int columnId,
  bool isRowSelected, Component* existingComponentToUpdate)
{
  // For use in the future
  return 0;
}

int DeviceTable::getColumnAutoSizeWidth(int columnId)
{
  Font headerFont(18.0);
  Font font(14);
  int widest = headerFont.getStringWidth(_cols[columnId - 1]);

  auto& rigRef = MainWindow::getRig();

  // find the widest bit of text in this column..
  for (int i = 0; i < getNumRows(); i++)
  {
    Device* device = rigRef->getDevice(_ids[i].toStdString());

    if (columnId == 1) {
      const String text(_ids[i]);
      widest = jmax(widest, font.getStringWidth(text));
    }
  }

  return widest + 8;
}

void DeviceTable::paint (Graphics& g)
{
    /* This demo code just fills the component's background and
       draws some placeholder text to get you started.

       You should replace everything in this method with your own
       drawing code..
    */

    g.fillAll (Colour(0xff0c0c0c));   // clear the background
}

void DeviceTable::resized()
{
  // This method is where you should set the bounds of any child
  // components that your component contains..
  _table.setBoundsInset(BorderSize<int>(0));
}

int DeviceTable::getUniverse(string deviceID) {
  auto& rigRef = MainWindow::getRig();

  // Search patches
  for (auto p : rigRef->getPatches()) {
    if (p.second->getType() == "DMXPatch") {
      // only look at DMX Patches for now.
      DMXPatch* patch = dynamic_cast<DMXPatch*>(p.second);
      if (patch == nullptr) continue;

      DMXDevicePatch* info = patch->getDevicePatch(deviceID);
      if (info == nullptr) continue;

      return info->getUniverse();
    }
  }

  // No universe assigned = -1
  return -1;
}

int DeviceTable::getAddress(string deviceID) {
  auto& rigRef = MainWindow::getRig();

  // Search patches
  for (auto p : rigRef->getPatches()) {
    if (p.second->getType() == "DMXPatch") {
      // only look at DMX Patches for now.
      DMXPatch* patch = dynamic_cast<DMXPatch*>(p.second);
      if (patch == nullptr) continue;

      DMXDevicePatch* info = patch->getDevicePatch(deviceID);
      if (info == nullptr) continue;

      return info->getBaseAddress();
    }
  }

  // No universe assigned = -1
  return -1;
}