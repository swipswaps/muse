#ifndef MUSEWINDOW_H
#define MUSEWINDOW_H

#include <QMainWindow>
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QSortFilterProxyModel>
#include <QTableView>
#include <QTimer>
#include <QMenu>
#include <fmod.hpp>
#include <fmod_errors.h>
#include <fmod_common.h>
#include <algorithm>
#include <dirent.h>

#include "songmodel.h"
#include "artistmodel.h"
#include "albummodel.h"
#include "genremodel.h"
#include "serverdialog.h"
#include "addtoplaylistdialog.h"
#include "removefromplaylistdialog.h"
#include "testdata.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MuseWindow; }
QT_END_NAMESPACE

struct songinfo {
    QString info;
    int song_id;
};

enum PlayState {
    NOT_PLAYING,
    STARTED,
    RESUMED,
    PAUSED
};

enum HistoryState {
    NO_HISTORY,
    HAS_HISTORY
};

enum QueueState {
    NO_QUEUE,
    HAS_QUEUE,
    END_OF_QUEUE
};

enum RepeatState {
    NO_REPEAT,
    REPEAT,
    REPEAT_ONE
};

enum ShuffleState {
    NO_SHUFFLE,
    SHUFFLE
};

enum ConnectionState {
    NOT_CONNECTED,
    CONNECTED
};

class MuseWindow : public QMainWindow
{
    Q_OBJECT

public:
    MuseWindow(QWidget *parent = nullptr);
    ~MuseWindow();

private slots:
    void on_tabWidget_tabBarClicked(int index);

    void on_songView_doubleClicked(const QModelIndex &index);
    void on_artistView_doubleClicked(const QModelIndex &index);
    void on_albumView_doubleClicked(const QModelIndex &index);
    void on_genreView_doubleClicked(const QModelIndex &index);
    void on_playlistView_doubleClicked(const QModelIndex &index);

    void on_playButton_clicked();
    void on_rewindButton_clicked();
    void on_skipButton_clicked();
    void on_repeatButton_clicked();
    void on_shuffleButton_clicked();
    void on_connectButton_clicked();

    void on_timeout();

    void on_songView_customContextMenuRequested(const QPoint &pos);
    void on_playlistView_customContextMenuRequested(const QPoint &pos);

    void on_songView_addSongToPlaylist();
    void on_playlistView_removeSongsFromPlaylist();
    void on_playlistView_deletePlaylist();

private:
    Ui::MuseWindow *ui;
    PlayState play_state = NOT_PLAYING;
    HistoryState history_state = NO_HISTORY;
    QueueState queue_state = NO_QUEUE;
    ConnectionState connection_state = NOT_CONNECTED;
    RepeatState repeat_state = NO_REPEAT;
    ShuffleState shuffle_state = NO_SHUFFLE;

    QString ip_address;
    QString port;

    std::vector<struct songinfo> queue;
    std::vector<struct songinfo> history;
    std::vector<int> downloaded;

    char* songProgressText;
    char* songLengthText;
    char* connectionText;

    QTimer *timer;

    struct playlist* playlists = NULL;

    FMOD_RESULT result;
    FMOD::System *system = NULL;
    FMOD::Sound* song_to_play = NULL;
    FMOD::Channel* song_channel = NULL;

    ArtistModel* artist_model;
    AlbumModel* album_model;
    GenreModel* genre_model;
    SongModel* song_model;
    PlaylistModel* playlist_model;

    void configureTableView(QTableView* view);
    void initializeFMOD();
    void clearModels();
    void stopAndReadyUpFMOD();
    void clearSongs();
    int downloadSong(char* song_path, int song_id);

    void changePlayState(PlayState state);
    void changeHistoryState(HistoryState state);
    void changeQueueState(QueueState state, const QModelIndex* index = NULL, bool shuffle_all = true);
    void changeConnectionState(ConnectionState state);
    void changeRepeatState(RepeatState state);
    void changeShuffleState(ShuffleState state);
};
#endif // MUSEWINDOW_H
