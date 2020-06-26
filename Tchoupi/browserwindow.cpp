/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** BSD License Usage
** Alternatively, you may use this file under the terms of the BSD license
** as follows:
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

#include "browser.h"
#include "browserwindow.h"
#include "downloadmanagerwidget.h"
#include "tabwidget.h"
#include "webview.h"
#include <QApplication>
#include <QCloseEvent>
#include <QDesktopWidget>
#include <QEvent>
#include <QFileDialog>
#include <QInputDialog>
#include <QMenuBar>
#include <QMessageBox>
#include <QProgressBar>
#include <QStatusBar>
#include <QToolBar>
#include <QVBoxLayout>
#include <QWebEngineProfile>
#include <QScreen>
#include <QDebug>

BrowserWindow::BrowserWindow(Browser *browser, QWebEngineProfile *profile, bool forDevTools)
    : m_browser(browser)
    , m_profile(profile)
    , m_tabWidget(new TabWidget(profile, this))
    , m_progressBar(nullptr)
    , m_historyBackAction(nullptr)
    , m_historyForwardAction(nullptr)
    , m_stopAction(nullptr)
    , m_reloadAction(nullptr)
    , m_stopReloadAction(nullptr)
    , m_urlLineEdit(nullptr)
    , m_favAction(nullptr)
{
    setAttribute(Qt::WA_DeleteOnClose, true);
    setFocusPolicy(Qt::ClickFocus);

    if (!forDevTools)
    {
        m_progressBar = new QProgressBar(this);

        QToolBar *toolbar = createToolBar();
        addToolBar(toolbar);
        menuBar()->addMenu(createViewMenu(toolbar));
    }

    QWidget *centralWidget = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout;
    layout->setSpacing(0);
    if (!forDevTools)
    {
        addToolBarBreak();

        m_progressBar->setMaximumHeight(1);
        m_progressBar->setTextVisible(false);
        m_progressBar->setStyleSheet(QStringLiteral("QProgressBar {border: 0px} QProgressBar::chunk {background-color: #da4453}"));

        layout->addWidget(m_progressBar);
    }

    layout->addWidget(m_tabWidget);
    centralWidget->setLayout(layout);
    setCentralWidget(centralWidget);

    connect(m_tabWidget, &TabWidget::titleChanged, this, &BrowserWindow::handleWebViewTitleChanged);
    if (!forDevTools)
    {
        connect(m_tabWidget, &TabWidget::linkHovered, [this](const QString& url) {
            statusBar()->showMessage(url);
        });
        connect(m_tabWidget, &TabWidget::loadProgress, this, &BrowserWindow::handleWebViewLoadProgress);
        connect(m_tabWidget, &TabWidget::webActionEnabledChanged, this, &BrowserWindow::handleWebActionEnabledChanged);
        connect(m_tabWidget, &TabWidget::urlChanged, [this](const QUrl &url) {
                m_urlLineEdit->setText(url.toDisplayString());
        });
        connect(m_tabWidget, &TabWidget::favIconChanged, m_favAction, &QAction::setIcon);
        connect(m_tabWidget, &TabWidget::devToolsRequested, this, &BrowserWindow::handleDevToolsRequested);
        connect(m_urlLineEdit, &QLineEdit::returnPressed, [this]() {
                m_tabWidget->setUrl(QUrl::fromUserInput(m_urlLineEdit->text()));
        });

        QAction *focusUrlLineEditAction = new QAction(this);
        addAction(focusUrlLineEditAction);
        focusUrlLineEditAction->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_L));
        connect(focusUrlLineEditAction, &QAction::triggered, this, [this] () {
            m_urlLineEdit->setFocus(Qt::ShortcutFocusReason);
        });
    }

    handleWebViewTitleChanged(QString());
    m_tabWidget->createTab();
}

QSize BrowserWindow::sizeHint() const
{
    const QScreen *screen = QGuiApplication::screens()[0];
    const QRect desktopRect = screen->availableGeometry();
    QSize size = desktopRect.size() * qreal(0.5);
    return size;
}

QMenu *BrowserWindow::createViewMenu(QToolBar *toolbar)
{
    QMenu *viewMenu = new QMenu(tr("&View"));
    m_stopAction = viewMenu->addAction(tr("&Stop"));
    QList<QKeySequence> shortcuts;
    shortcuts.append(QKeySequence(Qt::CTRL | Qt::Key_Period));
    shortcuts.append(Qt::Key_Escape);
    m_stopAction->setShortcuts(shortcuts);
    connect(m_stopAction, &QAction::triggered, [this]() {
        m_tabWidget->triggerWebPageAction(QWebEnginePage::Stop);
    });

    m_reloadAction = viewMenu->addAction(tr("Reload Page"));
    m_reloadAction->setShortcuts(QKeySequence::Refresh);
    connect(m_reloadAction, &QAction::triggered, [this]() {
        m_tabWidget->triggerWebPageAction(QWebEnginePage::Reload);
    });

    QAction *zoomIn = viewMenu->addAction(tr("Zoom &In"));
    zoomIn->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_Plus));
    connect(zoomIn, &QAction::triggered, [this]() {
        if (currentTab())
            currentTab()->setZoomFactor(currentTab()->zoomFactor() + 0.1);
    });

    QAction *zoomOut = viewMenu->addAction(tr("Zoom &Out"));
    zoomOut->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_Minus));
    connect(zoomOut, &QAction::triggered, [this]() {
        if (currentTab())
            currentTab()->setZoomFactor(currentTab()->zoomFactor() - 0.1);
    });

    QAction *resetZoom = viewMenu->addAction(tr("Reset &Zoom"));
    resetZoom->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_0));
    connect(resetZoom, &QAction::triggered, [this]() {
        if (currentTab())
            currentTab()->setZoomFactor(1.0);
    });
    viewMenu->addSeparator();
    QAction *viewToolbarAction = new QAction(tr("Hide Toolbar"),this);
    viewToolbarAction->setShortcut(tr("Ctrl+|"));
    connect(viewToolbarAction, &QAction::triggered, [toolbar,viewToolbarAction]() {
        if (toolbar->isVisible())
        {
            viewToolbarAction->setText(tr("Show Toolbar"));
            toolbar->close();
        } else
        {
            viewToolbarAction->setText(tr("Hide Toolbar"));
            toolbar->show();
        }
    });
    viewMenu->addAction(viewToolbarAction);

    QAction *viewStatusbarAction = new QAction(tr("Hide Status Bar"), this);
    viewStatusbarAction->setShortcut(tr("Ctrl+/"));
    connect(viewStatusbarAction, &QAction::triggered, [this, viewStatusbarAction]()
    {
        if (statusBar()->isVisible())
        {
            viewStatusbarAction->setText(tr("Show Status Bar"));
            statusBar()->close();
        } else
        {
            viewStatusbarAction->setText(tr("Hide Status Bar"));
            statusBar()->show();
        }
    });
    viewMenu->addAction(viewStatusbarAction);
    return viewMenu;
}

QToolBar *BrowserWindow::createToolBar()
{
    QToolBar *navigationBar = new QToolBar(tr("Navigation"));
    navigationBar->setMovable(false);
    navigationBar->toggleViewAction()->setEnabled(false);

    m_historyBackAction = new QAction(this);
    QList<QKeySequence> backShortcuts = QKeySequence::keyBindings(QKeySequence::Back);
    for (auto it = backShortcuts.begin(); it != backShortcuts.end();)
    {
        // Chromium already handles navigate on backspace when appropriate.
        if ((*it)[0] == Qt::Key_Backspace)
            it = backShortcuts.erase(it);
        else
            ++it;
    }
    // For some reason Qt doesn't bind the dedicated Back key to Back.
    backShortcuts.append(QKeySequence(Qt::Key_Back));
    m_historyBackAction->setShortcuts(backShortcuts);
    m_historyBackAction->setIconVisibleInMenu(false);
    m_historyBackAction->setIcon(QIcon(QStringLiteral(":go-previous.png")));
    m_historyBackAction->setToolTip(tr("Go back in history"));
    connect(m_historyBackAction, &QAction::triggered, [this]() {
        m_tabWidget->triggerWebPageAction(QWebEnginePage::Back);
    });
    navigationBar->addAction(m_historyBackAction);

    m_historyForwardAction = new QAction(this);
    QList<QKeySequence> fwdShortcuts = QKeySequence::keyBindings(QKeySequence::Forward);
    for (auto it = fwdShortcuts.begin(); it != fwdShortcuts.end();)
    {
        if (((*it)[0] & Qt::Key_unknown) == Qt::Key_Backspace)
            it = fwdShortcuts.erase(it);
        else
            ++it;
    }
    fwdShortcuts.append(QKeySequence(Qt::Key_Forward));
    m_historyForwardAction->setShortcuts(fwdShortcuts);
    m_historyForwardAction->setIconVisibleInMenu(false);
    m_historyForwardAction->setIcon(QIcon(QStringLiteral(":go-next.png")));
    m_historyForwardAction->setToolTip(tr("Go forward in history"));
    connect(m_historyForwardAction, &QAction::triggered, [this]() {
        m_tabWidget->triggerWebPageAction(QWebEnginePage::Forward);
    });
    navigationBar->addAction(m_historyForwardAction);

    m_stopReloadAction = new QAction(this);
    connect(m_stopReloadAction, &QAction::triggered, [this]() {
        m_tabWidget->triggerWebPageAction(QWebEnginePage::WebAction(m_stopReloadAction->data().toInt()));
    });
    navigationBar->addAction(m_stopReloadAction);

    m_urlLineEdit = new QLineEdit(this);
    m_favAction = new QAction(this);
    m_urlLineEdit->addAction(m_favAction, QLineEdit::LeadingPosition);
    m_urlLineEdit->setClearButtonEnabled(true);
    navigationBar->addWidget(m_urlLineEdit);

    auto downloadsAction = new QAction(this);
    downloadsAction->setIcon(QIcon(QStringLiteral(":go-bottom.png")));
    downloadsAction->setToolTip(tr("Show downloads"));
    navigationBar->addAction(downloadsAction);
    connect(downloadsAction, &QAction::triggered, [this]() {
        m_browser->downloadManagerWidget()->show();
    });

    return navigationBar;
}

void BrowserWindow::handleWebActionEnabledChanged(QWebEnginePage::WebAction action, bool enabled)
{
    switch (action)
    {
        case QWebEnginePage::Back:
            m_historyBackAction->setEnabled(enabled);
            break;
        case QWebEnginePage::Forward:
            m_historyForwardAction->setEnabled(enabled);
            break;
        case QWebEnginePage::Reload:
            m_reloadAction->setEnabled(enabled);
            break;
        case QWebEnginePage::Stop:
            m_stopAction->setEnabled(enabled);
            break;
        default:
            qWarning("Unhandled webActionChanged signal");
    }
}

void BrowserWindow::handleWebViewTitleChanged(const QString &title)
{
    QString suffix = m_profile->isOffTheRecord()
        ? tr("Qt Simple Browser (Incognito)")
        : tr("Qt Simple Browser");

    if (title.isEmpty())
        setWindowTitle(suffix);
    else
        setWindowTitle(title + " - " + suffix);
}

void BrowserWindow::closeEvent(QCloseEvent *event)
{
    if (m_tabWidget->count() > 1) {
        int ret = QMessageBox::warning(this, tr("Confirm close"),
                                       tr("Are you sure you want to close the window ?\n"
                                          "There are %1 tabs open.").arg(m_tabWidget->count()),
                                       QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
        if (ret == QMessageBox::No) {
            event->ignore();
            return;
        }
    }
    event->accept();
    deleteLater();
}

void BrowserWindow::setUrl(QUrl url) const
{
   m_tabWidget->setUrl(url);
}

WebView *BrowserWindow::currentTab() const
{
    return m_tabWidget->currentWebView();
}

void BrowserWindow::handleWebViewLoadProgress(int progress)
{
    static QIcon stopIcon(QStringLiteral(":process-stop.png"));
    static QIcon reloadIcon(QStringLiteral(":view-refresh.png"));

    if (0 < progress && progress < 100) {
        m_stopReloadAction->setData(QWebEnginePage::Stop);
        m_stopReloadAction->setIcon(stopIcon);
        m_stopReloadAction->setToolTip(tr("Stop loading the current page"));
        m_progressBar->setValue(progress);
    } else {
        m_stopReloadAction->setData(QWebEnginePage::Reload);
        m_stopReloadAction->setIcon(reloadIcon);
        m_stopReloadAction->setToolTip(tr("Reload the current page"));
        m_progressBar->setValue(0);
    }
}

void BrowserWindow::handleDevToolsRequested(QWebEnginePage *source)
{
    source->setDevToolsPage(m_browser->createDevToolsWindow()->currentTab()->page());
    source->triggerAction(QWebEnginePage::InspectElement);
}
