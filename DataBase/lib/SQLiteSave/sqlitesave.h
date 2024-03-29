#ifndef SQLITESAVE_H
#define SQLITESAVE_H

#include <QVariant>

#include <QSqlDatabase>
#include <QSqlQuery>

#include <QSqlTableModel>
#include <QSqlRecord>
#include <QSqlField>

#include <QStandardPaths>
#include <QDir>

class SQLiteSave : protected QObject
{
	Q_OBJECT
public:
	typedef enum Status{
		Ok,
		DB_Opened,
		DB_NotOpened,
		DB_AlreadyOpened,
		DB_Closed,
		DB_AlreadyClosed,
		Colum_Added,
		Colum_NotAdded,
		Colum_AlreadyExist,
		Row_Written,
		Row_NotWritten,
		Table_AlreadyExist,
		Table_Created,
		Table_NotCreated,
		Table_Deleted,
		Table_NotDeleted,
		Table_DontExist,
		WriteRow_DifferentSizes
	}Status;

	typedef enum ColType{
		_NULL,
		INTEGER,
		REAL,
		TEXT,
		BLOB
	}ColType;

	explicit SQLiteSave(QObject* parent = nullptr, const QString &dbName = tr("memory"));
	~SQLiteSave() override;

	Status openDB(const QString &dbName = tr("memory"));
	Status closeDB();

	Status addColum(const QString &newCol, const ColType &type);
	// Status addColums(const QStringList &newCols, const ColType &type); :TODO
	QStringList getColumnNames();

	QVector<QVariant> readRow(const int& index);
	Status writeRow(const QVector<QVariant> &row);

	Status openTable(const QString &tabName);
	Status deleteTable(const QString &tabName);

private:
	QSqlTableModel* _table;
	QSqlDatabase _database;

	QString _dbFullPath;
	QDir _dbDir;

	// Auxiliar variables and functions
	QSqlQuery _query;
	QSqlRecord _record;
	QVector<QVariant> _auxVec;

	QStringList _listColTypes;
	QString _colTypeToString(const ColType &type);
};

#endif // SQLITESAVE_H
