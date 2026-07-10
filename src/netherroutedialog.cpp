#include "netherroutedialog.h"

#include "mapview.h"

#include <QClipboard>
#include <QFormLayout>
#include <QGuiApplication>
#include <QHBoxLayout>
#include <QIntValidator>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>

NetherRouteDialog::NetherRouteDialog(MapView *map, int x, int z)
    : QDialog(map)
    , mapview(map)
{
    setWindowTitle(tr("Nether route planner"));

    owx = new QLineEdit(QString::number(x));
    owz = new QLineEdit(QString::number(z));
    nex = new QLineEdit(QString::number(x / 8));
    nez = new QLineEdit(QString::number(z / 8));
    ycoord = new QLineEdit("64");
    for (QLineEdit *e : {owx, owz, nex, nez})
        e->setValidator(new QIntValidator(-30000000, 30000000, e));
    ycoord->setValidator(new QIntValidator(-64, 320, ycoord));

    auto *form = new QFormLayout;
    auto *owrow = new QHBoxLayout;
    owrow->addWidget(new QLabel("X")); owrow->addWidget(owx);
    owrow->addWidget(new QLabel("Z")); owrow->addWidget(owz);
    form->addRow(tr("Overworld:"), owrow);
    auto *nerow = new QHBoxLayout;
    nerow->addWidget(new QLabel("X")); nerow->addWidget(nex);
    nerow->addWidget(new QLabel("Z")); nerow->addWidget(nez);
    form->addRow(tr("Nether:"), nerow);
    form->addRow(tr("Y (for /tp):"), ycoord);

    auto *btnOw = new QPushButton(tr("Copy Overworld /tp"));
    auto *btnNe = new QPushButton(tr("Copy Nether /tp"));
    auto *btnrow = new QHBoxLayout;
    btnrow->addWidget(btnOw);
    btnrow->addWidget(btnNe);

    auto *root = new QVBoxLayout(this);
    root->addLayout(form);
    root->addLayout(btnrow);

    connect(owx, &QLineEdit::textEdited, this, &NetherRouteDialog::overworldChanged);
    connect(owz, &QLineEdit::textEdited, this, &NetherRouteDialog::overworldChanged);
    connect(nex, &QLineEdit::textEdited, this, &NetherRouteDialog::netherChanged);
    connect(nez, &QLineEdit::textEdited, this, &NetherRouteDialog::netherChanged);
    connect(btnOw, &QPushButton::clicked, this, &NetherRouteDialog::copyOverworldTp);
    connect(btnNe, &QPushButton::clicked, this, &NetherRouteDialog::copyNetherTp);
}

void NetherRouteDialog::overworldChanged()
{
    if (updating) return;
    updating = true;
    nex->setText(QString::number(owx->text().toInt() / 8));
    nez->setText(QString::number(owz->text().toInt() / 8));
    updating = false;
}

void NetherRouteDialog::netherChanged()
{
    if (updating) return;
    updating = true;
    owx->setText(QString::number(nex->text().toInt() * 8));
    owz->setText(QString::number(nez->text().toInt() * 8));
    updating = false;
}

void NetherRouteDialog::copyOverworldTp()
{
    QString tp = QString("/execute in minecraft:overworld run tp @s %1 %2 %3")
                     .arg(owx->text().toInt()).arg(ycoord->text().toInt()).arg(owz->text().toInt());
    QGuiApplication::clipboard()->setText(tp);
}

void NetherRouteDialog::copyNetherTp()
{
    QString tp = QString("/execute in minecraft:the_nether run tp @s %1 %2 %3")
                     .arg(nex->text().toInt()).arg(ycoord->text().toInt()).arg(nez->text().toInt());
    QGuiApplication::clipboard()->setText(tp);
}
