// 
// Shijima-Qt - Cross-platform shimeji simulation app for desktop
// Copyright (C) 2025 pixelomer
// 
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.
// 

#include "../Platform.hpp"
#include <QApplication>
#include <QDebug>
#include <QHash>
#include <QPointer>
#include <QSet>
#include <QWidget>
#include <array>
#include <atomic>
#include <cstdint>
#include <cwchar>
#include <future>
#include <thread>
#include <windows.h>

namespace Platform {

namespace {

class GlobalMouseObserver : public QObject {
public:
    explicit GlobalMouseObserver(QObject *parent): QObject(parent) {
        // A separate message loop keeps input responsive while the GUI is busy.
        std::promise<DWORD> ready;
        auto threadId = ready.get_future();
        m_thread = std::thread([this, ready = std::move(ready)]() mutable {
            MSG message;
            PeekMessageW(&message, nullptr, 0, 0, PM_NOREMOVE);
            s_observer = this;
            HHOOK hook = SetWindowsHookExW(WH_MOUSE_LL, mouseHook, GetModuleHandleW(nullptr), 0);
            const DWORD error = hook ? ERROR_SUCCESS : GetLastError();
            ready.set_value(GetCurrentThreadId());
            if (!hook) {
                qWarning() << "Unable to observe global mouse clicks:" << error;
                return;
            }
            while (GetMessageW(&message, nullptr, 0, 0) > 0) {
                TranslateMessage(&message);
                DispatchMessageW(&message);
            }
            UnhookWindowsHookEx(hook);
            s_observer = nullptr;
        });
        m_threadId = threadId.get();
        connect(qApp, &QCoreApplication::aboutToQuit, this, &GlobalMouseObserver::stop);
    }

    ~GlobalMouseObserver() override {
        stop();
    }

    void addWidget(QWidget *widget) {
        if (m_widgets.contains(widget)) {
            return;
        }
        m_widgets.insert(widget);
        connect(widget, &QObject::destroyed, this, [this, widget] {
            m_widgets.remove(widget);
        });
    }

private:
    struct ButtonPress {
        uint64_t sequence = 0;
        bool taskbar = false;
    };

    static bool isTaskbar(POINT point) {
        HWND window = GetAncestor(WindowFromPoint(point), GA_ROOT);
        wchar_t name[256] = {};
        GetClassNameW(window, name, 256);
        return std::wcscmp(name, L"Shell_TrayWnd") == 0
            || std::wcscmp(name, L"Shell_SecondaryTrayWnd") == 0;
    }

    static LRESULT CALLBACK mouseHook(int code, WPARAM message, LPARAM data) {
        if (code == HC_ACTION && s_observer != nullptr) {
            auto &event = *reinterpret_cast<MSLLHOOKSTRUCT *>(data);
            int button = -1;
            bool down = false;
            switch (message) {
                case WM_LBUTTONDOWN: down = true; [[fallthrough]];
                case WM_LBUTTONUP: button = 0; break;
                case WM_RBUTTONDOWN: down = true; [[fallthrough]];
                case WM_RBUTTONUP: button = 1; break;
                case WM_MBUTTONDOWN: down = true; [[fallthrough]];
                case WM_MBUTTONUP: button = 2; break;
                case WM_XBUTTONDOWN: down = true; [[fallthrough]];
                case WM_XBUTTONUP: button = HIWORD(event.mouseData) == XBUTTON1 ? 3 : 4; break;
                default: break;
            }
            if (button >= 0) {
                s_observer->mouseButton(button, down, event.pt);
            }
        }
        // Observe only: always deliver input to the target and other hooks.
        return CallNextHookEx(nullptr, code, message, data);
    }

    void mouseButton(int button, bool down, POINT point) {
        auto &press = m_buttons[button];
        if (down) {
            press = { ++m_sequence, isTaskbar(point) };
        }
        else {
            const auto sequence = press.sequence;
            const bool raise = sequence != 0 && !press.taskbar && !isTaskbar(point);
            press.sequence = 0;
            if (raise) {
                // Wait for release so the clicked application has activated.
                // A later press (especially on the taskbar) cancels stale work.
                QMetaObject::invokeMethod(this, [this, sequence] {
                    if (sequence == m_sequence.load()) {
                        raiseWidgets();
                    }
                }, Qt::QueuedConnection);
            }
        }
    }

    void raiseWidgets() {
        struct Windows {
            QHash<HWND, QWidget *> widgets;
            QList<QWidget *> ordered;
        } windows;
        for (auto widget : m_widgets) {
            if (widget->isVisible() && widget->isWindow()) {
                windows.widgets.insert(reinterpret_cast<HWND>(widget->winId()), widget);
            }
        }
        if (windows.widgets.isEmpty()) {
            return;
        }
        EnumWindows([](HWND window, LPARAM data) -> BOOL {
            auto &windows = *reinterpret_cast<Windows *>(data);
            auto widget = windows.widgets.value(window, nullptr);
            if (widget != nullptr) {
                windows.ordered.append(widget);
            }
            return TRUE;
        }, reinterpret_cast<LPARAM>(&windows));
        // Raise back to front, preserving the order of overlapping mascots.
        for (auto iter = windows.ordered.crbegin(); iter != windows.ordered.crend(); ++iter) {
            (*iter)->raise();
        }
    }

    void stop() {
        if (m_thread.joinable()) {
            PostThreadMessageW(m_threadId, WM_QUIT, 0, 0);
            m_thread.join();
        }
    }

    static thread_local GlobalMouseObserver *s_observer;
    std::thread m_thread;
    DWORD m_threadId = 0;
    std::atomic<uint64_t> m_sequence { 0 };
    std::array<ButtonPress, 5> m_buttons;
    QSet<QWidget *> m_widgets;
};

thread_local GlobalMouseObserver *GlobalMouseObserver::s_observer = nullptr;

}

void initialize(int argc, char **argv) {
    freopen("shijima_stdout.txt", "a", stdout);
    freopen("shijima_stderr.txt", "a", stderr);
}

void showOnAllDesktops(QWidget *widget) {
    HWND window = (HWND)widget->winId();
    LONG_PTR exstyle = GetWindowLongPtr(window, GWL_EXSTYLE);
    if (exstyle != 0) {
        exstyle |= WS_EX_TOOLWINDOW;
        SetWindowLongPtr(window, GWL_EXSTYLE, exstyle);
    }
}

void raiseOnGlobalClick(QWidget *widget) {
    static QPointer<GlobalMouseObserver> observer;
    if (!observer) {
        observer = new GlobalMouseObserver(qApp);
    }
    observer->addWidget(widget);
}

bool useWindowMasks() {
    return false;
}

}
