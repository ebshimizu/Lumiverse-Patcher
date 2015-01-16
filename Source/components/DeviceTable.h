/*
  ==============================================================================

    DeviceTable.h
    Created: 6 Jan 2015 3:46:25pm
    Author:  eshimizu

  ==============================================================================
*/

#ifndef DEVICETABLE_H_INCLUDED
#define DEVICETABLE_H_INCLUDED

#include "../../JuceLibraryCode/JuceHeader.h"
#include "LumiverseCore.h"

using namespace std;
using namespace Lumiverse;

//==============================================================================
/*
*/
class DeviceTable : public Component, public TableListBoxModel
{
public:
  DeviceTable();
  ~DeviceTable();

  void paint (Graphics&);
  void resized();

  void init();
  void reinit();

  // TableListBoxModel overrides

  int getNumRows() override { return _numRows; }
  void paintRowBackground(Graphics& g, int rowNumber, int width, int height, bool rowIsSelected) override;
  void paintCell(Graphics& g, int rowNumber, int columnId, int width, int height, bool rowIsSelected) override;
  void sortOrderChanged(int newSortColumnId, bool isForwards) override;
  void selectedRowsChanged(int lastRowSelected) override;

  // This is overloaded from TableListBoxModel, and must update any custom components that we're using
  Component* refreshComponentForCell(int rowNumber, int columnId, bool isRowSelected, Component* existingComponentToUpdate);

  int getColumnAutoSizeWidth(int columnId) override;

  DeviceSet getSelectedDevices();
  StringArray getSelectedIds();
  void deselectAll();

protected:
  TableListBox _table;

  int _numRows;
  StringArray _ids;  // List of IDs from the rig. Can index the array 

  int _numCols;
  StringArray _cols; // List of parameters from the rig. Luckily, each parameter

private:
  // Looks in the patches to see what universe this device is in.
  // The current assumption is one device is used by one patch.
  int getUniverse(string deviceID);

  // Looks in the patches to see what address the device is in.
  int getAddress(string deviceID);

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DeviceTable)
};


#endif  // DEVICETABLE_H_INCLUDED
