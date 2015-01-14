#include "MainMenuComponent.h"
#include "../Main.h"

MainMenuComponent::MainMenuComponent()
{
  addAndMakeVisible(menuBar = new MenuBarComponent(this));
}

MainMenuComponent::~MainMenuComponent()
{
  PopupMenu::dismissAllActiveMenus();
  menuBar = nullptr;
}

StringArray MainMenuComponent::getMenuBarNames() {
  const char* const names[] = { "File", "Device", "Patch", nullptr };

  return StringArray(names);
}

PopupMenu MainMenuComponent::getMenuForIndex(int menuIndex, const String& /*menuName*/) {
  ApplicationCommandManager* commandManager = &MainWindow::getApplicationCommandManager();

  PopupMenu menu;

  if (menuIndex == 0) {
    menu.addCommandItem(commandManager, MainWindow::open);
    menu.addCommandItem(commandManager, MainWindow::save);
    menu.addCommandItem(commandManager, MainWindow::saveAs);
  }
  else if (menuIndex == 1) {
    menu.addCommandItem(commandManager, MainWindow::openProfileEditor);
  }
  else if (menuIndex == 2) {
    menu.addCommandItem(commandManager, MainWindow::addPatch);
    menu.addCommandItem(commandManager, MainWindow::deletePatch);
  }

  return menu;
}

void MainMenuComponent::menuItemSelected(int menuItemID, int /*topLevelMenuIndex*/) {
  // Handle custom commands
  // if (menuItemID == int)
}