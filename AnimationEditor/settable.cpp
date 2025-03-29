#include "settable.h"

Settable::Settable() {}

QWidget *Settable::menu() {
    if(!pMenu) {
        pMenu = newSettingMenu();
    }
    return pMenu;
}

void Settable::openMenu() {
    menu()->show();
    menu()->raise();
    menu()->activateWindow();
}

void Settable::closeMenu() {
    menu()->hide();
}
