#ifndef ESPUDP_H
#define ESPUDP_H

#include <QUdpSocket>
#include <QNetworkProxy>
#include <QNetworkAccessManager>

#define ESP_WIFI_SSID "MatrixSensor"
#define ESP_WIFI_PWD  "21sda6xsd87"

class EspUdp : public QObject
{
	Q_OBJECT

public:

	explicit EspUdp(QObject *parent = nullptr);
	virtual ~EspUdp();

	bool connect(const QString &hostIP = ESP_WIFI_SSID, const quint16 port = 1234);
	bool disconnect();

	bool send(const char &command);

	QVector<quint8> getBufferData8bits();
	QVector<quint16> getBufferData16bits();

signals:

	void connectionDone();
	void disconnectionDone();
	void dataReady();

private slots:

	void cb_receive();

private:
	QNetworkAccessManager *manager;

	QUdpSocket *_udpSocket;
	QByteArray _buffer;

	QVector<quint8> _data8b;
	QVector<quint16> _data16b;

	enum class LastCommand{
		Connect,
		Disconnect,
		Send,
		None,
	};

	enum class State{
		Connected,
		Disconnected,
	};

	LastCommand _lastCommand;
	State _state;
};

#endif // ESPUDP_H
