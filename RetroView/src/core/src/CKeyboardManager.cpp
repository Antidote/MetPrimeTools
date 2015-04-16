#include "core/CKeyboardManager.hpp"

#include <QKeyEvent>
#include <iostream>

CKeyboardManager::CKeyboardManager(QObject* parent)
    : QObject(parent)
{
    std::cout << "Keyboard Manager" << std::endl;
}

CKeyboardManager::~CKeyboardManager()
{
    std::cout << "~Keyboard Manager" << std::endl;
}

bool CKeyboardManager::eventFilter(QObject* o, QEvent* e)
{
    if(e->type() == QEvent::KeyPress || e->type() == QEvent::KeyRelease)
    {
        QKeyEvent* ke = static_cast<QKeyEvent*>(e);
        if (ke)
            m_keys[(Qt::Key)ke->key()] = (e->type() == QEvent::KeyPress ? true : false);
    }

    return QObject::eventFilter(o, e);
}

bool CKeyboardManager::isKeyPressed(Qt::Key k)
{
    return m_keys[k];
}

CKeyboardManager*CKeyboardManager::instance()
{
    static CKeyboardManager inst;
    return &inst;
}

