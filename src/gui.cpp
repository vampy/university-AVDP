#include <QtWidgets>

#include "gui.hpp"

GUI::GUI(qreal fps, qint8 screen_id, qint16 screen_x, qint16 screen_y, qint16 screen_w, qint16 screen_h)
    : m_recorder(new Recorder(nullptr, fps, screen_id, screen_x, screen_y, screen_w, screen_h)),
      m_thread_recorder(new QThread(this)),
      m_screenshot_label(new QLabel(this))
{
    m_screenshot_label->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_screenshot_label->setAlignment(Qt::AlignCenter);

    const QRect screenGeometry = QApplication::desktop()->screenGeometry(this);
    m_screenshot_label->setMinimumSize(screenGeometry.width() / 8, screenGeometry.height() / 8);

    auto main_layout = new QVBoxLayout(this);
    main_layout->addWidget(m_screenshot_label);

    auto optionsGroupBox = new QGroupBox("Options", this);

    m_hide_window_checkbox = new QCheckBox("Hide This Window", optionsGroupBox);

    auto options_groupbox_layout = new QGridLayout(optionsGroupBox);
    options_groupbox_layout->addWidget(m_hide_window_checkbox, 1, 0, 1, 2);
    main_layout->addWidget(optionsGroupBox);

    auto buttons_layout           = new QHBoxLayout;
    auto start_recording_button   = new QPushButton("Start Recording", this);
    auto stop_recording_button    = new QPushButton("Stop Recording", this);
    auto toggle_debug_mode_button = new QPushButton("Toggle Debug", this);
    auto quit_screenshot_button   = new QPushButton("Quit", this);

    // set shortcuts
    quit_screenshot_button->setShortcut(Qt::CTRL + Qt::Key_Q);
    start_recording_button->setShortcut(Qt::CTRL + Qt::Key_R);
    stop_recording_button->setShortcut(Qt::CTRL + Qt::Key_S);

    connect(start_recording_button, &QPushButton::clicked, this, &GUI::startRecordingClicked);
    connect(stop_recording_button, &QPushButton::clicked, this, &GUI::stopRecordingClicked);
    connect(toggle_debug_mode_button, &QPushButton::clicked, this, &GUI::toggleDebugModeButtonClicked);
    connect(quit_screenshot_button, &QPushButton::clicked, this, &QWidget::close);

    // move thread
    m_recorder->moveToThread(m_thread_recorder);
    connect(m_thread_recorder, &QThread::finished, m_recorder, &QObject::deleteLater);
    connect(this, &GUI::startRecording, m_recorder, &Recorder::startRecording);
    connect(this, &GUI::stopRecording, m_recorder, &Recorder::stopRecording);
    connect(this, &GUI::setDebug, m_recorder, &Recorder::setDebug);

    // new screenshot was taken signal
    connect(m_recorder, &Recorder::onFrameReady, this, &GUI::onFrameReady);

    buttons_layout->addWidget(start_recording_button);
    buttons_layout->addWidget(stop_recording_button);
    buttons_layout->addWidget(toggle_debug_mode_button);
    buttons_layout->addWidget(quit_screenshot_button);
    buttons_layout->addStretch();
    main_layout->addLayout(buttons_layout);

    setWindowTitle(constants::APP_NAME);
    resize(300, 200);

    m_thread_recorder->start();
}

void GUI::resizeEvent(QResizeEvent* /* event */) { updateFrameLabel(); }

void GUI::toggleDebugModeButtonClicked()
{
    static bool toggle = false;
    toggle             = !toggle;
    emit setDebug(toggle);
}

void GUI::startRecordingClicked()
{
    if (m_hide_window_checkbox->isChecked())
        showMinimized();

    emit startRecording();
}

void GUI::stopRecordingClicked() { emit stopRecording(); }

void GUI::onFrameReady() { updateFrameLabel(); }

void GUI::updateFrameLabel()
{
    auto image = m_recorder->getCurrentFrame();
    if (!image.isNull())
    {
        m_screenshot_label->setPixmap(QPixmap::fromImage(image).scaled(
            m_screenshot_label->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    }
}
