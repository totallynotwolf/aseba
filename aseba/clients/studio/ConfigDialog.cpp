/*
    Aseba - an event-based framework for distributed robot control
    Created by Stéphane Magnenat <stephane at magnenat dot net> (http://stephane.magnenat.net)
    with contributions from the community.
    Copyright (C) 2007--2018 the authors, see authors.txt for details.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published
    by the Free Software Foundation, version 3 of the License.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#include "ConfigDialog.h"

#include <QSettings>
#include <QtWidgets>

#define BOOL_TO_CHECKED(value) (value == true ? Qt::Checked : Qt::Unchecked)
#define CHECKED_TO_BOOL(value) (value == Qt::Checked ? true : false)

#define CONFIG_PROPERTY_CHECKBOX_HANDLER(name, page, keyword) \
    bool ConfigDialog::get##name() {                          \
        if(me)                                                \
            return me->page->checkboxCache[#keyword].value;   \
        else                                                  \
            return false;                                     \
    }                                                         \
                                                              \
    void ConfigDialog::set##name(bool value) {                \
        if(me && !me->isVisible())                            \
            me->page->checkboxCache[#keyword].value = value;  \
    }


#define TITLE_SPACING 10
#define HORIZONTAL_STRUT 700

namespace Aseba {
CONFIG_PROPERTY_CHECKBOX_HANDLER(ShowLineNumbers, generalpage, showlinenumbers)
CONFIG_PROPERTY_CHECKBOX_HANDLER(ShowHidden, generalpage, showhidden)
CONFIG_PROPERTY_CHECKBOX_HANDLER(ShowKeywordToolbar, generalpage, keywordToolbar)
CONFIG_PROPERTY_CHECKBOX_HANDLER(ShowMemoryUsage, generalpage, memoryusage)
CONFIG_PROPERTY_CHECKBOX_HANDLER(AutoCompletion, editorpage, autoKeyword)

/*** ConfigPage ***/
ConfigPage::ConfigPage(QString title, QWidget* parent) : QWidget(parent) {
    QLabel* myTitle = new QLabel(title);
    // bold & align center
    myTitle->setAlignment(Qt::AlignCenter);
    QFont current = myTitle->font();
    current.setBold(true);
    myTitle->setFont(current);

    mainLayout = new QVBoxLayout();
    mainLayout->addWidget(myTitle);
    mainLayout->addSpacing(TITLE_SPACING);
    mainLayout->addStrut(HORIZONTAL_STRUT);
    setLayout(mainLayout);
}

QCheckBox* ConfigPage::newCheckbox(QString label, QString ID, bool checked) {
    auto* widget = new QCheckBox(label);
    widget->setCheckState(BOOL_TO_CHECKED(checked));
    WidgetCache<bool> cache(widget, checked);
    checkboxCache.insert(std::pair<QString, WidgetCache<bool> >(ID, cache));
    connect(widget, SIGNAL(released()), ConfigDialog::getInstance(), SLOT(flushCache()));
    connect(widget, &QAbstractButton::released, ConfigDialog::getInstance(), &ConfigDialog::settingsChanged);
    return widget;
}

void ConfigPage::readSettings() {
    // update cache and widgets from disk
    QSettings settings;
    // iterate on checkboxes
    for(auto it = checkboxCache.begin(); it != checkboxCache.end(); it++) {
        QString name = it->first;
        auto* checkbox = dynamic_cast<QCheckBox*>((it->second).widget);
        Q_ASSERT(checkbox);
        if(settings.contains(name)) {
            bool value = settings.value(name, false).toBool();
            checkbox->setCheckState(BOOL_TO_CHECKED(value));  // update widget
            (it->second).value = value;                       // update cache
        }
    }
}

void ConfigPage::writeSettings() {
    // write the cache on disk
    QSettings settings;
    // iterate on checkboxes
    for(auto it = checkboxCache.begin(); it != checkboxCache.end(); it++) {
        QString name = it->first;
        auto* checkbox = dynamic_cast<QCheckBox*>((it->second).widget);
        Q_ASSERT(checkbox);
        Q_UNUSED(checkbox);
        settings.setValue(name, (it->second).value);
    }
}

void ConfigPage::saveState() {
    // create a temporary cache
    checkboxCacheSave = checkboxCache;
}

void ConfigPage::flushCache() {
    // sync the cache with the widgets' state
    // iterate on checkboxes
    for(auto it = checkboxCache.begin(); it != checkboxCache.end(); it++) {
        auto* checkbox = dynamic_cast<QCheckBox*>((it->second).widget);
        Q_ASSERT(checkbox);
        (it->second).value = CHECKED_TO_BOOL(checkbox->checkState());
    }
}

void ConfigPage::discardChanges() {
    // reload values from the temporary cache
    checkboxCache = checkboxCacheSave;
    // update widgets accordingly
    reloadFromCache();
}

void ConfigPage::reloadFromCache() {
    // update widgets based on the cache
    for(auto it = checkboxCache.begin(); it != checkboxCache.end(); it++) {
        auto* checkbox = dynamic_cast<QCheckBox*>((it->second).widget);
        Q_ASSERT(checkbox);
        checkbox->setCheckState(BOOL_TO_CHECKED((it->second).value));
    }
}


/*** GeneralPage ***/
GeneralPage::GeneralPage(QWidget* parent) : ConfigPage(tr("General Setup"), parent) {
    QGroupBox* gb1 = new QGroupBox(tr("Layout"));
    auto* gb1layout = new QVBoxLayout();
    gb1->setLayout(gb1layout);
    mainLayout->addWidget(gb1);
    // Show the keyword toolbar
    gb1layout->addWidget(newCheckbox(tr("Show keyword toolbar"), QStringLiteral("keywordToolbar"), true));
    // Show the memory usage gauge
    gb1layout->addWidget(newCheckbox(tr("Show memory usage"), QStringLiteral("memoryusage"), false));
    // Show hidden variables & functions
    gb1layout->addWidget(newCheckbox(tr("Show hidden variables"), QStringLiteral("showhidden"), false));
    // Show line numbers
    gb1layout->addWidget(newCheckbox(tr("Show line numbers"), QStringLiteral("showlinenumbers"), true));

    mainLayout->addStretch();
}

void GeneralPage::readSettings() {
    QSettings settings;
    settings.beginGroup(QStringLiteral("general-config"));
    ConfigPage::readSettings();
    settings.endGroup();
}

void GeneralPage::writeSettings() {
    QSettings settings;
    settings.beginGroup(QStringLiteral("general-config"));
    ConfigPage::writeSettings();
    settings.endGroup();
}


/*** EditorPage ***/
EditorPage::EditorPage(QWidget* parent) : ConfigPage(tr("Editor Setup"), parent) {
    //
    QGroupBox* gb1 = new QGroupBox(tr("Autocompletion"));
    auto* gb1layout = new QVBoxLayout();
    gb1->setLayout(gb1layout);
    mainLayout->addWidget(gb1);
    //
    gb1layout->addWidget(newCheckbox(tr("Keywords"), QStringLiteral("autoKeyword"), true));

    mainLayout->addStretch();
}

void EditorPage::readSettings() {
    QSettings settings;
    settings.beginGroup(QStringLiteral("editor-config"));
    ConfigPage::readSettings();
    settings.endGroup();
}

void EditorPage::writeSettings() {
    QSettings settings;
    settings.beginGroup(QStringLiteral("editor-config"));
    ConfigPage::writeSettings();
    settings.endGroup();
}


/*** ConfigDialog ***/
void ConfigDialog::init(QWidget* parent) {
    if(!me) {
        me = new ConfigDialog(parent);
        me->setupWidgets();
    }
}

void ConfigDialog::bye() {
    if(me) {
        delete me;
        me = nullptr;
    }
}

void ConfigDialog::showConfig() {
    me->setModal(true);
    me->reloadFromCache();
    me->saveState();
    me->show();
}

ConfigDialog* ConfigDialog::me = nullptr;

ConfigDialog::ConfigDialog(QWidget* parent) : QDialog(parent) {}

ConfigDialog::~ConfigDialog() {
    writeSettings();
}

void ConfigDialog::setupWidgets() {
    // list of topics
    topicList = new QListWidget();
    topicList->setViewMode(QListView::IconMode);
    topicList->setIconSize(QSize(64, 64));
    topicList->setMovement(QListView::Static);
    topicList->setMaximumWidth(128);
    topicList->setGridSize(QSize(100, 100));
    topicList->setMinimumHeight(300);
    // add topics
    QListWidgetItem* general = new QListWidgetItem(QIcon(":/images/exec.png"), tr("General"));
    general->setTextAlignment(Qt::AlignHCenter);
    topicList->addItem(general);
    QListWidgetItem* editor = new QListWidgetItem(QIcon(":/images/kedit.png"), tr("Editor"));
    editor->setTextAlignment(Qt::AlignHCenter);
    topicList->addItem(editor);

    // create pages
    configStack = new QStackedWidget();
    generalpage = new GeneralPage();
    configStack->addWidget(generalpage);
    editorpage = new EditorPage();
    configStack->addWidget(editorpage);

    connect(topicList, &QListWidget::currentRowChanged, configStack, &QStackedWidget::setCurrentIndex);
    topicList->setCurrentRow(0);

    // buttons
    okButton = new QPushButton(tr("Ok"));
    cancelButton = new QPushButton(tr("Cancel"));
    connect(okButton, SIGNAL(clicked()), SLOT(accept()));
    connect(cancelButton, SIGNAL(clicked()), SLOT(reject()));

    // main layout
    auto* contentLayout = new QHBoxLayout();
    contentLayout->addWidget(topicList);
    contentLayout->addWidget(configStack);

    auto* buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();
    buttonLayout->addWidget(okButton);
    buttonLayout->addWidget(cancelButton);

    auto* mainLayout = new QVBoxLayout();
    mainLayout->addLayout(contentLayout);
    mainLayout->addLayout(buttonLayout);

    setLayout(mainLayout);
    setWindowTitle(tr("Aseba Studio - Settings"));

    readSettings();
}

void ConfigDialog::saveState() {
    // save the state of pages, prior to editing
    for(int i = 0; i < configStack->count(); i++) {
        auto* config = dynamic_cast<ConfigPage*>(configStack->widget(i));
        if(config)
            config->saveState();
    }
}

void ConfigDialog::reloadFromCache() {
    // reload values from the cache
    for(int i = 0; i < configStack->count(); i++) {
        auto* config = dynamic_cast<ConfigPage*>(configStack->widget(i));
        if(config)
            config->reloadFromCache();
    }
}

void ConfigDialog::accept() {
    // update the cache with new values
    flushCache();
    QDialog::accept();
}

void ConfigDialog::reject() {
    // discard the cache and reload widgets with old values
    for(int i = 0; i < configStack->count(); i++) {
        auto* config = dynamic_cast<ConfigPage*>(configStack->widget(i));
        if(config)
            config->discardChanges();
    }
    // GUI should adopt changes
    emit settingsChanged();
    QDialog::reject();
}

void ConfigDialog::flushCache() {
    // update the cache with new values
    for(int i = 0; i < configStack->count(); i++) {
        auto* config = dynamic_cast<ConfigPage*>(configStack->widget(i));
        if(config)
            config->flushCache();
    }
}

void ConfigDialog::readSettings() {
    for(int i = 0; i < configStack->count(); i++) {
        auto* config = dynamic_cast<ConfigPage*>(configStack->widget(i));
        if(config)
            config->readSettings();
    }
}

void ConfigDialog::writeSettings() {
    for(int i = 0; i < configStack->count(); i++) {
        auto* config = dynamic_cast<ConfigPage*>(configStack->widget(i));
        if(config)
            config->writeSettings();
    }
}

}  // namespace Aseba
