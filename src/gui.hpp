#ifndef SCREENSHOTGUI_HPP
#define SCREENSHOTGUI_HPP

#include <QCommandLineParser>
#include <QWidget>
#include "recorder.hpp"
#include "screenshot.hpp"

class QCheckBox;
class QGridLayout;
class QGroupBox;
class QHBoxLayout;
class QLabel;
class QPushButton;
class QSpinBox;
class QVBoxLayout;
class QPixmap;

class GUI : public QWidget
{
    Q_OBJECT

public:
    GUI(qreal fps, qint8 screen_id, qint16 screen_x, qint16 screen_y, qint16 screen_w, qint16 screen_h);

protected:
    void resizeEvent(QResizeEvent* event) Q_DECL_OVERRIDE;

signals:
    void startRecording() const;
    void stopRecording() const;
    void setDebug(bool) const;

private slots:
    void toggleDebugModeButtonClicked();
    void startRecordingClicked();
    void stopRecordingClicked();
    void onFrameReady();

private:
    // this one actually updates the screenshot label
    void updateFrameLabel();

    Recorder* m_recorder;
    QThread* m_thread_recorder;

    // Display frames here
    QLabel* m_screenshot_label;

    // Hide program if checked
    QCheckBox* m_hide_window_checkbox;
};

#endif // SCREENSHOTGUI_HPP
