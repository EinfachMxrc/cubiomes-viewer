#include "gotodialog.h"
#include "ui_gotodialog.h"

#include "mapview.h"
#include "util.h"
#include "world.h"

#include <QApplication>
#include <QClipboard>
#include <QDoubleValidator>
#include <QKeyEvent>
#include <QMessageBox>

static bool g_animate;

GotoDialog::GotoDialog(MapView *map, qreal x, qreal z, qreal scale)
    : QDialog(map)
    , ui(new Ui::GotoDialog)
    , mapview(map)
{
    ui->setupUi(this);

    scalemin = 1.0 / 4096;
    scalemax = 65536;
    ui->lineX->setValidator(new QDoubleValidator(-3e7, 3e7, 1, ui->lineX));
    ui->lineZ->setValidator(new QDoubleValidator(-3e7, 3e7, 1, ui->lineZ));
    ui->lineScale->setValidator(new QDoubleValidator(scalemin, scalemax, 16, ui->lineScale));

    ui->lineX->setText(QString::asprintf("%.1f", x));
    ui->lineZ->setText(QString::asprintf("%.1f", z));
    ui->lineScale->setText(QString::asprintf("%.4f", scale));

    ui->checkAnimate->setChecked(g_animate);

    // populate the biome selector with biomes valid for the current version
    QWorld *w = mapview->world;
    if (w)
    {
        for (int id = 0; id < 256; id++)
        {
            if (!biomeExists(w->wi.mc, id))
                continue;
            QString name = getBiomeDisplay(w->wi.mc, id);
            if (name.isEmpty())
                continue;
            ui->comboBiome->addItem(name, id);
        }
        ui->comboBiome->model()->sort(0);
    }
    else
    {
        ui->comboBiome->setEnabled(false);
        ui->buttonLocate->setEnabled(false);
    }
}

void GotoDialog::on_buttonLocate_clicked()
{
    QWorld *w = mapview->world;
    if (!w)
        return;

    int target = ui->comboBiome->currentData().toInt();
    int x0 = (int) ui->lineX->text().toDouble();
    int z0 = (int) ui->lineZ->text().toDouble();
    int y = w->wi.y;

    // expanding-square scan outward from (x0,z0); returns the nearest cell
    // whose biome matches. Works for all biome id ranges (unlike the library
    // locateBiome, which only covers ids 0-63 and 192-255).
    const int step = 32;
    const int maxradius = 6144;
    bool found = false;
    int fx = x0, fz = z0;

    QApplication::setOverrideCursor(Qt::WaitCursor);
    if (getBiomeAt(&w->g, 1, x0, y, z0) == target)
    {
        found = true;
    }
    for (int r = step; r <= maxradius && !found; r += step)
    {
        for (int d = -r; d <= r && !found; d += step)
        {
            int cells[4][2] = {
                { x0 + d, z0 - r }, { x0 + d, z0 + r },
                { x0 - r, z0 + d }, { x0 + r, z0 + d },
            };
            for (int c = 0; c < 4; c++)
            {
                if (getBiomeAt(&w->g, 1, cells[c][0], y, cells[c][1]) == target)
                {
                    fx = cells[c][0];
                    fz = cells[c][1];
                    found = true;
                    break;
                }
            }
        }
    }
    QApplication::restoreOverrideCursor();

    if (found)
    {
        ui->lineX->setText(QString::number(fx));
        ui->lineZ->setText(QString::number(fz));
    }
    else
    {
        QMessageBox::information(this, tr("Biome not found"),
            tr("No %1 found within %2 blocks at Y = %3.")
                .arg(ui->comboBiome->currentText()).arg(maxradius).arg(y));
    }
}

GotoDialog::~GotoDialog()
{
    delete ui;
}

void GotoDialog::on_buttonBox_clicked(QAbstractButton *button)
{
    QDialogButtonBox::StandardButton b = ui->buttonBox->standardButton(button);

    if (b == QDialogButtonBox::Ok || b == QDialogButtonBox::Apply)
    {
        qreal x = ui->lineX->text().toDouble();
        qreal z = ui->lineZ->text().toDouble();
        qreal scale = ui->lineScale->text().toDouble();
        if (scale < scalemin) scale = scalemin;
        if (scale > scalemax) scale = scalemax;
        ui->lineScale->setText(QString::asprintf("%.4f", scale));
        g_animate = ui->checkAnimate->isChecked();
        if (g_animate)
            mapview->animateView(x, z, scale);
        else
            mapview->setView(x, z, scale);
    }
    else if (b == QDialogButtonBox::Reset)
    {
        ui->lineX->setText("0");
        ui->lineZ->setText("0");
        ui->lineScale->setText("16");
    }
}

void GotoDialog::on_lineScale_textChanged(const QString &text)
{
    qreal value = text.toDouble();
    ui->lineScale->setStyleSheet(value > 4096 ? "color: red" : "");
}

void GotoDialog::keyPressEvent(QKeyEvent *event)
{
    static QRegularExpression coord_delim = QRegularExpression("[, ]+");

    if (event->matches(QKeySequence::Paste))
    {
        QClipboard *clipboard = QGuiApplication::clipboard();
        QString s = clipboard->text().trimmed();
        QStringList xz = s.split(coord_delim);
        if (xz.count() == 2)
        {
            ui->lineX->setText(xz[0]);
            ui->lineZ->setText(xz[1]);
            return;
        }
        else if (xz.count() == 3)
        {
            ui->lineX->setText(xz[0]);
            ui->lineZ->setText(xz[2]);
            return;
        }
    }
    QWidget::keyReleaseEvent(event);
}


