#ifndef CKEYBOARDMANAGER_HPP
#define CKEYBOARDMANAGER_HPP

#include <QObject>
#include <QMap>

class CKeyboardManager : public QObject
{
    Q_OBJECT
public:
    CKeyboardManager(const CKeyboardManager&) = delete;
    CKeyboardManager& operator=(const CKeyboardManager&) = delete;
    ~CKeyboardManager();

    virtual bool eventFilter(QObject *o, QEvent *e) override;

    bool isKeyPressed(Qt::Key k);
    static CKeyboardManager* instance();
protected:
    QMap<Qt::Key, bool>              m_keys;
private:
    explicit CKeyboardManager(QObject* parent = 0);
};

#endif // CKEYBOARDMANAGER_HPP
