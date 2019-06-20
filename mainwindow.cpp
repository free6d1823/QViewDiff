#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QFileDialog>
#include <QDir>
#include <QScrollBar>
#include <QScreen>
#include <QImageReader>
#include <QTimer>
#include <QLabel>
#include "imagelib.h"
#include "imagewin.h"

MainWindow* gMainWnd = NULL;

void MainWindow::about()
{
    QMessageBox::about(this, tr("About ViewDiff"),
             tr("The <b>ViewDiff</b> show video differences"));
}

static double sZoomFactor[] = {0.1, 0.2, 0.25, 0.4, 0.5, 0.6, 0.75, 0.8, 1.0, 2.0,4.0,5.0,6.0, 8.0,10.0};
#define DEFAULT_ZOOM_INDEX  8
#define MAX_ZOOM_INDEX  14
int MainWindow::sMaxZoomIndex = MAX_ZOOM_INDEX;
int MainWindow::sCurZoomIndex = DEFAULT_ZOOM_INDEX;
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    mImageView(new ImageWin),
    mZoomFactor(1)
{
    gMainWnd = this;
    ui->setupUi(this);
    memset(&mSourceVideo, 0, sizeof(VideoFile));
    memset(&mTargetVideo, 0, sizeof(VideoFile));
    setCentralWidget(mImageView);
    createMenuAndToolbar();
    createUi();
    resize(QGuiApplication::primaryScreen()->availableSize() * 3 / 5);
    updateActions();
    mIsPlaying = false;
    mpTimer = NULL;
}

MainWindow::~MainWindow()
{
    delete ui;
    if (mSourceVideo.fd) {
        if(mSourceVideo.yuvBuffer)
            free(mSourceVideo.yuvBuffer);
        //rgbBuffer is freed by imagewin
        fclose(mSourceVideo.fd);
    }
    if (mTargetVideo.fd) {
        if(mTargetVideo.yuvBuffer)
            free(mTargetVideo.yuvBuffer);
        //rgbBuffer is freed by imagewin
        fclose(mTargetVideo.fd);
    }
    if(mpTimer)
        delete mpTimer;
}
void MainWindow::createMenuAndToolbar()
{
    QToolBar *fileToolBar = ui->toolBar;
    // FILE  /////////////////////////
    QMenu* fileMenu = ui->menubar->addMenu(tr("&File"));
    const QIcon openIcon = QIcon(":/images/open.png");
    mOpenSourceAct = fileMenu->addAction(openIcon, tr("&Open Source..."), this,
                    SLOT(onFileOpenSource()), QKeySequence::Open);
    mOpenSourceAct->setStatusTip(tr("Open the reference image or video file."));
    const QIcon opentIcon = QIcon(":/images/opent.png");
    mOpenTargetAct = fileMenu->addAction(opentIcon, tr("&Open Target..."), this,
                    SLOT(onFileOpenTarget()), tr("Ctrl+T"));
    mOpenTargetAct->setStatusTip(tr("Open the downgrade image or video file."));


    const QIcon saveIcon = QIcon(":/images/save.png");
    mSaveAct = fileMenu->addAction(saveIcon, tr("&Save"), this,
                    SLOT(onFileSave()), QKeySequence::Save);
    mSaveAct->setStatusTip(tr("Save resuts to project file."));
    fileMenu->addSeparator();
    const QIcon exitIcon =  QIcon(":/images/exit.png");
    QAction* exitAct = fileMenu->addAction(exitIcon,tr("E&xit"), this,
            SLOT(close()), QKeySequence::Quit);
    exitAct->setStatusTip(tr("Exit this program."));

    fileToolBar->addAction(mOpenSourceAct);
    fileToolBar->addAction(mOpenTargetAct);
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
    mZoomInAct = viewMenu->addAction(zoominIcon, tr("&Zoom In"), this, SLOT(onViewZoomin()), QKeySequence::ZoomIn);
    mZoomInAct->setStatusTip(tr("Magnify the view"));
    const QIcon zoomoutIcon = QIcon(":/images/zoomout.png");
    mZoomOutAct = viewMenu->addAction(zoomoutIcon, tr("&Zoom Out"), this, SLOT(onViewZoomout()), QKeySequence::ZoomOut);
    mZoomOutAct->setStatusTip(tr("Shrink the view"));
    const QIcon normalSizeIcon = QIcon(":/images/normalsize.png");
    mNormalSizeAct = viewMenu->addAction(normalSizeIcon, tr("Normal &Size"), this,
                     SLOT(onViewNormalSize()), tr("Ctrl+S") );
    viewMenu->addSeparator();
    mNormalSizeAct->setStatusTip(tr("set normal size image."));
    mNormalSizeAct->setEnabled(false);
     const QIcon fitIcon = QIcon(":/images/fittowindow.png");
    mFitToWindowAct = viewMenu->addAction(fitIcon, tr("Fit to Window"), this,
                     SLOT(onViewFitToWindow()), tr("Ctrl+S") );
    mFitToWindowAct->setStatusTip(tr("set normal size image."));
    mFitToWindowAct->setEnabled(false);
    mFitToWindowAct->setCheckable(true);
    mFitToWindowAct->setShortcut(tr("Ctrl+F"));

    const QIcon rulerIcon = QIcon(":/images/ruler.png");
    QAction* rulerAct = viewMenu->addAction(rulerIcon, tr("Show &Ruler"), this,
                     SLOT(onViewShowRuler()), tr("Ctrl+R") );

    fileToolBar->addSeparator();
    fileToolBar->addAction(mZoomInAct);
    fileToolBar->addAction(mZoomOutAct);
    fileToolBar->addAction(mNormalSizeAct);
    fileToolBar->addSeparator() ;
    fileToolBar->addAction(mFitToWindowAct);
    fileToolBar->addAction(rulerAct);

    // HELP  /////////////////////////
    QMenu *helpMenu = menuBar()->addMenu(tr("&Help"));
    const QIcon aboutIcon = QIcon(":/images/about.png");
    QAction *aboutAct = helpMenu->addAction(aboutIcon, tr("&About"), this, SLOT(about()), QKeySequence::HelpContents);
    aboutAct->setStatusTip(tr("Show the application's About box"));
    fileToolBar->addSeparator();
    fileToolBar->addAction(aboutAct);

}
void MainWindow::updateActions()
{

    mSaveAct->setEnabled(mSourceVideo.fd != NULL);
    mZoomInAct->setEnabled(!mFitToWindowAct->isChecked());
    mZoomOutAct->setEnabled(!mFitToWindowAct->isChecked());
    mNormalSizeAct->setEnabled(!mFitToWindowAct->isChecked());
    mBeginAct->setEnabled(mSourceVideo.curFrame !=0 && mSourceVideo.totalFrames >0);
    mPrevAct->setEnabled(mSourceVideo.curFrame !=0 && mSourceVideo.totalFrames >0);
    mPlayAct->setEnabled(mSourceVideo.curFrame !=mSourceVideo.totalFrames-1 && mSourceVideo.totalFrames >0);

    mNextAct->setEnabled(mSourceVideo.curFrame !=mSourceVideo.totalFrames-1 && mSourceVideo.totalFrames >0);
    mEndAct->setEnabled(mSourceVideo.curFrame !=mSourceVideo.totalFrames-1 && mSourceVideo.totalFrames >0);

    mZoomStatus->setText(tr("%1x").arg(mZoomFactor));
    if(mSourceVideo.width >0)
        mSizeStatus->setText(tr("%1x%2").arg(mSourceVideo.width).arg(mSourceVideo.height));
    else
        mSizeStatus->setText("");
    if(mSourceVideo.totalFrames >0)
        mPageStatus->setText(tr("%1/%2fr.").arg(mSourceVideo.curFrame+1).arg(mSourceVideo.totalFrames));
    else
        mSizeStatus->setText("");

}

void MainWindow::setImage(const QImage &newImage)
{
    int width = newImage.width();
    sMaxZoomIndex = MAX_ZOOM_INDEX;
    //limits maximum pixels to 25K

    while( width * sZoomFactor[sMaxZoomIndex] > 25000) {
        sMaxZoomIndex --;
    }
    sCurZoomIndex = DEFAULT_ZOOM_INDEX;

    mZoomFactor = sZoomFactor[sCurZoomIndex];
    mImageView->setImage(newImage);
    mImageView->scaleImage(mZoomFactor);

    //updateActions();
}
void MainWindow::scaleImage(int incIndex)
{
    sCurZoomIndex += incIndex;
    sCurZoomIndex = ((sCurZoomIndex<0)? 0:((sCurZoomIndex > sMaxZoomIndex)? sMaxZoomIndex:sCurZoomIndex));
    double oldZoom = mZoomFactor;
    mZoomFactor = sZoomFactor[sCurZoomIndex];
    mImageView->scaleImage(mZoomFactor);

    adjustScrollBar(mImageView->horizontalScrollBar(), mZoomFactor/oldZoom);
    adjustScrollBar(mImageView->verticalScrollBar(), mZoomFactor/oldZoom);

    mZoomInAct->setEnabled(sCurZoomIndex < sMaxZoomIndex);
    mZoomOutAct->setEnabled(sCurZoomIndex > 0);
}
void MainWindow::adjustScrollBar(QScrollBar *scrollBar, double factor)
{
    //to keep center remain on center after zoom
    scrollBar->setValue(int(factor * scrollBar->value() +
                            (factor-1.0)*scrollBar->pageStep()/2));
}


void MainWindow::onViewZoomin()
{
    scaleImage(1);
    mZoomStatus->setText(tr("%1x").arg(mZoomFactor));
}

void MainWindow::onViewZoomout()
{
    scaleImage(-1);
    mZoomStatus->setText(tr("%1x").arg(mZoomFactor));
}

void MainWindow::onViewNormalSize()
{
    sCurZoomIndex = DEFAULT_ZOOM_INDEX;
    mZoomFactor = sZoomFactor[sCurZoomIndex];
    mImageView->adjustSize();
    mZoomStatus->setText(tr("%1x").arg(mZoomFactor));
}

void MainWindow::onViewFitToWindow()
{
    bool fitToWindow = mFitToWindowAct->isChecked();
    mImageView->setWidgetResizable(fitToWindow);
    if (!fitToWindow)
        onViewNormalSize();
    updateActions();
    mZoomStatus->setText(tr("%1x").arg(mZoomFactor));
}
void MainWindow::onViewShowRuler()
{
    mImageView->showRulers(!mImageView->isRulersShown());
}

void MainWindow::createUi()
{
    mZoomStatus =  new QLabel(this);
    mSizeStatus = new QLabel(this);
    mPageStatus = new QLabel(this);
    ui->statusbar->addPermanentWidget(mZoomStatus);
    ui->statusbar->addPermanentWidget(mSizeStatus);
    ui->statusbar->addPermanentWidget(mPageStatus);
}

void MainWindow::onFileOpenSource()
{
    QString filename = QFileDialog::getOpenFileName(this,tr("Open Imahe"), QDir::currentPath(),
          tr( "Y4M file(*.y4m);;"
              "YUYV packet mode (*.yuv)") );

    if (!filename.isNull()){
        if (mSourceVideo.fd > 0){
            fclose(mSourceVideo.fd);
            if(mSourceVideo.yuvBuffer) free(mSourceVideo.yuvBuffer);
            if(mSourceVideo.rgbBuffer) free(mSourceVideo.rgbBuffer);
        }
        bool bOK;
        memset(&mSourceVideo, 0, sizeof(VideoFile));
        if(filename.contains(".yuv"))
            bOK = loadFileYuv(filename, mSourceVideo);
        else if (filename.contains(".y4m")) {
            bOK = loadFileY4m(filename.toUtf8().data(), mSourceVideo);
        }
        else if(filename.contains(".yuyv"))
            bOK = loadFileYuv(filename, mSourceVideo, false);
        else
            bOK = loadFile(filename, mSourceVideo);

        if (bOK ) {
            if(readCurrentFrame())
            {
                QString file1 = QDir::toNativeSeparators(filename);
                QString message = tr("Opened \"%1\", %2x%3, %4 frames")
                    .arg(file1).arg(mSourceVideo.width).arg(mSourceVideo.height).arg(mSourceVideo.totalFrames);
                statusBar()->showMessage(message);

            }
            setWindowFilePath(filename);
        }
        updateActions();
    }
}
void MainWindow::onFileOpenTagert()
{
    QString filename = QFileDialog::getOpenFileName(this,tr("Open Imahe"), QDir::currentPath(),
          tr( "Y4M file(*.y4m);;"
              "YUYV packet mode (*.yuv)") );

    if (!filename.isNull()){

        if (mSourceVideo.fd > 0){
            fclose(mSourceVideo.fd);
            if(mSourceVideo.yuvBuffer) free(mSourceVideo.yuvBuffer);
            if(mSourceVideo.rgbBuffer) free(mSourceVideo.rgbBuffer);
        }
        memset(&mSourceVideo, 0, sizeof(VideoFile));
        if(filename.contains(".yuv"))
            loadFileYuv(filename, mSourceVideo);
        else if (filename.contains(".y4m")) {
            loadFileY4m(filename.toUtf8().data(), mSourceVideo);
        }
        else if(filename.contains(".yuyv"))
            loadFileYuv(filename, mSourceVideo, false);
        else
            loadFile(filename, mSourceVideo);


        updateActions();
    }
}
bool MainWindow::loadFile(const QString &filename, VideoFile& vf)
{
    return true;
}

bool MainWindow::loadFileYuv(const QString & filename, VideoFile& vf, bool isPlanMode)
{
     return true;
}

bool MainWindow::loadFileY4m(const char* filename, VideoFile& vf)
{
    FILE* fp;
    fp = fopen(filename, "rb");
    if (fp == NULL) {
        return false;
    }

    char line[256];
    bool bOK = false;
    int frameFactor = 3;//used to count frame size w*h*frameFactor/2
    while(fgets(line, sizeof(line), fp) != NULL){
        char* p1 = strtok(line, " ");
        if(0!= strcmp(p1, "YUV4MPEG2")) {
            QMessageBox::information(this, QGuiApplication::applicationDisplayName(),
                                     tr("Not a y4m file format"));
            break;
        }

        vf.bIsY4M = true;
        vf.convertFunc = Yuy420ToRgb32;
       while((p1 = strtok(NULL, " \n")) != NULL){
           switch(p1[0]){
           case 'W':
               vf.width =  atoi(p1+1);
               break;
           case 'H':
               vf.height = atoi(p1+1);
               break;
           case 'I':
               if (p1[1] == 'p')
                   vf.interlace = 0;
               else if(p1[1] == 't')
                   vf.interlace = 1;
               else if(p1[1] == 'b')
                   vf.interlace = 2;
               break;
           case 'F': //Fdd:nn
           {
               char* p2 = strchr(p1, ':');
               if (p2!= NULL){
                   *p2 = 0;
                   int dd = atoi(p1+1);
                   int nn = atoi(p2+1);
                   if (nn>0)
                       vf.fps = (float)dd/(float)nn;
               }
               break;
           }
           case 'C':
           {
               char* p2 = p1+1;
               if ( strcmp(p2, "444") == 0) {
                   vf.colorspace = 2;
               } else if (strcmp(p2, "422") == 0) {
                   vf.colorspace = 1;
               } else  { //420
                   vf.colorspace = 0;
               }
               break;
           }
           default:
               break;

           }
           //check next param
       }
       //support plan mode only stride = width
       vf.stride = vf.width;

       switch(vf.colorspace) {
        case 2: //444
           frameFactor = 6;
           vf.convertFunc = Yuy444ToRgb32;
           break;
       case 1: //422
          frameFactor = 4;
          vf.convertFunc = Yuy422ToRgb32;
          break;
       case 0:
       default: //420
           frameFactor = 3;
           vf.convertFunc = Yuy420ToRgb32;


       }

       //next line, search FRAME
       if(fgets(line, sizeof(line), fp) == NULL) {
           break;
       }
       if (strstr(line, "FRAME")==NULL)
           break;

       vf.fd = fp;
       vf.frameHeader = strlen(line);
       vf.offset = ftell(fp) - vf.frameHeader;
       vf.bytesPerframe = frameFactor * vf.width * vf.height/2 + vf.frameHeader;

       fseek(fp, 0, SEEK_END);
       long len = ftell(fp);
       fseek(fp,vf.offset, SEEK_SET );
       vf.totalFrames = (len- vf.offset)/vf.bytesPerframe;
       //read first frame

       unsigned char* pRgb32 = (unsigned char*) malloc(vf.width*4*vf.height);
       if (!pRgb32)
           break;
       void* pYuv = malloc(vf.bytesPerframe);
       if (!pYuv) {
           free(pRgb32);
           break;
       }
       vf.yuvBuffer = pYuv;
       vf.rgbBuffer = pRgb32;

       vf.curFrame = 0;

        bOK = true;
        break;
    }
    if(!bOK){
        if(vf.yuvBuffer) free(vf.yuvBuffer);
        if(vf.rgbBuffer) free(vf.rgbBuffer);
        fclose(fp);
        memset(&vf, 0, sizeof(VideoFile));
    }
    return bOK;
}
void MainWindow::onFileSave()
{

}
void MainWindow::onControlBegin()
{
    mSourceVideo.curFrame = 0;
    updateActions();
}

void MainWindow::onControlPrev()
{
    if(mSourceVideo.curFrame > 0)
        mSourceVideo.curFrame --;
    updateActions();

}
bool MainWindow::readCurrentFrame()
{
    if (mSourceVideo.curFrame < 0 ||  mSourceVideo.curFrame >= mSourceVideo.totalFrames) {
        return false;
    }
    long offset = mSourceVideo.offset + mSourceVideo.bytesPerframe* mSourceVideo.curFrame;
    if(0 != fseek(mSourceVideo.fd, offset, SEEK_SET ))
            return false;
    if ( fread((char* )mSourceVideo.yuvBuffer, 1, mSourceVideo.bytesPerframe, mSourceVideo.fd) >0){
        mSourceVideo.convertFunc((unsigned char*)mSourceVideo.yuvBuffer + mSourceVideo.frameHeader,
                mSourceVideo.width, mSourceVideo.width, mSourceVideo.height,
                (unsigned char*)mSourceVideo.rgbBuffer);
    }else{
        return false;
    }
    QImage* newImage = mImageView->getImage();
    if(newImage->isNull()) {
            newImage = new QImage((unsigned char*)mSourceVideo.rgbBuffer,
                mSourceVideo.width, mSourceVideo.height, QImage::Format_RGBA8888);
    } else if(newImage->width()!= mSourceVideo.width || newImage->height() != mSourceVideo.height ) {

        newImage = new QImage((unsigned char*)mSourceVideo.rgbBuffer,
            mSourceVideo.width, mSourceVideo.height, QImage::Format_RGBA8888);

    }

    if(newImage) {
        setImage(*newImage);

    }
    return true;
}

void MainWindow::onControlPlay()
{
    mIsPlaying = !mIsPlaying;
    if(mIsPlaying && mSourceVideo.fps > 0){
        if(!mpTimer){
            mpTimer = new QTimer(this);
        }
        const QIcon pauseIcon = QIcon(":/images/ppause_on.png");
        mPlayAct->setIcon(pauseIcon);

//        connect (mpTimer, SIGNAL(timeout()), this, SLOT(onControlNext()));
//        mpTimer->start((int) (10000.0f/mSourceVideo.fps));
        mpTimer->singleShot((int) (10000.0f/mSourceVideo.fps), this, SLOT(onControlNext()));

    } else {
        if(mpTimer && mpTimer->isActive()){
            mpTimer->stop();
        }
        const QIcon playIcon = QIcon(":/images/pplay_on.png");
        mPlayAct->setIcon(playIcon);

    }
    updateActions();
}

void MainWindow::onControlNext()
{
    if (mSourceVideo.curFrame < mSourceVideo.totalFrames-1) {
        mSourceVideo.curFrame ++;
    }
    readCurrentFrame();
    if(mIsPlaying)
       mpTimer->singleShot((int) (10000.0f/mSourceVideo.fps), this, SLOT(onControlNext()));
    if(mSourceVideo.curFrame == mSourceVideo.totalFrames-1) {
        if (mpTimer) {
            mpTimer->stop();
            if(mIsPlaying)
                mIsPlaying = !mIsPlaying;
            const QIcon playIcon = QIcon(":/images/pplay_on.png");
            mPlayAct->setIcon(playIcon);
        }

    }
    updateActions();
}

void MainWindow::onControlEnd()
{
    if(mSourceVideo.totalFrames > 0)
        mSourceVideo.curFrame = mSourceVideo.totalFrames-1;
    if (mpTimer) {
        mpTimer->stop();
        if(mIsPlaying)
            mIsPlaying = !mIsPlaying;
        const QIcon playIcon = QIcon(":/images/pplay_on.png");
        mPlayAct->setIcon(playIcon);
    }
    updateActions();
}
