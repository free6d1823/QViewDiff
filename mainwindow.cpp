#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>

MainWindow* gMainWnd = NULL;

void MainWindow::about()
{
    QMessageBox::about(this, tr("About ViewDiff"),
             tr("The <b>ViewDiff</b> show video differences"));
}


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    gMainWnd = this;
    ui->setupUi(this);
    createMenuAndToolbar();
    createUi();
}

MainWindow::~MainWindow()
{
    delete ui;
}
void MainWindow::createMenuAndToolbar()
{
    QToolBar *fileToolBar = ui->toolBar;
    // FILE  /////////////////////////
    QMenu* fileMenu = ui->menubar->addMenu(tr("&File"));
    const QIcon openIcon = QIcon(":/images/open.png");
    mOpenAct = fileMenu->addAction(openIcon, tr("&Open Reference..."), this,
                    SLOT(onFileOpen()), QKeySequence::Open);
    mOpenAct->setStatusTip(tr("Open the reference image or video file."));
    const QIcon saveIcon = QIcon(":/images/save.png");
    mSaveAct = fileMenu->addAction(saveIcon, tr("&Save"), this,
                    SLOT(onFileSave()), QKeySequence::Save);
    mSaveAct->setStatusTip(tr("Save resuts to project file."));
    fileMenu->addSeparator();
    const QIcon exitIcon =  QIcon(":/images/exit.png");
    QAction* exitAct = fileMenu->addAction(exitIcon,tr("E&xit"), this,
            SLOT(close()), QKeySequence::Quit);
    exitAct->setStatusTip(tr("Exit this program."));

    fileToolBar->addAction(mOpenAct);
    fileToolBar->addAction(mSaveAct);

    // Control  /////////////////////////
    QMenu* ctrlMenu = ui->menubar->addMenu(tr("&Control"));
    const QIcon beginIcon = QIcon(":/images/pbegin_on.png");
    mBeginAct = ctrlMenu->addAction(beginIcon, tr("&Begin"), this,
                    SLOT(onControlBegin()), QKeySequence::MoveToStartOfDocument);
    mBeginAct->setStatusTip(tr("Go to first frame"));

    const QIcon prevIcon = QIcon(":/images/pslow_on.png");
    mPrevAct = ctrlMenu->addAction(prevIcon, tr("&Previous frame"), this,
                    SLOT(onControlPrev()), QKeySequence::MoveToPreviousPage);
    mPrevAct->setStatusTip(tr("Go to previous frame."));

    const QIcon playIcon = QIcon(":/images/pplay_on.png");
    mPlayAct = ctrlMenu->addAction(playIcon, tr("&Play"), this,
                    SLOT(onControlPlay()), QKeySequence::AddTab);
    mPlayAct->setStatusTip(tr("Play the sequence"));

    const QIcon nextIcon = QIcon(":/images/pfast_on.png");
    mNextAct = ctrlMenu->addAction(nextIcon, tr("&Next"), this,
                    SLOT(onControlNext()), QKeySequence::MoveToNextPage);
    mNextAct->setStatusTip(tr("Go to next frame"));

    const QIcon endIcon = QIcon(":/images/pend_on.png");
    mEndAct = ctrlMenu->addAction(endIcon, tr("&End"), this,
                    SLOT(onControlEnd()), QKeySequence::MoveToEndOfDocument);
    mEndAct->setStatusTip(tr("Go to the last page"));


    fileToolBar->addSeparator();
    fileToolBar->addAction(mBeginAct);
    fileToolBar->addAction(mPrevAct);
    fileToolBar->addAction(mPlayAct);
    fileToolBar->addAction(mNextAct);
    fileToolBar->addAction(mEndAct);

    // View  /////////////////////////
    QMenu *viewMenu = menuBar()->addMenu(tr("&View"));
    const QIcon zoominIcon = QIcon(":/images/zoomin.png");
    QAction *zoominAct = viewMenu->addAction(zoominIcon, tr("&Zoom In"), this, SLOT(onZoomin()), QKeySequence::ZoomIn);
    zoominAct->setStatusTip(tr("Magnify the view"));
    const QIcon zoomoutIcon = QIcon(":/images/zoomout.png");
    QAction *zoomoutAct = viewMenu->addAction(zoomoutIcon, tr("&Zoom Out"), this, SLOT(onZoomout()), QKeySequence::ZoomOut);
    zoomoutAct->setStatusTip(tr("Shrink the view"));

    fileToolBar->addSeparator();
    fileToolBar->addAction(zoominAct);
    fileToolBar->addAction(zoomoutAct);

    // HELP  /////////////////////////
    QMenu *helpMenu = menuBar()->addMenu(tr("&Help"));
    const QIcon aboutIcon = QIcon(":/images/about.png");
    QAction *aboutAct = helpMenu->addAction(aboutIcon, tr("&About"), this, SLOT(about()), QKeySequence::HelpContents);
    aboutAct->setStatusTip(tr("Show the application's About box"));
    fileToolBar->addSeparator();
    fileToolBar->addAction(aboutAct);

}

void MainWindow::createUi()
{

}

void MainWindow::onFileOpen()
{

}

void MainWindow::onFileSave()
{

}
void MainWindow::onControlBegin()
{

}

void MainWindow::onControlPrev()
{

}

void MainWindow::onControlPlay()
{

}

void MainWindow::onControlNext()
{

}

void MainWindow::onControlEnd()
{

}
