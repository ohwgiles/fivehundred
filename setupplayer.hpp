#ifndef SETUPPLAYER_HPP
#define SETUPPLAYER_HPP

#include <QWidget>

namespace Ui {
    class SetupPlayer;
}

class SetupPlayer : public QWidget
{
    Q_OBJECT

public:
    explicit SetupPlayer(QWidget *parent = 0);
    ~SetupPlayer();
    enum Type { HUMAN = 0, COMPUTER };
    void setPlayerType(Type type);
    Type getPlayerType() const;
    QString getScript() const;
    QString getName() const;

private slots:
    void on_playerType_currentIndexChanged(QString);
    void on_script_currentIndexChanged(QString);

private:
    Ui::SetupPlayer *ui;
};

#endif // SETUPPLAYER_HPP
