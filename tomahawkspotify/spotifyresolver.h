/*
    Copyright (c) 2011 Leo Franchi <leo@kdab.com>

    Permission is hereby granted, free of charge, to any person
    obtaining a copy of this software and associated documentation
    files (the "Software"), to deal in the Software without
    restriction, including without limitation the rights to use,
    copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the
    Software is furnished to do so, subject to the following
    conditions:

    The above copyright notice and this permission notice shall be
    included in all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
    EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
    OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
    NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
    HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
    WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
    OTHER DEALINGS IN THE SOFTWARE.
*/

#ifndef tomahawkspotify_H
#define tomahawkspotify_H

#include <libspotify/api.h>

#include <QCoreApplication>
#include <QTimer>
#include <QThread>
#include <QVariant>
#include <QMutex>
#include <QWaitCondition>
#include <QQueue>

#define sApp static_cast< SpotifyResolver* >( QCoreApplication::instance() )

struct AudioData;
class AudioHTTPServer;
class ConsoleWatcher;
class QSocketNotifer;

class SpotifyResolver : public QCoreApplication
{
    Q_OBJECT
public:
    SpotifyResolver( int argc, char** argv );
    virtual ~SpotifyResolver();

    void setLoggedIn( bool loggedIn );

    void sendNotifyThreadSignal();

    void search( const QString& qid, const QString& artist, const QString& track );

    // adds a track to the link map, returns a unique ID for identifying it
    QString addToTrackLinkMap( sp_link* link );
    void removeFromTrackLinkMap( const QString& linkStr );
    sp_link* linkFromTrack( const QString& linkStr );
    bool hasLinkFromTrack( const QString& linkStr );

    // audio data stuff
    QMutex& dataMutex();
    QWaitCondition& dataWaitCond();
    // only call if you are holding the dataMutex() from above
    void queueData( const AudioData& data );
    AudioData getData();
    void clearData();
    bool hasData() const;

    // called by callback when track is over
    void startPlaying();
    void endOfTrack();
    bool trackIsOver();

    sp_session* session() const { return m_session; }
    AudioHTTPServer* server() const { return m_server; }

    void sendMessage( const QVariant& v );

    static QString dataDir();
private slots:
    void notifyMainThread();
    void playdarMessage( const QVariant& );

signals:
    void notifyMainThreadSignal();

private:
    void loadSettings();
    void login();

    QThread m_stdinThread;
    ConsoleWatcher* m_stdinWatcher;
    AudioHTTPServer* m_server;

    QMutex m_dataMutex;
    QWaitCondition m_dataWaitCondition;
    QQueue< AudioData > m_audioData;

    sp_session_config m_config;
    sp_session *m_session;
    bool m_loggedIn;
    bool m_trackEnded;

    QHash< QString, sp_link* > m_trackLinkMap;

    QString m_username;
    QString m_pw;
};


#endif // tomahawkspotify_H
