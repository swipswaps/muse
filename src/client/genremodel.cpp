#include "genremodel.h"

GenreModel::GenreModel(QObject *parent) : QAbstractTableModel(parent)
{
}

// Create a method to populate the model with data:
void GenreModel::populateData(struct genreinfolst* genres)
{
    // clear previous data
    this->genres.clear();

    // populate new data
    struct genreinfolst* cursor = genres;
    while(cursor != NULL) {
        this->genres.append(cursor->genre);

        cursor = cursor->next;
    }

    free_genreinfolst(genres);

    return;
}

int GenreModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return genres.length();
}

int GenreModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 1;
}

QVariant GenreModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || role != Qt::DisplayRole) {
        return QVariant();
    }
    if (index.column() == 0) {
//        return titles[index.row()];
    } else if (index.column() == 1) {
//        return artists[index.row()];
    } else if (index.column() == 2) {
//        return albums[index.row()];
    } else if (index.column() == 3) {
//        return genres[index.row()];
    }
    return QVariant();
}

QVariant GenreModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole && orientation == Qt::Horizontal) {
        if (section == 0) {
            return QString("Genre");
        }
    }
    return QVariant();
}
