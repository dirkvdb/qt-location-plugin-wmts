#pragma once

#include <QDateTime>
#include <QTimer>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QPointer>
#include <QtCore/QUrl>
#include <QtCore/QVector>
#include <QtLocation/private/qgeocameracapabilities_p.h>
#include <QtLocation/private/qgeomaptype_p.h>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include <algorithm>

class TileProvider : public QObject
{
    Q_OBJECT
public:
    enum Status
    {
        Idle,
        Resolving,
        Valid,
        Invalid
    };

    TileProvider();
    // "Online" constructor. Needs resolution to fetch the parameters
    TileProvider(const QUrl& urlRedirector, bool highDpi = false);
    // Offline constructor. Doesn't need URLRedirector and networkmanager
    TileProvider(const QString& urlTemplate,
        const QString& format,
        const QString& copyRightMap,
        const QString& copyRightData,
        bool highDpi         = false,
        int minimumZoomLevel = 0,
        int maximumZoomLevel = 19);

    void setNetworkManager(QNetworkAccessManager* nm);

    void resolveProvider();
    void handleError(QNetworkReply::NetworkError error);
    void setupProvider();

    inline bool isValid() const;
    inline bool isInvalid() const;
    inline bool isResolved() const;
    inline Status status() const;

    inline QString mapCopyRight() const;
    inline QString dataCopyRight() const;
    inline QString styleCopyRight() const;
    inline QString format() const;
    inline int minimumZoomLevel() const;
    inline int maximumZoomLevel() const;
    inline const QDateTime& timestamp() const;
    inline bool isHighDpi() const;
    QUrl tileAddress(int x, int y, int z) const;

    // Optional properties, not needed to construct a provider
    void setStyleCopyRight(const QString& copyright);
    void setTimestamp(const QDateTime& timestamp);

    Status m_status;
    QUrl m_urlRedirector; // The URL from where to fetch the URL template in case of a provider to resolve.
    QNetworkAccessManager* m_nm;
    QString m_urlTemplate;
    QString m_format;
    QString m_copyRightMap;
    QString m_copyRightData;
    QString m_copyRightStyle;
    QString m_urlPrefix;
    QString m_urlSuffix;
    int m_minimumZoomLevel;
    int m_maximumZoomLevel;
    QDateTime m_timestamp;
    bool m_highDpi;

    int paramsLUT[3];     //Lookup table to handle possibly shuffled x,y,z
    QString paramsSep[2]; // what goes in between %x, %y and %z

Q_SIGNALS:
    void resolutionFinished(TileProvider* provider);
    void resolutionError(TileProvider* provider);

public Q_SLOTS:
    void onNetworkReplyFinished();
    void onNetworkReplyError(QNetworkReply::NetworkError error);

    friend class QGeoTileProviderWmts;
};

class QGeoTileProviderWmts : public QObject
{
    Q_OBJECT

    friend class QGeoTileFetcherOsm;
    friend class QGeoMapReplyOsm;
    friend class QGeoTiledMappingManagerEngineOsm;

public:
    enum Status
    {
        Idle,
        Resolving,
        Resolved
    };

    QGeoTileProviderWmts(QNetworkAccessManager* nm,
        const QGeoMapType& mapType,
        const QVector<TileProvider*>& providers,
        const QGeoCameraCapabilities& cameraCapabilities);

    QUrl tileAddress(int x, int y, int z) const;
    QString mapCopyRight() const;
    QString dataCopyRight() const;
    QString styleCopyRight() const;
    QString format() const;
    int minimumZoomLevel() const;
    int maximumZoomLevel() const;
    bool isHighDpi() const;
    const QGeoMapType& mapType() const;
    bool isValid() const;
    bool isResolved() const;
    const QDateTime timestamp() const;
    QGeoCameraCapabilities cameraCapabilities() const;

Q_SIGNALS:
    void resolutionFinished(const QGeoTileProviderWmts* provider);
    void resolutionError(const QGeoTileProviderWmts* provider);
    void resolutionRequired();

public Q_SLOTS:
    void resolveProvider();
    void disableRedirection();

protected Q_SLOTS:
    void onResolutionFinished(TileProvider* provider);
    void onResolutionError(TileProvider* provider);
    void updateCameraCapabilities();

protected:
    void addProvider(TileProvider* provider);

    /* Data members */

    QNetworkAccessManager* m_nm;
    QVector<TileProvider*> m_providerList;
    TileProvider* m_provider;
    int m_providerId;
    QGeoMapType m_mapType;
    Status m_status;
    QGeoCameraCapabilities m_cameraCapabilities;
};
