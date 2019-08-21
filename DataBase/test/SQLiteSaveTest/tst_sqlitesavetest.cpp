#include <QtTest>
#include <QtDebug>
// add necessary includes here
#include "sqlitesave.h"

class SQLiteSaveTest : public QObject
{
	Q_OBJECT

public:
	SQLiteSaveTest();
	~SQLiteSaveTest();

private:
	SQLiteSave sql;

private slots:
	void case1_OpenNoExistentTable();
	void case2_OpenExistentTable();

	// case 3 to 5 are interdependent
	void case3_AddColumn(); // creates a table
	void case4_WriteRow(); // reuse the previous table
	void case5_ReadRow(); // reuse the previous table and delete it.
};

SQLiteSaveTest::SQLiteSaveTest()
{

}

SQLiteSaveTest::~SQLiteSaveTest()
{
}

/**
 * @brief Check the open and closure of tables.
 */
void SQLiteSaveTest::case1_OpenNoExistentTable()
{
	switch (sql.openTable("AutoTest"))
	{
	case SQLiteSave::Table_Created:
		qInfo() << "Table " << "AutoTest" << " created.";
		break;
	case SQLiteSave::Table_AlreadyExist:
		qWarning() << "Table " << "AutoTest" << " already exist. "
																						"But it shouldn't exist at this point.";
		break;
	case SQLiteSave::Table_NotCreated:
		qFatal("Table AutoTest couldn't be created.");
	default:
		break;
	}
	switch (sql.deleteTable("AutoTest"))
	{
	case SQLiteSave::Table_Deleted:
		qInfo() << "Table " << "AutoTest" << " deleted.";
		break;
	case SQLiteSave::Table_DontExist:
		qFatal("Table AutoTest don't exist.");
	case SQLiteSave::Table_NotDeleted:
		qFatal("Table AutoTest couldn't be deleted.");
	default:
		break;
	}
}

/**
 * @brief Open a table two times, first time should produce
 * a 'table created' message, the second a 'table already exist' one.
 * Delete should be sucesss.
 */
void SQLiteSaveTest::case2_OpenExistentTable()
{
	switch (sql.openTable("AutoTest"))
	{
	case SQLiteSave::Table_Created:
		qInfo() << "Table " << "AutoTest" << " created.";
		break;
	case SQLiteSave::Table_AlreadyExist:
		qWarning() << "Table " << "AutoTest" << " already exist. "
																						"But it shouldn't exist at this point.";
		break;
	case SQLiteSave::Table_NotCreated:
		qFatal("Table AutoTest couldn't be created.");
	default:
		break;
	}

	switch (sql.openTable("AutoTest"))
	{
	case SQLiteSave::Table_Created:
		qFatal("Table AutoTest created.");
	case SQLiteSave::Table_AlreadyExist:
		qDebug() << "Table " << "AutoTest" << " already exist.";
		break;
	case SQLiteSave::Table_NotCreated:
		qFatal("Table AutoTest couldn't be created.");
	default:
		break;
	}

	switch (sql.deleteTable("AutoTest"))
	{
	case SQLiteSave::Table_Deleted:
		qDebug() << "Table " << "AutoTest" << " deleted.";
		break;
	case SQLiteSave::Table_DontExist:
		qFatal("Table AutoTest don't exist.");
	case SQLiteSave::Table_NotDeleted:
		qFatal("Table AutoTest couldn't be deleted.");
	default:
		break;
	}
}

void SQLiteSaveTest::case3_AddColumn()
{
	QString tabName = "AutoTest";
	switch (sql.openTable(tabName))
	{
	case SQLiteSave::Table_Created:
		qDebug("Table AutoTest created.");
		break;
	case SQLiteSave::Table_AlreadyExist:
		qDebug() << "Table " << "AutoTest" << " already exist.";
		break;
	default:
		break;
	}

	QString colName = "Dummy";
	switch(sql.addColum(colName, SQLiteSave::TEXT))
	{
	case SQLiteSave::Colum_Added:
		qDebug() << "Column " << colName << " added.";
		break;
	case SQLiteSave::Colum_NotAdded:
		qFatal("Column %s not added.", qPrintable(colName));
	case SQLiteSave::Colum_AlreadyExist:
		qFatal("Column %s already exist.", qPrintable(colName));
	default:
		break;
	}
}

void SQLiteSaveTest::case4_WriteRow()
{
	QVector<QVariant> row;
	QString colName = "Dummy";

	row << "TestCell";
	switch(sql.writeRow(row))
	{
	case SQLiteSave::Row_Written:
		qDebug() << "Row written!";
		break;
	case SQLiteSave::Row_NotWritten:
		qFatal("Row not written!");
	default:
		break;
	}
}

void SQLiteSaveTest::case5_ReadRow()
{
	QVector<QVariant> row;
	QString colName = "Dummy";

	row = sql.readRow(0);

	if(row.at(0).toString() == "TestCell")
		qInfo() << "Row read success!";
	else
		qFatal("Row read fail!");

	switch (sql.deleteTable("AutoTest"))
	{
	case SQLiteSave::Table_Deleted:
		qDebug() << "Table \"AutoTest\" deleted.";
		break;
	case SQLiteSave::Table_DontExist:
		qFatal("Table AutoTest don't exist.");
	case SQLiteSave::Table_NotDeleted:
		qFatal("Table AutoTest couldn't be deleted.");
	default:
		break;
	}
}

QTEST_APPLESS_MAIN(SQLiteSaveTest)

#include "tst_sqlitesavetest.moc"
