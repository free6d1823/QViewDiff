#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    void createMenuAndToolbar();
    void createUi();
public slots:
     void onFileOpen();
     void onFileSave();
     void onControlBegin();
     void onControlPrev();
     void onControlPlay();
     void onControlNext();
     void onControlEnd();

     void onZoomin(){}
     void onZoomout(){}
     void about();
private:
    QAction *mOpenAct;
    QAction *mSaveAct;
    QAction *mZoominAct;
    QAction *mZoomoutAct;
    QAction *mBeginAct;
    QAction *mPrevAct;
    QAction *mPlayAct;
    QAction *mNextAct;
    QAction *mEndAct;


};

#endif // MAINWINDOW_H
