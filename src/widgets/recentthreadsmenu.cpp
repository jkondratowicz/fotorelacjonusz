#include <QAction>
#include <QActionGroup>
#include <QSettings>

#include "recentthreadsmenu.h"
#include "settings/settingsdialog.h"

#define RECENT_THREAD_GROUP_CSTR "recent_threads"
#define RECENT_THREAD_TITLES_CSTR "titles"
#define RECENT_THREAD_NUMBERS_CSTR "numbers"

RecentThreadsMenu::RecentThreadsMenu(QSettings &settings, QWidget *parent):
	QMenu(parent),
	settings(settings)
{
	settings.beginGroup(RECENT_THREAD_GROUP_CSTR);
	threadTitles = settings.value(RECENT_THREAD_TITLES_CSTR).toHash();
	imageNumbers = settings.value(RECENT_THREAD_NUMBERS_CSTR).toHash();
	settings.endGroup();
	
	threadTitles.remove("");
	imageNumbers.remove("");
//	qDebug() << threadTitles << imageNumbers;
	
	setTitle(tr("Kontynuuj numerację"));
	firstAction = addSeparator();
	actionGroup = new QActionGroup(this);
	actionGroup->setExclusive(true);
	actions["none"] = new QAction(tr("Żaden"), this);
	connect(actions["none"], SIGNAL(triggered()), this, SLOT(unselect()));
	addAction(actions["none"]);
	
	foreach (QString threadId, threadTitles.keys())
		addUpdateAction(threadId);
}

RecentThreadsMenu::~RecentThreadsMenu()
{
	settings.beginGroup(RECENT_THREAD_GROUP_CSTR);
	settings.setValue(RECENT_THREAD_TITLES_CSTR, threadTitles);
	settings.setValue(RECENT_THREAD_NUMBERS_CSTR, imageNumbers);
	settings.endGroup();
}

QAction *RecentThreadsMenu::addUpdateAction(QString threadId)
{
	QAction *action = actions.contains(threadId) ? actions[threadId] : new QAction(this);
	action->setText(QString("%2. %1").arg(threadTitles[threadId].toString()).arg(imageNumbers[threadId].toInt()));
	action->setCheckable(true);
	action->setActionGroup(actionGroup);
	if (!actions.contains(threadId))
	{
		actions[threadId] = action;
		insertAction(firstAction, action);
		firstAction = action;
		connect(action, SIGNAL(triggered(bool)), this, SLOT(actionTriggered(bool)));
	}
	return action;
}

void RecentThreadsMenu::threadPosted(QString threadId, QString threadTitle, int imageNumber)
{
	if (!SETTINGS->numberImages || imageNumber < 0)
		return;
	threadTitles[threadId] = threadTitle;
	imageNumbers[threadId] = imageNumber + SETTINGS->startingNumber + 1;
	addUpdateAction(threadId);
	
	SETTINGS->setSelectedThread(threadId, imageNumbers[threadId].toInt());
	qDebug() << "RecentThreadsMenu: nowe zdjęcia w wątku zaczną się od" << SETTINGS->startingNumber.v();
}

void RecentThreadsMenu::unselect()
{
	if (actionGroup->checkedAction())
		actionGroup->checkedAction()->setChecked(false);
	SETTINGS->setSelectedThread();
}

void RecentThreadsMenu::actionTriggered(bool checked)
{
	QAction *action = qobject_cast<QAction *>(sender());
	if (action && checked)
	{
		QString threadId = actions.key(action);
		SETTINGS->setSelectedThread(threadId, imageNumbers[threadId].toInt());
	}
}
