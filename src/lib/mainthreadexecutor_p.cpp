/*
 *   Copyright (C) 2014 - 2016 by Ivan Cukic <ivan.cukic(at)kde.org>
 *
 *   This library is free software; you can redistribute it and/or
 *   modify it under the terms of the GNU Lesser General Public
 *   License as published by the Free Software Foundation; either
 *   version 2.1 of the License, or (at your option) version 3, or any
 *   later version accepted by the membership of KDE e.V. (or its
 *   successor approved by the membership of KDE e.V.), which shall
 *   act as a proxy defined in Section 6 of version 3 of the license.
 *
 *   This library is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *   Lesser General Public License for more details.
 *
 *   You should have received a copy of the GNU Lesser General Public
 *   License along with this library.
 *   If not, see <http://www.gnu.org/licenses/>.
 */

#include "mainthreadexecutor_p.h"

#include <mutex>

#include <QCoreApplication>
#include <QDBusConnection>
#include <QMetaObject>
#include <QThread>

namespace KActivities {

namespace detail {

MainThreadExecutor::MainThreadExecutor(std::function<void()> &&f)
    : m_function(std::forward<std::function<void()>>(f))
{
}

void MainThreadExecutor::start()
{
    m_function();
    deleteLater();
}

} // namespace detail

void runInMainThread(std::function<void()> &&f)
{
    static auto mainThread = QCoreApplication::instance()->thread();

    if (QThread::currentThread() == mainThread) {
        f();

    } else {
        auto executor = new detail::MainThreadExecutor(std::forward<std::function<void()>>(f));

        executor->moveToThread(mainThread);

        QMetaObject::invokeMethod(executor, "start", Qt::BlockingQueuedConnection);
    }
}

} // namespace KActivities
