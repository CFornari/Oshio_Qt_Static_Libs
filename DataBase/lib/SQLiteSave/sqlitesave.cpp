#include "sqlitesave.h"

#include <QDebug>

#include <QSqlDriver>
#include <QSqlError>
#include <QSqlQuery>

SQLiteSave::SQLiteSave(QObject* parent, const QString &dbName):
	QObject (parent),
	mDbDir(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation))
{
	const QString DRIVER("QSQLITE");

	if(!QSqlDatabase::isDriverAvailable(DRIVER))
		qFatal("Database - ERROR: no driver %s available", qPrintable(DRIVER));

	mDatabase = QSqlDatabase::addDatabase(DRIVER, DRIVER);

	// Ensure that we have a writable location on all devices.
	if (!mDbDir.mkpath("."))
			qWarning("Failed to create writable directory at %s", qPrintable(mDbDir.absolutePath()));

	openDB(dbName);

	mTable = new QSqlTableModel(parent, mDatabase);
	mTable->setEditStrategy(QSqlTableModel::OnManualSubmit);
	mQuery = QSqlQuery(mDatabase);
	qDebug() << "SQLiteSave ready!";
}

SQLiteSave::~SQLiteSave()
{
	delete mTable;
	if(mDatabase.isOpen())
		mDatabase.close();
}

SQLiteSave::Status SQLiteSave::openDB(const QString& dbName)
{
	mDbFullPath = mDbDir.absolutePath() + "/"+dbName+".sqlite3";
	qDebug() << "Database full path:" << mDbFullPath;
	mDatabase.setDatabaseName(mDbFullPath);

	// When using the SQLite driver, open() will create the SQLite database if it doesn't exist.
	if (!mDatabase.open())
	{
		QFile::remove(mDbFullPath);
		qWarning("Cannot open database: %s", qPrintable(mDatabase.lastError().text()));
		return DB_NotOpened;
	}
	return DB_Opened;
}

SQLiteSave::Status SQLiteSave::closeDB()
{
	if(mDatabase.isOpen())
	{
		mDatabase.close();
		return DB_Closed;
	}
	return DB_AlreadyClosed;
}

SQLiteSave::Status SQLiteSave::addColum(const QString& newCol, const ColType& type)
{
	// If the name exist don't add column.
	if(mTable->record().contains(newCol))
		return Colum_AlreadyExist;

	// Convert the type enum to a string.
	QString stringType = _colTypeToString(type);

	// Execute query to add the column 'name'.
	if (!mQuery.exec(QString("ALTER TABLE "+ mTable->tableName()+" ADD COLUMN "+newCol+" "+stringType+";")))
	{
		qDebug() << "Last query: " << mQuery.lastQuery();
		qWarning()<<"addColum(): Failed to query database: " << mQuery.lastError().text();
		return Colum_NotAdded;
	}

	return Colum_Added;
}

QStringList SQLiteSave::getColumnNames()
{
	mTable->select();
	QStringList list;
	for (int i = 0; i < mTable->record().count(); i++)
	{
		list.append(mTable->record().fieldName(i));
		qDebug() << mTable->record().fieldName(i);
	}
	return list;
}

QVector<QVariant> SQLiteSave::readRow(const int& index)
{	
	// WORKAROUND: The table should already be set by 'this.openTable()';
//	mTable->setTable(mTable->tableName());
	mTable->select();

	mAuxVec.clear();
	QSqlRecord rec;
	for (int i = 1; i < mTable->record().count(); i++)
	{
		// Doesn't append the first row.
		rec = mTable->record(index);
		if(rec.isEmpty())
			qFatal("Empty record at readRow()");
		mAuxVec.append(rec.value(1));
	}
	return mAuxVec;
}

SQLiteSave::Status SQLiteSave::writeRow(const QVector<QVariant>& row)
{
	// WORKAROUND: The table should already be set by 'this.openTable()';
	mTable->setTable(mTable->tableName());

	mRecord = mTable->record();
	qDebug() << "Name #2:" << mRecord.fieldName(1);
	if ( row.size() != mRecord.count()-1 )
	{
		qDebug("Input size: %d \t Table size: %d", row.size(), mTable->record().count()-1);
		qWarning("writeRow(): Row size doesn't match's database.");
	}

	for (int i = 0; i < row.size(); i++)
	{
		mRecord.setValue(i+1, row.at(i));
	}

	if (!mTable->insertRecord(mTable->rowCount(), mRecord)) {
			qWarning("writeRow(): insertRecord() failed -- %s", qPrintable(mTable->lastError().text()));
	}

	if(!mTable->submitAll())
	{
		qWarning("writeRow(): %s", qPrintable(mTable->lastError().text()));
		return Row_NotWritten;
	}
	return Row_Written;
}

SQLiteSave::Status SQLiteSave::openTable(const QString& tabName)
{
	if (mDatabase.tables().contains(tabName))
	{
		// The table already exists; just select the 'tabName' table.
		mTable->setTable(tabName);
		return Table_AlreadyExist;
	}

	if (!mQuery.exec("CREATE TABLE " + tabName + "(id INTEGER PRIMARY KEY AUTOINCREMENT);"))
	{
		qDebug() << "Last query: " << mQuery.lastQuery();
		qWarning("openTable(): Failed to create table: %s", qPrintable(mQuery.lastError().text()));
		return Table_NotCreated;
	}

	mTable->setTable(tabName);
	return Table_Created;
}

SQLiteSave::Status SQLiteSave::deleteTable(const QString& tabName)
{
	if (!mDatabase.tables().contains(tabName))
	{
		qDebug() << "Table don't exist!";
		return Table_DontExist;
	}

	if (!mQuery.exec("DROP TABLE " + tabName))
	{
		qDebug() << "Last query: " << mQuery.lastQuery();
		qWarning("deleteTable(): Failed to delete table: %s", qPrintable(mQuery.lastError().text()));
		return Table_NotDeleted;
	}

	return Table_Deleted;
}

QString SQLiteSave::_colTypeToString(const ColType& type)
{
	switch (type)
	{
	case _NULL:
		return tr("NULL");
	case INTEGER:
		return tr("INTEGER");
	case REAL:
		return tr("REAL");
	case TEXT:
		return tr("TEXT");
	case BLOB:
		return tr("BLOB");
	}
}



