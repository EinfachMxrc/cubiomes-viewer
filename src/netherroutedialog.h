#ifndef NETHERROUTEDIALOG_H
#define NETHERROUTEDIALOG_H

#include <QDialog>

class QLineEdit;
class MapView;

// Converts between Overworld and Nether coordinates (the 1:8 ratio) and
// offers ready /tp commands for both. Built in code, no .ui file.
class NetherRouteDialog : public QDialog
{
    Q_OBJECT
public:
    explicit NetherRouteDialog(MapView *map, int x, int z);

private slots:
    void overworldChanged();
    void netherChanged();
    void copyOverworldTp();
    void copyNetherTp();

private:
    MapView *mapview;
    QLineEdit *owx, *owz, *nex, *nez, *ycoord;
    bool updating = false;
};

#endif // NETHERROUTEDIALOG_H
