/*
  ==============================================================================

    DMXMap.h
    Created: 7 Jan 2015 3:48:06pm
    Author:  eshimizu

  ==============================================================================
*/

#ifndef DMXMAP_H_INCLUDED
#define DMXMAP_H_INCLUDED

#include "../../JuceLibraryCode/JuceHeader.h"

using namespace std;

enum AddrStatus {
  FREE,
  USED,
  CONFLICT,
  SELECTED
};

//==============================================================================
// Box containing address number and a status indicating if it's used, free,
// or conflicted.
class DMXWidget : public Component
{
public:
  DMXWidget(int addr);
  ~DMXWidget();

  void paint(Graphics& g);
  void resized();

  AddrStatus getStatus() { return _status; }

  void setUsed();
  void setFree();
  void setConflict();
private:
  AddrStatus _status;

  int _addr;
};

//==============================================================================
/*
*/
class DMXMap    : public Component
{
public:
  DMXMap(string patchID, int universe);
  ~DMXMap();

  void reinit();

  void paint (Graphics&);
  void resized();

  void setUniverse(int uni) { _uni = uni; }
private:
  vector<DMXWidget*> _addrs;

  string _patchID;
  int _uni;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DMXMap)
};


#endif  // DMXMAP_H_INCLUDED
