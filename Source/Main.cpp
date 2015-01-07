#include "Main.h"

//==============================================================================
class LumiversePatcherApplication : public JUCEApplication
{
public:
  //==============================================================================
  LumiversePatcherApplication() {}

  const String getApplicationName() override       { return ProjectInfo::projectName; }
  const String getApplicationVersion() override    { return ProjectInfo::versionString; }
  bool moreThanOneInstanceAllowed() override       { return true; }

  //==============================================================================
  void initialise(const String& commandLine) override
  {
    // This method is where you should put your application's initialisation code..

    mainWindow = new MainWindow(getApplicationName());
  }

  void shutdown() override
  {
    // Add your application's shutdown code here..
    mainWindow = nullptr; // (deletes our window)
  }

  //==============================================================================
  void systemRequestedQuit() override
  {
    // This is called when the app is being asked to quit: you can ignore this
    // request and let the app carry on running, or call quit() to allow the app to close.
    quit();
  }

  void anotherInstanceStarted(const String& commandLine) override
  {
    // When another instance of the app is launched while this one is running,
    // this method is invoked, and the commandLine parameter tells you what
    // the other instance's command-line arguments were.
  }

private:
  ScopedPointer<MainWindow> mainWindow;
};

static ScopedPointer<ApplicationCommandManager> applicationCommandManager;
static unique_ptr<Rig> rig;

MainWindow::MainWindow (String name) :
  DocumentWindow (name, Colours::lightgrey, DocumentWindow::allButtons)
{
  MainContentComponent* mc;

  setUsingNativeTitleBar (true);
  setContentOwned (mc = new MainContentComponent(), true);

  centreWithSize (getWidth(), getHeight());
  setResizable(true, false);

  setName("Lumiverse Patcher");
  setMenuBar(mainMenu = new MainMenuComponent());

  // this lets the command manager use keypresses that arrive in our window to send out commands
  addKeyListener(getApplicationCommandManager().getKeyMappings());

  ApplicationCommandManager& commandManager = getApplicationCommandManager();
  commandManager.registerAllCommandsForTarget(mc);

  setVisible (true);
}

MainWindow::~MainWindow() {
  setMenuBar(nullptr);

  mainMenu = nullptr;
  applicationCommandManager = nullptr;

  if (rig != nullptr) {
    rig->stop();
  }
}

void MainWindow::closeButtonPressed()
{
  // This is called when the user tries to close this window. Here, we'll just
  // ask the app to quit when this happens, but you can change this to do
  // whatever you need.
  JUCEApplication::getInstance()->systemRequestedQuit();
}

ApplicationCommandManager& MainWindow::getApplicationCommandManager() {
  if (applicationCommandManager == nullptr)
    applicationCommandManager = new ApplicationCommandManager();

  return *applicationCommandManager;
}

unique_ptr<Rig>& MainWindow::getRig() {
  if (rig == nullptr) {
    rig = unique_ptr<Rig>(new Rig());
  }

  return rig;
}

MainWindow* MainWindow::getMainWindow()
{
  for (int i = TopLevelWindow::getNumTopLevelWindows(); --i >= 0;)
    if (MainWindow* maw = dynamic_cast<MainWindow*> (TopLevelWindow::getTopLevelWindow(i)))
      return maw;

  return nullptr;
}

//==============================================================================
// This macro generates the main() routine that launches the app.
START_JUCE_APPLICATION(LumiversePatcherApplication)
