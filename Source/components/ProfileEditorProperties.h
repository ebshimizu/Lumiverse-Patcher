/*
  ==============================================================================

    ProfileEditorProperties.h
    Created: 8 Jan 2015 3:47:10pm
    Author:  eshimizu

  ==============================================================================
*/

#ifndef PROFILEEDITORPROPERTIES_H_INCLUDED
#define PROFILEEDITORPROPERTIES_H_INCLUDED

#include "../../JuceLibraryCode/JuceHeader.h"
#include "LumiverseCore.h"

using namespace Lumiverse;

class DeleteParameterButton : public ButtonPropertyComponent
{
public:
  DeleteParameterButton(Device* d, string paramName, function<void()> callback) :
    ButtonPropertyComponent("Delete", true), _d(d), _paramName(paramName), _cb(callback)
  {
  }

  ~DeleteParameterButton() {}

  void buttonClicked() override {
    juce::AlertWindow w("Delete Parameter",
      "Are you sure you want to delete " + _paramName + "?",
      juce::AlertWindow::WarningIcon);

    w.addButton("Delete", 1, KeyPress(KeyPress::returnKey, 0, 0));
    w.addButton("Cancel", 0, KeyPress(KeyPress::escapeKey, 0, 0));

    if (w.runModalLoop() != 0) // is they picked 'delete'
    {
      _d->deleteParameter(_paramName);
      _cb();
    }
  }

  String getButtonText() const {
    return "Delete";
  }

private:
  Device* _d;
  string _paramName;
  function<void()> _cb;
};

//==============================================================================
/*
*/
class LumiverseFloatProperty : public TextPropertyComponent
{
  // Indicates which part of the LumiverseFloat we're dealing with.
public:
  enum FloatPropAttr {
    DEFAULT, MIN, MAX
  };

  LumiverseFloatProperty(const String& propertyName, LumiverseFloat* data, FloatPropAttr attr) :
    TextPropertyComponent(propertyName, 50, false), _data(data), _attr(attr)
  {
  }

  ~LumiverseFloatProperty()
  {
  }

  void setText(const String &newText) override {
    float val = newText.getFloatValue();
    
    switch (_attr) {
    case (DEFAULT) :
      _data->setDefault(val);
      _data->setVal(val);
      break;
    case (MIN) :
      _data->setMin(val);
      break;
    case (MAX) :
      _data->setMax(val);
      break;
    default:
      break;
    }
  }

  String getText() const override {
    switch (_attr) {
    case (DEFAULT) :
      return String(_data->getDefault());
    case (MIN) :
      return String(_data->getMin());
    case (MAX) :
      return String(_data->getMax());
    default:
      return "";
    }
  }

private:
  LumiverseFloat* _data;
  FloatPropAttr _attr;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LumiverseFloatProperty)
};

class LumiverseOrientationProperty : public TextPropertyComponent
{
  // Indicates which part of the LumiverseFloat we're dealing with.
public:
  enum OriPropAttr {
    DEFAULT, MIN, MAX
  };

  LumiverseOrientationProperty(const String& propertyName, LumiverseOrientation* data, OriPropAttr attr) :
    TextPropertyComponent(propertyName, 50, false), _data(data), _attr(attr)
  {
  }

  ~LumiverseOrientationProperty()
  {
  }

  void setText(const String &newText) override {
    float val = newText.getFloatValue();

    switch (_attr) {
    case (DEFAULT) :
      _data->setDefault(val);
      _data->setVal(val);
      break;
    case (MIN) :
      _data->setMin(val);
      break;
    case (MAX) :
      _data->setMax(val);
      break;
    default:
      break;
    }
  }

  String getText() const override {
    switch (_attr) {
    case (DEFAULT) :
      return String(_data->getDefault());
    case (MIN) :
      return String(_data->getMin());
    case (MAX) :
      return String(_data->getMax());
    default:
      return "";
    }
  }

private:
  LumiverseOrientation* _data;
  OriPropAttr _attr;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LumiverseOrientationProperty)
};

class LumiverseOrientationUnits : public ChoicePropertyComponent
{
public:
  LumiverseOrientationUnits(const String& propertyName, LumiverseOrientation* data, function<void()> callback) :
    ChoicePropertyComponent(propertyName), _data(data), _cb(callback)
  {
    choices.add("Degrees");
    choices.add("Radians");
  }
  ~LumiverseOrientationUnits() {}

  void setIndex(int newIndex) override {
    if (newIndex == 0) {
      _data->setUnit(DEGREE);
    }
    else if (newIndex == 1) {
      _data->setUnit(RADIAN);
    }

    _cb();
  }

  int getIndex() const override {
    if (_data->getUnit() == DEGREE) {
      return 0;
    }
    else if (_data->getUnit() == RADIAN) {
      return 1;
    }
    else {
      return 0;
    }
  }

private:
  LumiverseOrientation* _data;
  function<void()> _cb;
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LumiverseOrientationUnits);
};

class LumiverseColorChannels : public Component, private TextEditorListener, private ButtonListener {
public:
  class defaultsFieldListener : public TextEditorListener {
  public:
    defaultsFieldListener(LumiverseColor* color) : _color(color) {}
    ~defaultsFieldListener() {}

    void textEditorTextChanged(TextEditor& e) override {
      _color->setColorChannel(e.getName().toStdString(), e.getText().getDoubleValue());
    }
  private:
    LumiverseColor* _color;
  };

  LumiverseColorChannels(LumiverseColor* data, function<void()> callback) :
    _color(data), _cb(callback)
  {
    _defaultsListener = new defaultsFieldListener(data);
    _add = new TextButton("add");
    addAndMakeVisible(_add);
    _add->setButtonText("Add Color Channel");
    _add->addListener(this);

    initFields(false);
  }

  ~LumiverseColorChannels() {
    deleteFields();

    _add = nullptr;
    _defaultsListener = nullptr;
  }

  void deleteFields() {
    for (auto kvp : _names) {
      kvp.second = nullptr;
    }

    for (auto kvp : _deletes) {
      kvp.second = nullptr;
    }

    for (auto kvp : _defaults) {
      kvp.second = nullptr;
    }

    _names.clear();
    _deletes.clear();
    _defaults.clear();
  }

  void initFields(bool doCallback = true) {
    deleteFields();

    // For each color field, add two text fields and a button
    for (const auto& kvp : _color->getColorParams()) {
      if (_names.count(kvp.first) > 0) {
        // Shouldn't have duplicates here.
        return;
      }

      auto nameEditor = new TextEditor(kvp.first);
      nameEditor->setMultiLine(false);
      nameEditor->setText(kvp.first, false);
      nameEditor->addListener(this);
      addAndMakeVisible(nameEditor);
      _names[kvp.first] = nameEditor;

      auto defaultsEditor = new TextEditor(kvp.first);
      defaultsEditor->setMultiLine(false);
      defaultsEditor->setText(String(kvp.second));
      defaultsEditor->addListener(_defaultsListener);
      addAndMakeVisible(defaultsEditor);
      _defaults[kvp.first] = defaultsEditor;

      auto deleteButton = new TextButton(kvp.first, "Delete Color Channel");
      deleteButton->setButtonText("Delete");
      deleteButton->addListener(this);
      addAndMakeVisible(deleteButton);
      _deletes[kvp.first] = deleteButton;
    }

    if (doCallback) _cb();
    resized();
  }

  void resized() override {
    int row = 0;
    auto bounds = getLocalBounds();

    for (const auto& kvp : _names) {
      auto rowBounds = bounds.removeFromTop(_rowHeight);
      _deletes[kvp.first]->setBounds(rowBounds.removeFromRight(50));
      _defaults[kvp.first]->setBounds(rowBounds.removeFromRight(100));
      _names[kvp.first]->setBounds(rowBounds);
    }

    if (_add != nullptr) _add->setBounds(bounds);
  }

  int getTotalHeight() {
    return _rowHeight * (_names.size() + 1) + 3;
  }

  //void paint(Graphics& g) override;

  void textEditorReturnKeyPressed(TextEditor& e) override {
    // Name change
    // First check if name already exists. If so change text field BG to red.
    string newName = e.getText().toStdString();
    if (_names.count(newName) > 0) {
      e.setColour(TextEditor::backgroundColourId, Colour(0xffff4d4d));
      return;
    }

    e.setColour(TextEditor::backgroundColourId, Colours::white);
    _color->deleteColorChannel(e.getName().toStdString());
    _color->addColorChannel(e.getText().toStdString());

    initFields();
  }

  void textEditorFocusLost(TextEditor& e) override {
    textEditorReturnKeyPressed(e);

    if (e.getText() == e.getName()) {
      e.setColour(TextEditor::backgroundColourId, Colours::white);
    }
  }

  void buttonClicked(Button* b) {
    // Confirm delete if add button not pressed.
    // Note that if someone names their color parameter "add" for some reason
    // we first kick them and then note that this won't work.
    if (b->getName() != "add") {
      juce::AlertWindow w("Delete Color Parameter",
        "Are you sure you want to delete " + b->getName() + "?",
        juce::AlertWindow::WarningIcon);

      w.addButton("Delete", 1, KeyPress(KeyPress::returnKey, 0, 0));
      w.addButton("Cancel", 0, KeyPress(KeyPress::escapeKey, 0, 0));

      if (w.runModalLoop() != 0) // is they picked 'delete'
      {
        _color->deleteColorChannel(b->getName().toStdString());
        initFields();
      }
    }
    else {
      juce::AlertWindow w("Add Color Channel",
        "Add a color channel to the parameter.",
        juce::AlertWindow::QuestionIcon);

      w.addTextEditor("name", "", "Name");

      w.addButton("Add", 1, KeyPress(KeyPress::returnKey, 0, 0));
      w.addButton("Cancel", 0, KeyPress(KeyPress::escapeKey, 0, 0));

      if (w.runModalLoop() != 0) // is they picked 'add'
      {
        // this is the item they chose in the drop-down list..
        string name = w.getTextEditor("name")->getText().toStdString();

        if (!_color->addColorChannel(name)) {
          juce::AlertWindow::showMessageBoxAsync(juce::AlertWindow::WarningIcon,
            "Unable to add Channel",
            "A channel with the name \"" + name + "\" already exists.",
            "OK");
          return;
        }

        initFields();
      }
    }
  }

private:
  LumiverseColor* _color;
  function<void()> _cb;
  int _rowHeight = 20;

  map<string, ScopedPointer<TextEditor> > _names;
  map<string, ScopedPointer<TextEditor> > _defaults;
  map<string, ScopedPointer<TextButton> > _deletes;
  ScopedPointer<TextButton> _add;
  ScopedPointer<defaultsFieldListener> _defaultsListener;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LumiverseColorChannels);
};

class LumiverseColorChannelsProperty : public PropertyComponent {
public:
  LumiverseColorChannelsProperty(const String& propertyName, LumiverseColor* data, function<void()> callback) :
    PropertyComponent(propertyName), _color(data), _cb(callback)
  {
    _comp = new LumiverseColorChannels(data, callback);
    addAndMakeVisible(_comp);
    preferredHeight = _comp->getTotalHeight();
  }

  ~LumiverseColorChannelsProperty() {
    _comp = nullptr;
  }

  void refresh() override {
    setPreferredHeight(_comp->getTotalHeight());
    resized();
    repaint();
  }

private:
  ScopedPointer<LumiverseColorChannels> _comp;
  LumiverseColor* _color;
  function<void()> _cb;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LumiverseColorChannelsProperty);
};

class LumiverseColorModeProperty : public ChoicePropertyComponent {
public:
  LumiverseColorModeProperty(const String& propertyName, LumiverseColor* data, function<void()> callback) :
    ChoicePropertyComponent(propertyName), _color(data), _cb(callback)
  {
    choices.add("Additive");
    choices.add("Subtractive");
    choices.add("Basic RGB");
    choices.add("Basic CMY");
  }

  ~LumiverseColorModeProperty() {}

  void setIndex(int newIndex) override {
    _color->changeMode((ColorMode)newIndex);

    _cb();
  }

  int getIndex() const override {
    return (int)_color->getMode();
  }

private:
  LumiverseColor* _color;
  function<void()> _cb;
};

class LumiverseColorWeightProperty : public TextPropertyComponent {
public: 
  LumiverseColorWeightProperty(const String& propertyName, LumiverseColor* data) :
    TextPropertyComponent(propertyName, 50, false), _color(data)
  { }

  ~LumiverseColorWeightProperty()
  { }

  void setText(const String &newText) override {
    double val = newText.getDoubleValue();

    _color->setWeight(val);
  }

  String getText() const override {
    return String(_color->getWeight());
  }

private:
  LumiverseColor* _color;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LumiverseColorWeightProperty);
};

class LumiverseColorBasisVectorButton : public ButtonPropertyComponent {
public: 
  LumiverseColorBasisVectorButton(LumiverseColor* data, function<void()> callback) :
    ButtonPropertyComponent("Basis Vectors", true), _color(data), _cb(callback)
  { }

  ~LumiverseColorBasisVectorButton() {}

  void buttonClicked() override {
    if (_color->getBasisVectors().size() > 0) {
      juce::AlertWindow w("Delete Basis Vectors",
        "Are you sure you want to delete the basis vectors for this color?",
        juce::AlertWindow::WarningIcon);

      w.addButton("Delete", 1, KeyPress(KeyPress::returnKey, 0, 0));
      w.addButton("Cancel", 0, KeyPress(KeyPress::escapeKey, 0, 0));

      if (w.runModalLoop() != 0) // is they picked 'delete'
      {
        vector<string> toDelete;
        for (const auto& kvp : _color->getBasisVectors()) {
          toDelete.push_back(kvp.first);
        }
        for (const auto& s : toDelete) {
          _color->removeBasisVector(s);
        }

        _cb();
      }
    }
    else {
      for (const auto& kvp : _color->getColorParams()) {
        _color->setBasisVector(kvp.first, 0, 0, 0);
      }

      _cb();
    }
  }

  String getButtonText() const {
    if (_color->getBasisVectors().size() > 0) {
      return "Delete Basis Vectors";
    }
    else {
      return "Add Basis Vectors";
    }
  }

private:
  LumiverseColor* _color;
  function<void()> _cb;
};
#endif  // PROFILEEDITORPROPERTIES_H_INCLUDED
