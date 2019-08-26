#include <QtTest>

// add necessary includes here
#include "EspUdp.h"

class EspUdpTest : public QObject
{
	Q_OBJECT

public:
	EspUdpTest();
	~EspUdpTest();

private slots:
	void case1_connection();
	void case2_requestData();
};

EspUdpTest::EspUdpTest()
{

}

EspUdpTest::~EspUdpTest()
{

}

void EspUdpTest::case1_connection()
{
	EspUdp esp(this);
	QSignalSpy spy(&esp, SIGNAL(connectionDone()));

	esp.connect("192.168.13.10", 1234);

	int counter = 0;
	while (spy.count() == 0)
	{
		QTest::qWait(5);
		counter++;
		if(counter > 500)
			qFatal("Couldn't connect");
	}

	esp.disconnect();
}

void EspUdpTest::case2_requestData()
{
	QVector<quint16> vector;
	EspUdp esp(this);
	esp.connect("192.168.13.10", 1234);

	esp.send('a');

	QObject::connect(&esp, &EspUdp::dataReady, this,
		[&esp](){
		esp.getBufferData16bits();

	});
}

QTEST_MAIN(EspUdpTest)

#include "tst_espudptest.moc"
