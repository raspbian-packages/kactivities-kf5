/*
 *   Copyright (C) 2012, 2013, 2014 Ivan Cukic <ivan.cukic(at)kde.org>
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License version 2,
 *   or (at your option) any later version, as published by the Free
 *   Software Foundation
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details
 *
 *   You should have received a copy of the GNU General Public
 *   License along with this program; if not, write to the
 *   Free Software Foundation, Inc.,
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#ifndef KACTIVITIES_IMPORTS_ACTIVITY_MODEL_H
#define KACTIVITIES_IMPORTS_ACTIVITY_MODEL_H

// Qt
#include <QObject>
#include <QAbstractListModel>
#include <QJSValue>

// STL and Boost
#include <boost/container/flat_set.hpp>
#include <memory>

// Local
#include <lib/controller.h>
#include <lib/consumer.h>
#include <lib/info.h>

class QModelIndex;
class QDBusPendingCallWatcher;

namespace KActivities {
namespace Imports {

/**
 * ActivityModel
 */

class ActivityModel : public QAbstractListModel {
    Q_OBJECT

    Q_ENUMS(State)
    Q_PROPERTY(QString shownStates READ shownStates WRITE setShownStates NOTIFY shownStatesChanged)

public:
    ActivityModel(QObject *parent = 0);
    virtual ~ActivityModel();

    int rowCount(const QModelIndex &parent = QModelIndex()) const
        Q_DECL_OVERRIDE;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const
        Q_DECL_OVERRIDE;

    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;

    QHash<int, QByteArray> roleNames() const Q_DECL_OVERRIDE;

    enum Roles {
        ActivityId          = Qt::UserRole,
        ActivityDescription = Qt::UserRole + 1,
        ActivityIcon        = Qt::UserRole + 2,
        ActivityState       = Qt::UserRole + 3,
        ActivityBackground  = Qt::UserRole + 4,
        ActivityCurrent     = Qt::UserRole + 5,
    };

    enum State {
        All = 0,
        Invalid = 0,
        Running = 2,
        Starting = 3,
        Stopped = 4,
        Stopping = 5
    };

public Q_SLOTS:
    // Activity control methods
    void setActivityName(const QString &id, const QString &name,
                         const QJSValue &callback);
    void setActivityDescription(const QString &id, const QString &description,
                                const QJSValue &callback);
    void setActivityIcon(const QString &id, const QString &icon,
                         const QJSValue &callback);

    void setCurrentActivity(const QString &id, const QJSValue &callback);

    void addActivity(const QString &name, const QJSValue &callback);
    void removeActivity(const QString &id, const QJSValue &callback);

    void stopActivity(const QString &id, const QJSValue &callback);
    void startActivity(const QString &id, const QJSValue &callback);

    // Model property getters and setters
    void setShownStates(const QString &states);
    QString shownStates() const;

Q_SIGNALS:
    void shownStatesChanged(const QString &state);

private Q_SLOTS:
    void onActivityNameChanged(const QString &name);
    void onActivityDescriptionChanged(const QString &description);
    void onActivityIconChanged(const QString &icon);
    void onActivityStateChanged(KActivities::Info::State state);

    void replaceActivities(const QStringList &activities);
    void onActivityAdded(const QString &id, bool notifyClients = true);
    void onActivityRemoved(const QString &id);
    void onCurrentActivityChanged(const QString &id);

    void setServiceStatus(KActivities::Consumer::ServiceStatus status);

private:
    KActivities::Controller m_service;
    boost::container::flat_set<State> m_shownStates;
    QString m_shownStatesString;

    typedef std::shared_ptr<Info> InfoPtr;

    struct InfoPtrComparator {
        bool operator() (const InfoPtr& left, const InfoPtr& right) const
        {
            const QString &leftName = left->name().toLower();
            const QString &rightName = right->name().toLower();

            return
                (leftName < rightName) ||
                (leftName == rightName && left->id() < right->id());
        }
    };

    boost::container::flat_set<InfoPtr, InfoPtrComparator> m_knownActivities;
    boost::container::flat_set<InfoPtr, InfoPtrComparator> m_shownActivities;

    InfoPtr registerActivity(const QString &id);
    void unregisterActivity(const QString &id);
    void showActivity(InfoPtr activityInfo, bool notifyClients);
    void hideActivity(const QString &id);
    void backgroundsUpdated(const QStringList &activities);

    InfoPtr findActivity(QObject *ptr) const;

    class Private;
    friend class Private;
};

} // namespace Imports
} // namespace KActivities

#endif // KACTIVITIES_IMPORTS_ACTIVITY_MODEL_H

