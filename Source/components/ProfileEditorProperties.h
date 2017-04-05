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

        if (_color->getBasisVectors().count(b->getName().toStdString()) > 0) {
          // delete the basis vector too
          _color->removeBasisVector(b->getName().toStdString());
        }

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

        if (_color->getBasisVectors().size() > 0) {
          // Add basis vector if we're using them
          _color->setBasisVector(name, 0, 0, 0);
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

class LumiverseColorBasisVectors : public Component, public TextEditorListener {
public:
  LumiverseColorBasisVectors(LumiverseColor* data, function<void()> callback) :
    _color(data), _cb(callback)
  {
    initFields();
  }
  
  ~LumiverseColorBasisVectors() { deleteFields(); }

  void deleteFields() {
    for (auto& kvp : _fields) {
      delete kvp.second[0];
      delete kvp.second[1];
      delete kvp.second[2];
      
      kvp.second.clear();
    }

    _fields.clear();
  }

  void initFields() {
    deleteFields();

    for (const auto& bv : _color->getBasisVectors()) {
      // Create three fields for each basis vector
      auto x = new TextEditor(bv.first);
      x->setMultiLine(false);
      x->setText(String(bv.second.x()), false);
      x->addListener(this);
      addAndMakeVisible(x);
      _fields[bv.first].add(x);

      auto y = new TextEditor(bv.first);
      y->setMultiLine(false);
      y->setText(String(bv.second.y()), false);
      y->addListener(this);
      addAndMakeVisible(y);
      _fields[bv.first].add(y);

      auto z = new TextEditor(bv.first);
      z->setMultiLine(false);
      z->setText(String(bv.second.z()), false);
      z->addListener(this);
      addAndMakeVisible(z);
      _fields[bv.first].add(z);
    }

    resized();
  }

  void resized() {
    auto bounds = getLocalBounds();
    float width = getWidth() - _textLabelSize;
    float fieldSize = width / 3;

    for (const auto& f : _fields) {
      auto row = bounds.removeFromTop(_rowHeight);
      row.removeFromLeft(_textLabelSize);
      f.second[0]->setBounds(row.removeFromLeft(fieldSize));
      f.second[1]->setBounds(row.removeFromLeft(fieldSize));
      f.second[2]->setBounds(row);
    }
  }

  void paint(Graphics& g) {
    auto bounds = getLocalBounds();

    g.setFont(12);
    for (const auto& f : _fields) {
      auto row = bounds.removeFromTop(_rowHeight);
      g.drawFittedText(f.first, row.removeFromLeft(_textLabelSize), Justification::centredLeft, 1);
    }
  }

  int getTotalHeight() {
    return _fields.size() * _rowHeight + 3;
  }

  void textEditorReturnKeyPressed(TextEditor& e) override {
    string name = e.getName().toStdString();
    _color->setBasisVector(name, _fields[name][0]->getText().getDoubleValue(), _fields[name][1]->getText().getDoubleValue(), _fields[name][2]->getText().getDoubleValue());
  }

  void textEditorFocusLost(TextEditor& e) override {
    textEditorReturnKeyPressed(e);
  }

private:
  LumiverseColor* _color;
  function<void()> _cb;
  float _textLabelSize = 100;
  int _rowHeight = 20;
  
  map<string, Array<TextEditor*> > _fields;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LumiverseColorBasisVectors);
};

class LumiverseColorBasisVectorsProperty : public PropertyComponent {
public:
  LumiverseColorBasisVectorsProperty(const String& propertyName, LumiverseColor* data, function<void()> callback) :
    PropertyComponent(propertyName), _color(data), _cb(callback)
  {
    _comp = new LumiverseColorBasisVectors(data, callback);
    addAndMakeVisible(_comp);
    preferredHeight = _comp->getTotalHeight();
  }

  ~LumiverseColorBasisVectorsProperty() {
    _comp = nullptr;
  }

  void refresh() override {
    _comp->initFields();
    setPreferredHeight(_comp->getTotalHeight());
    resized();
    repaint();
  }

private:
  ScopedPointer<LumiverseColorBasisVectors> _comp;
  LumiverseColor* _color;
  function<void()> _cb;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LumiverseColorBasisVectorsProperty);
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

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LumiverseColorModeProperty)
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

class LumiverseEnumTweakProperty : public TextPropertyComponent {
public:
  LumiverseEnumTweakProperty(const String& propertyName, LumiverseEnum* data) :
    TextPropertyComponent(propertyName, 50, false), _enum(data)
  {

  }

  ~LumiverseEnumTweakProperty() {}

  void setText(const String& newText) override {
    _enum->setTweak(newText.getFloatValue());
  }

  String getText() const override {
    return String(_enum->getTweak());
  }

private:
  LumiverseEnum* _enum;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LumiverseEnumTweakProperty);
};

class LumiverseEnumDefaultProperty : public PropertyComponent, private ComboBoxListener {
public:
  LumiverseEnumDefaultProperty(const String& propertyName, LumiverseEnum* data) :
    PropertyComponent(propertyName), _enum(data)
  {
    addAndMakeVisible(comboBox);
    comboBox.setEditableText(false);
    comboBox.addListener(this);
    initMenu();
  }

  ~LumiverseEnumDefaultProperty() {}

  void initMenu() {
    choices.clear();
    comboBox.clear();

    for (const auto& k : _enum->getVals()) {
      choices.add(k);
    }
    comboBox.addItemList(choices, 1);
  }

  void setIndex(int newIndex) {
    string key = choices[newIndex].toStdString();
    _enum->setVal(key);
    _enum->setDefault(key);
  }

  int getIndex() const {
    return choices.indexOf(String(_enum->getDefault()));
  }

  void refresh() override {
    initMenu();
    repaint();

    comboBox.setSelectedId (getIndex() + 1, dontSendNotification);
  }

protected:
  /** The list of options that will be shown in the combo box.

  Your subclass must populate this array in its constructor. If any empty
  strings are added, these will be replaced with horizontal separators (see
  ComboBox::addSeparator() for more info).
  */
  StringArray choices;

private:
  ComboBox comboBox;

  void comboBoxChanged(ComboBox*)
  {
    const int newIndex = comboBox.getSelectedId() - 1;

    if (newIndex != getIndex())
      setIndex(newIndex);
  }

  LumiverseEnum* _enum;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LumiverseEnumDefaultProperty);
};

class LumiverseEnumModeProperty : public ChoicePropertyComponent {
public:
  LumiverseEnumModeProperty(const String& propertyName, LumiverseEnum* data) :
    ChoicePropertyComponent(propertyName), _enum(data)
  {
    choices.add("First");
    choices.add("Center");
    choices.add("Last");
  }

  ~LumiverseEnumModeProperty() { }

  void setIndex(int newIndex) override {
    _enum->setMode((LumiverseEnum::Mode)newIndex);
  }

  int getIndex() const override {
    return _enum->getMode();
  }

private:
  LumiverseEnum* _enum;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LumiverseEnumModeProperty)
};

class LumiverseEnumInterpProperty : public ChoicePropertyComponent {
public:
  LumiverseEnumInterpProperty(const String& propertyName, LumiverseEnum* data) :
    ChoicePropertyComponent(propertyName), _enum(data)
  {
    choices.add("Snap");
    choices.add("Smooth Within Option");
    choices.add("Smooth");
  }

  ~LumiverseEnumInterpProperty() {}

  void setIndex(int newIndex) override {
    _enum->setInterpMode((LumiverseEnum::InterpolationMode)newIndex);
  }

  int getIndex() const override {
    return _enum->getInterpMode();
  }

private:
  LumiverseEnum* _enum;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LumiverseEnumInterpProperty);
};

class LumiverseEnumRangeMaxProperty : public TextPropertyComponent {
public: 
  LumiverseEnumRangeMaxProperty(const String& propertyName, LumiverseEnum* data) :
    TextPropertyComponent(propertyName, 50, false), _enum(data)
  { }

  ~LumiverseEnumRangeMaxProperty() {}

  void setText(const String& newText) override {
    int max = newText.getIntValue();

    if (max < _enum->getHighestStartValue()) {
      juce::AlertWindow::showMessageBoxAsync(juce::AlertWindow::WarningIcon,
        "Range Error",
        "A key in the enumeration has a lower start value than the specified max.",
        "OK");
      refresh();
      return;
    }
    else 
      _enum->setRangeMax(newText.getIntValue());
  }

  String getText() const override {
    return String(_enum->getRangeMax());
  }

private:
  LumiverseEnum* _enum;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LumiverseEnumRangeMaxProperty);
};

class LumiverseEnumValues : public Component, private TextEditorListener, private ButtonListener {
public:
  class startsFieldListener : public TextEditorListener {
  public:
    startsFieldListener(LumiverseEnum* e, function<void()> cb) : _enum(e), _cb(cb) {}
    ~startsFieldListener() {}

    void textEditorReturnKeyPressed(TextEditor& e) override {
      int val = e.getText().getIntValue();
      if (val > _enum->getRangeMax()) {
        juce::AlertWindow::showMessageBoxAsync(juce::AlertWindow::WarningIcon,
          "Range Error",
          "Start value for " + e.getName() + " exceeds the maximum value for the enumeration",
          "OK");
      }
      else
        _enum->addVal(e.getName().toStdString(), val);

      _cb();
    }

    void textEditorFocusLost(TextEditor& e) override {
      textEditorReturnKeyPressed(e);
    }
  private:
    LumiverseEnum* _enum;
    function<void()> _cb;
  };

  LumiverseEnumValues(LumiverseEnum* data, function<void()> callback) :
    _enum(data), _cb(callback)
  {
    _startsListener = new startsFieldListener(data, [this]{this->initFields(); });
    _add = new TextButton("add");
    addAndMakeVisible(_add);
    _add->setButtonText("Add Enumeration Option");
    _add->addListener(this);

    initFields(false);
  }

  ~LumiverseEnumValues() {
    deleteFields();

    _add = nullptr;
    _startsListener = nullptr;
  }

  void deleteFields() {
    for (auto kvp : _names) {
      kvp.second = nullptr;
    }

    for (auto kvp : _deletes) {
      kvp.second = nullptr;
    }

    for (auto kvp : _starts) {
      kvp.second = nullptr;
    }

    _names.clear();
    _deletes.clear();
    _starts.clear();
  }

  void initFields(bool doCallback = true) {
    deleteFields();

    // For each enum field, add two text fields and a button
    for (const auto& kvp : _enum->getStartToVals()) {
      if (_names.count(kvp.second) > 0) {
        // Shouldn't have duplicates here.
        return;
      }

      auto nameEditor = new TextEditor(kvp.second);
      nameEditor->setMultiLine(false);
      nameEditor->setText(kvp.second, false);
      nameEditor->addListener(this);
      addAndMakeVisible(nameEditor);
      _names[kvp.second] = nameEditor;

      auto startEditor = new TextEditor(kvp.second);
      startEditor->setMultiLine(false);
      startEditor->setText(String(kvp.first));
      startEditor->addListener(_startsListener);
      addAndMakeVisible(startEditor);
      _starts[kvp.second] = startEditor;

      auto deleteButton = new TextButton(kvp.second, "Delete Option");
      deleteButton->setButtonText("Delete");
      deleteButton->addListener(this);
      addAndMakeVisible(deleteButton);
      _deletes[kvp.second] = deleteButton;
    }

    if (doCallback) _cb();
    resized();
  }

  void resized() override {
    int row = 0;
    auto bounds = getLocalBounds();

    for (const auto& kvp : _enum->getStartToVals()) {
      auto rowBounds = bounds.removeFromTop(_rowHeight);
      _deletes[kvp.second]->setBounds(rowBounds.removeFromRight(50));
      _starts[kvp.second]->setBounds(rowBounds.removeFromRight(100));
      _names[kvp.second]->setBounds(rowBounds);
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
    auto vals = _enum->getValsToStart();
    int oldVal = vals[e.getText().toStdString()];
    _enum->removeVal(e.getName().toStdString());
    _enum->addVal(newName, oldVal);

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
      juce::AlertWindow w("Delete Enumeration Option",
        "Are you sure you want to delete " + b->getName() + "?",
        juce::AlertWindow::WarningIcon);

      w.addButton("Delete", 1, KeyPress(KeyPress::returnKey, 0, 0));
      w.addButton("Cancel", 0, KeyPress(KeyPress::escapeKey, 0, 0));

      if (w.runModalLoop() != 0) // is they picked 'delete'
      {
        _enum->removeVal(b->getName().toStdString());
        initFields();
      }
    }
    else {
      juce::AlertWindow w("Add Enumeration Option",
        "Add a new option to the enumeration parameter.",
        juce::AlertWindow::QuestionIcon);

      w.addTextEditor("name", "", "Name");

      w.addButton("Add", 1, KeyPress(KeyPress::returnKey, 0, 0));
      w.addButton("Cancel", 0, KeyPress(KeyPress::escapeKey, 0, 0));

      if (w.runModalLoop() != 0) // is they picked 'add'
      {
        // this is the item they chose in the drop-down list..
        string name = w.getTextEditor("name")->getText().toStdString();

        if (_enum->getValsToStart().count(name) > 0) {
          juce::AlertWindow::showMessageBoxAsync(juce::AlertWindow::WarningIcon,
            "Unable to add option",
            "An option with the name \"" + name + "\" already exists.",
            "OK");
          return;
        }

        _enum->addVal(name, _enum->getHighestStartValue() + 1);

        initFields();
      }
    }
  }

private:
  LumiverseEnum* _enum;
  function<void()> _cb;
  int _rowHeight = 20;

  map<string, ScopedPointer<TextEditor> > _names;
  map<string, ScopedPointer<TextEditor> > _starts;
  map<string, ScopedPointer<TextButton> > _deletes;
  ScopedPointer<TextButton> _add;
  ScopedPointer<startsFieldListener> _startsListener;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LumiverseEnumValues);
};

class LumiverseEnumValuesProperty : public PropertyComponent {
public:
  LumiverseEnumValuesProperty(const String& propertyName, LumiverseEnum* data, function<void()> callback) :
    PropertyComponent(propertyName), _enum(data), _cb(callback)
  {
    _comp = new LumiverseEnumValues(data, callback);
    addAndMakeVisible(_comp);
    preferredHeight = _comp->getTotalHeight();
  }

  ~LumiverseEnumValuesProperty() {
    _comp = nullptr;
  }

  void refresh() override {
    setPreferredHeight(_comp->getTotalHeight());
    resized();
    repaint();
  }

private:
  ScopedPointer<LumiverseEnumValues> _comp;
  LumiverseEnum* _enum;
  function<void()> _cb;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LumiverseEnumValuesProperty);
};

class LumiverseMetadataEdit : public Component, private TextEditorListener, private ButtonListener {
public:
  LumiverseMetadataEdit(string key, Device* device, function<void()> callback) :
    _key(key), _device(device), _cb(callback)
  {
    addAndMakeVisible(_name);
    addAndMakeVisible(_val);
    addAndMakeVisible(_delete);

    _name.setName("name");
    _name.addListener(this);
    _name.setMultiLine(false);
    _name.setText(key);
    
    _val.setName("val");
    _val.addListener(this);
    _val.setMultiLine(false);
    _val.setText(device->getMetadata(key));

    _delete.setButtonText("Delete");
    _delete.addListener(this);
  }

  ~LumiverseMetadataEdit() {}

  void resized() {
    auto bounds = getLocalBounds();
    _name.setBounds(bounds.removeFromLeft(150));
    _delete.setBounds(bounds.removeFromRight(50));
    _val.setBounds(bounds);
  }

  void textEditorReturnKeyPressed(TextEditor& e) override {
    if (e.getName() == "name") {
      // Name change
      // First check if name already exists. If so change text field BG to red.
      string newName = e.getText().toStdString();
      if (_device->metadataExists(newName)) {
        e.setColour(TextEditor::backgroundColourId, Colour(0xffff4d4d));
        return;
      }

      e.setColour(TextEditor::backgroundColourId, Colours::white);
      _device->deleteMetadata(_key);

      _key = _name.getText().toStdString();
      _device->setMetadata(_key, _val.getText().toStdString());

      _cb();
    }
    else {
      _device->setMetadata(_key, e.getText().toStdString());
    }
  }

  void textEditorFocusLost(TextEditor& e) override {
    textEditorReturnKeyPressed(e);

    if (e.getName() == "name" && (e.getText() == _key)) {
      e.setColour(TextEditor::backgroundColourId, Colours::white);
    }
  }

  void buttonClicked(Button* b) {
    // Confirm delete if add button not pressed.
    // Note that if someone names their color parameter "add" for some reason
    // we first kick them and then note that this won't work.
    juce::AlertWindow w("Delete Metadata Field",
      "Are you sure you want to delete " + _key + "?",
      juce::AlertWindow::WarningIcon);

    w.addButton("Delete", 1, KeyPress(KeyPress::returnKey, 0, 0));
    w.addButton("Cancel", 0, KeyPress(KeyPress::escapeKey, 0, 0));

    if (w.runModalLoop() != 0) // is they picked 'delete'
    {
      _device->deleteMetadata(_key);
      _cb();
    }
  }

private:
  TextEditor _name;
  TextEditor _val;
  TextButton _delete;

  string _key;
  Device* _device;
  function<void()> _cb;
};

class LumiverseMetadataEditProperty : public PropertyComponent
{
public:
  LumiverseMetadataEditProperty(string key, Device* device, function<void()> callback) :
    PropertyComponent(key)
  {
    addAndMakeVisible(_comp = new LumiverseMetadataEdit(key, device, callback));
    preferredHeight = 25;
  }

  ~LumiverseMetadataEditProperty() {
    _comp = nullptr;
  }

  void refresh() override {
    repaint();
  }

private:
  ScopedPointer<LumiverseMetadataEdit> _comp;
  
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LumiverseMetadataEditProperty);
};

class LumiverseDMXMapTypeProperty : public ChoicePropertyComponent
{
public:
  LumiverseDMXMapTypeProperty(map<string, patchData>* data, string key, Device* device) :
    ChoicePropertyComponent("Conversion Type"), _data(data), _key(key)
  {
    LumiverseType* param = device->getParam(key);
    if (param->getTypeName() == "float") {
      choices.add("FLOAT_TO_SINGLE");
      choices.add("FLOAT_TO_FINE");
      choices.add("RGB_REPEAT2");
      choices.add("RGB_REPEAT3");
      choices.add("RGB_REPEAT4");
      choices.add("IGNORE");
    }
    else if (param->getTypeName() == "enum") {
      choices.add("ENUM");
      choices.add("IGNORE");
    }
    else if (param->getTypeName() == "color") {
      choices.add("COLOR_RGB");
      choices.add("COLOR_RGBW");
      choices.add("COLOR_LUSTRPLUS");
      choices.add("IGNORE");
    }
    else if (param->getTypeName() == "orientation") {
      choices.add("ORI_TO_FINE");
      choices.add("IGNORE");
    }
  }

  ~LumiverseDMXMapTypeProperty() {}

  void setIndex(int newIndex) override {
    if (choices[newIndex] != "IGNORE") {
      (*_data)[_key].type = (conversionType)stringToConvType[choices[newIndex].toStdString()];
    }
  }

  int getIndex() const override {
    return choices.indexOf(String(convTypeToString[(*_data)[_key].type]));
  }

private:
  map<string, patchData>* _data;
  string _key;
};

class LumiverseDMXMapOffsetProperty : public TextPropertyComponent {
public:
  LumiverseDMXMapOffsetProperty(map<string, patchData>* data, string key) :
    TextPropertyComponent("Address Offset", 3, false), _data(data), _key(key)
  {

  }

  ~LumiverseDMXMapOffsetProperty() {}

  void setText(const String& newText) override {
    (*_data)[_key].startAddress = newText.getIntValue();
  }

  String getText() const override {
    return String((*_data)[_key].startAddress);
  }

private:
  map<string, patchData>* _data;
  string _key;
};

#endif  // PROFILEEDITORPROPERTIES_H_INCLUDED
