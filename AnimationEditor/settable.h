#ifndef SETTABLE_H
#define SETTABLE_H

#include <QWidget>

class Settable {
public:
    Settable();
    QWidget* menu();
    void openMenu();
    void closeMenu();
protected:
    virtual QWidget* newSettingMenu() = 0;
private:
    QWidget* pMenu = nullptr;
};

#endif // SETTABLE_H
