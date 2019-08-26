#include "EspUdp.h"

EspUdp::EspUdp(QObject* parent):
	QObject(parent)
{
	_lastCommand = LastCommand::None;
	_state = State::Disconnected;

	QNetworkProxy proxy;
	proxy.setType(QNetworkProxy::Socks5Proxy);
	proxy.setHostName("proxy.example.com");
	proxy.setPort(1080);
	proxy.setUser("username");
	proxy.setPassword("password");
	QNetworkProxy::setApplicationProxy(proxy);

	_udpSocket = new QUdpSocket(this);

	QObject::connect(_udpSocket, SIGNAL(readyRead()), this, SLOT(cb_receive()));

	// Instantiate the objects and connect to the finished signal.
	// QObject::connect(&watcher, SIGNAL(finished()), this, SLOT(handleFinished()));
	// Start the computation in another thread.
	// future = QtConcurrent::run(this, &EspUdp::connectThread);
	// watcher.setFuture(future);
}

EspUdp::~EspUdp()
{
	disconnect();
	_udpSocket->disconnectFromHost();
}

bool EspUdp::connect(const QString &hostIP, const quint16 port)
{
	if (_state == State::Connected)
		return false;

	_udpSocket->connectToHost(hostIP, port);

	if (_lastCommand != LastCommand::None)
		_udpSocket->waitForBytesWritten();

	if(_udpSocket->write("c") != -1) // send command
	{
	 _lastCommand = LastCommand::Connect;
	 _udpSocket->waitForBytesWritten();
	 return true;
	}
	else
	 return false;
}

bool EspUdp::disconnect()
{
	if (_state == State::Disconnected)
	{
		qDebug() << "Already Disconnected!";
		return false;
	}

	if (_lastCommand != LastCommand::None)
		_udpSocket->waitForBytesWritten();

	if(_udpSocket->write("d") != -1) // send command
	{
	 _lastCommand = LastCommand::Disconnect;
	 _udpSocket->waitForBytesWritten();
	 return true;
	}
	else
		return false;
}

bool EspUdp::send(const char& command)
{
	if (_state == State::Connected)
		return false;

	if (_lastCommand != LastCommand::None)
		return false;

	if(_udpSocket->write(&command) != -1){
	 _lastCommand = LastCommand::Send;
	 return true;
	}
	else
		return false;
}

QVector<quint8> EspUdp::getBufferData8bits()
{
	_data8b.clear();
	for (int i = 0; i < _buffer.size(); i++)
	{
		_data8b.append(uint8_t(_buffer.at(i)));
	}
	return _data8b;
}

QVector<quint16> EspUdp::getBufferData16bits()
{
	_data16b.clear();
	for (int i = 0; i < _buffer.size()-1; i+=2)
	{
		_data16b.append(quint16 ( quint8(_buffer.at(i+1))<<8 |
															quint8(_buffer.at(i))
															));
	}
	return _data16b;
}

void EspUdp::cb_receive()
{
	_buffer = _udpSocket->readAll();

	switch (_lastCommand)
	{
	case LastCommand::Send:
		_lastCommand = LastCommand::None;
		emit dataReady();
		break;

	case LastCommand::Connect:
		qDebug() << "CB_Connect:" << tr(_buffer);

		if(QString(_buffer) == QString("Connected! :)"))
		{

		 qDebug() << "Local IP: " << _udpSocket->localAddress().toString();
		 qDebug() << "Local Port: " << _udpSocket->localPort();
		 _lastCommand = LastCommand::None;
		 _state = State::Connected;
		 emit connectionDone();
		}
		break;

	case LastCommand::Disconnect:

		qDebug() << "CB_Disconnect:" << tr(_buffer);
		if(QString(_buffer) == QString("Disconnected! :("))
		{
		 _lastCommand = LastCommand::None;
		 _state = State::Disconnected;
		 emit disconnectionDone();
		}		
		break;

	case LastCommand::None:
		// Do nothing
		break;
	}
}
