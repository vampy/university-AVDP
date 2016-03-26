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

GUI::GUI() : screenshotLabel(new QLabel(this)), m_screenshot(new Screenshot), m_recorder(new Recorder(this, 25))
{
    screenshotLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    screenshotLabel->setAlignment(Qt::AlignCenter);

    const QRect screenGeometry = QApplication::desktop()->screenGeometry(this);
    screenshotLabel->setMinimumSize(screenGeometry.width() / 8, screenGeometry.height() / 8);

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(screenshotLabel);

    QGroupBox* optionsGroupBox = new QGroupBox("Options", this);
    delaySpinBox = new QSpinBox(optionsGroupBox);
    delaySpinBox->setSuffix(" s");
    delaySpinBox->setMaximum(60);

    typedef void (QSpinBox::*QSpinBoxIntSignal)(int);
    connect(delaySpinBox, static_cast<QSpinBoxIntSignal>(&QSpinBox::valueChanged), this, &GUI::updateCheckBox);

    hideThisWindowCheckBox = new QCheckBox("Hide This Window", optionsGroupBox);

    QGridLayout* optionsGroupBoxLayout = new QGridLayout(optionsGroupBox);
    optionsGroupBoxLayout->addWidget(new QLabel("Screenshot Delay:", this), 0, 0);
    optionsGroupBoxLayout->addWidget(delaySpinBox, 0, 1);
    optionsGroupBoxLayout->addWidget(hideThisWindowCheckBox, 1, 0, 1, 2);
    mainLayout->addWidget(optionsGroupBox);

    QHBoxLayout* buttonsLayout = new QHBoxLayout;
    newScreenshotButton = new QPushButton("New Screenshot", this);
    startRecordingButton = new QPushButton("Start Recording", this);
    stopRecordingButton = new QPushButton("Stop Recording", this);
    QPushButton* saveScreenshotButton = new QPushButton("Save Screenshot", this);
    QPushButton* quitScreenshotButton = new QPushButton("Quit", this);
    quitScreenshotButton->setShortcut(Qt::CTRL + Qt::Key_Q);

    connect(newScreenshotButton, &QPushButton::clicked, this, &GUI::newScreenshotClicked);
    connect(startRecordingButton, &QPushButton::clicked, this, &GUI::startRecordingClicked);
    connect(stopRecordingButton, &QPushButton::clicked, this, &GUI::stopRecordingClicked);
    connect(saveScreenshotButton, &QPushButton::clicked, this, &GUI::saveScreenshotClicked);
    connect(quitScreenshotButton, &QPushButton::clicked, this, &QWidget::close);

    // new screenshot was taken signal
    connect(m_screenshot, &Screenshot::onScreenshot, this, &GUI::newScreenshot);
    connect(m_recorder, &Recorder::onFrameReady, this, &GUI::newFrame);
    buttonsLayout->addWidget(newScreenshotButton);
    buttonsLayout->addWidget(startRecordingButton);
    buttonsLayout->addWidget(stopRecordingButton);
    buttonsLayout->addWidget(saveScreenshotButton);
    buttonsLayout->addWidget(quitScreenshotButton);
    buttonsLayout->addStretch();
    mainLayout->addLayout(buttonsLayout);

    //    m_screenshot->take();

    delaySpinBox->setValue(5);
    setWindowTitle("Screenshot");
    resize(300, 200);
}

void GUI::resizeEvent(QResizeEvent* /* event */)
{
    QSize scaledSize = m_screenshot->getPixmap().size();
    scaledSize.scale(screenshotLabel->size(), Qt::KeepAspectRatio);

    if (!screenshotLabel->pixmap() || scaledSize != screenshotLabel->pixmap()->size())
        updateScreenshotLabel();
}

void GUI::newScreenshotClicked()
{
    if (hideThisWindowCheckBox->isChecked())
        hide();
    newScreenshotButton->setDisabled(true);

    // after the delay will call the slot bellow, newScreenshot
    m_screenshot->take(delaySpinBox->value() * 1000);
}

void GUI::newScreenshot()
{
    qDebug() << "Screenshot taken";
    updateGuiAfterScreenshot();
}

void GUI::saveScreenshotClicked()
{
    const QString format = "png";
    QString initialPath = QStandardPaths::writableLocation(QStandardPaths::PicturesLocation);
    if (initialPath.isEmpty())
        initialPath = QDir::currentPath();

    initialPath += "/untitled." + format;

    QFileDialog fileDialog(this, "Save As", initialPath);
    fileDialog.setAcceptMode(QFileDialog::AcceptSave);
    fileDialog.setFileMode(QFileDialog::AnyFile);
    fileDialog.setDirectory(initialPath);

    QStringList mimeTypes;
    foreach (const QByteArray& bf, QImageWriter::supportedMimeTypes())
        mimeTypes.append(QLatin1String(bf));

    fileDialog.setMimeTypeFilters(mimeTypes);
    fileDialog.selectMimeTypeFilter("image/" + format);
    fileDialog.setDefaultSuffix(format);

    if (fileDialog.exec() != QDialog::Accepted)
        return;

    const QString fileName = fileDialog.selectedFiles().first();
    if (!m_screenshot->getPixmap().save(fileName))
    {
        QMessageBox::warning(this, "Save Error",
            QString("The image could not be saved to \"%1\".").arg(QDir::toNativeSeparators(fileName)));
    }
}

void GUI::updateCheckBox()
{
    if (delaySpinBox->value() == 0)
    {
        hideThisWindowCheckBox->setDisabled(true);
        hideThisWindowCheckBox->setChecked(false);
    }
    else
    {
        hideThisWindowCheckBox->setDisabled(false);
    }
}

void GUI::startRecordingClicked() { m_recorder->startRecording(); }

void GUI::stopRecordingClicked() { m_recorder->stopRecording(); }

void GUI::newFrame() { updateGuiAfterNewFrame(); }

void GUI::updateGuiAfterNewFrame() { updateFrameLabel(); }

void GUI::updateFrameLabel()
{
    auto image = m_recorder->getCurrentFrame();
    screenshotLabel->setPixmap(
        QPixmap::fromImage(image).scaled(screenshotLabel->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
}

void GUI::updateScreenshotLabel()
{
    auto pixmap = m_screenshot->getPixmap();
    // scale to the label
    screenshotLabel->setPixmap(pixmap.scaled(screenshotLabel->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
}

// Update all the componensts afte a screenshot was taken
void GUI::updateGuiAfterScreenshot()
{
    updateScreenshotLabel();

    newScreenshotButton->setDisabled(false);
    if (hideThisWindowCheckBox->isChecked())
        show();
}
