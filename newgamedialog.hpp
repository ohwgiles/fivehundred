#ifndef NEWGAMEDIALOG_HPP
#define NEWGAMEDIALOG_HPP

#include <QDialog>

namespace Ui {
    class NewGameDialog;
}

class SetupPlayer;

class NewGameDialog : public QDialog
{
    Q_OBJECT

public:
    explicit NewGameDialog(QWidget *parent = 0);
    ~NewGameDialog();
    SetupPlayer** result();

private:
    Ui::NewGameDialog *ui;
};

#endif // NEWGAMEDIALOG_HPP
