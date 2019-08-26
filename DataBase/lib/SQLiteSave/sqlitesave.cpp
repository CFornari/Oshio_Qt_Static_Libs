#include "sqlitesave.h"

#include <QDebug>

#include <QSqlDriver>
#include <QSqlError>
#include <QSqlQuery>

SQLiteSave::SQLiteSave(QObject* parent, const QString &dbName):
	QObject (parent),
	_dbDir(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation))
{
	const QString DRIVER("QSQLITE");

	if(!QSqlDatabase::isDriverAvailable(DRIVER))
		qFatal("Database - ERROR: no driver %s available", qPrintable(DRIVER));

	_database = QSqlDatabase::addDatabase(DRIVER, DRIVER);

	// Ensure that we have a writable location on all devices.
	if (!_dbDir.mkpath("."))
			qWarning("Failed to create writable directory at %s", qPrintable(_dbDir.absolutePath()));

	openDB(dbName);

	_table = new QSqlTableModel(parent, _database);
	_table->setEditStrategy(QSqlTableModel::OnManualSubmit);
	_query = QSqlQuery(_database);
	qDebug() << "SQLiteSave ready!";
}

SQLiteSave::~SQLiteSave()
{
	delete _table;
	if(_database.isOpen())
		_database.close();
}

SQLiteSave::Status SQLiteSave::openDB(const QString& dbName)
{
	_dbFullPath = _dbDir.absolutePath() + "/"+dbName+".sqlite3";
	qDebug() << "Database full path:" << _dbFullPath;
	_database.setDatabaseName(_dbFullPath);

	// When using the SQLite driver, open() will create the SQLite database if it doesn't exist.
	if (!_database.open())
	{
		QFile::remove(_dbFullPath);
		qWarning("Cannot open database: %s", qPrintable(_database.lastError().text()));
		return DB_NotOpened;
	}
	return DB_Opened;
}

SQLiteSave::Status SQLiteSave::closeDB()
{
	if(_database.isOpen())
	{
		_database.close();
		return DB_Closed;
	}
	return DB_AlreadyClosed;
}

SQLiteSave::Status SQLiteSave::addColum(const QString& newCol, const ColType& type)
{
	// If the name exist don't add column.
	if(_table->record().contains(newCol))
		return Colum_AlreadyExist;

	// Convert the type enum to a string.
	QString stringType = _colTypeToString(type);

	// Execute query to add the column 'name'.
	if (!_query.exec(QString("ALTER TABLE "+ _table->tableName()+" ADD COLUMN "+newCol+" "+stringType+";")))
	{
		qDebug() << "Last query: " << _query.lastQuery();
		qWarning()<<"addColum(): Failed to query database: " << _query.lastError().text();
		return Colum_NotAdded;
	}

	return Colum_Added;
}

QStringList SQLiteSave::getColumnNames()
{
	_table->select();
	QStringList list;
	for (int i = 0; i < _table->record().count(); i++)
	{
		list.append(_table->record().fieldName(i));
		qDebug() << _table->record().fieldName(i);
	}
	return list;
}

QVector<QVariant> SQLiteSave::readRow(const int& index)
{	
	// WORKAROUND: The table should already be set by 'this.openTable()';
//	mTable->setTable(mTable->tableName());
	_table->select();

	_auxVec.clear();
	QSqlRecord rec;
	for (int i = 1; i < _table->record().count(); i++)
	{
		// Doesn't append the first row.
		rec = _table->record(index);
		if(rec.isEmpty())
			qFatal("Empty record at readRow()");
		_auxVec.append(rec.value(1));
	}
	return _auxVec;
}

SQLiteSave::Status SQLiteSave::writeRow(const QVector<QVariant>& row)
{
	// WORKAROUND: The table should already be set by 'this.openTable()';
	_table->setTable(_table->tableName());

	_record = _table->record();
	qDebug() << "Name #2:" << _record.fieldName(1);
	if ( row.size() != _record.count()-1 )
	{
		qDebug("Input size: %d \t Table size: %d", row.size(), _table->record().count()-1);
		qWarning("writeRow(): Row size doesn't match's database.");
	}

	for (int i = 0; i < row.size(); i++)
	{
		_record.setValue(i+1, row.at(i));
	}

	if (!_table->insertRecord(_table->rowCount(), _record)) {
			qWarning("writeRow(): insertRecord() failed -- %s", qPrintable(_table->lastError().text()));
	}

	if(!_table->submitAll())
	{
		qWarning("writeRow(): %s", qPrintable(_table->lastError().text()));
		return Row_NotWritten;
	}
	return Row_Written;
}

SQLiteSave::Status SQLiteSave::openTable(const QString& tabName)
{
	if (_database.tables().contains(tabName))
	{
		// The table already exists; just select the 'tabName' table.
		_table->setTable(tabName);
		return Table_AlreadyExist;
	}

	if (!_query.exec("CREATE TABLE " + tabName + "(id INTEGER PRIMARY KEY AUTOINCREMENT);"))
	{
		qDebug() << "Last query: " << _query.lastQuery();
		qWarning("openTable(): Failed to create table: %s", qPrintable(_query.lastError().text()));
		return Table_NotCreated;
	}

	_table->setTable(tabName);
	return Table_Created;
}

SQLiteSave::Status SQLiteSave::deleteTable(const QString& tabName)
{
	if (!_database.tables().contains(tabName))
	{
		qDebug() << "Table don't exist!";
		return Table_DontExist;
	}

	if (!_query.exec("DROP TABLE " + tabName))
	{
		qDebug() << "Last query: " << _query.lastQuery();
		qWarning("deleteTable(): Failed to delete table: %s", qPrintable(_query.lastError().text()));
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



