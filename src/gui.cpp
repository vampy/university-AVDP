/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QtWidgets>

#include "gui.hpp"

GUI::GUI() : m_recorder(new Recorder(this, 25)), m_screenshot_label(new QLabel(this)), m_screenshot(new Screenshot)
{
    m_screenshot_label->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_screenshot_label->setAlignment(Qt::AlignCenter);

    const QRect screenGeometry = QApplication::desktop()->screenGeometry(this);
    m_screenshot_label->setMinimumSize(screenGeometry.width() / 8, screenGeometry.height() / 8);

    auto mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(m_screenshot_label);

    auto optionsGroupBox = new QGroupBox("Options", this);

    m_hide_window_checkbox = new QCheckBox("Hide This Window", optionsGroupBox);

    auto optionsGroupBoxLayout = new QGridLayout(optionsGroupBox);
    optionsGroupBoxLayout->addWidget(m_hide_window_checkbox, 1, 0, 1, 2);
    mainLayout->addWidget(optionsGroupBox);

    auto buttonsLayout = new QHBoxLayout;
    m_start_recording_button = new QPushButton("Start Recording", this);
    m_stop_recording_button = new QPushButton("Stop Recording", this);
    auto toggleDebugModeButton = new QPushButton("Toggle Debug", this);
    auto quitScreenshotButton = new QPushButton("Quit", this);
    quitScreenshotButton->setShortcut(Qt::CTRL + Qt::Key_Q);

    connect(m_start_recording_button, &QPushButton::clicked, this, &GUI::startRecordingClicked);
    connect(m_stop_recording_button, &QPushButton::clicked, this, &GUI::stopRecordingClicked);
    connect(toggleDebugModeButton, &QPushButton::clicked, this, &GUI::toggleDebugModeButtonClicked);
    connect(quitScreenshotButton, &QPushButton::clicked, this, &QWidget::close);

    // new screenshot was taken signal
    connect(m_screenshot, &Screenshot::onScreenshot, this, &GUI::newScreenshot);
    connect(m_recorder, &Recorder::onFrameReady, this, &GUI::newFrame);

    buttonsLayout->addWidget(m_start_recording_button);
    buttonsLayout->addWidget(m_stop_recording_button);
    buttonsLayout->addWidget(toggleDebugModeButton);
    buttonsLayout->addWidget(quitScreenshotButton);
    buttonsLayout->addStretch();
    mainLayout->addLayout(buttonsLayout);

    setWindowTitle(constants::APP_NAME);
    resize(300, 200);
}

void GUI::resizeEvent(QResizeEvent* /* event */)
{
    //    QSize scaledSize = m_screenshot->getPixmap().size();
    //    scaledSize.scale(screenshotLabel->size(), Qt::KeepAspectRatio);

    //    if (!screenshotLabel->pixmap() || scaledSize != screenshotLabel->pixmap()->size())
    //        updateScreenshotLabel();
}

void GUI::newScreenshot()
{
    qDebug() << "Screenshot taken";
    updateGuiAfterScreenshot();
}

void GUI::toggleDebugModeButtonClicked()
{
    static bool toggle = false;
    toggle = !toggle;
    m_recorder->setDebug(toggle);
}

void GUI::startRecordingClicked() { m_recorder->startRecording(); }

void GUI::stopRecordingClicked() { m_recorder->stopRecording(); }

void GUI::newFrame() { updateGuiAfterNewFrame(); }

void GUI::updateGuiAfterNewFrame() { updateFrameLabel(); }

void GUI::updateFrameLabel()
{
    auto image = m_recorder->getCurrentFrame();
    m_screenshot_label->setPixmap(
        QPixmap::fromImage(image).scaled(m_screenshot_label->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
}

void GUI::updateScreenshotLabel()
{
    auto pixmap = m_screenshot->getPixmap();
    // scale to the label
    m_screenshot_label->setPixmap(pixmap.scaled(m_screenshot_label->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
}

// Update all the componensts afte a screenshot was taken
void GUI::updateGuiAfterScreenshot()
{
    updateScreenshotLabel();

    if (m_hide_window_checkbox->isChecked())
        show();
}
