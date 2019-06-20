#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "imagelib.h"

namespace Ui {
class MainWindow;
}
typedef struct tagVideoFile {
    bool bIsY4M; //true if the file is Y4M
    FILE*   fd;
    int width;
    int stride;//bytes per line
    int height;
    float fps;
    int interlace;//0 progressive, 1 top field first, 2 bottom field first
    int colorspace; //0: YUV420, 1:YUV422, 2YUV444
    //
    int offset; //offset bytes of first frame;
    ColorspaceConvertFunc convertFunc;
    int bytesPerframe;//frame header + bit data
    int frameHeader; //length f frame header
    int totalFrames;
    int curFrame;
    void* yuvBuffer;
    void* rgbBuffer;
 }VideoFile;

class ImageWin;
class QScrollBar;
class QTimer;
class QLabel;
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    bool loadFile(const QString &filename, VideoFile& vf);
    bool loadFileYuv(const QString & filename, VideoFile& vf, bool isPlanMode=true);
    bool loadFileY4m(const char* filename, VideoFile& vf);
private:
    Ui::MainWindow *ui;
    void createMenuAndToolbar();
    void createUi();
    void updateActions();
    bool saveFile(const QString &fileName);
    void setImage(const QImage &newImage);
    void scaleImage(int incIndex);
    void adjustScrollBar(QScrollBar *scrollBar, double factor);
    bool readCurrentFrame();

public slots:
     void onFileOpenSource();
     void onFileOpenTagert();
     void onFileSave();
     void onControlBegin();
     void onControlPrev();
     void onControlPlay();
     void onControlNext();
     void onControlEnd();

     void onViewZoomin();
     void onViewZoomout();
     void onViewNormalSize();
     void onViewFitToWindow();
     void onViewShowRuler();
     void about();
private:
    ImageWin *mImageView;
    double mZoomFactor;
    static int sMaxZoomIndex;
    static int sCurZoomIndex;
    QAction *mOpenSourceAct;
    QAction *mOpenTargetAct;
    QAction *mSaveAct;
    QAction *mZoomInAct;
    QAction *mZoomOutAct;
    QAction *mNormalSizeAct;
    QAction *mFitToWindowAct;
    QAction *mBeginAct;
    QAction *mPrevAct;
    QAction *mPlayAct;
    QAction *mNextAct;
    QAction *mEndAct;
    QLabel* mZoomStatus;
    QLabel* mSizeStatus;
    QLabel* mPageStatus;
    bool mIsPlaying;
    VideoFile mSourceVideo;
    VideoFile mTargetVideo;

    QTimer* mpTimer;

};

#endif // MAINWINDOW_H
